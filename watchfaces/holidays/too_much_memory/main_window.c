#include <pebble.h>
#include <lignite.h>
#include "main_window.h"

#define MAX_LAYER_COUNT 10

Window *main_window;
Settings settings;
Layer *window_layer;
Notification *disconnect_notification, *reconnect_notification;

GBitmap *landscapeLayerElements[MAX_LAYER_COUNT];
BitmapLayer *landscapeLayers[MAX_LAYER_COUNT];

#define DAY_LAYER_COUNT 9
const uint32_t day_resources[DAY_LAYER_COUNT] = {
	RESOURCE_ID_IMAGE_DAY_SKY,
	RESOURCE_ID_IMAGE_DAY_LAND_1,
	RESOURCE_ID_IMAGE_DAY_LAND_2,
	RESOURCE_ID_IMAGE_DAY_LAND_3,
	RESOURCE_ID_IMAGE_DAY_LAND_4,
	RESOURCE_ID_IMAGE_DAY_LAND_5,
	RESOURCE_ID_IMAGE_DAY_LAND_6,
	RESOURCE_ID_IMAGE_DAY_SUN,
	RESOURCE_ID_IMAGE_DAY_TREE
};

#define EVENING_LAYER_COUNT 10
const uint32_t evening_resources[EVENING_LAYER_COUNT] = {
	RESOURCE_ID_IMAGE_EVENING_SKY,
	RESOURCE_ID_IMAGE_EVENING_LAND_1,
	RESOURCE_ID_IMAGE_EVENING_LAND_2,
	RESOURCE_ID_IMAGE_EVENING_LAND_3,
	RESOURCE_ID_IMAGE_EVENING_LAND_4,
	RESOURCE_ID_IMAGE_EVENING_LAND_5,
	RESOURCE_ID_IMAGE_EVENING_LAND_6,
	RESOURCE_ID_IMAGE_EVENING_MOON,
	RESOURCE_ID_IMAGE_EVENING_TREE,
	RESOURCE_ID_IMAGE_EVENING_LIGHTS
};

#define NIGHT_LAYER_COUNT 10
const uint32_t night_resources[NIGHT_LAYER_COUNT] = {
	RESOURCE_ID_IMAGE_NIGHT_SKY,
	RESOURCE_ID_IMAGE_NIGHT_LAND_1,
	RESOURCE_ID_IMAGE_NIGHT_LAND_2,
	RESOURCE_ID_IMAGE_NIGHT_LAND_3,
	RESOURCE_ID_IMAGE_NIGHT_LAND_4,
	RESOURCE_ID_IMAGE_NIGHT_LAND_5,
	RESOURCE_ID_IMAGE_NIGHT_LAND_6,
	RESOURCE_ID_IMAGE_NIGHT_MOON,
	RESOURCE_ID_IMAGE_NIGHT_TREE,
	RESOURCE_ID_IMAGE_NIGHT_LIGHTS
};

void main_tick_handler(struct tm *t, TimeUnits units){
	APP_LOG(APP_LOG_LEVEL_INFO, "Bytes free: %d", heap_bytes_free());
	GRect windowFrame = layer_get_frame(window_get_root_layer(main_window));
	for(int i = 0; i < EVENING_LAYER_COUNT; i++){
		if(landscapeLayers[i]){
			bitmap_layer_destroy(landscapeLayers[i]);
		}

		landscapeLayers[i] = bitmap_layer_create(windowFrame);
		bitmap_layer_set_bitmap(landscapeLayers[i], landscapeLayerElements[i]);
		bitmap_layer_set_compositing_mode(landscapeLayers[i], GCompOpSet);
		layer_add_child(window_layer, bitmap_layer_get_layer(landscapeLayers[i]));
	}
	APP_LOG(APP_LOG_LEVEL_INFO, "Bytes free at the end: %d", heap_bytes_free());
	/*
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
	*/
}

void bluetooth_handler(bool connected){
	if(!connected && settings.btdisalert){
		vibes_long_pulse();
	}
	if(connected && settings.btrealert){
		vibes_double_pulse();
	}
}

void battery_handler(BatteryChargeState state){
	//TODO: Remove
}

void notification_push_handler(bool pushed){
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

void main_window_load(Window *window){
	GRect frame = layer_get_frame(window_get_root_layer(window));
	window_layer = layer_create(frame);
	layer_add_child(window_get_root_layer(window), window_layer);

	for(int i = 0; i < EVENING_LAYER_COUNT; i++){
		landscapeLayerElements[i] = gbitmap_create_with_resource(evening_resources[i]);
	}

	tick_timer_service_subscribe(MINUTE_UNIT, main_tick_handler);

	data_framework_register_settings_callback(main_window_settings_callback, SETTINGS_CALLBACK_MAIN_WINDOW);

	main_window_settings_callback(data_framework_get_settings());

	app_timer_register(1800000, update_weather, NULL);
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
