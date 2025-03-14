#include <pebble.h>
#include <math.h>
#include "lignite.h"
#include "main_window.h"
#include "graphics.h"

Window *main_window;
Settings settings;
TextLayer *time_layer, *date_layer, *battery_charge_layer;
GFont visitor_42, visitor_50;

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

	static char date_buffer[] = "Hello there!";
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
	text_layer_set_text(date_layer, connected ? "Yes" : "No");
	graphics_bluetooth_handler(connected);
}

void battery_handler(BatteryChargeState state){
	graphics_battery_handler(state);

	static char battery_buffer[] = "100%";
	snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", state.charge_percent);
	text_layer_set_text(battery_charge_layer, battery_buffer);
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
	text_layer_set_text_color(battery_charge_layer, settings.timeColour);
	text_layer_set_text_color(date_layer, settings.timeColour);

	layer_set_hidden(text_layer_get_layer(date_layer), !settings.showDate);
	layer_set_hidden(text_layer_get_layer(battery_charge_layer), !settings.showBattery);

	if(settings.showBattery && !settings.showDate){
		layer_set_frame(text_layer_get_layer(battery_charge_layer), GRect(0, 136, 144, 168));
		text_layer_set_font(time_layer, visitor_42);
		layer_set_frame(text_layer_get_layer(time_layer), GRect(0, 88, 144, 168));
	}
	else if(!settings.showBattery && settings.showDate){
		layer_set_frame(text_layer_get_layer(date_layer), GRect(0, 136, 144, 168));
		text_layer_set_font(time_layer, visitor_42);
		layer_set_frame(text_layer_get_layer(time_layer), GRect(0, 88, 144, 168));
	}
	else if(!settings.showBattery && !settings.showDate){
		text_layer_set_font(time_layer, visitor_50);
		layer_set_frame(text_layer_get_layer(time_layer), GRect(0, 96, 144, 168));
	}
	else{
		layer_set_frame(text_layer_get_layer(date_layer), GRect(14, 136, 72, 168));
		layer_set_frame(text_layer_get_layer(battery_charge_layer), GRect(74, 136, 64, 168));
		text_layer_set_font(time_layer, visitor_42);
		layer_set_frame(text_layer_get_layer(time_layer), GRect(0, 88, 144, 168));
	}

	main_tick_handler(t, MINUTE_UNIT);

	graphics_set_settings(new_settings);
	graphics_animate();
}

void main_window_load(Window *window){
	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	visitor_42 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VISITOR_42));
	visitor_50 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VISITOR_50));

	//Create shit
	time_layer = text_layer_init(GRect(0, 88, 144, 168), GTextAlignmentCenter, visitor_42);
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	date_layer = text_layer_init(GRect(14, 136, 72, 168), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	battery_charge_layer = text_layer_init(GRect(74, 136, 64, 168), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text(battery_charge_layer, "40%");
	layer_add_child(window_layer, text_layer_get_layer(battery_charge_layer));

	BatteryChargeState state = battery_state_service_peek();
	static char battery_buffer[] = "100%";
	snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", state.charge_percent);
	text_layer_set_text(battery_charge_layer, battery_buffer);

	graphics_create(window_layer);

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
