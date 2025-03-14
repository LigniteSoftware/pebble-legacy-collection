#include <pebble.h>
#include <math.h>
#include "lignite.h"
#include <ctype.h>
#include "main_window.h"

Window *main_window;
Settings settings;

GFont bebas_neue_small, bebas_neue_large, bebas_neue_extra_small, droid_sans_small, droid_sans_large;
GFont bebas_neue_small_bold, bebas_neue_extra_small_bold, droid_sans_small_bold;

TextLayer *hour_layer, *minute_layer, *first_date_number_layer, *second_date_number_layer, *third_date_number_layer;
TextLayer *short_day_layer, *end_day_layer, *am_layer, *pm_layer;

Layer *window_layer, *battery_bar_layer;

Notification *disconnect_notification, *reconnect_notification;

BatteryChargeState currentChargeState;

//#define DEBUG_LAYER_FRAMES
//#define DEBUG_12_HOUR_CLOCK
//#define DEBUG_24_HOUR_CLOCK
//#define DEBUG_LONGEST_DAY
//#define DEBUG_BATTERY_BAR

bool isPM = false;

#ifdef DEBUG_12_HOUR_CLOCK
#define clock_is_24h_style() false
#endif

#ifdef DEBUG_24_HOUR_CLOCK
#define clock_is_24h_style() true
#endif

const char *hour_words[] = {
	"TWELVE", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE"
};

GFont text_layer_get_font(TextLayer *text_layer){
	if(text_layer == hour_layer){
		return bebas_neue_small;
	}
	else if(text_layer == minute_layer){
		return bebas_neue_large;
	}
	else if(text_layer == second_date_number_layer){
		return droid_sans_small_bold;
	}
	else if(text_layer == am_layer){
		if(isPM){
			return droid_sans_small;
		}
		return droid_sans_small_bold;
	}
	else if(text_layer == pm_layer){
		if(isPM){
			return droid_sans_small_bold;
		}
		return droid_sans_small;
	}
	else if(text_layer == first_date_number_layer || text_layer == third_date_number_layer){
		return droid_sans_small;
	}
	else if(text_layer == short_day_layer){
		return bebas_neue_extra_small_bold;
	}
	else if(text_layer == end_day_layer){
		return bebas_neue_extra_small;
	}
	APP_LOG(APP_LOG_LEVEL_WARNING, "Warning: Couldn't find font for %p.", text_layer);
	return droid_sans_large;
}

GSize text_layer_get_proper_content_size(TextLayer *text_layer){
	const char *text = text_layer_get_text(text_layer);
	GFont font = text_layer_get_font(text_layer);
	GTextAlignment alignment = GTextAlignmentLeft;
	GRect rect = GRect(0, 0, 100, 100);
	GTextOverflowMode overflow = GTextOverflowModeWordWrap;

	return graphics_text_layout_get_content_size(text, font, rect, overflow, alignment);
}

/**
Since we need to have part of the word highlighted in a different colour, this function
allows for that. Call it and it will adjust all content to fit their proper sizing.
*/
void update_frames_for_content_sizes(){
	GRect window_frame = layer_get_frame(window_get_root_layer(main_window));

	//Update time frames
	const int WINDOW_HALF_HEIGHT = window_frame.size.h/2;

	#ifdef PBL_ROUND
	const int FRAME_OFFSET_X = 22;
	const int HOUR_FRAME_OFFSET_Y = 7;
	#else
	const int FRAME_OFFSET_X = 12;
	const int HOUR_FRAME_OFFSET_Y = 2;
	#endif

	GSize hour_size = text_layer_get_proper_content_size(hour_layer);
	GSize minute_size = text_layer_get_proper_content_size(minute_layer);

	GRect minute_frame_new = GRect(FRAME_OFFSET_X, WINDOW_HALF_HEIGHT - minute_size.h/1.5 + 10, minute_size.w, minute_size.h);
	GRect hour_frame_new = GRect(minute_frame_new.origin.x, minute_frame_new.origin.y - hour_size.h/2 + HOUR_FRAME_OFFSET_Y, hour_size.w, hour_size.h);

	layer_set_frame(text_layer_get_layer(hour_layer), hour_frame_new);
	layer_set_frame(text_layer_get_layer(minute_layer), minute_frame_new);

	//Update the AM/PM frames
	GSize am_size = text_layer_get_proper_content_size(am_layer);
	GSize pm_size = text_layer_get_proper_content_size(pm_layer);
	GRect am_layer_frame_current = layer_get_frame(text_layer_get_layer(am_layer));
	GRect pm_layer_frame_current = layer_get_frame(text_layer_get_layer(pm_layer));
	GRect am_layer_frame_new = GRect(minute_frame_new.origin.x+minute_frame_new.size.w+ FRAME_OFFSET_X/2 + -2, am_layer_frame_current.origin.y, am_size.w, am_size.h);
	GRect pm_layer_frame_new = GRect(am_layer_frame_new.origin.x + am_layer_frame_new.size.w + 4, pm_layer_frame_current.origin.y, pm_size.w, pm_size.h);

	GRect battery_bar_frame = GRect(am_layer_frame_new.origin.x, am_layer_frame_new.origin.y+2, 70, am_layer_frame_new.size.h);
	layer_set_frame(battery_bar_layer, battery_bar_frame);
	layer_mark_dirty(battery_bar_layer);

	layer_set_frame(text_layer_get_layer(am_layer), am_layer_frame_new);
	layer_set_frame(text_layer_get_layer(pm_layer), pm_layer_frame_new);
	//Update the day frames
	GSize short_day_layer_size = text_layer_get_proper_content_size(short_day_layer);
	GRect current_short_day_layer_rect = layer_get_frame(text_layer_get_layer(short_day_layer));
	GRect short_day_layer_rect = GRect(am_layer_frame_new.origin.x, current_short_day_layer_rect.origin.y, short_day_layer_size.w, short_day_layer_size.h);
	GRect end_day_layer_rect = GRect(short_day_layer_rect.origin.x+short_day_layer_rect.size.w, short_day_layer_rect.origin.y, 100, short_day_layer_rect.size.h);

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Current text: %s. Short day size: (%d, %d). Set to (%d, %d).", text_layer_get_text(short_day_layer), short_day_layer_size.w, short_day_layer_size.h, short_day_layer_rect.size.w, short_day_layer_rect.size.h);

	layer_set_frame(text_layer_get_layer(short_day_layer), short_day_layer_rect);
	layer_set_frame(text_layer_get_layer(end_day_layer), end_day_layer_rect);

	//Update the date frames
	GSize first_date_number_size = text_layer_get_proper_content_size(first_date_number_layer);
	GSize second_date_number_size = text_layer_get_proper_content_size(second_date_number_layer);
	GRect first_date_number_frame_current = layer_get_frame(text_layer_get_layer(first_date_number_layer));
	GRect first_date_number_frame_new = GRect(am_layer_frame_new.origin.x, first_date_number_frame_current.origin.y, first_date_number_size.w, first_date_number_size.h);
	GRect second_date_number_frame_new = GRect(first_date_number_frame_new.origin.x+first_date_number_frame_new.size.w, first_date_number_frame_new.origin.y, second_date_number_size.w, second_date_number_size.h);
	GRect third_date_number_frame_new = GRect(second_date_number_frame_new.origin.x+second_date_number_frame_new.size.w, second_date_number_frame_new.origin.y, second_date_number_size.w*2.5, second_date_number_size.h);

	layer_set_frame(text_layer_get_layer(first_date_number_layer), first_date_number_frame_new);
	layer_set_frame(text_layer_get_layer(second_date_number_layer), second_date_number_frame_new);
	layer_set_frame(text_layer_get_layer(third_date_number_layer), third_date_number_frame_new);
}

void main_tick_handler(struct tm *t, TimeUnits units){
	static char hour_buffer[] = "STICKER HUNGRY";
	snprintf(hour_buffer, sizeof(hour_buffer), "%s", hour_words[t->tm_hour % 12]);
	text_layer_set_text(hour_layer, hour_buffer);

	static char minute_buffer[] = "00:00";
	strftime(minute_buffer, sizeof(minute_buffer), "%M", t);
	text_layer_set_text(minute_layer, minute_buffer);

	static char short_day_buffer[] = "Wed";
	#ifndef DEBUG_LONGEST_DAY
	strftime(short_day_buffer, sizeof(short_day_buffer), "%a", t);
	#endif
	for(int i = 0, x = strlen(short_day_buffer); i < x; i++){
		short_day_buffer[i] = toupper((unsigned char) short_day_buffer[i]);
	}
	text_layer_set_text(short_day_layer, short_day_buffer);

	//This is easier than dealing with FUCKING CHAR POINTERS AND ARRAYS AND SHIT
	//Shut up
	switch(t->tm_wday){
		case 0:
			text_layer_set_text(end_day_layer, "day");
			break;
		case 1:
			text_layer_set_text(end_day_layer, "day");
			break;
		case 2:
			text_layer_set_text(end_day_layer, "sday");
			break;
		case 3:
			text_layer_set_text(end_day_layer, "nesday");
			break;
		case 4:
			text_layer_set_text(end_day_layer, "rsday");
			break;
		case 5:
			text_layer_set_text(end_day_layer, "day");
			break;
		case 6:
			text_layer_set_text(end_day_layer, "urday");
			break;
	}
	#ifdef DEBUG_LONGEST_DAY
	text_layer_set_text(end_day_layer, "nesday");
	#endif

	static char first_date_number_buffer[] = "Hi-";
	strftime(first_date_number_buffer, sizeof(first_date_number_buffer), "%m-", t);
	text_layer_set_text(first_date_number_layer, first_date_number_buffer);

	static char second_date_number_buffer[] = "U ";
	strftime(second_date_number_buffer, sizeof(second_date_number_buffer), "%d", t);
	text_layer_set_text(second_date_number_layer, second_date_number_buffer);

	static char third_date_number_buffer[] = "-there";
	strftime(third_date_number_buffer, sizeof(third_date_number_buffer), "-%y", t);
	text_layer_set_text(third_date_number_layer, third_date_number_buffer);

	update_frames_for_content_sizes();

	if(!clock_is_24h_style()){
		isPM = (t->tm_hour > 12);
		text_layer_set_text_color(am_layer, isPM ? PBL_IF_COLOR_ELSE(GColorDarkGray, GColorBlack) : PBL_IF_COLOR_ELSE(settings.accentColour, GColorWhite));
		text_layer_set_text_color(pm_layer, isPM ? PBL_IF_COLOR_ELSE(settings.accentColour, GColorWhite) : PBL_IF_COLOR_ELSE(GColorDarkGray, GColorBlack));
		text_layer_set_font(am_layer, isPM ? droid_sans_small : droid_sans_small_bold);
		text_layer_set_font(pm_layer, isPM ? droid_sans_small_bold : droid_sans_small);
	}
	layer_set_hidden(text_layer_get_layer(am_layer), clock_is_24h_style() || settings.batteryBar);
	layer_set_hidden(text_layer_get_layer(pm_layer), clock_is_24h_style() || settings.batteryBar);
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
	if(state.charge_percent != currentChargeState.charge_percent && settings.batteryBar){
		currentChargeState = state;
		layer_mark_dirty(battery_bar_layer);
	}
}

void battery_bar_proc(Layer *layer, GContext *ctx){
	GRect frame = layer_get_frame(layer);
	GRect amFrame = layer_get_frame(text_layer_get_layer(am_layer));
	uint8_t spacing = 5;
	uint8_t radius = 2;

	graphics_context_set_fill_color(ctx, settings.accentColour);
	#ifdef DEBUG_BATTERY_BAR
	int amount = 10;
	#else
	int amount = currentChargeState.charge_percent/10;
	#endif
	for(int i = 0; i < amount; i++){
		GPoint point = GPoint(radius + (i*(spacing+radius)), (radius*2));
		APP_LOG(APP_LOG_LEVEL_INFO, "printing %d to GPoint(%d, %d)", i, point.x, point.y);
		graphics_fill_circle(ctx, point, radius);
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
	window_set_background_color(main_window, settings.backgroundColour);
	text_layer_set_text_color(minute_layer, settings.accentColour);
	text_layer_set_text_color(second_date_number_layer, settings.accentColour);
	text_layer_set_text_color(short_day_layer, settings.accentColour);
	text_layer_set_text_color(hour_layer, gcolor_legible_over(settings.backgroundColour));
	text_layer_set_text_color(first_date_number_layer, gcolor_legible_over(settings.backgroundColour));
	text_layer_set_text_color(third_date_number_layer, gcolor_legible_over(settings.backgroundColour));
	text_layer_set_text_color(end_day_layer, gcolor_legible_over(settings.backgroundColour));
	text_layer_set_background_color(short_day_layer, settings.backgroundColour);
	#endif

	layer_set_hidden(text_layer_get_layer(short_day_layer), !settings.day);
	layer_set_hidden(text_layer_get_layer(end_day_layer), !settings.day);
	layer_set_hidden(text_layer_get_layer(first_date_number_layer), !settings.date);
	layer_set_hidden(text_layer_get_layer(second_date_number_layer), !settings.date);
	layer_set_hidden(text_layer_get_layer(third_date_number_layer), !settings.date);
	#ifndef DEBUG_BATTERY_BAR
	layer_set_hidden(battery_bar_layer, !settings.batteryBar);
	#endif

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

	#ifdef PBL_ROUND
	uint8_t minute_font_size = 76;
	uint8_t hour_font_size = 26;
	bebas_neue_large = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_76));
	#else
	uint8_t minute_font_size = 60;
	uint8_t hour_font_size = 22;
	bebas_neue_large = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_62));
	#endif

	uint8_t day_font_size = 16;
	bebas_neue_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_20));
	bebas_neue_small_bold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_20));
	droid_sans_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_14));
	droid_sans_small_bold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_BOLD_14));
	droid_sans_large = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_16));
	bebas_neue_extra_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_18));
	bebas_neue_extra_small_bold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_18));

	//hour_layer, *minute_layer, *first_date_number_layer, *second_date_number_layer, *third_date_number_layer;
	//short_day_layer, *end_day_layer, *am_layer, *pm_layer;

	GRect nil = GRect(0, 0, 50, 50);

	GRect hour_layer_frame = GRect(10, frame.size.h/2 - hour_font_size/2, hour_font_size, hour_font_size+2);
	hour_layer = text_layer_init(hour_layer_frame, GTextAlignmentLeft, bebas_neue_small);
	text_layer_set_text_color(hour_layer, GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(hour_layer));

	GRect minute_layer_frame = GRect(hour_layer_frame.origin.x+hour_layer_frame.size.w, frame.size.h/2 - minute_font_size/2, minute_font_size, minute_font_size);
	minute_layer = text_layer_init(minute_layer_frame, GTextAlignmentLeft, bebas_neue_large);
	text_layer_set_text_color(minute_layer, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
	text_layer_set_background_color(minute_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(minute_layer));

	GRect am_layer_frame = GRect(minute_layer_frame.origin.x+minute_layer_frame.size.w+2, frame.size.h/2 + 18, 26, 22);
	am_layer = text_layer_init(am_layer_frame, GTextAlignmentLeft, droid_sans_small);
	text_layer_set_text(am_layer, "AM");
	layer_add_child(window_layer, text_layer_get_layer(am_layer));

	GRect pm_layer_frame = GRect(am_layer_frame.origin.x+am_layer_frame.size.w+2, am_layer_frame.origin.y, am_layer_frame.size.w, am_layer_frame.size.h);
	pm_layer = text_layer_init(pm_layer_frame, GTextAlignmentLeft, droid_sans_small);
	text_layer_set_text(pm_layer, "PM");
	layer_add_child(window_layer, text_layer_get_layer(pm_layer));

	GRect end_day_frame = GRect(am_layer_frame.origin.x+2, frame.size.h/2 - day_font_size/2 + 6, 80, day_font_size);
	end_day_layer = text_layer_init(end_day_frame, GTextAlignmentLeft, bebas_neue_extra_small_bold);
	text_layer_set_background_color(end_day_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(end_day_layer));

	GRect short_day_frame = GRect(end_day_frame.origin.x-2, end_day_frame.origin.y, 24, day_font_size);
	short_day_layer = text_layer_init(short_day_frame, GTextAlignmentLeft, bebas_neue_extra_small_bold);
	text_layer_set_background_color(short_day_layer, GColorBlack);
	text_layer_set_text_color(short_day_layer, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
	layer_add_child(window_layer, text_layer_get_layer(short_day_layer));

	first_date_number_layer = text_layer_init(GRect(short_day_frame.origin.x, short_day_frame.origin.y-15, 50, 50), GTextAlignmentLeft, droid_sans_small);
	text_layer_set_text(first_date_number_layer, "Hi-");
	layer_add_child(window_layer, text_layer_get_layer(first_date_number_layer));

	second_date_number_layer = text_layer_init(nil, GTextAlignmentLeft, droid_sans_small_bold);
	text_layer_set_text(second_date_number_layer, "UU");
	text_layer_set_text_color(second_date_number_layer, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
	layer_add_child(window_layer, text_layer_get_layer(second_date_number_layer));

	third_date_number_layer = text_layer_init(nil, GTextAlignmentLeft, droid_sans_small);
	text_layer_set_text(third_date_number_layer, "-ther");
	layer_add_child(window_layer, text_layer_get_layer(third_date_number_layer));

	battery_bar_layer = layer_create(GRect(am_layer_frame.origin.x, am_layer_frame.origin.y, am_layer_frame.size.w*2 + 10, am_layer_frame.size.h+4));
	layer_set_update_proc(battery_bar_layer, battery_bar_proc);
	layer_add_child(window_layer, battery_bar_layer);

	#ifdef DEBUG_LAYER_FRAMES
	#ifdef PBL_COLOR
	text_layer_set_background_color(hour_layer, GColorYellow);
	text_layer_set_background_color(minute_layer, GColorOrange);
	text_layer_set_background_color(short_day_layer, GColorBlue);
	text_layer_set_background_color(end_day_layer, GColorRed);
	text_layer_set_background_color(first_date_number_layer, GColorJazzberryJam);
	text_layer_set_background_color(second_date_number_layer, GColorMagenta);
	text_layer_set_background_color(third_date_number_layer, GColorRichBrilliantLavender);
	text_layer_set_background_color(am_layer, GColorCadetBlue);
	text_layer_set_background_color(pm_layer, GColorBabyBlueEyes);
	window_set_background_color(main_window, GColorLightGray);
	#else
	text_layer_set_background_color(hour_layer, GColorBlack);
	text_layer_set_background_color(minute_layer, GColorBlack);
	text_layer_set_background_color(short_day_layer, GColorBlack);
	text_layer_set_background_color(end_day_layer, GColorBlack);
	text_layer_set_background_color(first_date_number_layer, GColorBlack);
	text_layer_set_background_color(second_date_number_layer, GColorBlack);
	text_layer_set_background_color(third_date_number_layer, GColorBlack);
	text_layer_set_background_color(am_layer, GColorBlack);
	text_layer_set_background_color(pm_layer, GColorBlack);
	window_set_background_color(main_window, GColorWhite);
	#endif
	#endif

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

	battery_handler(battery_state_service_peek());
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
