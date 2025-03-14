#pragma once

#define NSLog(fmt, args...)                                \
  app_log(APP_LOG_LEVEL_INFO, __FILE_NAME__, __LINE__, fmt, ## args);

#define APP_NAME "Graph"

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
    APP_KEY_TOP_TIME,
	APP_KEY_DATE,
    APP_KEY_BATTERY_PERCENTAGE,
    APP_KEY_HOUR,
    APP_KEY_MINUTE,
    APP_KEY_BACKGROUND_COLOUR,
    APP_KEY_ACCENT_COLOUR,
    APP_KEY_TEXT_COLOUR,

    APP_KEY_GS_DATE_FORMAT = 2000
} AppKey;

typedef enum SettingsCallback {
	SETTINGS_CALLBACK_MAIN_WINDOW = 0,
	SETTINGS_CALLBACK_GRAPHICS
} SettingsCallback;

typedef struct v1Settings {
	bool btdisalert;
	bool btrealert;
	bool topTime;
    bool date;
    bool batteryBar;
    bool minuteBar;
    bool hourBar;
    GColor backgroundColour;
    GColor accentColour;
    GColor textColour;
} v1Settings;

typedef struct Settings {
	bool btdisalert;
	bool btrealert;
	bool topTime;
    bool date;
    bool batteryBar;
    bool minuteBar;
    bool hourBar;
    GColor backgroundColour;
    GColor accentColour;
    GColor textColour;

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
