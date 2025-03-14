#include <pebble.h>
#include "lignite.h"
#include "main_window.h"
#include "graphics.h"

Window *main_window;
Layer *window_layer;
Settings settings;

TextLayer *day_layer, *date_layer;
GFont bebas_neue_normal, bebas_neue_small;

//#define DEBUG_12_HOUR_CLOCK
//#define DEBUG_24_HOUR_CLOCK

Notification *disconnect_notification, *reconnect_notification;

#ifdef DEBUG_12_HOUR_CLOCK
#define clock_is_24h_style() false
#endif

#ifdef DEBUG_24_HOUR_CLOCK
#define clock_is_24h_style() true
#endif

void main_tick_handler(struct tm *t, TimeUnits units){
	if(!settings.timeInstead){
		static char day_buffer[] = "Day";
		strftime(day_buffer, sizeof(day_buffer), "%a", t);
		text_layer_set_text(day_layer, day_buffer);
	}
	else{
		static char time_buffer[] = "00:00";
		strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
		text_layer_set_text(day_layer, time_buffer);
	}

	static char date_buffer[] = "date date date date";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);

	graphics_tick_handler(t);
}

void bluetooth_handler(bool connected){
	if(connected && settings.btrealert){
		vibes_double_pulse();
	}
	else if(!connected && settings.btdisalert){
		vibes_long_pulse();
	}
	graphics_bluetooth_handler(connected);
}

void notification_push_handler(bool pushed, uint8_t id){
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

void main_window_settings_callback(Settings new_settings){
	settings = new_settings;
	#ifndef PBL_ROUND
	GRect frame = layer_get_frame(window_get_root_layer(main_window));
	#endif

	if(!settings.timeInstead){
		layer_set_hidden(text_layer_get_layer(day_layer), !settings.day);
		#ifndef PBL_ROUND
		int fontHeight = 22;
		text_layer_set_font(day_layer, bebas_neue_normal);
		layer_set_frame(text_layer_get_layer(day_layer), GRect(0, frame.size.h/2 - fontHeight + 4, frame.size.w, fontHeight+2));
		#endif
	}
	else{
		layer_set_hidden(text_layer_get_layer(day_layer), false);
		#ifndef PBL_ROUND
		int fontHeight = 18;
		text_layer_set_font(day_layer, bebas_neue_small);
		layer_set_frame(text_layer_get_layer(day_layer), GRect(0, frame.size.h/2 - fontHeight + 4, frame.size.w, fontHeight+2));
		#endif
	}
	layer_set_hidden(text_layer_get_layer(date_layer), !settings.date);

	#ifdef PBL_COLOR
	text_layer_set_text_color(day_layer, settings.dayDateColour);
	text_layer_set_text_color(date_layer, settings.dayDateColour);
	window_set_background_color(main_window, settings.backgroundColour);
	#endif

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

	bebas_neue_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_BOLD_16));
	bebas_neue_normal = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_BOLD_20));

	graphics_init(window_layer);

	int fontHeight = 22;
	day_layer = text_layer_init(GRect(0, frame.size.h/2 - fontHeight + 4, frame.size.w, fontHeight+2), GTextAlignmentCenter, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_BOLD_20)));
	layer_add_child(window_layer, text_layer_get_layer(day_layer));

	date_layer = text_layer_init(GRect(0, frame.size.h/2 + 4, frame.size.w, 16), GTextAlignmentCenter, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_REGULAR_10)));
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
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_deinit(){
	window_destroy(main_window);
}
