#pragma once

#ifdef PBL_ROUND
#define CENTRE GPoint(90, 90)
#define WINDOW_SIZE GSize(180, 180)
#define IS_ROUND true
#else
#define CENTRE GPoint(72, 84)
#define WINDOW_SIZE GSize(144, 168)
#define IS_ROUND false
#endif

void graphics_tick_handler(struct tm *t);
void graphics_battery_handler(BatteryChargeState charge_state);
void make_philipp_stop_crying(Layer *window_layer);
void graphics_set_hidden(int length);
void graphics_create(Window *window);
void graphics_destroy();
