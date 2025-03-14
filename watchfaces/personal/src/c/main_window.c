#include <pebble.h>
#include <math.h>
#include "lignite.h"
#include "main_window.h"
#include "data_layer.h"

//#define PHILIPP_BUILD

Window *main_window;
Settings settings;
TextLayer *time_layer, *date_layer, *am_layer, *pm_layer, *week_day_layer;
BitmapLayer *background_layer, *battery_layers[10];
GBitmap *background_image, *battery_bitmaps[10];
Layer *window_layer, *battery_layer, *date_selection_layer, *background_detail_layer;
Notification *disconnect_notification, *reconnect_notification;
BatteryChargeState previous_state;
int time_array[2];

void main_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(time_buffer, sizeof(time_buffer), "%H:%M", t);
	}
	else{
		strftime(time_buffer, sizeof(time_buffer), "%I:%M", t);
		#ifdef PBL_COLOR
		if(t->tm_hour < 12){
			text_layer_set_text_color(am_layer, GColorWhite);
			text_layer_set_text_color(pm_layer, GColorMelon);
		}
		else{
			text_layer_set_text_color(am_layer, GColorMelon);
			text_layer_set_text_color(pm_layer, GColorWhite);
		}
		#endif
	}
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "It's no problem for me...";
	                           //BUT IT'S A PROBLEM FOR YOU
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);

	DataLayerUpdate tick_update;
	if(t->tm_hour < 7 || t->tm_hour > 22){
		strcpy(tick_update.new_subtitle[0], "goodnight!");
	}
	else if(t->tm_hour < 11){
		strcpy(tick_update.new_subtitle[0], "good morning!");
	}
	else if(t->tm_hour < 14){
		strcpy(tick_update.new_subtitle[0], "good day!");
	}
	else if(t->tm_hour < 18){
		strcpy(tick_update.new_subtitle[0], "good afternoon!");
	}
	else{
		strcpy(tick_update.new_subtitle[0], "good evening!");
	}

	static char name_buffer[23];
	snprintf(name_buffer, sizeof(name_buffer), "Hey %s,", settings.name[0]);
	strcpy(tick_update.new_title[0], name_buffer);

	tick_update.bluetooth_connected = bluetooth_connection_service_peek();
	if(data_layer_set_update(tick_update)){
		data_layer_mark_dirty(true);
	}
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
	data_layer_set_settings(new_settings);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);

	if(!settings.bootAnimation){
		DataLayerUpdate tick_update;
		if(t->tm_hour < 7 || t->tm_hour > 22){
			strcpy(tick_update.new_subtitle[0], "goodnight!");
		}
		else if(t->tm_hour < 11){
			strcpy(tick_update.new_subtitle[0], "good morning!");
		}
		else if(t->tm_hour < 14){
			strcpy(tick_update.new_subtitle[0], "good day!");
		}
		else if(t->tm_hour < 18){
			strcpy(tick_update.new_subtitle[0], "good afternoon!");
		}
		else{
			strcpy(tick_update.new_subtitle[0], "good evening!");
		}

		static char name_buffer[23];
		snprintf(name_buffer, sizeof(name_buffer), "Hey %s,", settings.name[0]);
		strcpy(tick_update.new_title[0], name_buffer);

		tick_update.bluetooth_connected = bluetooth_connection_service_peek();
		data_layer_set_update(tick_update);
		data_layer_mark_dirty(false);
	}

	bool clock_is_24h = clock_is_24h_style();
	layer_set_hidden(text_layer_get_layer(am_layer), clock_is_24h);
	layer_set_hidden(text_layer_get_layer(pm_layer), clock_is_24h);
	text_layer_set_text_alignment(time_layer, clock_is_24h ? GTextAlignmentCenter : GTextAlignmentLeft);

	#ifdef PBL_COLOR
	window_set_background_color(main_window, settings.backgroundColour);
	GColor legible = gcolor_legible_over(settings.accentColour);
	text_layer_set_text_color(time_layer, legible);
	text_layer_set_text_color(date_layer, legible);
	text_layer_set_text_color(week_day_layer, legible);
	#endif

	main_tick_handler(t, MINUTE_UNIT);
}

void background_detail_proc(Layer *layer, GContext *ctx){
	#ifdef PBL_COLOR
	graphics_context_set_fill_color(ctx, GColorBlack);

	if(gcolor_equal(gcolor_legible_over(settings.accentColour), GColorBlack))
		graphics_context_set_fill_color(ctx, GColorLightGray);

	graphics_fill_rect(ctx, GRect(11, 54, 132-10, 75-53), 0, GCornerNone); //Weekday rect

	bool colour_is_red = gcolor_equal(settings.accentColour, GColorRed);

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, GRect(12, 83, 121, 64), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, colour_is_red ? GColorMelon : settings.accentColour);
	graphics_fill_rect(ctx, GRect(12, 83, 3, 61), 0, GCornerNone);
	graphics_fill_rect(ctx, GRect(21, 83, 50, colour_is_red ? 64 : 62), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, colour_is_red ? GColorDarkCandyAppleRed : settings.accentColour);
	graphics_fill_rect(ctx, GRect(17, 83, 112, 63), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, settings.accentColour);
	graphics_fill_rect(ctx, GRect(14, 83, 117, 61), 0, GCornerNone);

	graphics_context_set_fill_color(ctx, GColorLightGray);
	graphics_fill_rect(ctx, GRect(6, 54, 2, 22), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorDarkGray);
	graphics_fill_rect(ctx, GRect(8, 54, 3, 22), 0, GCornerNone);

	graphics_context_set_fill_color(ctx, GColorLightGray);
	graphics_fill_rect(ctx, GRect(136, 54, 2, 22), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorDarkGray);
	graphics_fill_rect(ctx, GRect(133, 54, 3, 22), 0, GCornerNone);
	#else
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(11, 54, 132-10, 75-53), 0, GCornerNone);
	#endif
}

void date_selection_proc(Layer *layer, GContext *ctx){
	#ifdef PBL_COLOR
	graphics_context_set_fill_color(ctx, settings.accentColour);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);
	#else
	GColor first = GColorBlack;
	GColor second = GColorWhite;
	int x_start = 0, y_start = 0, width = 144, height = 168;
	for(int x = x_start; x < x_start+width; x++){
		for(int y = y_start; y < y_start+height; y++){
			if((x%4 == 0 && y%2 == 0) || ((x+2)%4 == 0 && (y+1)%2 == 0)) graphics_context_set_stroke_color(ctx, second);
			else graphics_context_set_stroke_color(ctx, first);
			graphics_draw_pixel(ctx, GPoint(x, y));
		}
	}
	#endif
}

void battery_handler(BatteryChargeState state){
	if(state.charge_percent != previous_state.charge_percent){
		for(int i = state.charge_percent/10; i < 10; i++){
			GRect frame = layer_get_frame(bitmap_layer_get_layer(battery_layers[i]));
			if(frame.origin.y != 150){
				animate_layer(bitmap_layer_get_layer(battery_layers[i]), &GRect(15+(i*11)+(i == 5 ? 3 : 0), 154, 14, 14), &GRect(15+(i*11)+(i == 5 ? 3 : 0), 150, 14, 14), 400, 0+((10-i)*200));
			}
		}
	}
}

void initial_animation_callback(){
	battery_handler(battery_state_service_peek());
	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
	GRect rectangles[] = {
		GRect(115, 54, 16, 22), GRect(12, 54, 21, 22), GRect(34, 54, 15, 22), GRect(49, 54, 18, 22), GRect(67, 54, 17, 22),
		GRect(84, 54, 15, 22), GRect(99, 54, 16, 22)
	};
	if(settings.bootAnimation){
		animate_layer(date_selection_layer, &GRect(10, 54, 22, 22), &rectangles[t->tm_wday], 800, 0);
	}
	else{
		layer_set_frame(date_selection_layer, rectangles[t->tm_wday]);
	}
}

void main_window_load(Window *window){
	AppTimer *initial_timer = app_timer_register(1100, initial_animation_callback, NULL);

	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	data_layer_create(window_layer);

	for(int i = 0; i < 10; i++){
		#ifdef PBL_COLOR
		if(i < 2){
			battery_bitmaps[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_RED);
		}
		else if(i < 5){
			battery_bitmaps[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_ORANGE);
		}
		else{
			battery_bitmaps[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_GREEN);
		}
		#else
		if(i < 5){
			battery_bitmaps[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_APLITE_LEFT_WHITE);
		}
		else{
			battery_bitmaps[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_APLITE_RIGHT_WHITE);
		}
		#endif
		battery_layers[i] = bitmap_layer_create(GRect(14+(i*11)+(i >= 5 ? 3 : 0), 154, 14, 14));
		bitmap_layer_set_bitmap(battery_layers[i], battery_bitmaps[i]);
		#ifdef PBL_BW
		bitmap_layer_set_compositing_mode(battery_layers[i], GCompOpOr);
		#endif
		layer_add_child(window_layer, bitmap_layer_get_layer(battery_layers[i]));
	}

	#ifdef PBL_COLOR
	background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
	#else
	background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_APLITE_WHITE);
	#endif

	background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(background_layer, background_image);
	#ifdef PBL_COLOR
	bitmap_layer_set_compositing_mode(background_layer, GCompOpSet);
	#else
	bitmap_layer_set_compositing_mode(background_layer, GCompOpOr);
	#endif
	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

	background_detail_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(background_detail_layer, background_detail_proc);
	layer_add_child(window_layer, background_detail_layer);

	date_selection_layer = layer_create(GRect(2, 54, 22, 22));
	layer_set_update_proc(date_selection_layer, date_selection_proc);
	layer_add_child(window_layer, date_selection_layer);

	GFont droid_sans = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_12));
	GFont droid_sans_11 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_11));

	week_day_layer = text_layer_init(GRect(10, 58, 124, 30), GTextAlignmentCenter, droid_sans_11);
	text_layer_set_text(week_day_layer, "MO TU WE TH FR SA SU");
	layer_add_child(window_layer, text_layer_get_layer(week_day_layer));

	GFont impact_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_20));

	date_layer = text_layer_init(GRect(20, 124, 104, 30), GTextAlignmentCenter, droid_sans);
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	time_layer = text_layer_init(GRect(20, 84, 104, 60), GTextAlignmentLeft, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_36)));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	GFont impact_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_14));

	am_layer = text_layer_init(GRect(106, 86, 40, 40), GTextAlignmentLeft, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text(am_layer, "AM");
	layer_add_child(window_layer, text_layer_get_layer(am_layer));

	pm_layer = text_layer_init(GRect(106, 102, 40, 40), GTextAlignmentLeft, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text(pm_layer, "PM");
	layer_add_child(window_layer, text_layer_get_layer(pm_layer));

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
	data_layer_destroy();
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
