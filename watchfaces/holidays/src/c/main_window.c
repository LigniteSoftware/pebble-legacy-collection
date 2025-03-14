#include <pebble.h>
#include "lignite.h"
#include "main_window.h"

//#define DEBUG_WEATHER
//#define DEBUG_CHRISTMAS_DAY
//#define DEBUG_SPECIFIC_LAYER
//#define DEBUG_DAY
//#define DEBUG_EVENING
//#define DEBUG_NIGHT

#define MAX_LAYER_COUNT 10

typedef enum DayStatus {
	DAY_STATUS_DAY = 0,
	DAY_STATUS_EVENING,
	DAY_STATUS_NIGHT
} DayStatus;
DayStatus dayStatus;

Window *main_window;
Settings settings;
Layer *window_layer;
Notification *disconnect_notification, *reconnect_notification;

Layer *day_background_layer;
TextLayer *temperature_layer, *condition_layer;
TextLayer *time_layer, *date_layer, *day_layer;

GBitmap *landscapeLayerElement;
BitmapLayer *landscapeLayer;

bool isNight = false;
bool noConditionAvailable = false;

const char *condition_texts[] = {
	"Clear",
	"Clear",
	"Windy",
	"Cold",
	"Clouds",
	"Clouds",
	"Haze",
	"Clouds",
	"Rain",
	"Snow",
	"Hail",
	"Cloudy",
	"Stormy",
	"Windows error"
};

//
//BEGIN SNOWFLAKES
//

#define DELAY 50
#define MAX_NUM_SNOWFLAKES 350
#define MAXSPEED 2
#define FUZZYNESS 1
#define MAXSTEPS 10

static Layer *layer;
static uint8_t h[144];
static AppTimer *timer = NULL, *killTimer;
static int screenWidth = 144;
static int screenHeight = 168;

typedef struct {
	GPoint p;
	uint8_t vs, c, r;
} SnowFlake;

SnowFlake snowflakes[350];

static inline void newSnowFlake(int i, bool start) {
	SnowFlake *s = &snowflakes[i];
	s->p.x = rand()%screenWidth;
	s->p.y = start?rand()%h[s->p.x]:0;
	s->vs = (uint8_t)(1 + rand()%MAXSPEED);
	s->c = rand()%MAXSTEPS;
	s->r = (uint8_t)((int)s->vs*2/MAXSPEED);
}

static void initHeights() {
	int i;
	for (i=0; i<screenWidth; i++) {
		h[i] = screenHeight;
	}
}

static void initSnowFlakes() {
	int i;
	for (i=0; i<MAX_NUM_SNOWFLAKES; i++) {
		newSnowFlake(i, true);
	}
}

static inline void reset() {
	initHeights();
	initSnowFlakes();
}

static void updateScreen(Layer *layer, GContext* ctx) {
	AccelData a;
	SnowFlake *s;
	int i, d, w;

	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_fill_color(ctx, GColorWhite);

	if (accel_service_peek(&a) < 0) {
		w = 0;
	} else {
		w = a.x/250;
	}

	for (i=0; i<settings.snowFlakeAmount; i++) {
		s = &snowflakes[i];
		if (s->p.y >= h[s->p.x] && !settings.shakeToFlake) {
			newSnowFlake(i, false);
			for (d=s->p.x-s->r; d<=s->p.x+s->r; d++) {
				//stackSnowFlake(d);
			}
		}

		s->c++;
		if (s->c < MAXSTEPS) {
			d = 0;
		} else {
			s->c = 0;
			d = rand()%FUZZYNESS;
			if (rand()%2) d = -d;
		}

		s->p.x += d + w;
		if (s->p.x < 0) {
			s->p.x += screenWidth;
		}

		if (s->p.x >= screenWidth) {
			s->p.x -= screenWidth;
		}

		s->p.y += s->vs;

		graphics_fill_circle(ctx, s->p, s->r);
	}
}

static void killTimerCallback(){
	APP_LOG(APP_LOG_LEVEL_INFO, "Killing snowflakes. (More accurately, burning.)");
	if(timer != NULL) app_timer_cancel(timer);
	layer_set_hidden(layer, true);
}

static void timerCallback(void *data) {
	layer_mark_dirty(layer);

	timer = app_timer_register(DELAY, timerCallback, NULL);
}

void snowflake_shake(AccelAxisType axis, int32_t direction){
	if(settings.showSnowflakes && settings.shakeToFlake){
		layer_set_hidden(layer, false);
		timer = app_timer_register(DELAY, timerCallback, NULL);
		if(killTimer != NULL) app_timer_cancel(killTimer);
		killTimer = app_timer_register(15000, killTimerCallback, NULL);

		reset();
	}
}

void snowflakes_init(){
	srand(time(NULL));

	GRect window_frame = layer_get_frame(window_get_root_layer(main_window));
	screenHeight = window_frame.size.h;
	screenWidth = window_frame.size.w;

	layer = layer_create(GRect(0, 0, screenWidth, screenHeight));
	layer_set_update_proc(layer, updateScreen);
	layer_add_child(window_get_root_layer(main_window), layer);
	layer_set_hidden(layer, !settings.showSnowflakes);

	reset();

	accel_data_service_subscribe(0, NULL);
	timer = app_timer_register(DELAY, timerCallback, NULL);
}

void snowflakes_deinit(){
	accel_data_service_unsubscribe();
	accel_tap_service_unsubscribe();
	if (timer != NULL) app_timer_cancel(timer);
	if (layer != NULL) layer_destroy(layer);
}

//
//END SNOWFLAKES
//

void main_tick_handler(struct tm *t, TimeUnits units){
	isNight = false;

	static char time_buffer[] = "00:00";
	strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "Nov. 21, 2015";
	strftime(date_buffer, sizeof(date_buffer), "%b. %d, %Y", t);
	text_layer_set_text(date_layer, date_buffer);

	static char day_buffer[] = "Wednesday";
	strftime(day_buffer, sizeof(day_buffer), "%A", t);
	text_layer_set_text(day_layer, day_buffer);

	bool notChristmas = !((t->tm_mday == 25) && (t->tm_mon == 11));
	#ifdef DEBUG_CHRISTMAS_DAY
		notChristmas = false;
	#endif
	gbitmap_destroy(landscapeLayerElement);
	#ifndef DEBUG_SPECIFIC_LAYER
		if(t->tm_hour > 5 && t->tm_hour < 17){
			dayStatus = DAY_STATUS_DAY;
			landscapeLayerElement = gbitmap_create_with_resource(notChristmas ? RESOURCE_ID_IMAGE_DAY : RESOURCE_ID_IMAGE_DAY_PRESENTS);
		}
		#ifdef PBL_COLOR
			else if(t->tm_hour > 16 && t->tm_hour < 21){
				dayStatus = DAY_STATUS_EVENING;
				landscapeLayerElement = gbitmap_create_with_resource(notChristmas ? RESOURCE_ID_IMAGE_EVENING : RESOURCE_ID_IMAGE_EVENING_PRESENTS);
			}
		#endif
		else{
			dayStatus = DAY_STATUS_NIGHT;
			landscapeLayerElement = gbitmap_create_with_resource(notChristmas ? RESOURCE_ID_IMAGE_NIGHT : RESOURCE_ID_IMAGE_NIGHT_PRESENTS);
			isNight = true;
		}
	#else
		#ifdef DEBUG_DAY
			dayStatus = DAY_STATUS_DAY;
			landscapeLayerElement = gbitmap_create_with_resource(notChristmas ? RESOURCE_ID_IMAGE_DAY : RESOURCE_ID_IMAGE_DAY_PRESENTS);
		#endif
		#ifdef DEBUG_EVENING
			dayStatus = DAY_STATUS_EVENING;
			#ifdef PBL_COLOR
				landscapeLayerElement = gbitmap_create_with_resource(notChristmas ? RESOURCE_ID_IMAGE_EVENING : RESOURCE_ID_IMAGE_EVENING_PRESENTS);
			#else
				landscapeLayerElement = gbitmap_create_with_resource(notChristmas ? RESOURCE_ID_IMAGE_NIGHT : RESOURCE_ID_IMAGE_NIGHT_PRESENTS);
			#endif
		#endif
		#ifdef DEBUG_NIGHT
			dayStatus = DAY_STATUS_NIGHT;
			landscapeLayerElement = gbitmap_create_with_resource(notChristmas ? RESOURCE_ID_IMAGE_NIGHT : RESOURCE_ID_IMAGE_NIGHT_PRESENTS);
			isNight = true;
		#endif
	#endif
	bitmap_layer_set_bitmap(landscapeLayer, landscapeLayerElement);
	layer_mark_dirty(day_background_layer);

	// NSLog("Tick %d day status %d", heap_bytes_free(), dayStatus);
}

void bluetooth_handler(bool connected){
	if(connected && settings.btrealert){
		vibes_double_pulse();
	}
	else if(!connected && settings.btdisalert){
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

	static char temperature_buffer[] = "-69°";
	snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", settings.currentWeather.temperature);
	text_layer_set_text(temperature_layer, temperature_buffer);

	APP_LOG(APP_LOG_LEVEL_INFO, "Setting to %s and %d", temperature_buffer, settings.currentWeather.icon);

	GRect frame = layer_get_frame(window_get_root_layer(main_window));
	#ifdef PBL_COLOR
	GRect condition_frame = GRect(94, 20, frame.size.w-102, 16);
	#else
	GRect condition_frame = GRect(94, 30, frame.size.w-102, 16);
	#endif
	#ifdef PBL_ROUND
	condition_frame.origin.x -= 20;
	condition_frame.size.w -= 20;
	#endif
	int halfWidthCondition = condition_frame.size.w/2;
	#ifdef PBL_COLOR
	GRect temperature_frame = GRect(condition_frame.origin.x+halfWidthCondition-4, condition_frame.origin.y+condition_frame.size.h+4, halfWidthCondition+4, condition_frame.size.h);
	#else
	GRect temperature_frame = GRect(condition_frame.origin.x+halfWidthCondition-4, condition_frame.origin.y+condition_frame.size.h+4, halfWidthCondition+4, condition_frame.size.h);
	#endif
	text_layer_set_text_alignment(temperature_layer, GTextAlignmentRight);
	layer_set_frame(text_layer_get_layer(condition_layer), condition_frame);
	layer_set_frame(text_layer_get_layer(temperature_layer), temperature_frame);
	#ifdef PBL_ROUND
	layer_set_frame(text_layer_get_layer(temperature_layer), condition_frame);
	temperature_frame.size.w += 14;
	layer_set_frame(text_layer_get_layer(condition_layer), temperature_frame);
	#endif
	layer_mark_dirty(day_background_layer);

	noConditionAvailable = (settings.currentWeather.icon == 13);

	text_layer_set_text(condition_layer, condition_texts[settings.currentWeather.icon]);

	layer_set_hidden(text_layer_get_layer(temperature_layer), !settings.showWeather);
	layer_set_hidden(text_layer_get_layer(condition_layer), !settings.showWeather);
	if(noConditionAvailable){
		layer_set_hidden(text_layer_get_layer(condition_layer), true);
		GRect currentFrame = layer_get_frame(text_layer_get_layer(temperature_layer));
		text_layer_set_text_alignment(temperature_layer, GTextAlignmentLeft);
		#ifdef PBL_ROUND
		text_layer_set_font(temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
		layer_set_frame(text_layer_get_layer(temperature_layer), GRect(currentFrame.origin.x+42, currentFrame.origin.y+2, currentFrame.size.w, currentFrame.size.h+10));
		#else
		text_layer_set_font(temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
		layer_set_frame(text_layer_get_layer(temperature_layer), GRect(currentFrame.origin.x-12, currentFrame.origin.y-14, currentFrame.size.w+20, currentFrame.size.h+20));
		#endif
	}
	#ifdef DEBUG_WEATHER
	layer_set_hidden(text_layer_get_layer(temperature_layer), false);
	layer_set_hidden(text_layer_get_layer(condition_layer), false);
	#endif
	layer_set_hidden(text_layer_get_layer(day_layer), !settings.day);
	layer_set_hidden(text_layer_get_layer(date_layer), !settings.date);

	#ifdef PBL_COLOR
	text_layer_set_text_color(temperature_layer, settings.textColour);
	text_layer_set_text_color(condition_layer, settings.textColour);
	text_layer_set_text_color(time_layer, settings.textColour);
	text_layer_set_text_color(day_layer, settings.textColour);
	text_layer_set_text_color(date_layer, settings.textColour);
	#else
	text_layer_set_text_color(temperature_layer, isNight ? GColorWhite : GColorBlack);
	text_layer_set_text_color(condition_layer, isNight ? GColorWhite : GColorBlack);
	text_layer_set_text_color(time_layer, isNight ? GColorWhite : GColorBlack);
	text_layer_set_text_color(day_layer, isNight ? GColorWhite : GColorBlack);
	text_layer_set_text_color(date_layer, isNight ? GColorWhite : GColorBlack);
	#endif

	if(settings.shakeToFlake && settings.showSnowflakes){
		accel_tap_service_subscribe(snowflake_shake);
	}
	else{
		accel_tap_service_unsubscribe();
	}
	if(layer){
		layer_set_hidden(layer, !settings.showSnowflakes);
	}

	if(!settings.showSnowflakes){
		accel_tap_service_unsubscribe();
		if (timer != NULL) app_timer_cancel(timer);
	}
	else{
		accel_data_service_subscribe(0, NULL);
		timer = app_timer_register(DELAY+100, timerCallback, NULL);
	}
	initSnowFlakes();
	//reset();

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);

	main_tick_handler(t, SECOND_UNIT);
}

void day_background_proc(Layer *layer, GContext *ctx){
	#ifdef PBL_BW
	graphics_context_set_fill_color(ctx, isNight ? GColorBlack : GColorWhite);
	GRect day_frame = layer_get_frame(text_layer_get_layer(day_layer));
	GRect date_frame = layer_get_frame(text_layer_get_layer(date_layer));
	if(settings.day){
		graphics_fill_rect(ctx, day_frame, 2, GCornersRight);
	}
	if(settings.date){
		graphics_fill_rect(ctx, date_frame, 2, GCornersRight);
	}
	#else
	switch(dayStatus){
		case DAY_STATUS_DAY:
			graphics_context_set_fill_color(ctx, GColorVividCerulean);
			break;
		case DAY_STATUS_EVENING:
			graphics_context_set_fill_color(ctx, GColorCobaltBlue);
			break;
		case DAY_STATUS_NIGHT:
			graphics_context_set_fill_color(ctx, GColorDarkGray);
			break;
	}
	GRect temperature_frame = layer_get_frame(text_layer_get_layer(temperature_layer));
	GRect condition_frame = layer_get_frame(text_layer_get_layer(condition_layer));
	#ifdef PBL_ROUND
	GRect fixed_temperature_frame = GRect(condition_frame.origin.x+6, temperature_frame.origin.y, temperature_frame.size.w*2, temperature_frame.size.h+2);
	GRect fixed_condition_frame = GRect(condition_frame.origin.x+6, condition_frame.origin.y, condition_frame.size.w*2, condition_frame.size.h+2);
	if(noConditionAvailable){
		fixed_condition_frame.origin.y -= 5;
		fixed_condition_frame.origin.x += 2;
		fixed_temperature_frame.size.h -= 3;
		fixed_temperature_frame.origin.x += 2;
	}
	#else
	GRect fixed_temperature_frame = GRect(temperature_frame.origin.x, temperature_frame.origin.y, temperature_frame.size.w*2, temperature_frame.size.h+2);
	GRect fixed_condition_frame = GRect(condition_frame.origin.x, condition_frame.origin.y, condition_frame.size.w*2, condition_frame.size.h+2);
	if(noConditionAvailable){
		fixed_temperature_frame.origin.x = fixed_condition_frame.origin.x+4;
		fixed_condition_frame.origin.x = fixed_temperature_frame.origin.x;
		fixed_temperature_frame.origin.y += 2;
		fixed_temperature_frame.size.w = fixed_condition_frame.size.w;
		fixed_condition_frame.origin.y += 10;
		fixed_temperature_frame.size.h -= 10;
	}
	#endif
	if(settings.showWeather){
		graphics_fill_rect(ctx, fixed_temperature_frame, 2, GCornersLeft);
		graphics_fill_rect(ctx, fixed_condition_frame, 2, GCornersLeft);
	}

	#ifdef DEBUG_WEATHER
	graphics_fill_rect(ctx, fixed_temperature_frame, 2, GCornersLeft);
	if(!noConditionAvailable){
		graphics_fill_rect(ctx, fixed_condition_frame, 2, GCornersLeft);
	}
	#endif

	#endif
}

//Standard is half an hour (1800000)
#define WEATHER_DELAY 1800000

void update_weather(){
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	app_timer_register(WEATHER_DELAY, update_weather, NULL);

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

	landscapeLayer = bitmap_layer_create(frame);
	#ifndef PBL_ROUND
	APP_LOG(APP_LOG_LEVEL_INFO, "Running adjuster");
	layer_set_frame(bitmap_layer_get_layer(landscapeLayer), GRect(frame.origin.x-10, frame.origin.y, frame.size.w+10, frame.size.h));
	#endif
	layer_add_child(window_layer, bitmap_layer_get_layer(landscapeLayer));

	snowflakes_init();

	#ifdef PBL_COLOR
	GRect time_frame = GRect(0, 48, 74, 40);
	#else
	GRect time_frame = GRect(0, 45, 74, 41);
	#endif
	#ifdef PBL_ROUND
	time_frame.origin.x += 3;
	#endif

	NSLog("Available heap: %d", heap_bytes_free());
	NSLog("time_frame GRect(%d, %d, %d, %d)", time_frame.origin.x, time_frame.origin.y, time_frame.size.w, time_frame.size.h);

	GFont bebasRegular = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_38));
	time_layer = text_layer_init(time_frame, PBL_IF_BW_ELSE(GTextAlignmentCenter, GTextAlignmentRight), bebasRegular);
	NSLog("Is the layer null %d is window layer null %d", (time_layer == NULL), (window_layer == NULL));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	//GFont date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_REGULAR_12));

	day_background_layer = layer_create(frame);
	layer_set_update_proc(day_background_layer, day_background_proc);
	layer_add_child(window_layer, day_background_layer);

	GRect date_frame = GRect(time_frame.origin.x, time_frame.origin.y+time_frame.size.h, time_frame.size.w, 15);
	date_layer = text_layer_init(date_frame, PBL_IF_BW_ELSE(GTextAlignmentCenter, GTextAlignmentRight), fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text(date_layer, "Nov. 1, 2015");
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	GRect day_frame = GRect(date_frame.origin.x, date_frame.origin.y+date_frame.size.h, date_frame.size.w, 18);
	day_layer = text_layer_init(day_frame, PBL_IF_BW_ELSE(GTextAlignmentCenter, GTextAlignmentRight), fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text(day_layer, "Saturday");
	layer_add_child(window_layer, text_layer_get_layer(day_layer));

	#ifdef PBL_COLOR
	GRect condition_frame = GRect(94, 20, frame.size.w-102, 16);
	#else
	GRect condition_frame = GRect(94, 30, frame.size.w-102, 16);
	#endif
	#ifdef PBL_ROUND
	condition_frame.origin.x -= 20;
	condition_frame.size.w -= 20;
	#endif
	condition_layer = text_layer_init(condition_frame, GTextAlignmentRight, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_text(condition_layer, "Clear");
	layer_add_child(window_layer, text_layer_get_layer(condition_layer));

	int halfWidthCondition = condition_frame.size.w/2;
	#ifdef PBL_COLOR
	GRect temperature_frame = GRect(condition_frame.origin.x+halfWidthCondition, condition_frame.origin.y+condition_frame.size.h+4, halfWidthCondition, condition_frame.size.h);
	#else
	GRect temperature_frame = GRect(condition_frame.origin.x+halfWidthCondition+4, condition_frame.origin.y+condition_frame.size.h+4, halfWidthCondition, condition_frame.size.h);
	#endif
	temperature_layer = text_layer_init(temperature_frame, GTextAlignmentRight, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_text(temperature_layer, "-69°");
	layer_add_child(window_layer, text_layer_get_layer(temperature_layer));

	#ifdef PBL_ROUND
	layer_set_frame(text_layer_get_layer(temperature_layer), condition_frame);
	temperature_frame.size.w += 14;
	layer_set_frame(text_layer_get_layer(condition_layer), temperature_frame);
	#endif
/*
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
*/
	tick_timer_service_subscribe(SECOND_UNIT, main_tick_handler);
	data_framework_register_settings_callback(main_window_settings_callback, SETTINGS_CALLBACK_MAIN_WINDOW);
	main_window_settings_callback(data_framework_get_settings());
	app_timer_register(WEATHER_DELAY, update_weather, NULL);
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
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_deinit(){
	window_destroy(main_window);
}
