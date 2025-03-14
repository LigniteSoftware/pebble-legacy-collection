#include <pebble.h>
#include <math.h>
#include "lignite.h"
#include "main_window.h"
#include "graphics.h"

Window *main_window;
Settings settings;

GFont impact_24, impact_16, impact_12;

TextLayer *time_layer, *day_layer, *date_layer;
Layer *window_layer;

Notification *disconnect_notification, *reconnect_notification;

void main_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(time_buffer, sizeof(time_buffer), "%H:%M", t);
	}
	else{
		strftime(time_buffer, sizeof(time_buffer), "%I:%M", t);
	}
	text_layer_set_text(time_layer, time_buffer);

	static char day_buffer[] = "Memes are dreams";
	strftime(day_buffer, sizeof(day_buffer), "%A", t);
	text_layer_set_text(day_layer, day_buffer);

	static char date_buffer[] = "Hello there mateys";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);
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
	graphics_bluetooth_handler(connected);
}

void battery_handler(BatteryChargeState state){
	graphics_battery_handler(state);
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

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);

	window_set_background_color(main_window, settings.backgroundColour);
	text_layer_set_text_color(time_layer, settings.timeColour);
	text_layer_set_text_color(day_layer, settings.timeColour);
	text_layer_set_text_color(date_layer, settings.timeColour);

	layer_set_hidden(text_layer_get_layer(date_layer), !settings.showDate);
	layer_set_hidden(text_layer_get_layer(day_layer), !settings.showDay);

	main_tick_handler(t, MINUTE_UNIT);

	graphics_set_settings(new_settings);
	graphics_animate(false);
}

void main_window_animate_layers(bool out){
	if(out){
		animate_layer(text_layer_get_layer(time_layer), &GRect(4, 2, 144, 168), &GRect(-50, 2, 144, 168), 500, 0);
		animate_layer(text_layer_get_layer(day_layer), &GRect(0, 130, 134, 168), &GRect(100, 130, 134, 168), 500, 0);
		animate_layer(text_layer_get_layer(date_layer), &GRect(0, 146, 134, 168), &GRect(100, 146, 134, 168), 500, 0);
	}
	else{
		animate_layer(text_layer_get_layer(time_layer), &GRect(-50, 2, 144, 168), &GRect(4, 2, 144, 168), 500, 0);
		animate_layer(text_layer_get_layer(day_layer), &GRect(100, 130, 134, 168), &GRect(0, 130, 134, 168), 500, 0);
		animate_layer(text_layer_get_layer(date_layer), &GRect(100, 146, 134, 168), &GRect(0, 146, 134, 168), 500, 0);
	}
}

void main_window_load(Window *window){
	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	impact_24 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_24));
	impact_16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_16));
	impact_12 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_12));

	//Create shit
	time_layer = text_layer_init(GRect(4, 2, 144, 168), GTextAlignmentLeft, impact_24);
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	date_layer = text_layer_init(GRect(0, 146, 138, 168), GTextAlignmentRight, impact_12);
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	day_layer = text_layer_init(GRect(0, 130, 138, 168), GTextAlignmentRight, impact_16);
	layer_add_child(window_layer, text_layer_get_layer(day_layer));

	graphics_create(window_layer);
	graphics_battery_handler(battery_state_service_peek());

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

	tick_timer_service_subscribe(MINUTE_UNIT, main_tick_handler);

	data_framework_register_settings_callback(main_window_settings_callback, SETTINGS_CALLBACK_MAIN_WINDOW);

	main_window_settings_callback(data_framework_get_settings());
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
