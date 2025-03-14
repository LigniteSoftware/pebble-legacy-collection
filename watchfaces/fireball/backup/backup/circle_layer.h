#pragma once
#define CIRCLE_RADIUS 60
#define BATTERY_CIRCLE_RADIUS 52
#include <math.h>
#include "data_framework.h"

#ifdef PBL_ROUND
#define HALF_SCREEN_WIDTH 90
#define HALF_SCREEN_HEIGHT 90
#else
#define HALF_SCREEN_WIDTH 72
#define HALF_SCREEN_HEIGHT 84
#endif

typedef enum CircleType {
	CIRCLE_TYPE_MINUTE = 0,
	CIRCLE_TYPE_BATTERY = 1
} CircleType;

typedef enum CircleAlgorithm {
	CIRCLE_ALGORITHM_SCALING = 1,
	CIRCLE_ALGORITHM_LINEAR = 0
} CircleAlgorithm;

typedef enum AnimationType {
	ANIM_TYPE_REDRAW = 0,
	ANIM_TYPE_GROW_AND_SLIDE = 1,
	ANIM_TYPE_GROW = 2
} AnimationType;

typedef struct Circle {
	GColor colour;
	GPoint location;
	CircleType type;
	uint8_t animation_count;
	uint8_t new_position;
	uint8_t count;
	uint8_t size;
} Circle;

void circle_layer_update_settings(Settings new_settings);
void circle_layer_set_tm(struct tm *t, TimeUnits units);
Circle *circle_create();
void circle_layer_init(Window *window);
Layer *circle_layer_get_layer();
void circle_layer_update();
void circle_set_location(Circle *circle, int minute);
void circle_set_size(Circle *circle, int minute);
int circle_layer_get_battery_circle_size(int percentage, Settings settings);
void circle_layer_add_circle(Circle *circle);
int circle_get_position(int minute, bool x);
int battery_circle_get_position(int percentage, bool x);
void circle_travel(Circle *circle, uint8_t new_position, int delay, bool invert);
void circle_layer_battery_handler(BatteryChargeState state);
Circle *get_circle(int location, CircleType type);
bool is_booted();
int circle_layer_get_circle_size(int minute, Settings settings);
void circle_layer_OBLITERATE();
