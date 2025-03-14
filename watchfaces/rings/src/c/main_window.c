#include <pebble.h>
#include "lignite.h"
#include "main_window.h"
#include "graphics.h"

Window *main_window;
Layer *window_layer;
Settings settings;

TextLayer *date_layer, *time_layer;
GFont impact_normal;

Notification *disconnect_notification, *reconnect_notification;

bool animatingAlready = false;

void main_tick_handler(struct tm *t, TimeUnits units){
	static char minute_buffer[] = "00";
	strftime(minute_buffer, sizeof(minute_buffer), "%M", t);
	static char time_buffer[] = "00:00";
	snprintf(time_buffer, sizeof(time_buffer), "%d:%s", clock_is_24h_style() ? t->tm_hour : t->tm_hour % 12, minute_buffer);
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "Hello there my friend";
	strftime(date_buffer, sizeof(date_buffer), settings.weekNumber ? "w. %V - %a %b %d" : "%a %b %d", t);
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

#ifdef PBL_ROUND
void done_animating_callback(){
	animatingAlready = false;
}

void chalk_show_time_handler(AccelAxisType axis, int32_t direction){
	if(animatingAlready){
		return;
	}
	animatingAlready = true;

	app_timer_register(3600, done_animating_callback, NULL);

	Layer *raw_time_layer = text_layer_get_layer(time_layer);
	Layer *raw_date_layer = text_layer_get_layer(date_layer);

	GRect masterFrame = layer_get_frame(window_get_root_layer(main_window));

	GRect currentTimeFrame = layer_get_frame(raw_time_layer);
	GRect currentDateFrame = layer_get_frame(raw_date_layer);
	GRect finalTimeFrame = GRect(0, 116, masterFrame.size.w, 28);
	GRect finalDateFrame = GRect(0, finalTimeFrame.origin.y+finalTimeFrame.size.h-4, masterFrame.size.w, 18);

	animate_layer(raw_time_layer, &currentTimeFrame, &finalTimeFrame, 600, 0);
	animate_layer(raw_date_layer, &currentDateFrame, &finalDateFrame, 600, 0);
	animate_layer(raw_time_layer, &finalTimeFrame, &GRect(0, 300, currentTimeFrame.size.w, currentTimeFrame.size.h), 600, 3000);
	animate_layer(raw_date_layer, &finalDateFrame, &GRect(0, 300, currentDateFrame.size.w, currentDateFrame.size.h), 600, 3000);

	graphics_shake_handler();
}
#endif

void main_window_settings_callback(Settings new_settings){
	settings = new_settings;

	#ifdef PBL_COLOR
	text_layer_set_text_color(time_layer, settings.textColour);
	text_layer_set_text_color(date_layer, settings.textColour);
	window_set_background_color(main_window, settings.backgroundColour);
	#endif

	tick_timer_service_subscribe(settings.secondRadial ? SECOND_UNIT : MINUTE_UNIT, main_tick_handler);

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

	impact_normal = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_18));

	GRect time_frame = GRect(4, frame.size.h-24, frame.size.w-8, 28);
	time_layer = text_layer_init(time_frame, GTextAlignmentLeft, impact_normal);
	text_layer_set_background_color(time_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	GRect date_frame = GRect(4, frame.size.h-22, frame.size.w-8, 18);
	date_layer = text_layer_init(date_frame, GTextAlignmentRight, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_background_color(date_layer, GColorClear);
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

	#ifdef PBL_ROUND
	accel_tap_service_subscribe(chalk_show_time_handler);

	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);

	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));

	layer_set_frame(text_layer_get_layer(time_layer), GRect(0, 220, frame.size.w, 20));
	layer_set_frame(text_layer_get_layer(date_layer), GRect(0, 220, frame.size.w, 20));
	#endif
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
