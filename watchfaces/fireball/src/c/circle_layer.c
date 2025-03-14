#include <pebble.h>
#include "circle_layer.h"

//#define DISABLE_MINUTE_LAYER
//#define DISABLE_BATTERY_LAYER

#define FORCE_SPECIFIC_MINUTE 60
#define FORCE_SPECIFIC_BATTERY_LEVEL 100

Layer *circle_layer, *battery_layer;
AppTimer *update_timer, *battery_animation_timer;
Circle *minute_circles[60];
Circle *battery_circles[100];
struct tm *circle_t;
int current_minute = 0;
int circles_to_draw = 60, battery_circle_to_draw = 100;
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
		battery_circles[i]->colour = circle_layer_settings.batteryBarColour;
	}

	for(int i = 0; i < 60; i++){
		circle_set_size(minute_circles[i], i);
		minute_circles[i]->colour = circle_layer_settings.minuteBarColour;
	}
	circle_layer_update();
}

void circle_layer_update_proc(Layer *layer, GContext *ctx){
	#ifndef DISABLE_MINUTE_LAYER
	if(circles_to_draw > 0){
		for(int i = 0; i < circles_to_draw; i++){
			graphics_context_set_fill_color(ctx, minute_circles[i]->colour);
			graphics_fill_circle(ctx, minute_circles[i]->location, minute_circles[i]->size);
		}
	}
	#endif
}

void battery_layer_update_proc(Layer *layer, GContext *ctx){
	#ifndef DISABLE_BATTERY_LAYER
	if(circle_layer_settings.batteryBar){
		int toDraw = (99-battery_circle_to_draw);
		for(int i = 99; i > toDraw; i--){
			graphics_context_set_fill_color(ctx, battery_circles[i]->colour);
			graphics_fill_circle(ctx, battery_circles[i]->location, battery_circles[i]->size);
		}
	}
	#endif
}

void circle_layer_battery_handler(BatteryChargeState state){
	battery_circle_to_draw = state.charge_percent;
	#ifdef FORCE_SPECIFIC_BATTERY_LEVEL
	battery_circle_to_draw = FORCE_SPECIFIC_BATTERY_LEVEL;
	#endif
	layer_mark_dirty(circle_layer);
}

void circle_layer_set_tm(struct tm *t, TimeUnits units){
	circle_t = t;
	if(circle_layer_settings.minuteDependsOnMinute){
		circles_to_draw = t->tm_min-1;
		if(circles_to_draw < 0){
			circles_to_draw = 0;
		}
	}
	else{
		circles_to_draw = 60;
	}
	#ifdef FORCE_SPECIFIC_MINUTE
	circles_to_draw = FORCE_SPECIFIC_MINUTE;
	#endif
	circle_layer_update();
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
			return ((100-percentage)/settings.algorithm_radius/1.5)+2;
		case CIRCLE_ALGORITHM_LINEAR:
			return settings.algorithm_radius+4;
		default:
			return 3;
	}
}

Circle *circle_create(){
	Circle *circle = malloc(sizeof(Circle));
	#ifdef PBL_COLOR
	circle->colour = circle_layer_settings.minuteBarColour;
	#else
	circle->colour = GColorLightGray;
	#endif
	return circle;
}

void circle_layer_init(Layer *window_layer, Settings current_settings){
	circle_layer_settings = current_settings;

	GRect windowFrame = GRect(0, 0, HALF_SCREEN_WIDTH*2, HALF_SCREEN_HEIGHT*2);

	circle_layer = layer_create(windowFrame);
	layer_set_update_proc(circle_layer, circle_layer_update_proc);
	layer_add_child(window_layer, circle_layer);

	battery_layer = layer_create(windowFrame);
	layer_set_update_proc(battery_layer, battery_layer_update_proc);
	layer_add_child(window_layer, battery_layer);
}

void battery_layer_push_to_top(Layer *window_layer){
	layer_add_child(window_layer, battery_layer);
}

Layer *circle_layer_get_layer(){
	return circle_layer;
}

void circle_layer_update(){
	layer_mark_dirty(circle_layer);
	layer_mark_dirty(battery_layer);
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
	int fixedMinute = minute+10;
	if(x){
		result = (sin_lookup(TRIG_MAX_ANGLE*fixedMinute/68) * (int32_t)CIRCLE_RADIUS / TRIG_MAX_RATIO) + (HALF_SCREEN_WIDTH+2);
	}
	else{
		result = (-cos_lookup(TRIG_MAX_ANGLE*fixedMinute/68) * (int32_t)CIRCLE_RADIUS / TRIG_MAX_RATIO) + (HALF_SCREEN_HEIGHT);
	}
	return result;
}

int battery_circle_get_position(int percentage, bool x){
	int result = 0;
	int fixedPercentage = percentage+14;
	if(x){
		result = (sin_lookup(TRIG_MAX_ANGLE*fixedPercentage/160) * (int32_t)BATTERY_CIRCLE_RADIUS / TRIG_MAX_RATIO) + (HALF_SCREEN_WIDTH+2);
	}
	else{
		result = (-cos_lookup(TRIG_MAX_ANGLE*fixedPercentage/160) * (int32_t)BATTERY_CIRCLE_RADIUS / TRIG_MAX_RATIO) + (HALF_SCREEN_HEIGHT+1);
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
