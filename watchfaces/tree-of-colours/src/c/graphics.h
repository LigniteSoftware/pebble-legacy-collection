#pragma once
#define AMOUNT_OF_SECTIONS_IN_SESSION 3

#ifdef PBL_ROUND
#define CENTER GPoint(90, 90)
#define WINDOW_SIZE GSize(180, 180)
#define MAIN_CIRCLE_RADIUS 40
#else
#define CENTER GPoint(72, 84)
#define WINDOW_SIZE GSize(144, 168)
#define MAIN_CIRCLE_RADIUS 40
#endif

typedef struct Branch {
	int amount_of_hits;
	GPoint final_coordinate;
	GPoint hit_coordinates[16];
	GColor colour;
} Branch;

void graphics_init(Layer *window_layer);
void graphics_deinit();
void graphics_set_time(struct tm *t);
