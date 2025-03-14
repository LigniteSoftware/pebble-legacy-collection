#include <pebble.h>
#include "lignite.h"
#include "main_window.h"
#include "gbitmap_color_palette_manipulator.h"

//#define DEBUG_FONTS

Window *main_window;
Settings settings;

GFont simplifica, droid_sans_regular, droid_sans_bold, weather_icon;

TextLayer *hour_layer, *date_layer, *day_layer, *temperature_layer, *weather_layer;
Layer *window_layer, *bottom_colour_layer, *top_colour_layer, *date_background_layer;
BitmapLayer *bottom_number_left, *bottom_number_right;
GBitmap *bottom_number_left_bitmap, *bottom_number_right_bitmap;

Notification *disconnect_notification, *reconnect_notification;

bool boot = true;
bool animating = false;

int HOUR_NUMBER_HIDDEN = 0, HOUR_NUMBER_SHOWING = 0;
int MINUTE_NUMBER_HIDDEN = 0, MINUTE_NUMBER_SHOWING = 0;

#ifdef PBL_COLOR
const uint32_t number_resource_ids[] = {
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_0,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_1,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_2,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_3,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_4,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_5,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_6,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_7,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_8,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_9
};
#else
const uint32_t number_resource_ids[] = {
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_0_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_1_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_2_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_3_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_4_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_5_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_6_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_7_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_8_BLACK,
		RESOURCE_ID_IMAGE_UPSIDE_DOWN_NUMBER_9_BLACK
};
#endif

static const char *climacon_condition_codes[14] = {
	"B", //CLEAR_DAY
	"I", //CLEAR_NIGHT
	"Z", //WINDY
	"O", //COLD
	"C", //PARTLY_CLOUDY_DAY
	"J", //PARTLY_CLOUDY_NIGHT
	"N", //HAZE
	"P", //CLOUD
	"Q", //RAIN
	"W", //SNOW
	"X", //HAIL
	"P", //CLOUDY
	"V", //STORM
	"C", //NA
};


void update_upside_down(struct tm *t){
	gbitmap_destroy(bottom_number_left_bitmap);
	gbitmap_destroy(bottom_number_right_bitmap);
	bottom_number_left_bitmap = gbitmap_create_with_resource(number_resource_ids[t->tm_min % 10]);
	bottom_number_right_bitmap = gbitmap_create_with_resource(number_resource_ids[t->tm_min / 10]);
	bitmap_layer_set_bitmap(bottom_number_left, bottom_number_left_bitmap);
	bitmap_layer_set_bitmap(bottom_number_right, bottom_number_right_bitmap);
	#ifdef PBL_COLOR
	gbitmap_fill_all_except(GColorClear, settings.topColour, false, bottom_number_left_bitmap, bottom_number_left);
	gbitmap_fill_all_except(GColorClear, settings.topColour, false, bottom_number_right_bitmap, bottom_number_right);
	#endif
}

void manual_animate_numbers(Layer *layer, bool inward){
	bool isTopLayer = (text_layer_get_layer(hour_layer) == layer);
	GRect current_frame = layer_get_frame(layer);
	GRect new_frame = GRect(current_frame.origin.x, 0, current_frame.size.w, current_frame.size.h);
	new_frame.origin.y = (isTopLayer ? (inward ? HOUR_NUMBER_HIDDEN : HOUR_NUMBER_SHOWING) : (inward ? MINUTE_NUMBER_HIDDEN : MINUTE_NUMBER_SHOWING));
	animate_layer(layer, &current_frame, &new_frame, 700, boot ? 800 : 0);
}

void main_tick_handler(struct tm *t, TimeUnits units){
	#ifndef DEBUG_FONTS
	if(t->tm_sec != 59 && t->tm_sec != 0 && !boot){
		return;
	}
	else{
		if(!boot){
			if(settings.minutelyAnimations){
				bool isLastSecond = t->tm_sec == 59;
				manual_animate_numbers(text_layer_get_layer(hour_layer), isLastSecond);
				manual_animate_numbers(bitmap_layer_get_layer(bottom_number_left), isLastSecond);
				manual_animate_numbers(bitmap_layer_get_layer(bottom_number_right), isLastSecond);
			}
		}
		else{
			manual_animate_numbers(text_layer_get_layer(hour_layer), false);
			manual_animate_numbers(bitmap_layer_get_layer(bottom_number_left), false);
			manual_animate_numbers(bitmap_layer_get_layer(bottom_number_right), false);
		}
	}
	#endif

	#ifdef DEBUG_FONTS
	static char hour_buffer[] = "00";
	strftime(hour_buffer, sizeof(hour_buffer), "%S", t);
	text_layer_set_text(hour_layer, hour_buffer);
	#else
	static char hour_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(hour_buffer, sizeof(hour_buffer), "%H", t);
	}
	else{
		strftime(hour_buffer, sizeof(hour_buffer), "%I", t);
	}
	text_layer_set_text(hour_layer, hour_buffer);
	#endif

	static char date_buffer[] = "Tuc";
	strftime(date_buffer, sizeof(date_buffer), "%d", t);
	text_layer_set_text(date_layer, date_buffer);

	static char day_buffer[] = "Meme";
	strftime(day_buffer, sizeof(day_buffer), "%a", t);
	text_layer_set_text(day_layer, day_buffer);

	update_upside_down(t);

	boot = false;
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

void battery_handler(BatteryChargeState state){
	//TODO: Remove
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

	static char temperature_buffer[] = "69 degrees";
	snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", settings.currentWeather.temperature);
	text_layer_set_text(temperature_layer, temperature_buffer);

	text_layer_set_text(weather_layer, climacon_condition_codes[settings.currentWeather.icon]);
	//APP_LOG(APP_LOG_LEVEL_INFO, "Weather layer set to %s (%d)", text_layer_get_text(weather_layer), settings.currentWeather.icon);

	text_layer_set_text_color(weather_layer, settings.bottomColour);
	text_layer_set_text_color(temperature_layer, settings.bottomColour);
	text_layer_set_text_color(hour_layer, settings.bottomColour);
	text_layer_set_text_color(date_layer, settings.bottomColour);
	text_layer_set_text_color(day_layer, settings.bottomColour);

	layer_mark_dirty(top_colour_layer);
	layer_mark_dirty(bottom_colour_layer);
	layer_mark_dirty(date_background_layer);

	layer_set_hidden(text_layer_get_layer(temperature_layer), !settings.weather);
	layer_set_hidden(text_layer_get_layer(weather_layer), !settings.weather);
	layer_set_hidden(text_layer_get_layer(day_layer), !settings.day);
	layer_set_hidden(text_layer_get_layer(date_layer), !settings.date);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);

	main_tick_handler(t, MINUTE_UNIT);

	update_upside_down(t);
}

void date_background_proc(Layer *layer, GContext *ctx){
	GRect day_frame = layer_get_frame(text_layer_get_layer(day_layer));
	GRect date_frame = layer_get_frame(text_layer_get_layer(date_layer));

	graphics_context_set_fill_color(ctx, settings.topColour);
	if(settings.day){
		graphics_fill_rect(ctx, day_frame, 3, GCornersAll);
	}
	if(settings.date){
		graphics_fill_rect(ctx, date_frame, 3, GCornersAll);
	}
}

void top_colour_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, settings.topColour);
	graphics_fill_rect(ctx, GRect(0, 0, 200, 200), 0, GCornerNone);
}

void bottom_colour_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, settings.bottomColour);
	graphics_fill_rect(ctx, GRect(0, 0, 200, 200), 0, GCornerNone);
}

void update_weather(){
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	app_timer_register(1800000, update_weather, NULL);

	if (iter == NULL) {
		APP_LOG(APP_LOG_LEVEL_ERROR, "Iter is NULL, refusing to send message.");
		return;
	}

	dict_write_uint16(iter, 200, 1);
	dict_write_end(iter);

	app_message_outbox_send();

	APP_LOG(APP_LOG_LEVEL_INFO, "Sending message.\n");
}

int current_icon = 0;
void test_new(){
	app_timer_register(1500, test_new, NULL);
	APP_LOG(APP_LOG_LEVEL_INFO, "Weather icon %d: %s", current_icon, climacon_condition_codes[current_icon]);
	text_layer_set_text(weather_layer, climacon_condition_codes[current_icon]);
	current_icon++;
	if(current_icon > WEATHER_STATUS_NOT_AVAILABLE_OR_ERROR){
		current_icon = 0;
	}
}

void main_window_load(Window *window){
	GRect frame = layer_get_frame(window_get_root_layer(window));
	window_layer = layer_create(frame);
	layer_add_child(window_get_root_layer(window), window_layer);

	simplifica = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIMPLIFICA_88));
	weather_icon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_ICON_45));
	droid_sans_regular = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_BOLD_14));
	droid_sans_bold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_REGULAR_18));

	GRect nil = GRect(0, 0, 0, 0);

	date_layer = text_layer_init(nil, GTextAlignmentCenter, droid_sans_regular);
	day_layer = text_layer_init(nil, GTextAlignmentCenter, droid_sans_regular);
	weather_layer = text_layer_init(nil, GTextAlignmentCenter, weather_icon);
	temperature_layer = text_layer_init(nil, GTextAlignmentRight, droid_sans_bold);
	hour_layer = text_layer_init(nil, GTextAlignmentLeft, simplifica);

	bottom_number_left = bitmap_layer_create(nil);
	bottom_number_right = bitmap_layer_create(nil);
	#ifdef PBL_COLOR
	GCompOp op = GCompOpSet;
	#else
	GCompOp op = GCompOpClear;
	#endif
	bitmap_layer_set_compositing_mode(bottom_number_left, op);
	bitmap_layer_set_compositing_mode(bottom_number_right, op);

	top_colour_layer = layer_create(GRect(0, 0, frame.size.w, frame.size.h/2));
	layer_set_update_proc(top_colour_layer, top_colour_proc);

	bottom_colour_layer = layer_create(GRect(0, frame.size.h/2, frame.size.w, frame.size.h/2));
	layer_set_update_proc(bottom_colour_layer, bottom_colour_proc);

	date_background_layer = layer_create(frame);
	layer_set_update_proc(date_background_layer, date_background_proc);

	const int fontSize = 90;
	#ifdef PBL_ROUND
	int adjust = 26;
	int hourFrameAdjust = 10;
	int weatherAdjust = 0;
	int weatherIconAdjust = 0;
	#else
	int adjust = 12;
	int hourFrameAdjust = -2;
	int weatherAdjust = 4;
	int weatherIconAdjust = -10;
	#endif
	GRect hour_frame = GRect(adjust, frame.size.h/2 - fontSize + 6, frame.size.w/2-hourFrameAdjust, fontSize);
	GRect date_frame = GRect(frame.size.w/2 - 10 - 40, frame.size.w/2 + 13 - weatherIconAdjust, 40, 20);
	GRect day_frame = GRect(frame.size.w/2 - 10 - 30, date_frame.origin.y+date_frame.size.h + 3, 30, date_frame.size.h);
	GRect weather_icon_frame = GRect(hour_frame.origin.x+hour_frame.size.w-15+weatherAdjust, 16 + weatherIconAdjust, frame.size.h/4 + 15, frame.size.w/4+4 + 10);
	GRect weather_text_frame = GRect(weather_icon_frame.origin.x+4+weatherAdjust, weather_icon_frame.origin.y + weather_icon_frame.size.h - 10 - weatherIconAdjust, weather_icon_frame.size.w, weather_icon_frame.size.h);
	GRect bottom_minute_frame_left = GRect(day_frame.origin.x+date_frame.size.w+4, frame.size.h/2 - 6, 27, 66);
	GRect bottom_minute_frame_right = GRect(bottom_minute_frame_left.origin.x+bottom_minute_frame_left.size.w+4, bottom_minute_frame_left.origin.y, bottom_minute_frame_left.size.w, bottom_minute_frame_left.size.h);

	HOUR_NUMBER_SHOWING = hour_frame.origin.y;
	HOUR_NUMBER_HIDDEN = hour_frame.origin.y+hour_frame.size.h;
	MINUTE_NUMBER_SHOWING = bottom_minute_frame_left.origin.y;
	MINUTE_NUMBER_HIDDEN = bottom_minute_frame_left.origin.y-bottom_minute_frame_left.size.h;

	hour_frame.origin.y += hour_frame.size.h;
	bottom_minute_frame_left.origin.y -= bottom_minute_frame_left.size.h;
	bottom_minute_frame_right.origin.y -= bottom_minute_frame_right.size.h;

	layer_set_frame(text_layer_get_layer(hour_layer), hour_frame);
	layer_set_frame(text_layer_get_layer(date_layer), day_frame);
	layer_set_frame(text_layer_get_layer(day_layer), date_frame);
	layer_set_frame(text_layer_get_layer(weather_layer), weather_icon_frame);
	layer_set_frame(text_layer_get_layer(temperature_layer), weather_text_frame);
	layer_set_frame(bitmap_layer_get_layer(bottom_number_left), bottom_minute_frame_left);
	layer_set_frame(bitmap_layer_get_layer(bottom_number_right), bottom_minute_frame_right);

	layer_add_child(window_layer, bottom_colour_layer);
	layer_add_child(window_layer, top_colour_layer);
	layer_add_child(window_layer, text_layer_get_layer(hour_layer));
	layer_add_child(window_layer, bitmap_layer_get_layer(bottom_number_left));
	layer_add_child(window_layer, bitmap_layer_get_layer(bottom_number_right));
	layer_add_child(window_layer, text_layer_get_layer(temperature_layer));
	layer_add_child(window_layer, text_layer_get_layer(weather_layer));
	layer_add_child(window_layer, date_background_layer);
	layer_add_child(window_layer, text_layer_get_layer(day_layer));
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

	tick_timer_service_subscribe(SECOND_UNIT, main_tick_handler);

	data_framework_register_settings_callback(main_window_settings_callback, SETTINGS_CALLBACK_MAIN_WINDOW);

	main_window_settings_callback(data_framework_get_settings());

	app_timer_register(1800000, update_weather, NULL);

	//test_new();
}

void main_window_unload(Window *window){
	//Destroy shit
}

void main_window_init(){
	main_window = window_create();
	window_set_background_color(main_window, GColorWhite);
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
