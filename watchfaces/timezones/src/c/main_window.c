#include <pebble.h>
#include <math.h>
#define M_PI 3.14159265359
#define TIME_UNIT_MINUTE 0
#define TIME_UNIT_HOUR 1
#include "lignite.h"
#include "main_window.h"

Window *main_window;
Settings settings;
TextLayer *time_layers[2], *name_layers[2], *date_layer;
Layer *circle_layer, *other_graphics_layer, *window_layer;
Notification *disconnect_notification, *reconnect_notification;
BatteryChargeState previous_state;
int edwin_time[2], philipp_time[2];

void main_tick_handler(struct tm *t, TimeUnits units){
	//Main time
	static char edwin_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(edwin_buffer, sizeof(edwin_buffer), "%H:%M", t);
	}
	else{
		strftime(edwin_buffer, sizeof(edwin_buffer), "%I:%M", t);
	}
	text_layer_set_text(time_layers[0], edwin_buffer);

	//Other timezone
	time_t current_time = time(NULL);
	current_time -= settings.alt_timezone;
	if(settings.subtract_hour){
		current_time -= 3600;
	}
	struct tm *philipp_t = localtime(&current_time);

	edwin_time[TIME_UNIT_HOUR] = t->tm_hour;
	edwin_time[TIME_UNIT_MINUTE] = t->tm_min;
	philipp_time[TIME_UNIT_HOUR] = philipp_t->tm_hour;
	philipp_time[TIME_UNIT_MINUTE] = philipp_t->tm_min;

	layer_mark_dirty(other_graphics_layer);

	static char philipp_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(philipp_buffer, sizeof(philipp_buffer), "%H:%M", philipp_t);
	}
	else{
		strftime(philipp_buffer, sizeof(philipp_buffer), "%I:%M", philipp_t);
	}
	text_layer_set_text(time_layers[1], philipp_buffer);

	static char date_buffer[] = "Philipp is 6 hours ahead of Edwin";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);
}

void bluetooth_handler(bool connected){
	if(connected && settings.btrealert){
		if(disconnect_notification->is_live){
			notification_force_dismiss(disconnect_notification);
		}
		notification_push(reconnect_notification, 5000);
		vibes_double_pulse();
	}
	else if(!connected && settings.btdisalert){
		if(reconnect_notification->is_live){
			notification_force_dismiss(reconnect_notification);
		}
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

void shake_action(AccelAxisType axis, int32_t direction){
	if(disconnect_notification->is_live){
		notification_force_dismiss(disconnect_notification);
	}
	if(reconnect_notification->is_live){
		notification_force_dismiss(reconnect_notification);
	}
}

void main_window_settings_callback(Settings new_settings){
	settings = new_settings;

	for(int i = 0; i < 2; i++){
		#ifdef PBL_COLOR
		text_layer_set_text_color(name_layers[i], settings.colour[i]);
		#endif
		text_layer_set_text(name_layers[i], settings.names[i]);
	}

	//layer_mark_dirty(circle_layer);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);
}

void battery_handlder(BatteryChargeState state){
	if(state.charge_percent != previous_state.charge_percent){
		layer_mark_dirty(other_graphics_layer);
	}
	previous_state = state;
}

void l(char *tolog){
	APP_LOG(APP_LOG_LEVEL_INFO, tolog);
}

void other_graphics_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, GRect(42, 67, 60, 2), 0, GCornerNone);

	//Battery bar
	for(int i = 0; i < previous_state.charge_percent/10; i++){
		GPoint battery_bar_points[] = {
			GPoint(14, 120), GPoint(132, 120), GPoint(24, 128), GPoint(122, 128), GPoint(35, 136),
			GPoint(111, 136), GPoint(50, 141), GPoint(96, 141), GPoint(65, 144), GPoint(81, 144)
		};
		#ifdef PBL_COLOR
		GColor color = GColorGreen;
		if(i < 2){
			color = GColorRed;
		}
		else if(i > 1 && i < 4){
			color = GColorOrange;
		}
		#else
		GColor color = GColorWhite;
		#endif
		graphics_context_set_fill_color(ctx, color);
		graphics_fill_circle(ctx, battery_bar_points[i], 2);
	}

	int radius[] = { 55, 55, 63, 63 };
	int time[] = { philipp_time[TIME_UNIT_MINUTE], philipp_time[TIME_UNIT_HOUR], edwin_time[TIME_UNIT_MINUTE], edwin_time[TIME_UNIT_HOUR] };
	int y_dist = 68, x_dist = 72;
	#ifdef PBL_COLOR
	graphics_context_set_fill_color(ctx, GColorWhite);
	#else
	graphics_context_set_fill_color(ctx, GColorBlack);
	#endif
	for(int i = 0; i < 4; i++){
		if(settings.analogue[(i < 2)]){
			int angle = (i % 2 == 0) ? (TRIG_MAX_ANGLE * time[i] / 60) : (TRIG_MAX_ANGLE * (((time[i] % 12) * 6) + (time[i-1] / 10))) / (12 * 6);
			int y = (-cos_lookup(angle) * radius[i] / TRIG_MAX_RATIO) + y_dist;
			int x = (sin_lookup(angle) * radius[i] / TRIG_MAX_RATIO) + x_dist;
			graphics_fill_circle(ctx, GPoint(x, y), 2);
		}
	}
}

void circle_proc(Layer *layer, GContext *ctx){
	#ifdef PBL_COLOR
	GPoint centre = GPoint(72, 68);
	graphics_context_set_fill_color(ctx, settings.colour[0]);
	graphics_fill_circle(ctx, centre, 66);
	graphics_context_set_fill_color(ctx, settings.colour[1]);
	graphics_fill_circle(ctx, centre, 58);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, centre, 50);
	#else
	GPoint centre = GPoint(72, 68);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, centre, 66);
	GColor second = GColorBlack;
	GColor first = GColorWhite;
	int radius = 58;
	int x_center = centre.x, y_center = centre.y;
	for(int x = x_center-radius; x < x_center+radius; x++){
		for(int y = y_center-radius; y < y_center+radius; y++){
			if((x+y)%2 == 0) graphics_context_set_stroke_color(ctx, second);
			else graphics_context_set_stroke_color(ctx, first);

			if((x-x_center) * (x-x_center) + (y-y_center) * (y-y_center) < radius * radius) graphics_draw_pixel(ctx, GPoint(x,y));
		}
	}
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, centre, 49);
	#endif
}

void main_window_load(Window *window){
	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	circle_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(circle_layer, circle_proc);
	layer_add_child(window_layer, circle_layer);

	GFont font = fonts_load_custom_font(resource_get_handle(CUSTOM_FONT));
	GFont font_small = fonts_load_custom_font(resource_get_handle(CUSTOM_FONT_SMALL));

	for(int i = 0; i < 2; i++){
		time_layers[i] = text_layer_init(GRect(0, 36+(i*44), 144, 168), GTextAlignmentCenter, font);
		name_layers[i] = text_layer_init(GRect(0, 26+(i*44), 144, 168), GTextAlignmentCenter, font_small);
		text_layer_set_text(time_layers[i], "00:00");
		text_layer_set_text(name_layers[i], settings.names[i]);
		layer_add_child(window_layer, text_layer_get_layer(time_layers[i]));
		layer_add_child(window_layer, text_layer_get_layer(name_layers[i]));
	}

	text_layer_set_font(time_layers[0], fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HTT_30)));
	layer_set_frame(text_layer_get_layer(time_layers[0]), GRect(0, 32, 144, 168));
	layer_set_frame(text_layer_get_layer(name_layers[0]), GRect(0, 22, 144, 168));

	date_layer = text_layer_init(GRect(0, 148, 144, 168), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	other_graphics_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(other_graphics_layer, other_graphics_proc);
	layer_add_child(window_layer, other_graphics_layer);

	//Everything else
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
	settings.btrealert = true;
	settings.btdisalert = true;
	main_window_settings_callback(settings);

	battery_handlder(battery_state_service_peek());
}

void main_window_unload(Window *window){
	notification_destroy(reconnect_notification);
	notification_destroy(disconnect_notification);
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
	battery_state_service_subscribe(battery_handlder);
	accel_tap_service_subscribe(shake_action);
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_deinit(){
	window_destroy(main_window);
}
