#include <pebble.h>
#include "lignite.h"
#include "data_layer.h"

DataLayer *data_layer;
DataLayerUpdate data_update;
Settings data_layer_settings;
bool update_data = false, no_animation;

void data_layer_on_animation_stopped(Animation *anim, bool finished, void *context){
    #ifdef PBL_BW
        property_animation_destroy((PropertyAnimation*) anim);
    #endif
	if(update_data){
		text_layer_set_text(data_layer->title_layer, data_update.new_title[0]);
		text_layer_set_text(data_layer->subtitle_layer, data_update.new_subtitle[0]);
		layer_set_hidden(bitmap_layer_get_layer(data_layer->side_layer), !data_update.bluetooth_connected);
		update_data = false;
	}
}

void data_layer_animate_layer(Layer *layer, GRect *start, GRect *finish, int length, int delay){
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);

    if(no_animation){
        length = 1;
        delay = 1;
    }
    animation_set_duration(property_animation_get_animation(anim), length);
    animation_set_delay(property_animation_get_animation(anim), delay);

	AnimationHandlers handlers = {
    	.stopped = (AnimationStoppedHandler) data_layer_on_animation_stopped
    };
    animation_set_handlers(property_animation_get_animation(anim), handlers, NULL);

    animation_schedule(property_animation_get_animation(anim));
}

void data_layer_temporarily_disable_animation(){
    no_animation = true;
}

bool data_layer_set_update(DataLayerUpdate update){
	bool is_new = true;
	if(update.bluetooth_connected == data_update.bluetooth_connected){
		if(strcmp(update.new_title[0], data_update.new_title[0]) == 0){
            if(strcmp(update.new_subtitle[0], data_update.new_subtitle[0]) == 0){
                is_new = false;
            }
		}
	}
	data_update = update;
	return is_new;
}

DataLayer *data_layer_get_layer(){
	return data_layer;
}

void data_layer_mark_dirty(bool animated){
    if(animated){
    	update_data = true;
    	data_layer_animate_layer(data_layer->root_layer, &GRect(0, 0, 144, 168), &GRect(0, 40, 144, 168), 600, 0);
    	data_layer_animate_layer(data_layer->root_layer, &GRect(0, 40, 144, 168), &GRect(0, 0, 144, 168), 600, 700);
    }
    else{
        text_layer_set_text(data_layer->title_layer, data_update.new_title[0]);
		text_layer_set_text(data_layer->subtitle_layer, data_update.new_subtitle[0]);
		layer_set_hidden(bitmap_layer_get_layer(data_layer->side_layer), !data_update.bluetooth_connected);
		update_data = false;
    }
}

void data_layer_set_settings(Settings settings){
    data_layer_settings = settings;
    layer_mark_dirty(data_layer->root_layer);
    layer_mark_dirty(data_layer->graphics_layer);
    #ifdef PBL_COLOR
    text_layer_set_text_color(data_layer->title_layer, gcolor_legible_over(data_layer_settings.accentColour));
    text_layer_set_text_color(data_layer->subtitle_layer, gcolor_legible_over(data_layer_settings.accentColour));
    #endif
}

void data_layer_graphics_proc(Layer *layer, GContext *ctx){
	#ifdef PBL_COLOR
	graphics_context_set_fill_color(ctx, data_layer_settings.accentColour);
    #else
    graphics_context_set_fill_color(ctx, GColorWhite);
	#endif
    graphics_fill_rect(ctx, GRect(18, 6, 108, 37), 2, GCornersTop);
}

void data_layer_create(Layer *window_layer){
	data_layer = malloc(sizeof(DataLayer));

	data_layer->root_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, data_layer->root_layer);

	data_layer->graphics_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(data_layer->graphics_layer, data_layer_graphics_proc);
	layer_add_child(data_layer->root_layer, data_layer->graphics_layer);

    GFont droid_sans_light = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_12));

	data_layer->title_layer = text_layer_init(GRect(24, 9, 112, 30), GTextAlignmentLeft, droid_sans_light);
	text_layer_set_text(data_layer->title_layer, "Good day!");
	layer_add_child(data_layer->root_layer, text_layer_get_layer(data_layer->title_layer));

    GFont droid_sans_bold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_BOLD_12));

	data_layer->subtitle_layer = text_layer_init(GRect(24, 23, 112, 30), GTextAlignmentLeft, droid_sans_bold);
	text_layer_set_text(data_layer->subtitle_layer, "Enjoy Personal.");
	layer_add_child(data_layer->root_layer, text_layer_get_layer(data_layer->subtitle_layer));

    #ifdef PBL_BW
    text_layer_set_text_color(data_layer->title_layer, GColorBlack);
    text_layer_set_text_color(data_layer->subtitle_layer, GColorBlack);
    #endif

	#ifdef PBL_COLOR
	data_layer->side_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_ICON_BASALT);
    #else
    data_layer->side_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_ICON_APLITE);
	#endif

	data_layer->side_layer = bitmap_layer_create(GRect(108, 10, 30, 30));
    #ifdef PBL_COLOR
	bitmap_layer_set_compositing_mode(data_layer->side_layer, GCompOpSet);
    #endif
	bitmap_layer_set_bitmap(data_layer->side_layer, data_layer->side_icon);
	layer_add_child(data_layer->root_layer, bitmap_layer_get_layer(data_layer->side_layer));
}

void data_layer_destroy(){
	layer_destroy(data_layer->root_layer);
	layer_destroy(data_layer->graphics_layer);
	text_layer_destroy(data_layer->title_layer);
	text_layer_destroy(data_layer->subtitle_layer);
	bitmap_layer_destroy(data_layer->side_layer);
	free(data_layer);
}
