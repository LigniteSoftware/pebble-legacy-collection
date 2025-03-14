#include <pebble.h>
#include "lignite.h"
#include "main_window.h"
#include "graphics.h"

Window *main_window;
Layer *window_layer;
Settings settings;

TextLayer *date_layer, *time_layer;
GFont impact_normal, squared_small;

Notification *disconnect_notification, *reconnect_notification;

bool animatingAlready = false;

void main_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "Hello there mate";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);

	graphics_tick_handler(t);
}

void bluetooth_handler(bool connected){
	if(connected && settings.btrealert){
		if(disconnect_notification->is_live){
			notification_force_dismiss(disconnect_notification);
		}
		notification_push(reconnect_notification, 10000);
		vibes_double_pulse();
	}
	else if(!connected && settings.btdisalert){
		if(reconnect_notification->is_live){
			notification_force_dismiss(reconnect_notification);
		}
		notification_push(disconnect_notification, 10000);
		vibes_long_pulse();
	}
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

	#ifdef PBL_COLOR
	text_layer_set_text_color(time_layer, settings.textColour);
	text_layer_set_text_color(date_layer, settings.textColour);
	window_set_background_color(main_window, settings.backgroundColour);
	#endif

	layer_set_hidden(text_layer_get_layer(time_layer), !settings.topTime);
	layer_set_hidden(text_layer_get_layer(date_layer), !settings.date);

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

	graphics_create(window);

	impact_normal = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_14));

	GRect time_frame = GRect(18, 2, frame.size.w-28, 28);
	time_layer = text_layer_init(time_frame, GTextAlignmentLeft, impact_normal);
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	GRect date_frame = GRect(18, time_frame.origin.y, frame.size.w-36, 18);
	date_layer = text_layer_init(date_frame, GTextAlignmentRight, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

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
