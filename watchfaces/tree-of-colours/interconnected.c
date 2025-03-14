#include <pebble.h>
#include <math.h>
#include "graphics.h"
#include "lignite_data_framework.h"
#include "effectlayer/effect_layer.h"
#include "main_window.h"

Settings graphics_settings;
Branch *minute_branches[60];
GPoint reserved_coordinates[60];
Layer *graphics_layer;

GPoint new_coord(int i, GPoint initial, GPoint final){
    int slopeX = initial.x-final.x;
    int slopeY = initial.y-final.y;

    //APP_LOG(APP_LOG_LEVEL_INFO, "Slope of: %d/%d", slopeY, slopeX);

    return GPoint(initial.x-slopeX+(rand() * 4), initial.y-slopeY+(rand() * 4));

    //return ((o > 84) ? o-i-(i*(rand() % 2)) : o+i+(i*(rand() % 2)));
}

Branch *branch_create(){
    Branch *branch = malloc(sizeof(Branch));
    branch->colour = GColorFromRGB(rand() % 255, rand() % 255, rand() % 255);
    uint8_t x = rand() % 144;
    uint8_t y = rand() % 168;
    int BUFFER = 15;
    GPoint main_circle_location = MAIN_CIRCLE_LOCATION;
    if(x > main_circle_location.x-MAIN_CIRCLE_RADIUS && x < main_circle_location.x+MAIN_CIRCLE_RADIUS){
        while(y > main_circle_location.y-(MAIN_CIRCLE_RADIUS+BUFFER) && y < main_circle_location.y+(MAIN_CIRCLE_RADIUS+BUFFER)){
            y = rand() % 168;
        }
    }
    branch->final_coordinate = GPoint(x, y);

    int amount_of_hits = rand() % 8;
    while(amount_of_hits < 1){
        amount_of_hits = rand() % 8;
    }
    branch->amount_of_hits = amount_of_hits;

    for(int i = 0; i < amount_of_hits; i++){
        branch->hit_coordinates[i] = new_coord(i, branch->final_coordinate, MAIN_CIRCLE_LOCATION);
    }

    return branch;
}

void graphics_proc(Layer *layer, GContext *ctx){
    graphics_context_set_stroke_width(ctx, 2);
    for(int i = 0; i < 60; i++){
        Branch *branch = branch_create();
        graphics_context_set_stroke_color(ctx, branch->colour);
        for(int i = 0; i < branch->amount_of_hits; i++){
            graphics_draw_line(ctx, (i == 0) ? branch->final_coordinate : branch->hit_coordinates[i-1], branch->hit_coordinates[i]);
        }
        //graphics_draw_line(ctx, MAIN_CIRCLE_LOCATION, branch->final_coordinate);
    }
}

void graphics_settings_callback(Settings new_settings){
    graphics_settings = new_settings;
}

void graphics_init(Window *window){
    Layer *window_layer = window_get_root_layer(window);

    graphics_settings = data_framework_get_settings();

    graphics_settings_callback(graphics_settings);

    graphics_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(graphics_layer, graphics_proc);
    layer_add_child(window_layer, graphics_layer);
}

void graphics_deinit(){
    
}