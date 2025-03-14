#pragma once
#define DATE_LAYERS 3
#define TIME_LAYERS 4

typedef enum DateLayer {
	DATE_LAYER_DAY_1 = 0,
	DATE_LAYER_DAY_2,
	DATE_LAYER_DAY_3
} DateLayer;

typedef enum TimeLayer {
	TIME_LAYER_HOUR_1,
	TIME_LAYER_HOUR_2,
	TIME_LAYER_MINUTE_1,
	TIME_LAYER_MINUTE_2
} TimeLayer;

void main_window_init();
Window *main_window_get_window();
void main_window_deinit();
void main_window_update_settings(Settings new_settings);
