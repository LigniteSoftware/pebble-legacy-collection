#include <pebble.h>
#include <math.h>
#include "lignite.h"
#include "main_window.h"

Window *main_window;
Settings settings;
TextLayer *time_layer, *date_layer, *week_day_layer;
Layer *window_layer, *main_graphics_layer, *battery_graphics_layer;
Notification *disconnect_notification, *reconnect_notification;
BatteryChargeState batteryState;
int time_array[2];

void main_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(time_buffer, sizeof(time_buffer), "%H:%M", t);
	}
	else{
		strftime(time_buffer, sizeof(time_buffer), "%I:%M", t);
	}
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "Get rickity rect in front of what";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);

	static char week_day_buffer[] = "Wednesday sucks";
	strftime(week_day_buffer, sizeof(week_day_buffer), "%A", t);
	text_layer_set_text(week_day_layer, week_day_buffer);

	time_array[0] = t->tm_min;
	time_array[1] = t->tm_hour;

	layer_mark_dirty(main_graphics_layer);
	layer_mark_dirty(battery_graphics_layer);
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

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);

	text_layer_set_text_color(time_layer, settings.timeColour);
	text_layer_set_text_color(date_layer, settings.dateColour);
	text_layer_set_text_color(week_day_layer, settings.dayColour);
	window_set_background_color(main_window, settings.backgroundColour);

	layer_set_hidden(text_layer_get_layer(week_day_layer), !settings.weekday);
	layer_set_hidden(text_layer_get_layer(date_layer), !settings.advancedDate);
	layer_set_hidden(text_layer_get_layer(time_layer), !settings.digitalTime);
}

void main_graphics_proc(Layer *layer, GContext *ctx){
	#ifdef PBL_COLOR
	graphics_context_set_stroke_color(ctx, gcolor_legible_over(settings.backgroundColour));
		graphics_context_set_antialiased(ctx, false);
	#else
	graphics_context_set_stroke_color(ctx, GColorWhite);
	#endif
	graphics_draw_circle(ctx, GPoint(72, 70), 64);
	graphics_draw_circle(ctx, GPoint(72, 70), 63);

	//Analogue hands
	int radius[] = { 56, 56 };
	int time[] = { time_array[0], time_array[1] };
	int y_dist = 70, x_dist = 72;
	int size[] = { 5, 3 };
	graphics_context_set_fill_color(ctx, GColorWhite);
	for(int i = 0; i < 2; i++){
		int angle = (i % 2 == 0) ? (TRIG_MAX_ANGLE * time[i] / 60) : (TRIG_MAX_ANGLE * (((time[i] % 12) * 6) + (time[i-1] / 10))) / (12 * 6);
		int y = (-cos_lookup(angle) * radius[i] / TRIG_MAX_RATIO) + y_dist;
		int x = (sin_lookup(angle) * radius[i] / TRIG_MAX_RATIO) + x_dist;
		#ifdef PBL_COLOR
		if(i == 0){
			graphics_context_set_fill_color(ctx, settings.minuteColour);
		}
		else{
			graphics_context_set_fill_color(ctx, settings.hourColour);
		}
		#endif
		if(i == 0 && settings.minuteAnalogue){
			graphics_fill_circle(ctx, GPoint(x, y), size[i]);
		}
		if(i == 1 && settings.hourAnalogue){
			graphics_fill_circle(ctx, GPoint(x, y), size[i]);
		}
	}
}

void battery_graphics_proc(Layer *layer, GContext *ctx){
	#ifdef PBL_COLOR
	graphics_context_set_fill_color(ctx, settings.batteryBarColour);
	#else
	graphics_context_set_fill_color(ctx, GColorWhite);
	#endif

	batteryState = battery_state_service_peek();
	for(int i = 0; i < batteryState.charge_percent/10; i++){
		graphics_fill_rect(ctx, GRect(7+(i*13), 150, 12, 6), 1, GCornersAll);
	}
}

void battery_handler(BatteryChargeState state){
	batteryState = state;
	layer_mark_dirty(battery_graphics_layer);
}

void main_window_load(Window *window){
	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	main_graphics_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(main_graphics_layer, main_graphics_proc);
	layer_add_child(window_layer, main_graphics_layer);

	battery_graphics_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(battery_graphics_layer, battery_graphics_proc);
	layer_add_child(window_layer, battery_graphics_layer);

	week_day_layer = text_layer_init(GRect(0, 40, 144, 168), GTextAlignmentCenter, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_20)));
	layer_add_child(window_layer, text_layer_get_layer(week_day_layer));

	date_layer = text_layer_init(GRect(0, 66, 144, 168), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	time_layer = text_layer_init(GRect(0, 86, 144, 168), GTextAlignmentCenter, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_16)));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	tick_timer_service_subscribe(MINUTE_UNIT, main_tick_handler);

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
}

void main_window_unload(Window *window){
	//Maybe later
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
