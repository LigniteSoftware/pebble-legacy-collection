#include <pebble.h>
#include "lignite.h"
#include "graphics.h"
#include "main_window.h"

#define AMOUNT_OF_BANNED_COLOURS 5

Layer *graphics_layer, *graphics_battery_layer, *graphics_window_layer;
AppTimer *graphics_animation_timer;
Settings graphics_settings;

bool graphics_connected, super_debug, boot_animation;
BatteryChargeState graphics_charge_state;

Bar *bars[60];

int16_t colour_session[AMOUNT_OF_SECTIONS_IN_SESSION];
int16_t colour_build_ups[AMOUNT_OF_SECTIONS_IN_SESSION];
bool colour_section_taken[AMOUNT_OF_SECTIONS_IN_SESSION];

AnimationType animation_type;
int animation_frame = 0;

void generate_colour_session(){
    for(int i = 0; i < AMOUNT_OF_SECTIONS_IN_SESSION; i++){
        if(i == 0){
            colour_session[i] = rand() % AMOUNT_OF_SECTIONS_IN_SESSION;
            colour_section_taken[colour_session[i]] = true;
        }
        else if(i > 0){
            int i1 = 0, timesRun = 0;
            while(colour_section_taken[i1] && timesRun != 20){
                timesRun++;
                i1 = rand() % AMOUNT_OF_SECTIONS_IN_SESSION;
            }
            colour_section_taken[i1] = true;
            colour_session[i] = i1;
        }
    }
}

void clear_colour_session(){
    for(int i = 0; i < AMOUNT_OF_SECTIONS_IN_SESSION; i++){
        colour_build_ups[i] = 0;
        colour_section_taken[i] = false;
        colour_session[i] = 0;
    }
}

#ifdef PBL_COLOR
GColor get_colour_based_on_section(int section, int width){
    colour_build_ups[colour_session[section]] += ((255/48)*width);
    if(colour_build_ups[colour_session[section]] > 215){
        colour_build_ups[colour_session[section]] = 215;
    }
    if(rand() % 3 == 2){
        colour_build_ups[rand() % 3] += 20;
    }
    return GColorFromRGB(colour_build_ups[0] + (rand() % 20), colour_build_ups[1] + (rand() % 20), colour_build_ups[2] + (rand() % 20));
}

bool is_banned_colour(GColor colour){
    GColor banned_colours[AMOUNT_OF_BANNED_COLOURS] = {
        GColorOxfordBlue, GColorBlack, GColorDarkGray, GColorBulgarianRose, GColorImperialPurple
    };
    for(int i = 0; i < AMOUNT_OF_BANNED_COLOURS; i++){
        if(gcolor_equal(colour, banned_colours[i])){
            return true;
        }
    }
    return false;
}

#endif

void generate_new_colours(){
    clear_colour_session();
    generate_colour_session();
    for(int i = 0; i < 60; i++){
        #ifdef PBL_COLOR
        bars[i]->colour = GColorFromRGB(rand() % 255, rand() % 255, rand() % 255);
        if(graphics_settings.organizeColours){
            bars[i]->colour = get_colour_based_on_section(i/20, i/2);
        }
        #endif
    }
}

Bar *bar_create(uint8_t location){
    Bar *new_bar = malloc(sizeof(Bar));
    #ifdef PBL_COLOR
    update_colour_randomize:
    new_bar->colour = GColorFromRGB(rand() % 255, rand() % 255, rand() % 255);
    if(graphics_settings.organizeColours){
        new_bar->colour = get_colour_based_on_section(location/20, location/2);
    }
    if(is_banned_colour(new_bar->colour)){
        goto update_colour_randomize;
    }
    #else
    new_bar->colour = GColorWhite;
    #endif
    if(location < 30){
        uint8_t number = (30-location)*2 + (rand() % 10);
        uint8_t addition = location/3;
        bool shortened = true;
        if(rand() % 2 == 0 || 83-number-(number/4) < 4){
            shortened = false;
        }
        new_bar->rect = GRect(22+(location*3)+addition, number+ (shortened ? number/4 : 0), 2, 83-number-(shortened ? number/4 : 0));
    }
    else{
        uint8_t number = (60-location)*2 + (rand() % 10) + 88 + 10;
        uint8_t addition = (location-30)/3;
        bool shortened = true;
        if(rand() % 2 == 0 || number-88-(number/8) < 4){
            shortened = false;
        }
        new_bar->rect = GRect(22+((location-30)*3)+addition, 88, 2, number-88-(shortened ? number/8 : 0));
        new_bar->bottom = true;
    }
    return new_bar;
}

void graphics_battery_proc(Layer *layer, GContext *ctx){
    #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, graphics_settings.batteryBarColour);
    graphics_context_set_stroke_color(ctx, graphics_settings.batteryBarColour);
    #else
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    #endif
    for(int i = 0; i < 10; i++){
        GRect rectangle = GRect(22+(i*10), 84, 9, 3);
        if(graphics_charge_state.charge_percent/10 > i){
            graphics_fill_rect(ctx, rectangle, 0, GCornersAll);
        }
        graphics_draw_rect(ctx, rectangle);
    }
}

void graphics_proc(Layer *layer, GContext *ctx){
    for(int i = 0; i < 60; i++){
        Bar *bar = bars[i];
        if(bar == NULL){
            return;
        }

        graphics_context_set_fill_color(ctx, bar->colour);
        GRect to_draw;
        if(bar->bottom){
            to_draw = GRect(bar->rect.origin.x, bar->rect.origin.y, bar->rect.size.w, bar->currentHeight);
        }
        else{
            to_draw = GRect(bar->rect.origin.x, 83-bar->currentHeight, bar->rect.size.w, bar->currentHeight);
        }
        graphics_fill_rect(ctx, to_draw, 0, GCornersAll);
    }
}

void graphics_generate_new_session(bool destroy_all){
    for(int i = 0; i < 60; i++){
        if(destroy_all && bars[i]){
            free(bars[i]);
        }
        bars[i] = bar_create(i);
    }
}

void graphics_animation_roll_started(Animation *animation, void *context){}
void graphics_animation_roll_stopped(Animation *animation, bool finished, void *context){}
void graphics_animation_roll_setup(Animation *animation){}

void graphics_animation_roll_update(Animation *animation, const AnimationProgress distance_normalized){
    Bar *bar = animation_get_context(animation);
    long percent = (distance_normalized*100)/ANIMATION_NORMALIZED_MAX;
    if(!boot_animation){
        percent = ((ANIMATION_NORMALIZED_MAX-distance_normalized)*100)/ANIMATION_NORMALIZED_MAX;
    }
    long newHeight = percent*bar->rect.size.h;
    newHeight = newHeight/100;
    /*
    int barNumber = 0;
    for(int i = 0; i < 60; i++){
        if(bars[i] == bar){
            barNumber = i;
            break;
        }
    }
    */
    if(percent-bar->previousPercent < -10){
        //APP_LOG(APP_LOG_LEVEL_WARNING, "Bar %ld jumped by %ld to %ld!", barNumber, percent-bar->previousPercent, bar->currentHeight);
    }
    else{
        bar->currentHeight = (int)newHeight;
    }
    bar->previousPercent = percent;
    layer_mark_dirty(graphics_layer);
}

void graphics_animation_roll_teardown(Animation *animation){
    Bar *bar = animation_get_context(animation);
    if(boot_animation && bar == bars[59]){
        boot_animation = false;
    }
    animation_destroy(animation);
    AnimationImplementation *implementation = (AnimationImplementation*)animation_get_implementation(animation);
    free(implementation);
}

void graphics_animate_custom_frame(){
    if(animation_type == ANIMATION_TYPE_ROLL){
        for(int i = 0; i < 60; i++){
            Animation *animation = animation_create();
            animation_set_duration(animation, 300 + rand() % 100);
            animation_set_delay(animation, i*30);
            AnimationImplementation *implementation = malloc(sizeof(AnimationImplementation));
            implementation->setup = graphics_animation_roll_setup;
            implementation->update = graphics_animation_roll_update;
            implementation->teardown = graphics_animation_roll_teardown;
            AnimationHandlers handlers = (AnimationHandlers){
                .started = graphics_animation_roll_started,
                .stopped = graphics_animation_roll_stopped
            };
            animation_set_handlers(animation, handlers, bars[i]);
            animation_set_implementation(animation, implementation);
            animation_schedule(animation);
        }
    }
    else if(animation_type == ANIMATION_TYPE_MUSIC){

    }
}

void graphics_animate(bool boot){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Animating!");
    graphics_animation_timer = app_timer_register(1500, graphics_animate_custom_frame, NULL);
    boot_animation = !boot;
    //animation_type = graphics_settings.mode;
    animation_type = ANIMATION_TYPE_ROLL;
}

void graphics_shake_animate(AccelAxisType axis, int32_t direction){
    animation_unschedule_all();
    graphics_animate(false);
}

void graphics_set_settings(Settings new_settings){
    graphics_settings = new_settings;
    if(new_settings.shakeToAnimate){
        accel_tap_service_subscribe(graphics_shake_animate);
    }
    else{
        accel_tap_service_unsubscribe();
    }
}

void graphics_bluetooth_handler(bool connected){
    graphics_connected = connected;
}

void graphics_battery_handler(BatteryChargeState state){
    graphics_charge_state = state;
    layer_mark_dirty(graphics_battery_layer);
}

void graphics_create(Layer *window_layer){
    graphics_window_layer = window_layer;

    graphics_connected = bluetooth_connection_service_peek();
    graphics_charge_state = battery_state_service_peek();

    graphics_settings.organizeColours = true;

    generate_colour_session();
    graphics_generate_new_session(false);

    srand(time(NULL));

    graphics_battery_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(graphics_battery_layer, graphics_battery_proc);
    layer_add_child(window_layer, graphics_battery_layer);

    graphics_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(graphics_layer, graphics_proc);
    layer_add_child(window_layer, graphics_layer);

    graphics_animate(true);
}

void graphics_destroy(){
    layer_destroy(graphics_layer);
}
