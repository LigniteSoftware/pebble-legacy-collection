#pragma once
#define CUSTOM_FONT RESOURCE_ID_FONT_HTT_26
#define CUSTOM_FONT_SMALL RESOURCE_ID_FONT_HTT_14

void main_window_init();
Window *main_window_get_window();
void main_window_deinit();
void main_window_update_settings(Settings new_settings);
