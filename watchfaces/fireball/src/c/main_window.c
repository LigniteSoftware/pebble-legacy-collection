#include <pebble.h>
#include "lignite.h"
#include "main_window.h"
#include "circle_layer.h"
#include "gbitmap_color_palette_manipulator.h"

//#define DISABLE_MASKS

Window *main_window;
Layer *window_layer, *graphics_layer;
Settings settings;

TextLayer *time_layer, *date_layer;
BitmapLayer *minute_mask_layer, *battery_mask_layer;

GFont square_deal_large, square_deal_small;

Notification *disconnect_notification, *reconnect_notification;
int current_minute_main = 0;

GBitmap *battery_mask_bitmap;
GBitmap *minute_mask_bitmap;

void main_tick_handler(struct tm *t, TimeUnits units){
	Circle *current_circle = get_circle(t->tm_min, CIRCLE_TYPE_MINUTE);
	circle_set_location(current_circle, t->tm_min);

	if(current_minute_main > -1){
		circle_layer_set_tm(t, units);
		circle_layer_update(units);
	}

	static char time_buffer[] = "00:00";
	strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "Wed., 13.37.69. mememe";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);
}

void bluetooth_handler(bool connected){
	if(!connected && settings.btdisalert){
		vibes_long_pulse();
	}
	if(connected && settings.btrealert){
		vibes_double_pulse();
	}
}

void battery_handler(BatteryChargeState new_state){
	circle_layer_battery_handler(new_state);
}

void notification_push_handler(bool pushed){
	if(pushed){
		layer_remove_from_parent(window_layer);
	}
	else{
		if(disconnect_notification->is_live || reconnect_notification->is_live){
			return;
		}
		layer_add_child(window_get_root_layer(main_window), window_layer);
	}
}

void reload_mask_data(){
	if(battery_mask_bitmap)
		gbitmap_destroy(battery_mask_bitmap);
	if(minute_mask_bitmap)
		gbitmap_destroy(minute_mask_bitmap);

	#ifdef PBL_COLOR
	battery_mask_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_MASK);
	minute_mask_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIME_MASK);

	replace_gbitmap_color(GColorBlack, settings.backgroundColour, battery_mask_bitmap, battery_mask_layer);
	replace_gbitmap_color(GColorBlack, settings.backgroundColour, minute_mask_bitmap, minute_mask_layer);
	#else
	battery_mask_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_MASK);
	minute_mask_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIME_MASK);

	bitmap_layer_set_bitmap(battery_mask_layer, battery_mask_bitmap);
	bitmap_layer_set_bitmap(minute_mask_layer, minute_mask_bitmap);
	#endif
}

void main_window_settings_callback(Settings new_settings){
	settings = new_settings;

	circle_layer_update_settings(settings);

	layer_set_hidden(text_layer_get_layer(date_layer), !settings.date);

	#ifdef PBL_COLOR
	text_layer_set_text_color(time_layer, settings.textColour);
	text_layer_set_text_color(date_layer, settings.textColour);
	window_set_background_color(main_window, settings.backgroundColour);

	reload_mask_data();
	#endif

	layer_add_child(window_layer, circle_layer_get_layer());
	layer_add_child(window_layer, bitmap_layer_get_layer(minute_mask_layer));
	battery_layer_push_to_top(window_layer);
	layer_add_child(window_layer, bitmap_layer_get_layer(battery_mask_layer));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
	main_tick_handler(t, MINUTE_UNIT);
}

void main_window_load(Window *window){
	GRect frame = layer_get_frame(window_get_root_layer(window));
	window_layer = layer_create(frame);
	layer_add_child(window_get_root_layer(window), window_layer);

	setlocale(LC_ALL, "");

	Settings current_settings = data_framework_get_settings();

	circle_layer_init(window_layer, current_settings);

	for(int i = 0; i < 60; i++){
  		Circle *new_circle = circle_create();
  		//circle_set_size(new_circle, i);
  		new_circle->type = CIRCLE_TYPE_MINUTE;
		#ifdef PBL_COLOR
		new_circle->colour = current_settings.minuteBarColour;
		#endif
		circle_set_size(new_circle, i);
		circle_set_location(new_circle, i);
  		circle_layer_add_circle(new_circle);
  	}

	for(int i = 0; i < 100; i++){
  		Circle *new_battery_circle = circle_create();
  		new_battery_circle->type = CIRCLE_TYPE_BATTERY;
  		new_battery_circle->count = i;
		#ifdef PBL_COLOR
		new_battery_circle->colour = current_settings.batteryBarColour;
		#else
		new_battery_circle->colour = GColorWhite;
		#endif
		circle_set_location(new_battery_circle, i);
  		circle_layer_add_circle(new_battery_circle);
  	}
	battery_handler(battery_state_service_peek());

	GRect time_mask_frame = frame;
	minute_mask_layer = bitmap_layer_create(time_mask_frame);
	bitmap_layer_set_compositing_mode(minute_mask_layer, GCompOpSet);
	#ifndef DISABLE_MASKS
	layer_add_child(window_layer, bitmap_layer_get_layer(minute_mask_layer));
	#endif

	battery_layer_push_to_top(window_layer);

	GRect battery_mask_frame = frame;
	battery_mask_layer = bitmap_layer_create(battery_mask_frame);
	bitmap_layer_set_compositing_mode(battery_mask_layer, GCompOpSet);
	#ifndef DISABLE_MASKS
	layer_add_child(window_layer, bitmap_layer_get_layer(battery_mask_layer));
	#endif

	GRect time_frame = GRect(0, frame.size.h/2 - 22, frame.size.w, 30);
	time_layer = text_layer_init(time_frame, GTextAlignmentCenter, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_28)));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	GRect date_frame = GRect(0, time_frame.origin.y+time_frame.size.h+2, frame.size.w, 16);
	date_layer = text_layer_init(date_frame, GTextAlignmentCenter, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_REGULAR_10)));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	data_framework_register_settings_callback(main_window_settings_callback, SETTINGS_CALLBACK_MAIN_WINDOW);
	main_window_settings_callback(data_framework_get_settings());
	tick_timer_service_subscribe(MINUTE_UNIT, main_tick_handler);
}

void main_window_unload(Window *window){
	//Destroy shit
}

void main_window_init(){
	main_window = window_create();
	window_set_background_color(main_window, GColorBlack);
	window_set_window_handlers(main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
	bluetooth_connection_service_subscribe(bluetooth_handler);
	battery_state_service_subscribe(battery_handler);
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_deinit(){
	window_destroy(main_window);
}
