#pragma once

typedef struct BatteryBarState {
	int currentAngle;
	int requiredAngle;
	int currentRotation;
	bool isAnimatingCharge;
	BatteryChargeState chargeState;
} BatteryBarState;

#ifdef PBL_ROUND
void graphics_shake_handler();
#endif
void graphics_tick_handler(struct tm *t);
void graphics_bluetooth_handler(bool connected);
void graphics_create(Window *window);
void graphics_destroy();
