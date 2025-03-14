#pragma once
#define AMOUNT_OF_ANGLES 3

typedef enum AngleType {
	ANGLE_TYPE_WEEKDAY = 0,
	ANGLE_TYPE_MINUTE,
	ANGLE_TYPE_BATTERY
} AngleType;

void main_window_init();
Window *main_window_get_window();
void main_window_deinit();
void main_window_update_settings(Settings new_settings);
