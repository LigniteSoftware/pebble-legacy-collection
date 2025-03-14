#pragma once
#define DATE_LAYER_DAY 0
#define DATE_LAYER_DETAILED 1

void main_window_init();
Window *main_window_get_window();
void main_window_deinit();
void main_window_update_settings(Settings new_settings);
