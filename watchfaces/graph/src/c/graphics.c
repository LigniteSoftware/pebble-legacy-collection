#include <pebble.h>
#include "lignite.h"
#include "graphics.h"

Layer *graphics_layer;
Settings graphics_settings;
BatteryChargeState graphics_charge_state;
int currentMinute, currentHour;

#ifdef PBL_COLOR
GColor graphics_get_random_colour(){
    GColor colour = GColorFromRGB((rand() % 255), (rand() % 255), (rand() % 255));
    while(gcolor_equal(colour, graphics_settings.backgroundColour)){
        colour = GColorFromRGB((rand() % 255), (rand() % 255), (rand() % 255));
    }
    return colour;
}
#endif

void graphics_proc(Layer *layer, GContext *ctx){
    graphics_context_set_fill_color(ctx, graphics_settings.accentColour);

    int height = 11;
    int width = 2;
    int lastPixel = 12+(10*(height+2))+3;
    for(int i = 0; i < 11; i++){
        graphics_fill_rect(ctx, GRect(12, 4 + (i*(height+2)), width, height), 0, GCornerNone);
        graphics_fill_rect(ctx, GRect(12+width + (i*(height)), lastPixel, height-2, width), 0, GCornerNone);
    }

    int startXLoc = 12+width+1;

    GColor minuteColour, hourColour, batteryColour;

    if(graphics_settings.minuteBar){
        #ifdef PBL_COLOR
        minuteColour = graphics_get_random_colour();
        GColor textColour = gcolor_legible_over(minuteColour);
        graphics_context_set_fill_color(ctx, minuteColour);
        #else
        GColor textColour = GColorBlack;
        #endif
        graphics_context_set_text_color(ctx, textColour);

        uint16_t bottomY = 11 * (height+2) + 4;
        uint16_t topY = 1 * (height+2) + 4;
        uint16_t distanceBetween = bottomY-topY;
        uint16_t interval = distanceBetween/60;
        uint16_t amountToDraw = currentMinute*interval;
        GRect minuteRect = GRect(startXLoc, bottomY-amountToDraw-3, 118, amountToDraw);
        graphics_fill_rect(ctx, minuteRect, 0, GCornerNone);

        #ifdef PBL_BW
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_draw_rect(ctx, GRect(minuteRect.origin.x-1, minuteRect.origin.y+1, minuteRect.size.w, minuteRect.size.h));
        #endif

        if(minuteRect.size.h >= 18){
            static char minuteBuffer[] = "00";
            snprintf(minuteBuffer, sizeof(minuteBuffer), "%d", currentMinute);
            graphics_draw_text(ctx, minuteBuffer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(minuteRect.origin.x+minuteRect.size.w-28, minuteRect.origin.y, 24, minuteRect.size.h), GTextOverflowModeFill, GTextAlignmentRight, NULL);
        }
    }

    if(graphics_settings.hourBar){
        #ifdef PBL_COLOR
        hourColour = graphics_get_random_colour();
        GColor textColour = gcolor_legible_over(hourColour);
        graphics_context_set_fill_color(ctx, hourColour);
        #else
        GColor textColour = GColorBlack;
        #endif
        graphics_context_set_stroke_color(ctx, textColour);

        uint16_t bottomY = 11 * (height+2) + 4;
        uint16_t topY = 1 * (height+2) + 4;
        uint16_t distanceBetween = bottomY-topY;
        uint16_t interval = distanceBetween/(clock_is_24h_style() ? 24 : 12);
        uint8_t fixedHour;
        if(clock_is_24h_style()){
            fixedHour = currentHour;
        }
        else{
            fixedHour = currentHour % 12;
        }
        uint16_t amountToDraw = fixedHour*interval;
        GRect hourRect = GRect(startXLoc, bottomY-amountToDraw-3, 85, amountToDraw);
        graphics_fill_rect(ctx, hourRect, 0, GCornerNone);

        #ifdef PBL_BW
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_draw_rect(ctx, GRect(hourRect.origin.x-1, hourRect.origin.y+1, hourRect.size.w, hourRect.size.h));
        #endif

        if(hourRect.size.h >= 18){
            static char hourBuffer[] = "00";
            snprintf(hourBuffer, sizeof(hourBuffer), "%d", clock_is_24h_style() ? currentHour : (currentHour % 12));
            graphics_draw_text(ctx, hourBuffer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(hourRect.origin.x+hourRect.size.w-28, hourRect.origin.y, 24, hourRect.size.h), GTextOverflowModeFill, GTextAlignmentRight, NULL);
        }
    }

    if(graphics_settings.batteryBar){
        #ifdef PBL_COLOR
        batteryColour = graphics_get_random_colour();
        GColor textColour = gcolor_legible_over(batteryColour);
        graphics_context_set_fill_color(ctx, batteryColour);
        #else
        GColor textColour = GColorBlack;
        #endif
        graphics_context_set_text_color(ctx, textColour);

        GRect batteryRect = GRect(startXLoc, 4+((11-(graphics_charge_state.charge_percent/10))*(height+2)), 50, (graphics_charge_state.charge_percent/10)*(height+2)-3);
        graphics_fill_rect(ctx, batteryRect, 0, GCornerNone);

        #ifdef PBL_BW
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_draw_rect(ctx, GRect(batteryRect.origin.x-1, batteryRect.origin.y+1, batteryRect.size.w, batteryRect.size.h));
        #endif

        if(batteryRect.size.h >= 18){
            static char batteryBuffer[] = "00";
            snprintf(batteryBuffer, sizeof(batteryBuffer), "%d%%", graphics_charge_state.charge_percent);
            graphics_draw_text(ctx, batteryBuffer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(batteryRect.origin.x+batteryRect.size.w-40, batteryRect.origin.y, 36, batteryRect.size.h), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
        }
    }

    GRect rootFrame = layer_get_frame(graphics_layer);
    GRect textBox = GRect(startXLoc, 4 + (11*(height+2)), rootFrame.size.w-startXLoc-4, 18);

    if(graphics_settings.batteryBar){
        graphics_context_set_text_color(ctx, batteryColour);
        graphics_draw_text(ctx, "batt.", fonts_get_system_font(FONT_KEY_GOTHIC_14), textBox, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    }

    if(graphics_settings.hourBar){
        graphics_context_set_text_color(ctx, hourColour);
        graphics_draw_text(ctx, "hour", fonts_get_system_font(FONT_KEY_GOTHIC_14), textBox, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }

    if(graphics_settings.minuteBar){
        graphics_context_set_text_color(ctx, minuteColour);
        graphics_draw_text(ctx, "minute", fonts_get_system_font(FONT_KEY_GOTHIC_14), textBox, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    }
}

void graphics_tick_handler(struct tm *t){
    currentMinute = t->tm_min;
    currentHour = t->tm_hour;
    layer_mark_dirty(graphics_layer);
}

void graphics_settings_handler(Settings new_settings){
    graphics_settings = new_settings;
}

void graphics_battery_handler(BatteryChargeState state){
    graphics_charge_state = state;
    layer_mark_dirty(graphics_layer);
}

void graphics_create(Window *window){
    Layer *window_layer = window_get_root_layer(window);

    #ifdef PBL_ROUND
    GRect beforeRect = layer_get_frame(window_layer);
    GRect windowRect = GRect(beforeRect.origin.x+20, beforeRect.origin.y+10, beforeRect.size.w-36, beforeRect.size.h-12);
    #else
    GRect windowRect = layer_get_frame(window_layer);
    #endif

    graphics_layer = layer_create(windowRect);
    layer_set_update_proc(graphics_layer, graphics_proc);
    layer_add_child(window_layer, graphics_layer);

    battery_state_service_subscribe(graphics_battery_handler);
    graphics_battery_handler(battery_state_service_peek());

    data_framework_register_settings_callback(graphics_settings_handler, SETTINGS_CALLBACK_GRAPHICS);
    graphics_settings_handler(data_framework_get_settings());
}

void graphics_destroy(){
    layer_destroy(graphics_layer);
}
