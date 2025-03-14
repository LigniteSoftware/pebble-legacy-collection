#include <pebble.h>
#include "lignite.h"
#include "main_window.h"

Window *main_window;
Layer *window_layer, *graphics_layer;
Settings settings;

GFont square_deal_large, square_deal_small;
TextLayer *hour_layer, *minute_layer, *day_layer, *date_day_layer, *date_month_layer;

Notification *disconnect_notification, *reconnect_notification;

BatteryChargeState battery_charge_state;
AppTimer *battery_animation_timer;

void main_tick_handler(struct tm *t, TimeUnits units){
	static char hour_buffer[] = "69";
	strftime(hour_buffer, sizeof(hour_buffer), clock_is_24h_style() ? "%H" : "%I", t);
	text_layer_set_text(hour_layer, hour_buffer);

	static char minute_buffer[] = "42";
	strftime(minute_buffer, sizeof(minute_buffer), "%M", t);
	text_layer_set_text(minute_layer, minute_buffer);

	static char day_buffer[] = "Fri";
	strftime(day_buffer, sizeof(day_buffer), "%a", t);
	text_layer_set_text(day_layer, day_buffer);

	static char date_day_buffer[] = "Fri";
	strftime(date_day_buffer, sizeof(date_day_buffer), "%d", t);
	text_layer_set_text(date_day_layer, date_day_buffer);

	static char date_month_buffer[] = "06";
	strftime(date_month_buffer, sizeof(date_month_buffer), "%m", t);
	text_layer_set_text(date_month_layer, date_month_buffer);
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

void battery_animation_handler(){
	if(battery_charge_state.is_charging){
		battery_animation_timer = app_timer_register(400, battery_animation_handler, NULL);
	}
	battery_charge_state.charge_percent += 10;
	if(battery_charge_state.charge_percent > 100){
		battery_charge_state.charge_percent = 10;
	}
	layer_mark_dirty(graphics_layer);
}

void battery_handler(BatteryChargeState new_state){
	if(new_state.is_charging && !battery_charge_state.is_charging){
		battery_animation_timer = app_timer_register(200, battery_animation_handler, NULL);
	}
	else{
		if(battery_animation_timer)
			app_timer_cancel(battery_animation_timer);
	}
	battery_charge_state = new_state;
	layer_mark_dirty(graphics_layer);
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
	text_layer_set_text_color(hour_layer, settings.hourColour);
	text_layer_set_text_color(minute_layer, settings.minuteColour);
	text_layer_set_text_color(day_layer, settings.dayColour);
	text_layer_set_text_color(date_day_layer, settings.dateColour);
	text_layer_set_text_color(date_month_layer, settings.dateColour);
	window_set_background_color(main_window, new_settings.backgroundColour);
	#endif

	layer_set_hidden(text_layer_get_layer(day_layer), !settings.day);
	layer_set_hidden(text_layer_get_layer(date_day_layer), !settings.date);
	layer_set_hidden(text_layer_get_layer(date_month_layer), !settings.date);

	if(!settings.day && !settings.date){
		//Set center aligned
	}

	layer_mark_dirty(graphics_layer);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
	main_tick_handler(t, MINUTE_UNIT);
}

void graphics_proc(Layer *layer, GContext *ctx){
	if(!settings.batteryBar){
		return;
	}
	graphics_context_set_fill_color(ctx, settings.batteryBarColour);

	GRect layer_frame = layer_get_frame(layer);

	int boxSize = 6;
	GPoint start = GPoint(14, layer_frame.size.h/2 - boxSize/2);
	GPoint end = GPoint(layer_frame.size.w - 14, start.y);
	int distanceBetweenEachBox = ((end.x-start.x)-(boxSize*10))/10;
	for(int i = 0; i < battery_charge_state.charge_percent/10; i++){
		graphics_fill_rect(ctx, GRect(start.x + (i*(boxSize+distanceBetweenEachBox+1)), start.y, boxSize, boxSize), 0, GCornerNone);
	}
}

void main_window_load(Window *window){
	GRect frame = layer_get_frame(window_get_root_layer(window));
	window_layer = layer_create(frame);
	layer_add_child(window_get_root_layer(window), window_layer);

	setlocale(LC_ALL, "");

	square_deal_large = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_DEAL_60));
	square_deal_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_DEAL_22));

	GRect hour_frame = GRect(14, 0, 62, 62);
	hour_layer = text_layer_init(hour_frame, GTextAlignmentLeft, square_deal_large);
	text_layer_set_text(hour_layer, "00");
	layer_add_child(window_layer, text_layer_get_layer(hour_layer));

	GRect minute_frame = GRect(14, frame.size.h-76, 62, 62);
	minute_layer = text_layer_init(minute_frame, GTextAlignmentLeft, square_deal_large);
	text_layer_set_text(minute_layer, "00");
	layer_add_child(window_layer, text_layer_get_layer(minute_layer));

	GRect day_frame = GRect(14, 10, frame.size.w-28, 24);
	day_layer = text_layer_init(day_frame, GTextAlignmentRight, square_deal_small);
	text_layer_set_text(day_layer, "Wed");
	#ifndef PBL_ROUND
	layer_add_child(window_layer, text_layer_get_layer(day_layer));
	#endif

	GRect date_day_frame = GRect(14, minute_frame.origin.y+8, frame.size.w-28, 24);
	date_day_layer = text_layer_init(date_day_frame, GTextAlignmentRight, square_deal_small);
	text_layer_set_text(date_day_layer, "00");
	layer_add_child(window_layer, text_layer_get_layer(date_day_layer));

	GRect date_month_frame = GRect(14, minute_frame.origin.y+minute_frame.size.h-22, frame.size.w-28, 22);
	date_month_layer = text_layer_init(date_month_frame, GTextAlignmentRight, square_deal_small);
	text_layer_set_text(date_month_layer, "00");
	layer_add_child(window_layer, text_layer_get_layer(date_month_layer));

	#ifdef PBL_ROUND
	GRect newHourFrame = GRect(0, hour_frame.origin.y+10, frame.size.w, hour_frame.size.h);
	layer_set_frame(text_layer_get_layer(hour_layer), newHourFrame);
	text_layer_set_text_alignment(hour_layer, GTextAlignmentCenter);

	GRect newMinuteFrame = GRect(0, minute_frame.origin.y-10, frame.size.w, minute_frame.size.h);
	layer_set_frame(text_layer_get_layer(minute_layer), newMinuteFrame);
	text_layer_set_text_alignment(minute_layer, GTextAlignmentCenter);

	GRect newDayFrame = GRect(newHourFrame.size.w/2 + 28, newHourFrame.origin.y+newHourFrame.size.h/2 - 10, frame.size.w, day_frame.size.h);
	layer_set_frame(text_layer_get_layer(day_layer), newDayFrame);
	text_layer_set_text_alignment(day_layer, GTextAlignmentLeft);

	GRect newDateDayFrame = GRect(newMinuteFrame.size.w/2 + 32, newMinuteFrame.origin.y+newMinuteFrame.size.h/2 - 10, frame.size.w, date_day_frame.size.h);
	layer_set_frame(text_layer_get_layer(date_day_layer), newDateDayFrame);
	text_layer_set_text_alignment(date_day_layer, GTextAlignmentLeft);

	GRect newDateMonthFrame = GRect(0, newDateDayFrame.origin.y, newMinuteFrame.size.w/2 - 36, date_day_frame.size.h);
	layer_set_frame(text_layer_get_layer(date_month_layer), newDateMonthFrame);
	text_layer_set_text_alignment(date_month_layer, GTextAlignmentRight);
	#endif

	graphics_layer = layer_create(frame);
	layer_set_update_proc(graphics_layer, graphics_proc);
	layer_add_child(window_layer, graphics_layer);

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

	battery_handler(battery_state_service_peek());
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
