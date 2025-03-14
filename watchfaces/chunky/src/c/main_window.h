#pragma once

typedef enum LayerType {
	LAYER_TYPE_DAY = 0,
	LAYER_TYPE_DETAILED = 1,
	LAYER_TYPE_HOUR = 0,
	LAYER_TYPE_MINUTE = 1
} LayerType;

void main_window_init();
Window *main_window_get_window();
void main_window_destroy();
void main_window_update_settings(Settings new_settings);
