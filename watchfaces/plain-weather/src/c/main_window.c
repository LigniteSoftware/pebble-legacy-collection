#include <pebble.h>
#include <math.h>
#include "lignite.h"
#include <ctype.h>
#include "main_window.h"

Window *main_window;
Settings settings;

GFont bebas, droid_sans_regular, droid_sans_bold, weather_icon;

TextLayer *time_layer, *date_layer, *day_layer, *temperature_layer, *weather_layer;
Layer *window_layer, *graphics_layer;

Notification *disconnect_notification, *reconnect_notification;

int currentBatteryLevel = 0;

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
	"P", //NA
};

char lower_to_upper(char c) {
  return (c>='a' && c<='z')?c&0xdf:c;
}

void main_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(time_buffer, sizeof(time_buffer), "%H:%M", t);
	}
	else{
		strftime(time_buffer, sizeof(time_buffer), "%I:%M", t);
	}
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "Tucker && Memes go tog...";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(day_layer, date_buffer);

	static char day_buffer[10];
	strftime(day_buffer, sizeof(day_buffer), "%A", t);
	for(int i = 0, x = strlen(day_buffer); i < x; i++){
		day_buffer[i] = toupper((unsigned char) day_buffer[i]);
	}
	text_layer_set_text(date_layer, day_buffer);
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
	currentBatteryLevel = state.charge_percent;
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

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
	main_tick_handler(t, MINUTE_UNIT);

	static char temperature_buffer[] = "69 degrees";
	snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", settings.currentWeather.temperature);
	text_layer_set_text(temperature_layer, temperature_buffer);

	text_layer_set_text(weather_layer, climacon_condition_codes[settings.currentWeather.icon]);

	layer_set_hidden(text_layer_get_layer(day_layer), !settings.day);
	layer_set_hidden(text_layer_get_layer(date_layer), !settings.date);
	layer_set_hidden(text_layer_get_layer(weather_layer), !settings.weather);
	layer_set_hidden(text_layer_get_layer(temperature_layer), !settings.weather);

	#ifdef PBL_COLOR
	text_layer_set_text_color(day_layer, settings.accentColour);
	text_layer_set_text_color(date_layer, settings.accentColour);
	text_layer_set_text_color(weather_layer, settings.accentColour);
	text_layer_set_text_color(temperature_layer, settings.accentColour);
	text_layer_set_text_color(time_layer, settings.accentColour);
	window_set_background_color(main_window, settings.backgroundColour);
	#endif

	battery_handler(battery_state_service_peek());
}

void update_weather(){
	if(!settings.weather){
		return;
	}
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

void graphics_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, settings.accentColour);
	if(settings.batteryBar){
		#ifdef PBL_ROUND
		GRect frame = grect_inset(layer_get_frame(layer), GEdgeInsets(7));
		graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 3, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(365/10 * currentBatteryLevel/10));
		#else
		graphics_fill_rect(ctx, GRect(0, 166, (currentBatteryLevel*144)/100, 2), 0, GCornerNone);
		#endif
	}
}

void main_window_load(Window *window){
	GRect frame = layer_get_frame(window_get_root_layer(window));
	window_layer = layer_create(frame);
	layer_add_child(window_get_root_layer(window), window_layer);

	setlocale(LC_ALL, "");

	bebas = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_60));
	weather_icon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_ICON_45));
	droid_sans_regular = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_REGULAR_14));
	droid_sans_bold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_BOLD_18));

	GRect nil = GRect(0, 0, 0, 0);

	const int fontSize = 62;
	//Create shit

	date_layer = text_layer_init(nil, GTextAlignmentCenter, droid_sans_regular);
	text_layer_set_text_color(date_layer, GColorBlack);

	day_layer = text_layer_init(nil, GTextAlignmentCenter, droid_sans_regular);
	text_layer_set_text_color(day_layer, GColorBlack);

	weather_layer = text_layer_init(nil, GTextAlignmentRight, weather_icon);
	text_layer_set_text_color(weather_layer, GColorBlack);
	text_layer_set_text(weather_layer, "C");

	temperature_layer = text_layer_init(nil, GTextAlignmentLeft, droid_sans_bold);
	text_layer_set_text_color(temperature_layer, GColorBlack);
	text_layer_set_text(temperature_layer, "18°");

	time_layer = text_layer_init(nil, GTextAlignmentCenter, bebas);
	text_layer_set_text_color(time_layer, GColorBlack);

	graphics_layer = layer_create(frame);
	layer_set_update_proc(graphics_layer, graphics_proc);
	layer_add_child(window_layer, graphics_layer);

	#ifdef PBL_ROUND
	int weatherAdjust = 0;
	int timeAdjust = 0;
	int dateAdjust = 0;
	int dayAdjust = 0;
	#else
	int weatherAdjust = -10;
	int timeAdjust = -2;
	int dateAdjust = 3;
	int dayAdjust = 3;
	#endif

	GRect time_frame = GRect(0, frame.size.h/2 - 28 + timeAdjust, frame.size.w, fontSize);
	GRect date_frame = GRect(0, time_frame.origin.y+time_frame.size.h + dateAdjust,  frame.size.w, 16);
	GRect day_frame = GRect(0, date_frame.origin.y+date_frame.size.h + dayAdjust, frame.size.w, 16);
	GRect weather_icon_frame = GRect(4, 16 + weatherAdjust, frame.size.w/2 + 4, 54);
	GRect weather_text_frame = GRect(weather_icon_frame.origin.x+weather_icon_frame.size.w+8, 10 + weather_icon_frame.size.h/2 + weatherAdjust, frame.size.w/2 - 8, 24);
	layer_set_frame(text_layer_get_layer(time_layer), time_frame);
	layer_set_frame(text_layer_get_layer(date_layer), date_frame);
	layer_set_frame(text_layer_get_layer(day_layer), day_frame);
	layer_set_frame(text_layer_get_layer(weather_layer), weather_icon_frame);
	layer_set_frame(text_layer_get_layer(temperature_layer), weather_text_frame);

	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	layer_add_child(window_layer, text_layer_get_layer(temperature_layer));
	layer_add_child(window_layer, text_layer_get_layer(weather_layer));
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
