#pragma once

#define NSLog(fmt, args...)                                \
  app_log(APP_LOG_LEVEL_INFO, __FILE_NAME__, __LINE__, fmt, ## args);

#define APP_NAME "Equalizer"

#define LOG_ALL false

#define SETTINGS_KEY 0
#define STORAGE_VERSION_KEY 100
#define USER_WAS_1X_KEY 101
#define AMOUNT_OF_SETTINGS_CALLBACKS 1

typedef enum AppKey {
	//Do not delete these
    APP_KEY_SECURITY_EMAIL = 1000,
    APP_KEY_SECURITY_ACCESS_CODE = 1001,
    APP_KEY_SECURITY_CHECKSUM = 1002,
    //Do not delete these ^

	APP_KEY_BTDISALERT = 0,
	APP_KEY_BTREALERT,
	APP_KEY_SHAKE_TO_ANIMATE,
	APP_KEY_MODE,
	APP_KEY_ORGANIZE_COLOURS,
	APP_KEY_SHOW_DAY,
	APP_KEY_SHOW_DATE,
	APP_KEY_TIME_COLOUR,
	APP_KEY_BACKGROUND_COLOUR,
	APP_KEY_BATTERY_BAR_COLOUR,

    APP_KEY_GS_DATE_FORMAT = 2000
} AppKey;

typedef enum SettingsCallback {
	SETTINGS_CALLBACK_MAIN_WINDOW = 0
} SettingsCallback;

typedef struct v1Settings {
	bool btdisalert;
	bool btrealert;
	bool organizeColours;
	bool showDate;
	bool showDay;
	bool shakeToAnimate;
	uint8_t mode;
	GColor timeColour;
	GColor backgroundColour;
	GColor batteryBarColour;
} v1Settings;

typedef struct Settings {
	bool btdisalert;
	bool btrealert;
	bool organizeColours;
	bool showDate;
	bool showDay;
	bool shakeToAnimate;
	uint8_t mode;
	GColor timeColour;
	GColor backgroundColour;
	GColor batteryBarColour;

    char dateFormat[1][16];
} Settings;

typedef void (*SettingsChangeCallback)(Settings settings);

void data_framework_setup();
void data_framework_finish();
void process_tuple(Tuple *t);
void data_framework_inbox_dropped(AppMessageResult reason, void *context);
void data_framework_inbox(DictionaryIterator *iter, void *context);
Settings data_framework_get_settings();
unsigned int data_framework_hex_string_to_uint(char const* hexstring);
void data_framework_register_settings_callback(SettingsChangeCallback callback, SettingsCallback callbackIdentity);

extern Settings data_framework_local_settings;
extern SettingsChangeCallback settings_callbacks[AMOUNT_OF_SETTINGS_CALLBACKS];
