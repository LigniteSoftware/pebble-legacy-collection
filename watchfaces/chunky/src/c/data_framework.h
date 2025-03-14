#pragma once

#define SETTINGS_KEY 0
#define AMOUNT_OF_SETTINGS_CALLBACKS 1

typedef enum AppKey {
	APP_KEY_BTDISALERT = 0,
	APP_KEY_BTREALERT,
	APP_KEY_HEADER_1,
	APP_KEY_INVERT,
	APP_KEY_BATTERY_BAR,
	APP_KEY_HEADER_2,
	APP_KEY_MINUTE_COLOUR,
	APP_KEY_HOUR_COLOUR,
	APP_KEY_BACKGROUND_COLOUR
} AppKey;

typedef struct Settings {
	bool invert;
	bool btdisalert;
	bool btrealert;
	bool battery_bar;
	GColor hour_colour;
	GColor background_colour;
	GColor minute_colour;
} Settings;

typedef enum SettingsCallback {
	SETTINGS_CALLBACK_MAIN_WINDOW = 0
} SettingsCallback;

typedef void (*SettingsChangeCallback)(Settings settings);

void process_tuple(Tuple *t);
void data_framework_inbox(DictionaryIterator *iter, void *context);
void data_framework_load_default_settings();
Settings data_framework_get_settings();

extern Settings data_framework_local_settings;
extern SettingsChangeCallback settings_callbacks[AMOUNT_OF_SETTINGS_CALLBACKS];
