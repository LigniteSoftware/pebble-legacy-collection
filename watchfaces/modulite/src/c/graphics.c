#include <pebble.h>
#include "lignite.h"
#include "graphics.h"
#include "_resources/gbitmap_color_palette_manipulator.h"

Layer *graphics_layer, *g_window_layer;
Settings graphics_settings;
BatteryChargeState graphics_charge_state;
struct tm *current_time;
bool graphics_hide_arms = false;

#ifndef PBL_ROUND
static GBitmap *analogueBackground;
#endif

#ifndef PBL_ROUND
void graphics_draw_square_radial(GContext *ctx, int amountToDraw, uint8_t thickness){
    graphics_context_set_stroke_width(ctx, thickness);

    GPoint previousPoint = GPoint(72, 3);
    for(int i = 0; i < 5; i++){
        int16_t costOfLine = 0;
        GPoint position = GPoint(72, 3);
        bool vertical = false;
        switch(i){
            case 0:
                costOfLine = 69;
                break;
            case 1:
                position.x = 141;
                costOfLine = 162;
                vertical = true;
                break;
            case 2:
                position.x = 141;
                position.y = 165;
                costOfLine = -138;
                break;
            case 3:
                position.x = 3;
                position.y = 165;
                costOfLine = -162;
                vertical = true;
                break;
            case 4:
                position.x = 3;
                costOfLine = 69;
                break;
        }
        amountToDraw -= abs(costOfLine);
        if(amountToDraw < 0) costOfLine += (costOfLine < 0) ? abs(amountToDraw) : amountToDraw;

        GPoint nextPoint = !vertical ? GPoint(position.x+costOfLine, position.y) : GPoint(position.x, position.y+costOfLine);
        graphics_draw_line(ctx, previousPoint, nextPoint);
        previousPoint = nextPoint;

        if(amountToDraw < 0) break;
    }
}
#endif

void graphics_draw_background_ring(GContext *ctx){
    #ifdef PBL_ROUND
    graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.outerCircleBackgroundColour, GColorWhite));
    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_circle(ctx, CENTRE, WINDOW_SIZE.w/2 - 10);
    #else
    graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.outerCircleBackgroundColour, GColorWhite));
    graphics_draw_square_radial(ctx, 600, 3);
    #endif
    graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.outerCircleColour, GColorWhite));
}

#ifndef PBL_ROUND
void graphics_create_analogue_ticks(){
    if(graphics_settings.outerCircleType == OuterCircleTypeAnalogueLines || graphics_settings.outerCircleType == OuterCircleTypeAnalogueCircles){
        if(analogueBackground){
            NSLog("Destroying analogue background");
            gbitmap_destroy(analogueBackground);
        }
        if(graphics_settings.outerCircleType == OuterCircleTypeAnalogueLines){
            analogueBackground = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_ANALOGUE_TICKS);
            NSLog("Creating background with ticks");
        }
        else{
            analogueBackground = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_ANALOGUE_DOTS);
            NSLog("Creating background with dots");
        }
        #ifdef PBL_COLOR
        gbitmap_fill_all_except(GColorClear, graphics_settings.outerCircleColour, true, analogueBackground, NULL);
        #endif
    }
    else{
        NSLog("Outer circle type %d", graphics_settings.outerCircleType);
    }
}
#endif

int32_t get_angle_for_hour(int hour) {
    return (hour * 360) / 12;
}

int32_t get_angle_for_minute(int minute) {
    return (minute * 360) / 60;
}

void graphics_proc(Layer *layer, GContext *ctx){
    graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.outerCircleColour, GColorWhite));
    graphics_context_set_stroke_width(ctx, 2);

    bool analogueTicks = (graphics_settings.outerCircleType == OuterCircleTypeAnalogueLines || graphics_settings.outerCircleType == OuterCircleTypeAnalogueCircles);

    if(!analogueTicks){
        graphics_draw_background_ring(ctx);
    }

    #ifdef PBL_ROUND
    if(graphics_settings.outerCircleType == OuterCircleTypeDefault){
        graphics_context_set_stroke_width(ctx, 2);
        graphics_draw_circle(ctx, CENTRE, WINDOW_SIZE.w/2 - 10);
    }
    else if(graphics_settings.outerCircleType == OuterCircleTypeBattery){
        GRect foregroundFrame = grect_inset(layer_get_frame(layer), GEdgeInsets(8));
    	graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.outerCircleColour, GColorWhite));
    	graphics_fill_radial(ctx, foregroundFrame, GOvalScaleModeFitCircle, 3, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(36*(graphics_charge_state.charge_percent/10)));
    }
    else if(analogueTicks){
        GRect bounds = layer_get_bounds(graphics_layer);
        GRect frame = grect_inset(bounds, GEdgeInsets(4));
        GRect inner_hour_frame = grect_inset(bounds, GEdgeInsets(graphics_settings.outerCircleType == OuterCircleTypeAnalogueCircles ? 10 : 12));
	    GRect inner_minute_frame = grect_inset(bounds, GEdgeInsets(10));

        graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.outerCircleColour, GColorWhite));
    	graphics_context_set_stroke_width(ctx, 3);

    	// Hours marks
    	for(int i = 0; i < 12; i++) {
    		int hour_angle = get_angle_for_hour(i);
    		GPoint p0 = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));
    		GPoint p1 = gpoint_from_polar(inner_hour_frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));
            if(graphics_settings.outerCircleType == OuterCircleTypeAnalogueLines){
                graphics_draw_line(ctx, p0, p1);
            }
            else{
                graphics_fill_circle(ctx, p1, 3);
            }
    	}

    	// Minute Marks
    	graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.outerCircleColour, GColorWhite));
    	graphics_context_set_stroke_width(ctx, 1);
    	for(int i = 0; i < 60; i++) {
    		if (i % 5) {
    			int minute_angle = get_angle_for_minute(i);
    			GPoint p0 = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(minute_angle));
    			GPoint p1 = gpoint_from_polar(inner_minute_frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(minute_angle));
                if(graphics_settings.outerCircleType == OuterCircleTypeAnalogueLines){
                    graphics_draw_line(ctx, p0, p1);
                }
                else{
                    graphics_fill_circle(ctx, p1, 1);
                }
    		}
    	}

        graphics_context_set_stroke_width(ctx, 2);
    }
    #else
    if(graphics_settings.outerCircleType == OuterCircleTypeDefault){
        graphics_context_set_stroke_width(ctx, 3);
        graphics_draw_round_rect(ctx, GRect(3, 3, 138, 162), 3);
    }
    else if(graphics_settings.outerCircleType == OuterCircleTypeBattery){
        int amountToDraw = 60 * (graphics_charge_state.charge_percent/10);
        graphics_draw_square_radial(ctx, amountToDraw, 3);
    }
    else if(analogueTicks && analogueBackground){
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(ctx, analogueBackground, GRect(0, 0, 144, 168));
    }
    else if(!analogueBackground && analogueTicks){
        graphics_create_analogue_ticks();
        layer_mark_dirty(graphics_layer);
        return;
    }
    #endif

    #ifdef PBL_ROUND
    int radius[] = { 65, 45, 50 };
    if(graphics_settings.outerCircleType == OuterCircleTypeMinute){
        radius[0] += 14;
    }
    #else
    int radius[] = { 58, 38, 44 };
    #endif
    if(!graphics_settings.analogueHands){
        NSLog("No analogue hands");
        return;
    }

    if(graphics_hide_arms){
        NSLog("Hide arms");
        return;
    }

    if(!current_time){
        NSLog("Current time doesn't exist :(");
        return;
    }

    //minute, hour
	int time[] = { current_time->tm_min, current_time->tm_hour, current_time->tm_sec };
    uint8_t amountToDraw = graphics_settings.secondHand ? 3 : 2;
	for(int i = 0; i < amountToDraw; i++){
		int angle = (i % 2 == 0) ? (TRIG_MAX_ANGLE * time[i] / 60) : (TRIG_MAX_ANGLE * (((time[i] % 12) * 6) + (time[i-1] / 10))) / (12 * 6);
		int y = (-cos_lookup(angle) * radius[i] / TRIG_MAX_RATIO) + CENTRE.y;
		int x = (sin_lookup(angle) * radius[i] / TRIG_MAX_RATIO) + CENTRE.x;

        //If minutes
        if(i == 0){
            if(graphics_settings.minuteHand){
                if(graphics_settings.outerCircleType == OuterCircleTypeMinute){
                    #ifdef PBL_ROUND
                    GRect foregroundFrame = grect_inset(layer_get_frame(layer), GEdgeInsets(8));
                	graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.outerCircleColour, GColorWhite));
                	graphics_fill_radial(ctx, foregroundFrame, GOvalScaleModeFitCircle, 3, DEG_TO_TRIGANGLE(0), angle);
                    #else
                    graphics_draw_square_radial(ctx, 10 * time[i], 3);
                    #endif
                }
                graphics_context_set_stroke_width(ctx, 4);
                graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.minuteHandColour, GColorWhite));
                graphics_draw_line(ctx, CENTRE, GPoint(x, y));
            }
        }
        else if(i == 2){
            graphics_context_set_stroke_width(ctx, 2);
            graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.secondHandColour, GColorWhite));
            graphics_draw_line(ctx, CENTRE, GPoint(x, y));
        }
        else{
            if(graphics_settings.hourHand){
                graphics_context_set_stroke_width(ctx, 3);
                graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.hourHandColour, GColorWhite));
                graphics_draw_line(ctx, CENTRE, GPoint(x, y));
            }
        }
	}
    graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(graphics_settings.minuteHandColour, GColorWhite));
    graphics_fill_circle(ctx, CENTRE, 5);
}

void graphics_tick_handler(struct tm *t){
    if(!current_time){
        current_time = malloc(sizeof(struct tm));
    }
    memcpy(current_time, t, sizeof(struct tm));
    layer_mark_dirty(graphics_layer);
}

void graphics_settings_handler(Settings new_settings){
    graphics_settings = new_settings;
    #ifndef PBL_ROUND
    NSLog("Creating ticks");
    graphics_create_analogue_ticks();
    #endif
    layer_mark_dirty(graphics_layer);
}

void graphics_battery_handler(BatteryChargeState charge_state){
    graphics_charge_state = charge_state;
    layer_mark_dirty(graphics_layer);
}

void graphics_show(){
    graphics_hide_arms = false;
    layer_mark_dirty(graphics_layer);
}

void graphics_set_hidden(int length){
    graphics_hide_arms = !graphics_hide_arms;
    layer_mark_dirty(graphics_layer);
    if(length > 0){
        app_timer_register(length, graphics_show, NULL);
    }
}

void graphics_create(Window *window){
    g_window_layer = window_get_root_layer(window);

    GRect windowRect = layer_get_frame(g_window_layer);

    // NSLog("Creating with rect %d, %d, %d, %d", windowRect.origin.x, windowRect.origin.y, windowRect.size.w, windowRect.size.h);

    graphics_layer = layer_create(windowRect);

    layer_set_update_proc(graphics_layer, graphics_proc);
    layer_add_child(g_window_layer, graphics_layer);

    data_framework_register_settings_callback(graphics_settings_handler, SETTINGS_CALLBACK_GRAPHICS);
    graphics_settings_handler(data_framework_get_settings());
}

void make_philipp_stop_crying(Layer *window_layer){
    layer_add_child(window_layer, graphics_layer);
}

void graphics_destroy(){
    layer_destroy(graphics_layer);
}
