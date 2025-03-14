#include <pebble.h>
#include "lignite.h"
#include "main_window.h"

Window *main_window;
TextLayer *date_layers[DATE_LAYERS], *battery_text_layer, *percent_layer;
BitmapLayer *time_layers[TIME_LAYERS];
GBitmap *numbers[10];
GFont squared_20, squared_16;
Layer *lines_layer, *animation_layer, *window_layer, *extra_layer, *adjust_layer, *battery_layer;
Settings settings;
Notification *disconnect_notification, *reconnect_notification;
BatteryChargeState charge_state;
bool boot = true;

void main_tick_handler(struct tm *t, TimeUnits units){
	int8_t minute_dec = t->tm_min / 10, minute_sin = t->tm_min % 10;
	int8_t hour_dec = t->tm_hour / 10, hour_sin = t->tm_hour % 10;

	if(!clock_is_24h_style()){
		int fixed_hour = t->tm_hour;
		if(fixed_hour > 12){
			fixed_hour -= 12;
		}
		hour_dec = fixed_hour / 10;
		if(hour_dec == 0){
			hour_dec = -1;
		}
		hour_sin = fixed_hour % 10;
	}

	bitmap_layer_set_bitmap(time_layers[TIME_LAYER_MINUTE_1], numbers[minute_dec]);
	bitmap_layer_set_bitmap(time_layers[TIME_LAYER_MINUTE_2], numbers[minute_sin]);
	bitmap_layer_set_bitmap(time_layers[TIME_LAYER_HOUR_1], hour_dec < 0 ? NULL : numbers[hour_dec]);
	bitmap_layer_set_bitmap(time_layers[TIME_LAYER_HOUR_2], numbers[hour_sin]);

	static char time_buffer_d1[] = "00";
	strftime(time_buffer_d1, sizeof(time_buffer_d1), "%d", t);

	static char time_buffer_d2[] = "00";
	strftime(time_buffer_d2, sizeof(time_buffer_d2), "%m", t);

	static char time_buffer_d3[] = "00";
	strftime(time_buffer_d3, sizeof(time_buffer_d3), "%y", t);

	text_layer_set_text(date_layers[DATE_LAYER_DAY_1], settings.alt_date_format ? time_buffer_d1 : time_buffer_d2);
	text_layer_set_text(date_layers[DATE_LAYER_DAY_2], settings.alt_date_format ? time_buffer_d2 : time_buffer_d1);
	text_layer_set_text(date_layers[DATE_LAYER_DAY_3], time_buffer_d3);
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

void battery_handler(BatteryChargeState state){
	charge_state = state;
	layer_mark_dirty(lines_layer);
	static char battery_buffer[] = "Joelle"; //Is beautiful
	snprintf(battery_buffer, sizeof(battery_buffer), "%d", state.charge_percent);
	text_layer_set_text(battery_text_layer, battery_buffer);
}

bool battery_mode_shows_text(){
	return (settings.battery_mode % 2 == 0);
}

void main_window_update_settings(Settings new_settings){
  	settings = new_settings;

  	text_layer_set_text_color(date_layers[DATE_LAYER_DAY_1], settings.date_colour);
  	text_layer_set_text_color(date_layers[DATE_LAYER_DAY_2], settings.date_colour);
  	text_layer_set_text_color(date_layers[DATE_LAYER_DAY_3], settings.date_colour);

	GRect date_rects[] = { GRect(30, 136, 30, 24), GRect(62, 136, 22, 24), GRect(86, 136, 30, 24) };
	GRect alt_date_rects[] = { GRect(28, 136, 30, 24), GRect(48, 136, 22, 24), GRect(58, 136, 30, 24) };
	for(int i = 0; i < DATE_LAYERS; i++){
		text_layer_set_font(date_layers[i], (new_settings.battery_mode < 2) ? squared_20 : squared_16);
		layer_set_frame(text_layer_get_layer(date_layers[i]), (new_settings.battery_mode < 2) ? date_rects[i] : alt_date_rects[i]);
	}
	layer_set_hidden(text_layer_get_layer(battery_text_layer), (new_settings.battery_mode < 2));
	layer_set_hidden(text_layer_get_layer(percent_layer), (new_settings.battery_mode < 2));
	text_layer_set_text_alignment(date_layers[2], (new_settings.battery_mode < 2) ? GTextAlignmentCenter : GTextAlignmentRight);
	layer_set_frame(extra_layer, (new_settings.battery_mode < 2) ? GRect(0, 0, 144, 168) : GRect(-6, 0, 144, 168));
	layer_mark_dirty(lines_layer);

	layer_set_frame(text_layer_get_layer(battery_text_layer), !settings.show_date ? GRect(60, 136, 60, 24) : GRect(100, 136, 60, 24));
	layer_set_frame(text_layer_get_layer(percent_layer), !settings.show_date ? GRect(74, 137, 20, 24) : GRect(114, 137, 20, 24));
	layer_set_frame(adjust_layer, (settings.battery_mode == 1 || settings.battery_mode == 3) ? GRect(0, -4, 144, 168) : GRect(0, 0, 144, 168));

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);

  	layer_set_hidden(text_layer_get_layer(date_layers[DATE_LAYER_DAY_1]), !settings.show_date);
  	layer_set_hidden(text_layer_get_layer(date_layers[DATE_LAYER_DAY_2]), !settings.show_date);
  	layer_set_hidden(text_layer_get_layer(date_layers[DATE_LAYER_DAY_3]), !settings.show_date);

  	if(!boot){
  		vibes_double_pulse();
  	}
  	else{
  		boot = false;
  	}
}

void lines_proc(Layer *layer, GContext *ctx){
	graphics_context_set_stroke_color(ctx, settings.date_colour);
	if(settings.show_date){
		if(settings.battery_mode < 2){
			graphics_draw_line(ctx, GPoint(56, 140), GPoint(56, 158));
			graphics_draw_line(ctx, GPoint(88, 140), GPoint(88, 158));
		}
		else{
			graphics_draw_line(ctx, GPoint(46, 140), GPoint(46, 154));
			graphics_draw_line(ctx, GPoint(70, 140), GPoint(70, 154));
			//graphics_draw_line(ctx, GPoint(94, 140), GPoint(94, 154));
		}
	}
}

void battery_proc(Layer *layer, GContext *ctx){
	if(settings.battery_mode % 2 != 0){
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, GRect(-4, 166, 14.4*(charge_state.charge_percent/10)+4, 4), 0, GCornerNone);
	}
}

void animation_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorBlack);
	GRect frame = layer_get_frame(layer);
	graphics_fill_rect(ctx, frame, 0, GCornerNone);
}

void main_window_load(Window *window){
	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	#ifdef PBL_COLOR
	uint32_t number_resources[] = {
		RESOURCE_ID_NUMBER_0,
		RESOURCE_ID_NUMBER_1,
		RESOURCE_ID_NUMBER_2,
		RESOURCE_ID_NUMBER_3,
		RESOURCE_ID_NUMBER_4,
		RESOURCE_ID_NUMBER_5,
		RESOURCE_ID_NUMBER_6,
		RESOURCE_ID_NUMBER_7,
		RESOURCE_ID_NUMBER_8,
		RESOURCE_ID_NUMBER_9
	};
	#else
	uint32_t number_resources[] = {
		RESOURCE_ID_NUMBER_0_APLITE_WHITE,
		RESOURCE_ID_NUMBER_1_APLITE_WHITE,
		RESOURCE_ID_NUMBER_2_APLITE_WHITE,
		RESOURCE_ID_NUMBER_3_APLITE_WHITE,
		RESOURCE_ID_NUMBER_4_APLITE_WHITE,
		RESOURCE_ID_NUMBER_5_APLITE_WHITE,
		RESOURCE_ID_NUMBER_6_APLITE_WHITE,
		RESOURCE_ID_NUMBER_7_APLITE_WHITE,
		RESOURCE_ID_NUMBER_8_APLITE_WHITE,
		RESOURCE_ID_NUMBER_9_APLITE_WHITE
	};
	#endif
	/*

	*/

	for(int i = 0; i < 10; i++){
		numbers[i] = gbitmap_create_with_resource(number_resources[i]);
	}

	const uint8_t width = 37, height = 56, x_offset = 32, y_offset = 16, x_extra_offset = 4;
	int8_t y_extra_offset = -2;

	adjust_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, adjust_layer);

	#ifdef PBL_COLOR
	GCompOp op = GCompOpSet;
	#else
	GCompOp op = GCompOpOr;
	#endif

	time_layers[TIME_LAYER_HOUR_1] = bitmap_layer_create(GRect(x_offset, y_offset, width, height));
	bitmap_layer_set_compositing_mode(time_layers[TIME_LAYER_HOUR_1], op);
	layer_add_child(adjust_layer, bitmap_layer_get_layer(time_layers[TIME_LAYER_HOUR_1]));

	time_layers[TIME_LAYER_HOUR_2] = bitmap_layer_create(GRect(x_offset+width+x_extra_offset, y_offset, width, height));
	bitmap_layer_set_compositing_mode(time_layers[TIME_LAYER_HOUR_2], op);
	layer_add_child(adjust_layer, bitmap_layer_get_layer(time_layers[TIME_LAYER_HOUR_2]));

	time_layers[TIME_LAYER_MINUTE_1] = bitmap_layer_create(GRect(x_offset, y_offset+height+y_extra_offset, width, height));
	bitmap_layer_set_compositing_mode(time_layers[TIME_LAYER_MINUTE_1], op);
	layer_add_child(adjust_layer, bitmap_layer_get_layer(time_layers[TIME_LAYER_MINUTE_1]));

	time_layers[TIME_LAYER_MINUTE_2] = bitmap_layer_create(GRect(x_offset+width+x_extra_offset, y_offset+height+y_extra_offset, width, height));
	bitmap_layer_set_compositing_mode(time_layers[TIME_LAYER_MINUTE_2], op);
	layer_add_child(adjust_layer, bitmap_layer_get_layer(time_layers[TIME_LAYER_MINUTE_2]));

	extra_layer = layer_create(GRect(-6, 0, 144, 168));
	layer_add_child(adjust_layer, extra_layer);

	squared_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARED_20));
	squared_16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARED_14));

	GRect date_rects[] = { GRect(30, 136, 30, 24), GRect(62, 136, 22, 24), GRect(86, 136, 30, 24) };
	GTextAlignment date_aligns[] = { GTextAlignmentLeft, GTextAlignmentCenter, GTextAlignmentRight };
	for(int i = 0; i < DATE_LAYERS; i++){
		date_layers[i] = text_layer_init(date_rects[i], date_aligns[i], squared_20);
		layer_add_child(extra_layer, text_layer_get_layer(date_layers[i]));
	}

	battery_text_layer = text_layer_init(GRect(100, 136, 60, 24), GTextAlignmentLeft, squared_16);
	text_layer_set_text(battery_text_layer, "100");
	layer_add_child(extra_layer, text_layer_get_layer(battery_text_layer));

	percent_layer = text_layer_init(GRect(114, 137, 20, 24), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text(percent_layer, "%");
	layer_add_child(extra_layer, text_layer_get_layer(percent_layer));

	lines_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(lines_layer, lines_proc);
	layer_add_child(extra_layer, lines_layer);

	battery_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(battery_layer, battery_proc);
	layer_add_child(window_layer, battery_layer);

	animation_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(animation_layer, animation_proc);
	layer_add_child(window_layer, animation_layer);

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

	animate_layer(animation_layer, &GRect(0, 0, 144, 168), &GRect(0, -168, 144, 168), 700, 300);

	settings = data_framework_get_settings();

	data_framework_register_settings_callback(main_window_update_settings, SETTINGS_CALLBACK_MAIN_WINDOW);
	main_window_update_settings(settings);

	battery_handler(battery_state_service_peek());
 }

void main_window_unload(Window *window){

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
	battery_state_service_subscribe(battery_handler);
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_deinit(){
	window_destroy(main_window);
}
