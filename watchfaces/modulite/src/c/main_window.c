#include <pebble.h>
#include "lignite.h"
#include "main_window.h"
#include "graphics.h"
#include "data.h"

Window *main_window;
Layer *window_layer;
Settings settings;
DataLayer *module_layer[4];

uint32_t healthData[7];
bool newDataAvailable = false;
bool boot = true;

Notification *disconnect_notification, *reconnect_notification;

void main_tick_handler(struct tm *t, TimeUnits units){
	for(int i = 0; i < 4; i++){
		if(data_get_general_type(module_layer[i]->dataType) == GeneralDataTypeTime){
			//NSLog("CALLING TIME FOR %d", i);
			TimeData *timeData = (TimeData*)module_layer[i]->data;
			timeData->updateCallback(module_layer[i], t);
		}
	}

	if(newDataAvailable){
		health_display_data();
		newDataAvailable = false;
	}
	else{
		//NSLog("No new data available :(");
	}

	graphics_tick_handler(t);

	//#pragma message ("TRIAL IS DISABLED, users will be able to use it for free")
}

void health_display_data(){
	for(int i = 0; i < 4; i++){
		DataLayer *dataLayer = module_layer[i];
		if(data_get_general_type(module_layer[i]->dataType) == GeneralDataTypeHealth){
			HealthData *data = (HealthData*)dataLayer->data;
			data->value = healthData[data->healthMetric];
			layer_mark_dirty(dataLayer->root_layer);
		}
	}
	//vibes_double_pulse();
}

void health_update_data_for_metric(HealthMetric metric){
	time_t end = time(NULL);
	time_t start = time_start_of_today();

	HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
	if(mask == HealthServiceAccessibilityMaskAvailable) {
		healthData[metric] = health_service_sum_today(metric);
		//NSLog("Got value %ld for metric %d", healthData[metric], metric);
		newDataAvailable = true;
	}
	else {
		APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
	}
}

void health_reload_sleep(){
	health_update_data_for_metric(HealthMetricSleepSeconds);
	health_update_data_for_metric(HealthMetricSleepRestfulSeconds);
}

void health_reload_movement(){
	health_update_data_for_metric(HealthMetricStepCount);
	health_update_data_for_metric(HealthMetricActiveSeconds);
	health_update_data_for_metric(HealthMetricWalkedDistanceMeters);
	health_update_data_for_metric(HealthMetricActiveKCalories);
	health_update_data_for_metric(HealthMetricRestingKCalories);
}

void health_reload_everything(){
	health_reload_sleep();
	health_reload_movement();
}

void health_handler(HealthEventType event, void *context) {
	newDataAvailable = true;

	switch(event) {
		case HealthEventSignificantUpdate:
			health_reload_everything();
			break;
		case HealthEventMovementUpdate:
			health_reload_movement();
			break;
		case HealthEventSleepUpdate:
			health_reload_sleep();
			break;
		default:
			break;
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

DataType set_data_from_setting(DataLocation location){
	DataLayer *dataLayer = module_layer[location];

	switch(data_get_general_type(dataLayer->dataType)){
		case GeneralDataTypeWeather:
		case GeneralDataTypeHidden:{
			//Nothing, yet
			break;
		}
		case GeneralDataTypeTime:{
			uint8_t fixedSetting = settings.dataTypes[location] - DataTypeTime;
			TimeData *timeData = (TimeData*)dataLayer->data;
			timeData->timeFormat = fixedSetting;
			return (DataType)fixedSetting;
		}
		case GeneralDataTypeHealth:{
			uint8_t fixedSetting = settings.dataTypes[location] - DataTypeStepCount;
			HealthData *healthData = (HealthData*)dataLayer->data;
			healthData->healthMetric = fixedSetting;
			return (DataType)fixedSetting;
		}
		case GeneralDataTypeNumieric:{
			break;
		}
	}
	return DataTypeHidden;
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
}

void main_window_settings_callback(Settings new_settings){
	settings = new_settings;

	#ifdef PBL_COLOR
	window_set_background_color(main_window, new_settings.backgroundColour);
	#endif

	//NSLog("Second hand %d", settings.secondHand);
	tick_timer_service_subscribe(settings.secondHand ? SECOND_UNIT : MINUTE_UNIT, main_tick_handler);

	for(int i = 0; i < 4; i++){
		//NSLog("Got data type %d for %d", new_settings.dataTypes[i], i);
		if(module_layer[i]){
			data_destroy_data_layer(module_layer[i]);
		}
		module_layer[i] = data_create_data_layer(new_settings.dataTypes[i], i);
		set_data_from_setting(i); //reminder: this returns the datatype as an int (but also does it for you automatically)

		GeneralDataType type = data_get_general_type(new_settings.dataTypes[i]);
		switch(type){
			case GeneralDataTypeWeather:{
				WeatherData *data = (WeatherData*)module_layer[i]->data;
				data->currentWeather = settings.currentWeather;
				break;
			}
			case GeneralDataTypeTime:{
				data_reload_time_fonts(module_layer[i]);
				break;
			}
			default:
				break;
		}

		layer_add_child(window_layer, module_layer[i]->root_layer);
	}

	boot = true;

	health_reload_everything();
	main_window_battery_handler(battery_state_service_peek());
	main_window_bluetooth_handler(bluetooth_connection_service_peek());

	//Philipp was crying because there was a "bug" where the module layers would be added on top of everything else,
	//resulting in the graphics layer being below them. I've never seen a man so disappointed about a bug not being fixed,
	//so the function is now named in his honor. Praise Philipp.
	make_philipp_stop_crying(window_layer);

	settings.hideArmsOnShake ? accel_tap_service_subscribe(main_window_shake_handler) : accel_tap_service_unsubscribe();

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
	main_tick_handler(t, MINUTE_UNIT);

	data_settings_update_callback(settings);
}

void main_window_battery_handler(BatteryChargeState charge_state){
	graphics_battery_handler(charge_state);
	for(int i = 0; i < 4; i++){
		if(module_layer[i]->dataType == DataTypeBatteryLevel){
			NumericData *numericData = (NumericData*)module_layer[i]->data;
			numericData->value = charge_state.charge_percent;
			snprintf(numericData->buffer[0], sizeof(numericData->buffer[0]), "%d%%", numericData->value);
			layer_mark_dirty(module_layer[i]->root_layer);
		}
	}
}

void main_window_bluetooth_handler(bool connected){
	if(!boot){
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

	for(int i = 0; i < 4; i++){
		if(module_layer[i]->dataType == DataTypeBluetoothStatus){
			NumericData *numericData = (NumericData*)module_layer[i]->data;
			numericData->value = connected;
			snprintf(numericData->buffer[0], sizeof(numericData->buffer[0]), connected ? "On" : "Off");
			layer_mark_dirty(module_layer[i]->root_layer);
		}
	}

	boot = false;
}

void main_window_shake_handler(AccelAxisType axis, int32_t direction){
	graphics_set_hidden(5000);
}

void main_window_load(Window *window){
	// return;
	GRect frame = layer_get_frame(window_get_root_layer(window));
	window_layer = layer_create(frame);
	layer_add_child(window_get_root_layer(window), window_layer);

	setlocale(LC_ALL, "");

	graphics_create(window);

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

	bluetooth_connection_service_subscribe(main_window_bluetooth_handler);
	battery_state_service_subscribe(main_window_battery_handler);
	tick_timer_service_subscribe(MINUTE_UNIT, main_tick_handler);
	health_service_events_subscribe(health_handler, NULL);

	data_framework_register_settings_callback(main_window_settings_callback, SETTINGS_CALLBACK_MAIN_WINDOW);

	Settings currentSettings = data_framework_get_settings();
	main_window_settings_callback(currentSettings);

	app_timer_register(1800000, update_weather, NULL);
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
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_deinit(){
	window_destroy(main_window);
}
