#include <pebble.h>
#include "lignite.h"
#include "graphics.h"
#include "gbitmap_color_palette_manipulator.h"

Layer *graphics_layer;
Settings graphics_settings;
int currentSecond, currentMinute, currentHour;
bool bluetooth_connected = false;

BatteryBarState batteryBarState;

GBitmap *bluetooth_icon;
int randomOffsets[3];

#ifdef PBL_COLOR
GColor graphics_get_random_colour(){
    GColor colour = GColorFromRGB((rand() % 255), (rand() % 255), (rand() % 255));
    while(gcolor_equal(colour, graphics_settings.backgroundColour)){
        colour = GColorFromRGB((rand() % 255), (rand() % 255), (rand() % 255));
    }
    return colour;
}
#endif

GBitmap *get_bluetooth_icon_bitmap(){
    if(bluetooth_icon != NULL){
        gbitmap_destroy(bluetooth_icon);
    }
    bluetooth_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_FILLED);
    #ifdef PBL_COLOR
    replace_gbitmap_color(GColorWhite, graphics_settings.bluetoothIconColour, bluetooth_icon, NULL);
    #endif
    return bluetooth_icon;
}

void graphics_proc(Layer *layer, GContext *ctx){
    GRect layer_frame = layer_get_frame(layer);

    if(graphics_settings.batteryRadial){ //Battery ring
        GRect frame = grect_inset(layer_frame, GEdgeInsets(2));
        graphics_context_set_fill_color(ctx, graphics_settings.batteryColour);
        graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(batteryBarState.currentAngle+batteryBarState.currentRotation), DEG_TO_TRIGANGLE(batteryBarState.currentAngle*2 + batteryBarState.currentRotation));
    }

    if(graphics_settings.minuteRadial){ //Minute ring
        GRect frame = grect_inset(layer_frame, GEdgeInsets(10));
        graphics_context_set_fill_color(ctx, graphics_settings.minuteColour);
        graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 19, DEG_TO_TRIGANGLE(0 + randomOffsets[0]), DEG_TO_TRIGANGLE(6*currentMinute  + randomOffsets[0]));
    }

    if(graphics_settings.hourRadial){ //Hour ring
        GRect frame = grect_inset(layer_frame, GEdgeInsets(28));
        graphics_context_set_fill_color(ctx, graphics_settings.hourColour);
        graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 19, DEG_TO_TRIGANGLE(0 + randomOffsets[1]), DEG_TO_TRIGANGLE((clock_is_24h_style() ? 15 : 30)*currentHour  + randomOffsets[1]));
    }

    if(graphics_settings.secondRadial){ //Second ring
        GRect frame = grect_inset(layer_frame, GEdgeInsets(46));
        graphics_context_set_fill_color(ctx, graphics_settings.secondColour);
        graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 5, DEG_TO_TRIGANGLE(0 + randomOffsets[2]), DEG_TO_TRIGANGLE(6*currentSecond + randomOffsets[2]));
    }

    if(bluetooth_connected){
        GPoint center = grect_center_point(&layer_frame);
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(ctx, get_bluetooth_icon_bitmap(), GRect(center.x-13, center.y-13, 26, 26));
    }
}

void graphics_fire_charging_animation();
void graphics_started_placeholder(Animation *animation, void *context){}
void graphics_stopped_placeholder(Animation *animation, bool finished, void *context){}
void graphics_setup_placeholder(Animation *animation){}

/*
Charging animation
*/
void graphics_charging_animation_update(Animation *animation, const AnimationProgress distance_normalized){
	batteryBarState.isAnimatingCharge = true;

	long percent = (distance_normalized*100)/ANIMATION_NORMALIZED_MAX;
	long newRotation = 360*percent;
	batteryBarState.currentRotation = newRotation/100;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Current rotation %d percent %ld", batteryBarState.currentRotation, percent);
    layer_mark_dirty(graphics_layer);
	if(percent == 100 && batteryBarState.chargeState.is_charging){
		graphics_fire_charging_animation();
	}
}

void graphics_charging_animation_teardown(Animation *animation){
    AnimationImplementation *implementation = (AnimationImplementation*)animation_get_implementation(animation);
    free(implementation);
	animation_destroy(animation);
	batteryBarState.isAnimatingCharge = false;
}

void graphics_fire_charging_animation(){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Firing charging animation");
    Animation *animation = animation_create();
    animation_set_duration(animation, 1500);
    animation_set_delay(animation, 0);
    AnimationImplementation *implementation = malloc(sizeof(AnimationImplementation));
    implementation->setup = graphics_setup_placeholder;
    implementation->update = graphics_charging_animation_update;
    implementation->teardown = graphics_charging_animation_teardown;
    AnimationHandlers handlers = (AnimationHandlers){
        .started = graphics_started_placeholder,
        .stopped = graphics_stopped_placeholder
    };
    animation_set_handlers(animation, handlers, NULL);
    animation_set_implementation(animation, implementation);
    animation_schedule(animation);
}
//End charging animations

#ifdef PBL_ROUND
void graphics_shake_handler(){
    GRect currentFrame = layer_get_frame(graphics_layer);
    GRect finalFrame = GRect(currentFrame.origin.x, currentFrame.origin.y-50, currentFrame.size.w, currentFrame.size.h);

    animate_layer(graphics_layer, &currentFrame, &finalFrame, 600, 0);
    animate_layer(graphics_layer, &finalFrame, &currentFrame, 600, 3000);
}
#endif

void graphics_tick_handler(struct tm *t){
    currentSecond = t->tm_sec;
    currentMinute = t->tm_min;
    currentHour = t->tm_hour;
    layer_mark_dirty(graphics_layer);
}

void graphics_settings_handler(Settings new_settings){
    graphics_settings = new_settings;
    layer_mark_dirty(graphics_layer);
}

void graphics_battery_handler(BatteryChargeState new_state){
	batteryBarState.requiredAngle = (new_state.charge_percent)/10 * 36;
    batteryBarState.currentAngle = batteryBarState.requiredAngle;

    APP_LOG(APP_LOG_LEVEL_INFO, "Got percent of %d, required angle of %d", new_state.charge_percent, batteryBarState.requiredAngle);

	if(!batteryBarState.chargeState.is_charging && new_state.is_charging){
		graphics_fire_charging_animation();
	}
	else{
		batteryBarState.currentRotation = 0;
	}
	batteryBarState.chargeState = new_state;

	layer_mark_dirty(graphics_layer);
}

void graphics_bluetooth_handler(bool connected){
    bluetooth_connected = connected;
    layer_mark_dirty(graphics_layer);
}

void graphics_create(Window *window){
    Layer *window_layer = window_get_root_layer(window);

    srand(time(NULL));

    GRect windowRect = layer_get_frame(window_layer);
    #ifndef PBL_ROUND
    windowRect.origin.y += 2;
    windowRect.size.h -= 30;
    #endif

    for(int i = 0; i < 3; i++){
        randomOffsets[i] = rand() % 360;
    }

    graphics_layer = layer_create(windowRect);
    layer_set_update_proc(graphics_layer, graphics_proc);
    layer_add_child(window_layer, graphics_layer);

    battery_state_service_subscribe(graphics_battery_handler);
    graphics_battery_handler(battery_state_service_peek());
    graphics_bluetooth_handler(bluetooth_connection_service_peek());

    data_framework_register_settings_callback(graphics_settings_handler, SETTINGS_CALLBACK_GRAPHICS);
    graphics_settings_handler(data_framework_get_settings());
}

void graphics_destroy(){
    layer_destroy(graphics_layer);
}
