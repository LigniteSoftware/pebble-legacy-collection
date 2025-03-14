#include <pebble.h>
#include "lignite.h"
#include "graphics.h"

#define BASE_Y 54

Layer *graphics_layer, *graphics_background_layer, *graphics_old_background_layer;
Pulse *graphics_pulse, *background_pulse, *old_background_pulse;
AppTimer *graphics_animation_timer;
Settings graphics_settings;

bool graphics_connected;
BatteryChargeState graphics_charge_state;

bool pulse_generate_spike(Pulse *pulse){
    switch(pulse->mode){
        case PULSE_MODE_BLUETOOTH:
            if(graphics_connected){
                if(rand() % 3 == 0){
                    return true;
                }
            }
            return false;
        case PULSE_MODE_BATTERY:
            if(rand() % ((graphics_charge_state.charge_percent > 70 ? 15 : 10) - (graphics_charge_state.charge_percent/10)) == 0 || graphics_charge_state.is_charging){
                return true;
            }
            return false;
        case PULSE_MODE_RANDOM:
            if(rand() % 4 == 0){
                return true;
            }
            return false;
    }
    return false;
}

GPoint pulse_generate_new_point(Pulse *pulse, uint8_t index){
    GPoint point;
    point.x = index * 10;
    if(index > 0){
        if(pulse->allPoints[index-1].y > BASE_Y || pulse->allPoints[index-1].y < BASE_Y){
            point.y = BASE_Y;
        }
        else{
            if(pulse_generate_spike(pulse)){
                if(!pulse->lastUp){
                    if(pulse->mode == PULSE_MODE_BATTERY){
                        point.y = BASE_Y + (graphics_charge_state.charge_percent/3) + rand() % 5;
                    }
                    else if(pulse->mode == PULSE_MODE_BLUETOOTH){
                        point.y = BASE_Y + 24;
                    }
                    else{
                        point.y = BASE_Y + 10 + rand() % 20;
                    }
                    pulse->lastUp = true;
                }
                else{
                    if(pulse->mode == PULSE_MODE_BATTERY){
                        point.y = BASE_Y - (graphics_charge_state.charge_percent/3) - rand() % 5;
                    }
                    else if(pulse->mode == PULSE_MODE_BLUETOOTH){
                        point.y = BASE_Y - 24;
                    }
                    else{
                        point.y = BASE_Y - 10 - rand() % 20;
                    }
                    pulse->lastUp = false;
                }
            }
            else{
                point.y = BASE_Y;
            }
        }
    }
    else{
        point.y = BASE_Y;
    }
    return point;
}

Pulse *pulse_create(){
    Pulse *new_pulse = malloc(sizeof(Pulse));
    new_pulse->currentLength = 1;
    new_pulse->maxLength = 7;
    new_pulse->numberOfPoints = 24 + rand() % 10;
    for(int i = 0; i < new_pulse->numberOfPoints; i++){
        new_pulse->allPoints[i] = pulse_generate_new_point(new_pulse, i);
    }
    #ifdef PBL_COLOR
    new_pulse->frontColour = graphics_settings.frontColour;
    new_pulse->backColour = graphics_settings.backColour;
    #else
    new_pulse->frontColour = GColorWhite;
    new_pulse->backColour = GColorWhite;
    #endif
    new_pulse->animationMode = graphics_settings.animationMode;
    return new_pulse;
}

Pulse* pulse_copy(Pulse *original_pulse){
    Pulse *copy_pulse = malloc(sizeof(Pulse));
    memcpy(copy_pulse, original_pulse, sizeof(Pulse));
    copy_pulse->maxLength = 20;
    return copy_pulse;
}

void pulse_destroy(Pulse *pulse){
    if(pulse != NULL){
        free(pulse);
    }
}

int pulse_generate_i(Pulse *pulse){
    switch(pulse->animationMode){
        case PULSE_ANIMATION_MODE_SECONDLY:
            return (pulse->currentLength-pulse->maxLength) > 0 ? pulse->currentLength-pulse->maxLength : 1;
        default:
            return 1;
    }
}

void graphics_old_background_proc(Layer *layer, GContext *ctx){
    if(graphics_pulse->animationMode != PULSE_ANIMATION_MODE_SECONDLY){
        return;
    }
    for(int i = pulse_generate_i(old_background_pulse); i < old_background_pulse->currentLength; i++){
        GPoint previousPoint = old_background_pulse->allPoints[i-1];
        GPoint currentPoint = old_background_pulse->allPoints[i];

        #ifdef PBL_COLOR
        graphics_context_set_stroke_width(ctx, 2);
        graphics_context_set_stroke_color(ctx, GColorDarkGray);
        #endif

        if(currentPoint.x-previousPoint.x == 10){
            graphics_draw_line(ctx, previousPoint, currentPoint);
        }
    }
}

void graphics_background_proc(Layer *layer, GContext *ctx){
    if(graphics_pulse->animationMode != PULSE_ANIMATION_MODE_SECONDLY){
        return;
    }
    for(int i = pulse_generate_i(background_pulse); i < background_pulse->currentLength; i++){
        GPoint previousPoint = graphics_pulse->allPoints[i-1];
        GPoint currentPoint = graphics_pulse->allPoints[i];

        #ifdef PBL_COLOR
        graphics_context_set_stroke_width(ctx, 2);
        graphics_context_set_stroke_color(ctx, GColorDarkGray);
        #endif

        graphics_draw_line(ctx, previousPoint, currentPoint);
    }
}

void graphics_proc(Layer *layer, GContext *ctx){
    for(int i = pulse_generate_i(graphics_pulse); i < graphics_pulse->currentLength; i++){
        GPoint previousPoint = graphics_pulse->allPoints[i-1];
        GPoint currentPoint = graphics_pulse->allPoints[i];

        #ifdef PBL_COLOR
        graphics_context_set_stroke_width(ctx, 3);
        #endif

        graphics_context_set_stroke_color(ctx, graphics_pulse->allPoints[i].x >= 72 ? graphics_pulse->frontColour : graphics_pulse->backColour);
        graphics_draw_line(ctx, previousPoint, currentPoint);
    }
}

void graphics_refresh_pulse(){
    pulse_destroy(graphics_pulse);
    pulse_destroy(background_pulse);
    pulse_destroy(old_background_pulse);
    graphics_pulse = pulse_create();
    background_pulse = pulse_copy(graphics_pulse);
    background_pulse->currentLength = -2;
    background_pulse->numberOfPoints = graphics_pulse->numberOfPoints;
    old_background_pulse = pulse_copy(graphics_pulse);
    old_background_pulse->currentLength = -30;
    old_background_pulse->numberOfPoints = graphics_pulse->numberOfPoints;
}

void graphics_animate_old_next(){
    old_background_pulse->currentLength++;
    if(old_background_pulse->currentLength < old_background_pulse->numberOfPoints){
        graphics_animation_timer = app_timer_register(60, graphics_animate_old_next, NULL);
    }
    layer_mark_dirty(graphics_old_background_layer);
}

void graphics_animate_next(){
    graphics_pulse->currentLength++;
    background_pulse->currentLength++;
    old_background_pulse->currentLength++;

    if(graphics_pulse->currentLength < graphics_pulse->numberOfPoints){
        graphics_animation_timer = app_timer_register(60, graphics_animate_next, NULL);
        layer_mark_dirty(graphics_layer);
        layer_mark_dirty(graphics_background_layer);
        layer_mark_dirty(graphics_old_background_layer);
    }
    else{
        switch(graphics_pulse->animationMode){
            case PULSE_ANIMATION_MODE_SECONDLY:
                graphics_animation_timer = app_timer_register(100, graphics_animate, NULL);
                graphics_pulse->currentLength = 1;
                old_background_pulse = pulse_copy(background_pulse);
                background_pulse->currentLength = -2;
                graphics_animate_old_next();
                break;
            case PULSE_ANIMATION_MODE_MINUTELY:
                graphics_animation_timer = app_timer_register(60000, graphics_animate, NULL);
                break;
            default:
                layer_mark_dirty(graphics_layer);
                layer_mark_dirty(graphics_background_layer);
                layer_mark_dirty(graphics_old_background_layer);
                break;
        }
    }
}

void graphics_animate(){
    if(graphics_pulse){
        graphics_refresh_pulse();
    }
    if(graphics_animation_timer){
        app_timer_cancel(graphics_animation_timer);
    }
    graphics_animation_timer = app_timer_register(100, graphics_animate_next, NULL);
}

void graphics_shake_animate(AccelAxisType axis, int32_t direction){
    graphics_animate();
}

void graphics_set_settings(Settings new_settings){
    graphics_settings = new_settings;
    graphics_pulse->mode = new_settings.mode;
    graphics_pulse->animationMode = new_settings.animationMode;
    if(new_settings.shakeToAnimate){
        accel_tap_service_subscribe(graphics_shake_animate);
    }
    else{
        accel_tap_service_unsubscribe();
    }
    graphics_animate();
}

void graphics_bluetooth_handler(bool connected){
    graphics_connected = connected;
    if(graphics_pulse->mode == PULSE_MODE_BLUETOOTH && graphics_pulse->animationMode == PULSE_ANIMATION_MODE_ON_CHANGE){
        graphics_animate();
    }
}

void graphics_battery_handler(BatteryChargeState state){
    graphics_charge_state = state;
    if(graphics_pulse->mode == PULSE_MODE_BATTERY && graphics_pulse->animationMode == PULSE_ANIMATION_MODE_ON_CHANGE){
        graphics_animate();
    }
}

void graphics_create(Layer *window_layer){
    #ifdef PBL_COLOR
    graphics_settings.frontColour = GColorWhite;
    graphics_settings.backColour = GColorRed;
    #endif

    graphics_connected = bluetooth_connection_service_peek();
    graphics_charge_state = battery_state_service_peek();

    srand(time(NULL));

    graphics_pulse = pulse_create();
    background_pulse = pulse_copy(graphics_pulse);
    background_pulse->currentLength = -2;
    background_pulse->numberOfPoints = graphics_pulse->numberOfPoints;
    old_background_pulse = pulse_copy(graphics_pulse);
    old_background_pulse->currentLength = -30;
    old_background_pulse->numberOfPoints = graphics_pulse->numberOfPoints;

    graphics_old_background_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(graphics_old_background_layer, graphics_old_background_proc);
    layer_add_child(window_layer, graphics_old_background_layer);

    graphics_background_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(graphics_background_layer, graphics_background_proc);
    layer_add_child(window_layer, graphics_background_layer);

    graphics_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(graphics_layer, graphics_proc);
    layer_add_child(window_layer, graphics_layer);

    graphics_animate();
}

void graphics_destroy(){
    pulse_destroy(graphics_pulse);
    layer_destroy(graphics_layer);
}
