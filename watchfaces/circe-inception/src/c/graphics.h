#pragma once
#include "fctx/fctx.h"
#include "fctx/ffont.h"

typedef struct State {
	FPoint center;
	FPoint origin;
	int32_t rotation;

	Window* window;
	Layer* layer;
	FFont* font;
	AppTimer* timer_handle;
	uint32_t timer_timeout;
} State;

typedef struct BatteryBarState {
	int currentAngle;
	int requiredAngle;
	int currentRotation;
	bool isAnimatingCharge;
	BatteryChargeState chargeState;
} BatteryBarState;

typedef struct ClockHandState {
	GPoint innerGPoint;
	GPoint outerGPoint;
	int32_t currentAngle;
	int32_t distanceToTravel;
	bool isAnimating;
} ClockHandState;

void demo_init(State* state);
void demo_update(void* data);
void demo_draw(State* state, Layer* layer, GContext* ctx);

void graphics_tick_handler(struct tm *t);
void graphics_battery_handler(BatteryChargeState charge_state);
void graphics_bluetooth_handler(bool connected);
void graphics_init(Layer *root_layer);
void graphics_deinit();
