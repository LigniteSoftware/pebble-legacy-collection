#pragma once
#define AMOUNT_OF_SECTIONS_IN_SESSION 3

typedef struct Rectangle {
	GColor colour;
	GPoint location;
	uint8_t width;
} Rectangle;

typedef enum BootAnimation {
	BOOT_ANIMATION_OFF = 0,
	BOOT_ANIMATION_QUICK,
	BOOT_ANIMATION_SLOW
} BootAnimation;

void graphics_init(Layer *window_layer, Window *window);
void graphics_deinit();
