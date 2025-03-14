#pragma once
#define AMOUNT_OF_SECTIONS_IN_SESSION 3

typedef enum AnimationType {
    ANIMATION_TYPE_MUSIC,
    ANIMATION_TYPE_ROLL,
    ANIMATION_TYPE_EXPAND_AND_SLAM,
    ANIMATION_TYPE_RANDOM
} AnimationType;

typedef enum BarAnimation {
    BAR_ANIMATION_CONSTANT,
    BAR_ANIMATION_MINUTELY,
    BAR_ANIMATION_ON_CHANGE,
} BarAnimation;

typedef enum BarMode {
    BAR_MODE_RANDOM,
    BAR_MODE_ORGANIZE_COLOURS,
    BAR_MODE_CUSTOM_COLOURS
} BarMode;

typedef struct Bar {
    GRect rect;
    GColor colour;
    bool bottom;
    int16_t previousPercent;
    int16_t height;
    int16_t currentHeight;
} Bar;

void graphics_animate(bool boot);
void graphics_set_settings(Settings new_settings);
void graphics_bluetooth_handler(bool connected);
void graphics_battery_handler(BatteryChargeState state);
void graphics_animate_custom_frame();
void graphics_create(Layer *window_layer);
void graphics_destroy();
