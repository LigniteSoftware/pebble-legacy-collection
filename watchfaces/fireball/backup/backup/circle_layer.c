#include <pebble.h>
#include "circle_layer.h"

Layer *circle_layer;
AppTimer *update_timer, *battery_animation_timer;
Circle *minute_circles[60];
Circle *battery_circles[100];
AppTimer *circle_travel_timers[60];
struct tm *circle_t;
int current_minute = 0;
int circles_to_draw = 0, battery_circle_to_draw = 0, charge_percent = 0, runs = 0;
bool running = false, charging = false, animating_circles = false;
Settings circle_layer_settings;
bool booted = false;

bool is_booted(){
	return booted;
}

void circle_layer_OBLITERATE(){
	for(int i = 0; i < 60; i++){
		free(minute_circles[i]);
	}
}

void circle_layer_update_settings(Settings new_settings){
	circle_layer_settings = new_settings;
	for(int i = 0; i < 100; i++){
		circle_set_size(battery_circles[i], i);
	}
	circle_layer_update();
}

void circle_update_travel_path(void *c){
	Circle *circle = (Circle*)c;
	circle->location.x = circle_get_position(circle->count+circle->animation_count, true);
	circle->location.y = circle_get_position(circle->count+circle->animation_count, false);
	circle->animation_count++;
	if((circle->count+circle->animation_count) == circle->new_position){
			circle->size = 0;
	}
	layer_mark_dirty(circle_layer);
	if(circle->size != 0){
		circle_travel_timers[circle->count] = app_timer_register(5, circle_update_travel_path, circle);
	}
	if(circle->size == 0 && circle->count == 59){
		animating_circles = false;
	}
}

void circle_update_travel_path_invert(void *c){
	Circle *circle = (Circle*)c;
	bool update_count = false;
	if(circle->size < circle_layer_get_circle_size(circle->new_position, circle_layer_settings)){
		circle->size++;
	}
	else{
		circle->location.x = circle_get_position(circle->count, true);
		circle->location.y = circle_get_position(circle->count, false);
		circle->animation_count++;
		update_count = true;
	}
	layer_mark_dirty(circle_layer);
	if(circle->count == circle->new_position){
		APP_LOG(APP_LOG_LEVEL_INFO, "Returning at %d", circle->count);
		return;
	}
	if(!update_count){
		circle_travel_timers[circle->count] = app_timer_register(50/* 1/x! It's a radical! */, circle_update_travel_path_invert, circle);
	}
	if(update_count){
		circle->count--;
		circle_travel_timers[circle->count] = app_timer_register(15, circle_update_travel_path_invert, circle);
	}
}

void circle_travel(Circle *circle, uint8_t new_position, int delay, bool invert){
	circle->new_position = new_position;
	circle->animation_count = 0;
	if(!invert){
		circle_travel_timers[circle->count] = app_timer_register(delay+10, circle_update_travel_path, circle);
	}
	else{
		circle_travel_timers[circle->count] = app_timer_register(delay+10, circle_update_travel_path_invert, circle);
	}
}

void circle_layer_update_proc(Layer *layer, GContext *ctx){
	for(int i = 0; i < circles_to_draw; i++){
		graphics_context_set_fill_color(ctx, minute_circles[i]->colour);
		graphics_fill_circle(ctx, minute_circles[i]->location, minute_circles[i]->size);
	}
	for(int i = 0; i < battery_circle_to_draw; i++){
		graphics_context_set_fill_color(ctx, battery_circles[i]->colour);
		graphics_fill_circle(ctx, battery_circles[i]->location, battery_circles[i]->size);
	}
}

void battery_animation_update(){
	battery_circle_to_draw++;
	if(battery_circle_to_draw > charge_percent){
		battery_circle_to_draw = 0;
	}
	if(charging){
		runs = 100;
	}
	if(runs > 0){
		battery_animation_timer = app_timer_register(100, battery_animation_update, NULL);
		runs--;
	}
	layer_mark_dirty(circle_layer);
}

void circle_layer_battery_handler(BatteryChargeState state){
	battery_circle_to_draw = state.charge_percent;
	if(state.is_charging){
		battery_animation_timer = app_timer_register(100, battery_animation_update, NULL);
	}
	else if(!state.is_charging && charging){
		runs = (state.charge_percent/10)-battery_circle_to_draw;
	}
	charging = state.is_charging;
	charge_percent = state.charge_percent/10;
	layer_mark_dirty(circle_layer);
}

void circle_layer_set_tm(struct tm *t, TimeUnits units){
	if(t->tm_min == 0 && units == 7){
		for(int i = 0; i < 60; i++){
			circle_travel(minute_circles[i], 60, 50, false);
		}
		animating_circles = true;
	}
	else{
		animating_circles = false;
	}
	circle_t = t;
	//current_minute = t->tm_min-1;
	current_minute = 50;
}

void circle_layer_add_circle(Circle *circle){
	switch(circle->type){
		case CIRCLE_TYPE_MINUTE:
			minute_circles[circle->count] = circle;
			break;
		case CIRCLE_TYPE_BATTERY:
			battery_circles[circle->count] = circle;
			break;
	}
}

int circle_layer_get_circle_size(int minute, Settings settings){
	switch(settings.algorithm_type){
		case CIRCLE_ALGORITHM_SCALING:
			return (minute/settings.algorithm_radius)+4;
		case CIRCLE_ALGORITHM_LINEAR:
			return settings.algorithm_radius+4;
		default:
			return 3;
	}
}

int circle_layer_get_battery_circle_size(int percentage, Settings settings){
	switch(settings.algorithm_type){
		case CIRCLE_ALGORITHM_SCALING:
			return ((100-percentage)/settings.algorithm_radius/2);
		case CIRCLE_ALGORITHM_LINEAR:
			return settings.algorithm_radius+4;
		default:
			return 3;
	}
}

Circle *circle_create(){
	Circle *circle = malloc(sizeof(Circle));
	#ifdef PBL_COLOR
	circle->colour = GColorRed;
	#else
	circle->colour = GColorLightGray;
	#endif
	return circle;
}

void circle_layer_init(Window *window){
	circle_layer = layer_create(GRect(0, 0, HALF_SCREEN_WIDTH*2, HALF_SCREEN_HEIGHT*2));
	layer_set_update_proc(circle_layer, circle_layer_update_proc);
	layer_add_child(window_get_root_layer(window), circle_layer);
}

Layer *circle_layer_get_layer(){
	return circle_layer;
}

void circle_layer_load_update(){
	if(circles_to_draw < current_minute+1){
		if(circles_to_draw < 1){
			circles_to_draw++;
		}
		else{
			Circle *curr_c = minute_circles[circles_to_draw-1];
			if(curr_c->size <= circle_layer_get_circle_size(circles_to_draw, circle_layer_settings)){
				curr_c->size++;
			}
			else{
				circles_to_draw++;
			}
		}
		update_timer = app_timer_register(5, circle_layer_load_update, NULL);
		layer_mark_dirty(circle_layer);
	}
	else{
		running = false;
	}
}

void circle_layer_update(){
	if(running || current_minute == 0){
		return;
	}
	running = true;
	APP_LOG(APP_LOG_LEVEL_INFO, "Called animation for minute %d", circles_to_draw);
 	update_timer = app_timer_register(500, circle_layer_load_update, NULL);
 	minute_circles[current_minute]->location.x = circle_get_position(60, true);
 	minute_circles[current_minute]->location.y = circle_get_position(60, false);
 	minute_circles[current_minute]->size = 0;
 	minute_circles[current_minute]->count = 60;
 	circle_travel(minute_circles[current_minute], current_minute, 100, true);

	/*
	switch(circle_layer_settings.minute_change_anim){
		case ANIM_TYPE_REDRAW:
			for(int i = 0; i < current_minute; i++){
				minute_circles[i]->size = 0;
			}
			break;
		case ANIM_TYPE_GROW_AND_SLIDE:
		*/

			/*
			break;
		case ANIM_TYPE_GROW:
			minute_circles[current_minute-1]->size = 0;
			break;
	}
	*/
}

void circle_set_size(Circle *circle, int data){
	if(circle->type == CIRCLE_TYPE_MINUTE){
		circle->size = circle_layer_get_circle_size(data, circle_layer_settings);
	}
	else{
		circle->size = circle_layer_get_battery_circle_size(data, circle_layer_settings);
	}
}

Circle *get_circle(int location, CircleType type){
	switch(type){
		case CIRCLE_TYPE_MINUTE:
			return minute_circles[location];
		case CIRCLE_TYPE_BATTERY:
			return battery_circles[location];
		default:
			return NULL;
	}
}

int circle_get_position(int minute, bool x){
	int result = 0;
	int fixedMinute = minute+4;
	if(x){
		result = (sin_lookup(TRIG_MAX_ANGLE*fixedMinute/60) * (int32_t)CIRCLE_RADIUS / TRIG_MAX_RATIO) + (HALF_SCREEN_WIDTH+2);
	}
	else{
		result = (-cos_lookup(TRIG_MAX_ANGLE*fixedMinute/60) * (int32_t)CIRCLE_RADIUS / TRIG_MAX_RATIO) + (HALF_SCREEN_HEIGHT-6);
	}
	return result;
}

int battery_circle_get_position(int percentage, bool x){
	int result = 0;
	int fixedPercentage = percentage+8;
	if(x){
		result = (sin_lookup(TRIG_MAX_ANGLE*fixedPercentage/100) * (int32_t)BATTERY_CIRCLE_RADIUS / TRIG_MAX_RATIO) + (HALF_SCREEN_WIDTH+4);
	}
	else{
		result = (-cos_lookup(TRIG_MAX_ANGLE*fixedPercentage/100) * (int32_t)BATTERY_CIRCLE_RADIUS / TRIG_MAX_RATIO) + (HALF_SCREEN_HEIGHT-6);
	}
	return result;
}

void circle_set_location(Circle *circle, int data){
	if(circle->type == CIRCLE_TYPE_MINUTE){
		circle->location.x = circle_get_position(data, true);
		circle->location.y = circle_get_position(data, false);
	}
	else{
		circle->location.x = battery_circle_get_position(data, true);
		circle->location.y = battery_circle_get_position(data, false);
	}
	circle->count = data;
}
