#pragma once

void main_window_init();
Window *main_window_get_window();
void main_window_deinit();
void main_window_shake_handler(AccelAxisType axis, int32_t direction);
void main_window_battery_handler(BatteryChargeState charge_state);
void main_window_bluetooth_handler(bool connected);
void health_display_data();
void main_window_update_settings(Settings new_settings);
void main_window_animate_layers(bool out);
