#include <pebble.h>
#include <math.h>
#include "lignite.h"
#include "graphics.h"
#include "main_window.h"

Settings graphics_settings;
Branch *minute_branches[60];
GPoint reserved_coordinates[60];
Layer *graphics_layer;
int graphics_minute = 0;

void inject_new_coords(Branch *branch){
    GPoint new_coord = branch->final_coordinate;
    GPoint origin_coord = CENTER;

    GPoint coords[branch->amount_of_hits];
    for(int i = 0; i < branch->amount_of_hits; i++){
        int x_hit = 0, y_hit = 0;

        int distance_to_new_x = origin_coord.x-new_coord.x;
        int distance_to_new_y = origin_coord.y-new_coord.y;

        x_hit = (distance_to_new_x/branch->amount_of_hits)*(i+1);
        y_hit = (distance_to_new_y/branch->amount_of_hits)*(i+1);

        GPoint new = branch->final_coordinate;

        coords[i] = GPoint(new.x+x_hit, new.y+y_hit);

        //APP_LOG(APP_LOG_LEVEL_INFO, "Possible coord: GPoint(%d, %d) for GPoint(%d, %d)", coords[i].x, coords[i].y, new.x, new.y);
        //APP_LOG(APP_LOG_LEVEL_INFO, "---");
    }

    GPoint previousPoint = branch->final_coordinate;
    for(int i = 0; i < (branch->amount_of_hits*2); i+=2){
        GPoint firstCoord = GPoint(coords[i].x, previousPoint.y);
        GPoint secondCoord = GPoint(coords[i].x, coords[i].y);

        branch->hit_coordinates[i] = firstCoord;
        branch->hit_coordinates[i+1] = secondCoord;
        previousPoint = branch->hit_coordinates[i+1];
/*
        int pepperoni = 0;
        while(previousPoint.y > 168 || previousPoint.y < 0){
            previousPoint = branch->hit_coordinates[i+1-pepperoni];
            pepperoni++;
        }
*/
        //APP_LOG(APP_LOG_LEVEL_INFO, "Previous point: GPoint(%d, %d)", previousPoint.x, previousPoint.y);
        //APP_LOG(APP_LOG_LEVEL_INFO, "Hit:  GPoint(%d, %d) and next: GPoint(%d, %d)", branch->hit_coordinates[i].x, branch->hit_coordinates[i].y, branch->hit_coordinates[i+1].x, branch->hit_coordinates[i+1].y);
    }
    //APP_LOG(APP_LOG_LEVEL_INFO, "---");
}

Branch *branch_create(){
    Branch *branch = malloc(sizeof(Branch));
    #ifdef PBL_COLOR
        if(graphics_settings.randomize_colour){
            branch->colour = GColorFromRGB(rand() % 255, rand() % 255, rand() % 255);
        }
        else{
            branch->colour = graphics_settings.custom_colour[rand() % 3];
        }
    #else
        branch->colour = GColorWhite;
    #endif
    uint8_t x = rand() % WINDOW_SIZE.w;
    uint8_t y = rand() % WINDOW_SIZE.h;
    int BUFFER = 0;
    GPoint main_circle_location = CENTER;
    bool hadToRunX = false;
    while(x > (main_circle_location.x-(MAIN_CIRCLE_RADIUS+(BUFFER))) && x < (main_circle_location.x+MAIN_CIRCLE_RADIUS+(BUFFER))){
        x = rand() % WINDOW_SIZE.w;
        hadToRunX = true;
    }
    if(!hadToRunX){
        while(y > main_circle_location.y-(MAIN_CIRCLE_RADIUS+BUFFER) && y < main_circle_location.y+(MAIN_CIRCLE_RADIUS+BUFFER)){
            y = rand() % WINDOW_SIZE.h;
        }
        /*
        if(y > 84 && (rand() % 2) == 1){
            y = y/2;
        }
        */
    }

    branch->final_coordinate = GPoint(x, y);

    int amount_of_hits = rand() % 8;
    while(amount_of_hits < 3){
        amount_of_hits = rand() % 8;
    }
    branch->amount_of_hits = amount_of_hits;

    inject_new_coords(branch);

    return branch;
}

void graphics_set_time(struct tm *t){
    graphics_minute = t->tm_min;
    layer_mark_dirty(graphics_layer);
}

void generate_new_branch_session(){
    for(int i = 0; i < 60; i++){
        if(minute_branches[i] != NULL){
            free(minute_branches[i]);
        }
        minute_branches[i] = branch_create();
    }
}

void graphics_proc(Layer *layer, GContext *ctx){
    graphics_context_set_stroke_width(ctx, graphics_settings.width);
    generate_new_branch_session();
    for(int i = 0; i < graphics_minute; i++){
        Branch *branch = minute_branches[i];
        graphics_context_set_stroke_color(ctx, branch->colour);
        for(int i = 0; i < branch->amount_of_hits-1; i++){
            graphics_draw_line(ctx, ((i == 0) ? branch->final_coordinate : branch->hit_coordinates[i]), branch->hit_coordinates[i+1]);
            //APP_LOG(APP_LOG_LEVEL_INFO, "Drawing line from GPoint(%d, %d) to GPoint(%d, %d) for i = %d", ((i == 0) ? branch->final_coordinate.x : branch->hit_coordinates[i-1].x), ((i == 0) ? branch->final_coordinate.y : branch->hit_coordinates[i-1].y), branch->hit_coordinates[i].x, branch->hit_coordinates[i].y, i);
        }
        //APP_LOG(APP_LOG_LEVEL_INFO, "-------------");
    }
}

void graphics_settings_callback(Settings new_settings){
    graphics_settings = new_settings;
}

void graphics_init(Layer *window_layer){
    srand(time(NULL));

    data_framework_register_settings_callback(graphics_settings_callback, SETTINGS_CALLBACK_GRAPHICS);
    graphics_settings = data_framework_get_settings();

    graphics_settings_callback(graphics_settings);

    graphics_layer = layer_create(layer_get_frame(window_layer));
    layer_set_update_proc(graphics_layer, graphics_proc);
    layer_add_child(window_layer, graphics_layer);
}

void graphics_deinit(){
    layer_destroy(graphics_layer);
}
