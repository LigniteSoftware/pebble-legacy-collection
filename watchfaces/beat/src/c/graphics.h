#pragma once

typedef enum PulseMode {
    PULSE_MODE_BATTERY = 0,
    PULSE_MODE_BLUETOOTH,
    PULSE_MODE_RANDOM,
} PulseMode;

typedef enum PulseAnimationMode {
    PULSE_ANIMATION_MODE_SECONDLY = 0,
    PULSE_ANIMATION_MODE_MINUTELY,
    PULSE_ANIMATION_MODE_ON_CHANGE
} PulseAnimationMode;

typedef struct Pulse {
    bool lastUp;
    uint8_t numberOfPoints;
    uint8_t maxLength;
    int16_t currentLength;
    GColor frontColour;
    GColor backColour;
    GPoint frontPoint;
    GPoint backPoint;
    GPoint allPoints[40];
    PulseMode mode;
	PulseAnimationMode animationMode;
} Pulse;

void graphics_animate();
void graphics_set_settings(Settings new_settings);
void graphics_bluetooth_handler(bool connected);
void graphics_battery_handler(BatteryChargeState state);
void graphics_create(Layer *window_layer);
void graphics_destroy();
