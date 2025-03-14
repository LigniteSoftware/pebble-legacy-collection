#include <pebble.h>
#include "other.h"
#include "notification.h"

NotificationPushHandler notification_handler;
GRect hiddenFrame, showingFrame;

void notification_root_proc(Layer *layer, GContext *ctx){
	NotificationData *data = layer_get_data(layer);

	#ifdef PBL_ROUND
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(0, 0, 180, 180), 0, GCornerNone);

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, GPoint(90, 90), 82);

	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, GPoint(90, 32), 32);

	uint8_t circleRadius = 23;
	GPoint circleOrigin = GPoint(90, 32);
	graphics_context_set_fill_color(ctx, data->accentColour);
	graphics_fill_circle(ctx, circleOrigin, circleRadius);

	graphics_context_set_text_color(ctx, GColorBlack);

	GRect titleFrame = GRect(14, 68, 152, 26);
	graphics_draw_text(ctx, data->title[0], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), titleFrame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

	GRect descriptionFrame = GRect(titleFrame.origin.x, titleFrame.origin.y+titleFrame.size.h+2, titleFrame.size.w, 80);
	graphics_draw_text(ctx, data->description[0], fonts_get_system_font(FONT_KEY_GOTHIC_18), descriptionFrame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

	if(data->bitmap){
		GRect bounds = gbitmap_get_bounds(data->bitmap);
		graphics_context_set_compositing_mode(ctx, GCompOpSet);
		graphics_draw_bitmap_in_rect(ctx, data->bitmap, GRect(circleOrigin.x-(bounds.size.w/2) + 1, circleOrigin.y-(bounds.size.h/2), bounds.size.w, bounds.size.h));
	}
	#else
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, GRect(8, 30, 128, 130), 3, GCornersAll);

	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, GPoint(72, 32), 32);

	uint8_t circleRadius = 23;
	GPoint circleOrigin = GPoint(72, 31);
	graphics_context_set_fill_color(ctx, data->accentColour);
	graphics_fill_circle(ctx, circleOrigin, circleRadius);

	graphics_context_set_text_color(ctx, GColorBlack);

	GRect titleFrame = GRect(8, 66, 128, 26);
	graphics_draw_text(ctx, data->title[0], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), titleFrame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

	GRect descriptionFrame = GRect(titleFrame.origin.x, titleFrame.origin.y+titleFrame.size.h+2, titleFrame.size.w, 80);
	graphics_draw_text(ctx, data->description[0], fonts_get_system_font(FONT_KEY_GOTHIC_18), descriptionFrame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

	if(data->bitmap){
		GRect bounds = gbitmap_get_bounds(data->bitmap);
		graphics_context_set_compositing_mode(ctx, GCompOpSet);
		graphics_draw_bitmap_in_rect(ctx, data->bitmap, GRect(circleOrigin.x-(bounds.size.w/2) + 1, circleOrigin.y-(bounds.size.h/2), bounds.size.w, bounds.size.h));
	}
	#endif
}

Notification *notification_create(Window *window){
	Notification *new_notif = malloc(sizeof(Notification));

	showingFrame = layer_get_frame(window_get_root_layer(window));
	hiddenFrame = GRect(-showingFrame.origin.x, 0, showingFrame.size.w, showingFrame.size.h);

	new_notif->root_layer = layer_create_with_data(hiddenFrame, sizeof(NotificationData));
	layer_set_update_proc(new_notif->root_layer, notification_root_proc);

	new_notif->destroy_on_finish = false;
	new_notif->root_window = window;
	return new_notif;
}

void notification_set_icon(Notification *notification, GBitmap *icon){
	NotificationData *data = layer_get_data(notification->root_layer);
	data->bitmap = icon;
}

void notification_set_contents(Notification *notification, char *title, char *description){
	NotificationData *data = layer_get_data(notification->root_layer);
	strncpy(data->title[0], title, sizeof(data->title[0]));
	strncpy(data->description[0], description, sizeof(data->description[0]));
}

void notification_set_accent_colour(Notification *notification, GColor accentColour){
	NotificationData *data = layer_get_data(notification->root_layer);
	data->accentColour = accentColour;
}

void notification_destroy(Notification *notification){
	layer_destroy(notification->root_layer);
	if(notification->timer){
		app_timer_cancel(notification->timer);
	}
	free(notification);
}

void notification_finished_callback(void *void_layer){
	Notification *layer = void_layer;
	layer_set_frame(layer->root_layer, hiddenFrame);
	layer_remove_from_parent(layer->root_layer);
	if(layer->destroy_on_finish){
		notification_destroy(layer);
	}
	layer->is_live = false;
	if(notification_handler){
		notification_handler(false, layer->id);
	}
}

void notification_force_dismiss(Notification *notification){
	animation_unschedule(notification->end_animation);
	app_timer_cancel(notification->timer);
	GRect middle_frame = showingFrame;
	GRect end_frame = GRect(showingFrame.origin.x, -showingFrame.size.h, showingFrame.size.w, showingFrame.size.h);
	notification->end_animation = animate_layer_return(notification->root_layer, &middle_frame, &end_frame, 400, 0);
	notification->timer = app_timer_register(400, notification_finished_callback, notification);
}

void notification_push(Notification* notification, unsigned int length){
	if(!notification){ return; }
	//notification_handler(true);
	layer_add_child(window_get_root_layer(notification->root_window), notification->root_layer);
	notification->is_live = true;
	if(length != 0){
		notification->timer = app_timer_register(length, notification_finished_callback, notification);
	}
	GRect start_frame = layer_get_frame(notification->root_layer);
	GRect middle_frame = showingFrame;
	GRect end_frame = GRect(showingFrame.size.w, showingFrame.origin.x, showingFrame.size.w, showingFrame.size.h);
	animate_layer(notification->root_layer, &start_frame, &middle_frame, 400, 0);
	notification->end_animation = animate_layer_return(notification->root_layer, &middle_frame, &end_frame, 400, length-500);
}

void notification_register_push_handler(NotificationPushHandler handler){
	notification_handler = handler;
}
