#include <pebble.h>
#include "lignite.h"
#include "main_window.h"
#include "graphics.h"

Window *main_window;
Settings settings;
TextLayer *time_layer, *day_layer, *date_layer;
Layer *main_graphics_layer, *window_layer;
Notification *disconnect_notification, *reconnect_notification;

void main_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
	text_layer_set_text(time_layer, time_buffer);

	static char day_buffer[] = "Wednesday";
	strftime(day_buffer, sizeof(day_buffer), "%A", t);
	text_layer_set_text(day_layer, day_buffer);

	static char date_buffer[] = "06/08/98";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);

	graphics_set_time(t);
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

	text_layer_set_text_color(time_layer, settings.time_colour);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);
}

void main_graphics_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, settings.circle_colour);
	graphics_context_set_stroke_color(ctx, GColorBlack);

	graphics_fill_circle(ctx, CENTER, MAIN_CIRCLE_RADIUS);
	for(int i = 0; i < 6; i++){
		graphics_draw_rect(ctx, GRect(i, i, WINDOW_SIZE.w-(i*2), WINDOW_SIZE.h-(i*2)));
	}
}

void main_window_load(Window *window){
	Layer *root_window_layer = window_get_root_layer(window);
	GRect window_frame = layer_get_frame(root_window_layer);
	window_layer = layer_create(window_frame);
	layer_add_child(root_window_layer, window_layer);

	graphics_init(window_layer);

	GFont font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_24));

	main_graphics_layer = layer_create(window_frame);
	layer_set_update_proc(main_graphics_layer, main_graphics_proc);
	layer_add_child(window_layer, main_graphics_layer);

	GRect time_layer_frame = GRect(0, window_frame.size.h/2 - 28, window_frame.size.w, 30);
	time_layer = text_layer_init(time_layer_frame, GTextAlignmentCenter, font);
	text_layer_set_text(time_layer, "13:00");
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	GRect day_layer_frame = GRect(0, time_layer_frame.origin.y+time_layer_frame.size.h-4, window_frame.size.w, 16);
	day_layer = text_layer_init(day_layer_frame, GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	layer_add_child(window_layer, text_layer_get_layer(day_layer));

	GRect date_layer_frame = GRect(0, day_layer_frame.origin.y+day_layer_frame.size.h-2, window_frame.size.w, 16);
	date_layer = text_layer_init(date_layer_frame, GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_14));
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
