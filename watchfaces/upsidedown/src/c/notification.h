#pragma once
#include <pebble.h>

typedef struct Notification {
	Layer *root_layer;
	Window *root_window;
	AppTimer *timer;
	Animation *end_animation;
	uint8_t id;
	bool destroy_on_finish;
	bool is_live;
} Notification;

typedef struct NotificationData {
	GBitmap *bitmap;
	GColor accentColour;
	char title[1][14];
	char description[1][56];
} NotificationData;

typedef void (*NotificationPushHandler)(bool pushed, uint8_t id);

Notification *notification_create(Window *window);
void notification_set_icon(Notification *notification, GBitmap *icon);
void notification_set_contents(Notification *notification, char *title, char *description);
void notification_set_accent_colour(Notification *notification, GColor accentColour);
void notification_destroy(Notification *notification);
void notification_push(Notification* notification, unsigned int length);
void notification_force_dismiss(Notification *notification);
void notification_register_push_handler(NotificationPushHandler handler);
