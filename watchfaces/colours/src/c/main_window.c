#include <pebble.h>
#include "lignite.h"
#include "main_window.h"
#include "graphics.h"

Window *main_window;
Settings settings;
TextLayer *minute_layer, *hour_layer;
TextLayer *minute_outline, *hour_outline;
Layer *window_layer;
Notification *disconnect_notification, *reconnect_notification;
bool boot = true;

void main_tick_handler(struct tm *t, TimeUnits units){
	static char hour_buffer[] = "00";
	if(clock_is_24h_style()){
		strftime(hour_buffer, sizeof(hour_buffer), "%I", t);
	}
	else{
		strftime(hour_buffer, sizeof(hour_buffer), "%H", t);
	}
	text_layer_set_text(hour_layer, hour_buffer);
	text_layer_set_text(hour_outline, hour_buffer);

	static char minute_buffer[] = "00";
	strftime(minute_buffer, sizeof(minute_buffer), "%M", t);
	text_layer_set_text(minute_layer, minute_buffer);
	text_layer_set_text(minute_outline, minute_buffer);
}

void bluetooth_handler(bool connected){
	if(connected && settings.btrealert){
		notification_push(reconnect_notification, 5000);
		vibes_double_pulse();
	}
	else if(!connected && settings.btdisalert){
		notification_push(disconnect_notification, 5000);
		vibes_long_pulse();
	}
}

void main_window_settings_callback(Settings new_settings){
	settings = new_settings;

	text_layer_set_text_color(minute_layer, settings.colour_minute);
	text_layer_set_text_color(hour_layer, settings.colour_hour);
	text_layer_set_background_color(minute_layer, GColorClear);
	text_layer_set_background_color(hour_layer, GColorClear);

	layer_set_hidden(text_layer_get_layer(minute_outline), !settings.outline_font);
	layer_set_hidden(text_layer_get_layer(hour_outline), !settings.outline_font);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);

  	if(!boot){
  		vibes_double_pulse();
  	}
  	boot = false;
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

void main_window_load(Window *window){
	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	graphics_init(window_layer, window);

	GRect hour_grect = GRect(8, 14, 144, 168);
	GRect minute_grect = GRect(5, 70, 144, 168);

	GFont font1 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COLOURS_62));

	hour_outline = text_layer_init(hour_grect, GTextAlignmentCenter, font1);
	text_layer_set_text_color(hour_outline, GColorBlack);
	layer_add_child(window_layer, text_layer_get_layer(hour_outline));

	minute_outline = text_layer_init(minute_grect, GTextAlignmentCenter, font1);
	text_layer_set_text_color(minute_outline, GColorBlack);
	layer_add_child(window_layer, text_layer_get_layer(minute_outline));

	GFont font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COLOURS_60));

	hour_layer = text_layer_init(hour_grect, GTextAlignmentCenter, font);
	layer_add_child(window_layer, text_layer_get_layer(hour_layer));

	minute_layer = text_layer_init(minute_grect, GTextAlignmentCenter, font);
	layer_add_child(window_layer, text_layer_get_layer(minute_layer));

	GBitmap *bluetoothIcon = gbitmap_create_with_resource(RESOURCE_ID_LIGNITE_IMAGE_BLUETOOTH_ICON);

    disconnect_notification = notification_create(window);
    notification_set_icon(disconnect_notification, bluetoothIcon);
    notification_set_accent_colour(disconnect_notification, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
    notification_set_contents(disconnect_notification, "Oh boy...", "I lost connection to\nyour phone, sorry!");

    reconnect_notification = notification_create(window);
    notification_set_icon(reconnect_notification, bluetoothIcon);
    notification_set_accent_colour(reconnect_notification, PBL_IF_COLOR_ELSE(GColorBlue, GColorWhite));
    notification_set_contents(reconnect_notification, "Woohoo!", "You are now\nconnected to your\nphone again!");

    notification_register_push_handler(notification_push_handler);

	data_framework_register_settings_callback(main_window_settings_callback, SETTINGS_CALLBACK_MAIN_WINDOW);

	settings = data_framework_get_settings();
	main_window_settings_callback(settings);
}

void main_window_unload(Window *window){
	graphics_deinit();
}

void main_window_init(){
	main_window = window_create();
	window_set_background_color(main_window, GColorBlack);
	window_set_window_handlers(main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
	tick_timer_service_subscribe(MINUTE_UNIT, main_tick_handler);
	bluetooth_connection_service_subscribe(bluetooth_handler);
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_deinit(){
	window_destroy(main_window);
}
