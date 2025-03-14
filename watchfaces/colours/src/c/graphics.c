#include <pebble.h>
#include <math.h>
#include "lignite.h"
#include "graphics.h"
#include "effectlayer/effect_layer.h"

Layer *graphics_layer;
EffectLayer *effect_layer;
Window *window_ptr;

Settings graphics_settings;
Rectangle *rectangles[144];
AppTimer *animation_timer, *constant_animation_timer;
GColor last_colour;

int animation_showing[2] = { 0, 0 };
int minute, hour;
int amount_of_rectangles = 0;
int lastRand = 0;
int16_t colour_session[AMOUNT_OF_SECTIONS_IN_SESSION];
int16_t colour_build_ups[AMOUNT_OF_SECTIONS_IN_SESSION];
bool rotate_layer_created = false;
bool colour_section_taken[AMOUNT_OF_SECTIONS_IN_SESSION];

GColor get_colour_based_on_section(int section, int x_pos, int width){
    colour_build_ups[colour_session[section]] += ((255/48)*width);
    if(colour_build_ups[colour_session[section]] > 215){
        colour_build_ups[colour_session[section]] = 215;
    }
    if(rand() % 3 == 2){
        colour_build_ups[rand() % 3] += 20;
    }
    return GColorFromRGB(colour_build_ups[0] + (rand() % 20), colour_build_ups[1] + (rand() % 20), colour_build_ups[2] + (rand() % 20));
}

Rectangle *rectangle_create(int x_pos){
    Rectangle *rect = malloc(sizeof(Rectangle));
    if(graphics_settings.randomize_width){
        int randNum = rand() % 6;
        rect->width = graphics_settings.default_width += randNum;
        rect->location = GPoint(x_pos+randNum, 0);
        lastRand = randNum;
    }
    else{
        rect->width = graphics_settings.default_width;
        rect->location = GPoint(x_pos, 0);
    }
    if(graphics_settings.organize_colours){
        update_colour_randomize:;
        if(x_pos < 48){
            rect->colour = get_colour_based_on_section(0, x_pos, rect->width);
        }
        else if(x_pos > 47 && x_pos < 96){
            rect->colour = get_colour_based_on_section(1, x_pos, rect->width);
        }
        else{
            rect->colour = get_colour_based_on_section(2, x_pos, rect->width);
        }
        if(gcolor_equal(rect->colour, last_colour) || gcolor_equal(rect->colour, GColorWhite) || gcolor_equal(rect->colour, GColorLightGray)){
            goto update_colour_randomize;
        }
        last_colour = rect->colour;
    }
    else{
        rect->colour = GColorFromRGB((rand() % 255), (rand() % 255), (rand() % 255));
        while(gcolor_equal(rect->colour, GColorWhite) || gcolor_equal(rect->colour, GColorLightGray) || gcolor_equal(rect->colour, last_colour)){
            rect->colour = GColorFromRGB((rand() % 255), (rand() % 255), (rand() % 255));
        }
        last_colour = rect->colour;
    }
    return rect;
}

bool section_taken(int i){
    return colour_section_taken[i];
}

void generate_colour_session(){
    for(int i = 0; i < amount_of_rectangles; i++){
        free(rectangles[i]);
    }
    for(int i = 0; i < (144/graphics_settings.default_width); i++){
        rectangles[i] = rectangle_create(i*graphics_settings.default_width+lastRand);
    }
    amount_of_rectangles = 144/graphics_settings.default_width;
    for(int i = 0; i < AMOUNT_OF_SECTIONS_IN_SESSION; i++){
        if(i == 0){
            colour_session[i] = rand() % AMOUNT_OF_SECTIONS_IN_SESSION;
            colour_section_taken[colour_session[i]] = true;
        }
        else if(i > 0){
            int i1 = 0, timesRun = 0;
            while(section_taken(i1) && timesRun != 20){
                timesRun++;
                i1 = rand() % AMOUNT_OF_SECTIONS_IN_SESSION;
            }
            colour_section_taken[i1] = true;
            colour_session[i] = i1;
        }
        colour_build_ups[i] = 40;
    }
}

void graphics_proc(Layer *layer, GContext *ctx){
    generate_colour_session();
    for(int i = 0; i < amount_of_rectangles; i++){
        if(rectangles[i] == NULL){
            APP_LOG(APP_LOG_LEVEL_WARNING, "%d is null", i);
            return;
        }
        graphics_context_set_fill_color(ctx, rectangles[i]->colour);
        graphics_fill_rect(ctx, GRect(rectangles[i]->location.x, 0, graphics_settings.default_width, 168), 0, GCornerNone);
    }
}

void constant_animation_fire(){
    layer_mark_dirty(graphics_layer);
    constant_animation_timer = app_timer_register(250, constant_animation_fire, NULL);
}

void graphics_settings_callback(Settings new_settings){
    graphics_settings = new_settings;
    graphics_settings.organize_colours = true;
    if(new_settings.constant_animation && !constant_animation_timer){
        constant_animation_timer = app_timer_register(250, constant_animation_fire, NULL);
    }
    else{
        app_timer_cancel(constant_animation_timer);
    }
}

void graphics_init(Layer *window_layer, Window *window){
    window_ptr = window;
    graphics_settings = data_framework_get_settings();

    amount_of_rectangles = (144/graphics_settings.default_width);

    srand(time(NULL));

    data_framework_register_settings_callback(graphics_settings_callback, SETTINGS_CALLBACK_GRAPHICS);

    graphics_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(graphics_layer, graphics_proc);
    layer_add_child(window_layer, graphics_layer);
    layer_mark_dirty(graphics_layer);

    effect_layer = effect_layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_layer, effect_layer_get_layer(effect_layer));
    layer_mark_dirty(effect_layer_get_layer(effect_layer));

    graphics_settings_callback(graphics_settings);
}

void graphics_deinit(){
    layer_destroy(graphics_layer);
    for(int i = 0; i < 144; i++){
        if(rectangles[i]){
            free(rectangles[i]);
        }
    }
}
