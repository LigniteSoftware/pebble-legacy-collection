#include <pebble.h>
#include "lignite.h"
#include "effectlayer/effect_layer.h"
#include "main_window.h"

Window *main_window;
TextLayer *date_layer[2];
TextLayer *time_layer[2];
Layer *graphics_layer, *window_layer;
EffectLayer *invert_layer;
Settings settings;
BatteryChargeState current_state;
Notification *reconnect_notification, *disconnect_notification;

void main_tick_handler(struct tm *t, TimeUnits units){
	static char minute_buffer[] = "00";
	static char hour_buffer[] = "00";
	static char day_buffer[] = "Day day day.";
	static char date_buffer[] = "Mememememememememememem";
	strftime(minute_buffer, sizeof(minute_buffer), "%M", t);
	if(clock_is_24h_style()){
		strftime(hour_buffer, sizeof(hour_buffer), "%H", t);
	}
	else{
		strftime(hour_buffer, sizeof(hour_buffer), "%I", t);
	}
	strftime(day_buffer, sizeof(day_buffer), "%A", t);
	strftime(date_buffer, sizeof(date_buffer), "%d - %m - %y", t);

	text_layer_set_text(time_layer[LAYER_TYPE_MINUTE], hour_buffer);
	text_layer_set_text(time_layer[LAYER_TYPE_HOUR], minute_buffer);
	text_layer_set_text(date_layer[LAYER_TYPE_DAY], day_buffer);
	text_layer_set_text(date_layer[LAYER_TYPE_DETAILED], date_buffer);
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
	current_state = state;
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

void graphics_proc(Layer *layer, GContext *ctx){
	for(int i = 0; i < (current_state.charge_percent/10); i++){
		GColor colour = GColorWhite;
		#ifdef PBL_COLOR
		if(i < 2){
			colour = GColorRed;
		}
		else if(i < 4 && i > 1){
			colour = GColorOrange;
		}
		else if(i < 6 && i > 3){
			colour = GColorYellow;
		}
		else{
			colour = GColorGreen;
		}
		#endif
		graphics_context_set_fill_color(ctx, colour);
		GRect get_rect = GRect(22+(i*10), 154, 11, 11);
		graphics_fill_rect(ctx, get_rect, 0, GCornerNone);
		graphics_draw_rect(ctx, get_rect);

		graphics_context_set_fill_color(ctx, GColorBlack);
		GRect inner_rect = GRect(26+(i*10), 158, 3, 3);
		graphics_fill_rect(ctx, inner_rect, 0, GCornerNone);
		graphics_draw_rect(ctx, inner_rect);
	}
}

void main_window_update_settings(Settings new_settings){
	settings = new_settings;

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);

  	layer_set_hidden(graphics_layer, !new_settings.battery_bar);
  	layer_set_hidden(effect_layer_get_layer(invert_layer), !settings.invert);

  	text_layer_set_text_color(time_layer[LAYER_TYPE_MINUTE], settings.hour_colour);
  	text_layer_set_text_color(time_layer[LAYER_TYPE_HOUR], settings.minute_colour);
  	window_set_background_color(main_window, settings.background_colour);
}

void main_window_load(Window *window){
	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	// int result = data_framework_load_settings();
	// APP_LOG(APP_LOG_LEVEL_INFO, "Read %d bytes from settings.", result);
	settings = data_framework_get_settings();

	time_layer[LAYER_TYPE_MINUTE] = text_layer_init(GRect(0, 8, 144, 168), GTextAlignmentCenter, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKED_30)));
	text_layer_set_text(time_layer[LAYER_TYPE_MINUTE], "00");
	text_layer_set_text_color(time_layer[LAYER_TYPE_MINUTE], GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(time_layer[LAYER_TYPE_MINUTE]));

	time_layer[LAYER_TYPE_HOUR] = text_layer_init(GRect(0, 30, 144, 168), GTextAlignmentCenter, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKED_50)));
	text_layer_set_text(time_layer[LAYER_TYPE_HOUR], "00");
	text_layer_set_text_color(time_layer[LAYER_TYPE_HOUR], GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(time_layer[LAYER_TYPE_HOUR]));

	date_layer[LAYER_TYPE_DAY] = text_layer_init(GRect(0, 98, 144, 168), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text(date_layer[LAYER_TYPE_DAY], "Saturday");
	layer_add_child(window_layer, text_layer_get_layer(date_layer[LAYER_TYPE_DAY]));

	date_layer[LAYER_TYPE_DETAILED] = text_layer_init(GRect(0, 120, 144, 168), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text(date_layer[LAYER_TYPE_DETAILED], "10 - 04 - 2015");
	layer_add_child(window_layer, text_layer_get_layer(date_layer[LAYER_TYPE_DETAILED]));

	graphics_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(graphics_layer, graphics_proc);
	layer_add_child(window_layer, graphics_layer);
	layer_set_hidden(graphics_layer, !settings.battery_bar);

	// disconnect_notification = notification_create(window, "Whoops!", "Your Bluetooth connection has been lost, sorry.", gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_DISCONNECTED));
    // reconnect_notification = notification_create(window, "Woohoo!", "You are now connected again!", gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_RECONNECTED));
    // notification_register_push_handler(notification_push_handler);

	invert_layer = effect_layer_create(GRect(0, 0, 144, 168));
	effect_layer_add_effect(invert_layer, effect_invert, NULL);
	layer_add_child(window_layer, effect_layer_get_layer(invert_layer));
	layer_set_hidden(effect_layer_get_layer(invert_layer), !settings.invert);

	// data_framework_register_settings_callback(main_window_update_settings, SETTINGS_CALLBACK_MAIN_WINDOW);

	text_layer_set_text_color(time_layer[LAYER_TYPE_MINUTE], settings.hour_colour);
  	text_layer_set_text_color(time_layer[LAYER_TYPE_HOUR], settings.minute_colour);
	window_set_background_color(window, settings.background_colour);

	BatteryChargeState s = battery_state_service_peek();
	battery_handler(s);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);
  }

void main_window_unload(Window *window){
	// int result = data_framework_save_settings(settings);
	// APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes to settings", result);
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

void main_window_destroy(){
	window_destroy(main_window);
}
