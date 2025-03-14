#pragma once

#define NSLog(fmt, args...)                                \
  app_log(APP_LOG_LEVEL_INFO, __FILE_NAME__, __LINE__, fmt, ## args);

#define APP_NAME "Timezones"

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
	APP_KEY_HEADER_1,
	APP_KEY_NAME_1,
	APP_KEY_COLOUR_1,
	APP_KEY_ANALOGUE_1,
	APP_KEY_HEADER_2,
	APP_KEY_TIMEZONE_2,
	APP_KEY_SUBTRACT_HOUR,
	APP_KEY_NAME_2,
	APP_KEY_COLOUR_2,
	APP_KEY_ANALOGUE_2,

    APP_KEY_GS_DATE_FORMAT = 2000
} AppKey;

typedef enum SettingsCallback {
	SETTINGS_CALLBACK_MAIN_WINDOW = 0
} SettingsCallback;

typedef struct v1Settings {
	bool btdisalert;
	bool btrealert;
	bool analogue[2];
	bool subtract_hour;
	char names[2][10];
	GColor colour[2];
	int32_t alt_timezone;
} v1Settings;

typedef struct Settings {
	bool btdisalert;
	bool btrealert;
	bool analogue[2];
	bool subtract_hour;
	char names[2][10];
	GColor colour[2];
	int32_t alt_timezone;

    char dateFormat[1][20];
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
