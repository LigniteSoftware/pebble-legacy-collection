#pragma once

#define NSLog(fmt, args...)                                \
  app_log(APP_LOG_LEVEL_INFO, __FILE_NAME__, __LINE__, fmt, ## args);

#define NSWarn(fmt, args...)                                \
  app_log(APP_LOG_LEVEL_WARNING, __FILE_NAME__, __LINE__, fmt, ## args);

#define NSError(fmt, args...)                                \
  app_log(APP_LOG_LEVEL_ERROR, __FILE_NAME__, __LINE__, fmt, ## args);

#define APP_NAME "Modulite"

#define LOG_ALL false

#define SETTINGS_KEY 0
#define STORAGE_VERSION_KEY 100
#define USER_WAS_1X_KEY 101
#define AMOUNT_OF_SETTINGS_CALLBACKS 2

typedef enum AppKey {
	//Do not delete these
    APP_KEY_SECURITY_EMAIL = 1000,
    APP_KEY_SECURITY_ACCESS_CODE = 1001,
    APP_KEY_SECURITY_CHECKSUM = 1002,
    //Do not delete these ^

    APP_KEY_BTDISALERT = 0,
    APP_KEY_BTREALERT,
    APP_KEY_DATA_TYPE_TOP,
    APP_KEY_ACCENT_COLOUR_TOP,
    APP_KEY_DATA_TYPE_RIGHT,
    APP_KEY_ACCENT_COLOUR_RIGHT,
    APP_KEY_DATA_TYPE_BOTTOM,
    APP_KEY_ACCENT_COLOUR_BOTTOM,
    APP_KEY_DATA_TYPE_LEFT,
    APP_KEY_ACCENT_COLOUR_LEFT,
    APP_KEY_BACKGROUND_COLOUR,
    APP_KEY_HOUR_HAND_COLOUR,
    APP_KEY_MINUTE_HAND_COLOUR,
    APP_KEY_OUTER_CIRCLE_COLOUR,
    APP_KEY_ANALOGUE_HANDS,
    APP_KEY_SECOND_HAND,
    APP_KEY_OUTER_CIRCLE_TYPE,
    APP_KEY_WEATHER_DATA_1,
    APP_KEY_WEATHER_DATA_2,
    APP_KEY_SHAKE_TO_HIDE_HANDS,
    APP_KEY_IMPERIAL_MEASUREMENTS,
    APP_KEY_SECOND_HAND_COLOUR,
    APP_KEY_OUTER_CIRCLE_BACKGROUND_COLOUR,
    APP_KEY_HOUR_HAND,
    APP_KEY_MINUTE_HAND,

    APP_KEY_GS_DATE_FORMAT = 2000,

	APP_KEY_ICON = 100,
	APP_KEY_TEMPERATURE = 101
} AppKey;

typedef enum SettingsCallback {
	SETTINGS_CALLBACK_MAIN_WINDOW = 0,
    SETTINGS_CALLBACK_GRAPHICS
} SettingsCallback;

typedef enum WeatherStatus {
	WEATHER_STATUS_CLEAR_DAY = 0,
	WEATHER_STATUS_CLEAR_NIGHT,
	WEATHER_STATUS_WINDY,
	WEATHER_STATUS_COLD, //wut
	WEATHER_STATUS_PARTIALLY_CLOUDY_DAY,
	WEATHER_STATUS_PARTIALLY_CLOUDY_NIGHT,
	WEATHER_STATUS_HAZE,
	WEATHER_STATUS_LIGHT_CLOUDS,
	WEATHER_STATUS_RAINY,
	WEATHER_STATUS_SNOWING,
	WEATHER_STATUS_HAIL,
	WEATHER_STATUS_CLOUDY,
	WEATHER_STATUS_STORM,
	WEATHER_STATUS_NOT_AVAILABLE_OR_ERROR
} WeatherStatus;

typedef struct Weather {
	int16_t temperature;
	WeatherStatus icon;
} Weather;

typedef enum {
    OuterCircleTypeBattery = 0,
    OuterCircleTypeDefault,
    OuterCircleTypeMinute,
    OuterCircleTypeAnalogueLines,
    OuterCircleTypeAnalogueCircles
} OuterCircleType;

typedef struct Settings {
	bool btdisalert;
	bool btrealert;

    uint8_t dataTypes[4];
    GColor dataColours[4];

    GColor backgroundColour;
    GColor hourHandColour;
    GColor minuteHandColour;
    GColor secondHandColour;
    GColor outerCircleColour;
    GColor outerCircleBackgroundColour;

    bool analogueHands;
    bool secondHand;

    OuterCircleType outerCircleType;

    Weather currentWeather;

    bool imperialMeasurements;
    bool hideArmsOnShake;

    char dateFormat[1][20];
    bool minuteHand;
    bool hourHand;
} Settings;

typedef struct v2Settings {
	bool btdisalert;
	bool btrealert;

    uint8_t dataTypes[4];
    GColor dataColours[4];

    GColor backgroundColour;
    GColor hourHandColour;
    GColor minuteHandColour;
    GColor secondHandColour;
    GColor outerCircleColour;

    bool analogueHands;
    bool secondHand;

    OuterCircleType outerCircleType;

    Weather currentWeather;

    bool imperialMeasurements;
    bool hideArmsOnShake;
} v2Settings;

typedef struct v1Settings {
	bool btdisalert;
	bool btrealert;

    uint8_t dataTypes[4];
    GColor dataColours[4];

    GColor backgroundColour;
    GColor hourHandColour;
    GColor minuteHandColour;
    GColor outerCircleColour;

    bool analogueHands;
    bool secondHand;

    uint8_t outerCircleType;

    Weather currentWeather;
} v1Settings;

typedef void (*SettingsChangeCallback)(Settings settings);

void data_framework_setup();
void data_framework_finish();
void process_tuple(Tuple *t);
void data_framework_inbox_dropped(AppMessageResult reason, void *context);
void data_framework_inbox(DictionaryIterator *iter, void *context);
void data_framework_log_settings_struct(Settings settings);
Settings data_framework_get_settings();
unsigned int data_framework_hex_string_to_uint(char const* hexstring);
void data_framework_register_settings_callback(SettingsChangeCallback callback, SettingsCallback callbackIdentity);

extern Settings data_framework_local_settings;
extern SettingsChangeCallback settings_callbacks[AMOUNT_OF_SETTINGS_CALLBACKS];
