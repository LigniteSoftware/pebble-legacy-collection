#include <pebble.h>
#include "lignite.h"
#include "effectlayer/effect_layer.h"
#include "main_window.h"

#define SDK_3

Window *main_window;
TextLayer *time_layer, *date_layer;
EffectLayer *invert_layer;
Layer *graphics_layer, *window_layer;
GPath *minute_segment_path;
GPath *weekday_segment_path;
GPath *battery_path;
GBitmap *bt_icon_bitmap;
BitmapLayer *bt_icon_layer;
BatteryChargeState last_known_charge_state;

uint16_t weekday_anim_count = 0, minute_anim_count = 0, battery_anim_count = 0, animation_count_total = 0;
bool animating = true;
AppTimer *animation_timer;
Settings settings;
bool angles_complete[AMOUNT_OF_ANGLES], settings_just_updated = false;
Notification *disconnect_notification, *reconnect_notification;

#ifdef SDK_2
const GPathInfo WEEKDAY_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-8, -80},
    {8,  -80},
  }
};
const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-9, -60},
    {9,  -60},
  }
};
const GPathInfo BATTERY_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-5, -45},
    {5,  -45},
  }
};
#endif

int get_anim_count(AngleType type){
	const int TO_ADD = 1;
	switch(type){
		case ANGLE_TYPE_WEEKDAY:
			weekday_anim_count += TO_ADD;
			return weekday_anim_count;
		case ANGLE_TYPE_MINUTE:
			minute_anim_count += TO_ADD;
			return minute_anim_count;
		case ANGLE_TYPE_BATTERY:
			battery_anim_count += TO_ADD;
			return battery_anim_count;
	}
	return animation_count_total;
}

void set_anim_complete(AngleType type){
	angles_complete[type] = true;
}

int get_angle(AngleType angletype, struct tm *t, bool autosub){
	int angle = 0;
	switch(angletype){
        case ANGLE_TYPE_WEEKDAY:
			angle = t->tm_wday*46;
			break;
		case ANGLE_TYPE_MINUTE:
            #ifdef PBL_COLOR
			angle = (t->tm_min * 4.5);
            #else
            angle = ((t->tm_min+2) * 4.5);
            #endif
			break;
		case ANGLE_TYPE_BATTERY:;
			BatteryChargeState state = last_known_charge_state;
			int battery_charge = state.charge_percent/10;
			angle = 27 * battery_charge;
			break;
        default:
			angle = 90;
			break;
	}
    #ifdef PBL_ROUND
    //angle -= 90;
    #endif
	if(settings.boot_animation){
		if(angle > get_anim_count(angletype)){
			angle = get_anim_count(angletype);
		}
		else{
			set_anim_complete(angletype);
		}
	}
	if(autosub){
		angle -= 90;
	}
	return angle;
}

void animation_callback(){
	layer_mark_dirty(graphics_layer);
	animation_count_total++;
	if(animation_count_total > 700){
		animating = false;
	}
	if(animating){
		animation_timer = app_timer_register(5, animation_callback, NULL);
	}
}

void main_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	if(!clock_is_24h_style()){
		strftime(time_buffer, sizeof(time_buffer), "%I:%M", t);
	}
	else{
		strftime(time_buffer, sizeof(time_buffer), "%H:%M", t);
	}
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "Some random day...........";
	strftime(date_buffer, sizeof(date_buffer), settings.dateFormat[0], t);
	text_layer_set_text(date_layer, date_buffer);
	layer_mark_dirty(graphics_layer);
}

void notification_push_handler(bool pushed, uint8_t id){
	if(pushed){
		layer_remove_from_parent(window_layer);
	}
	else{
		if(disconnect_notification->is_live || reconnect_notification->is_live){
			return;
		}
		layer_add_child(window_get_root_layer(main_window), window_layer);
	}
}

void bluetooth_handler(bool connected){
	if(!settings_just_updated){
        if(connected && settings.btrealert){
    		if(disconnect_notification->is_live){
    			notification_force_dismiss(disconnect_notification);
    		}
    		notification_push(reconnect_notification, 10000);
    		vibes_double_pulse();
    	}
    	else if(!connected && settings.btdisalert){
    		if(reconnect_notification->is_live){
    			notification_force_dismiss(reconnect_notification);
    		}
    		notification_push(disconnect_notification, 10000);
    		vibes_long_pulse();
    	}
	}
	else{
		if(settings.bluetooth_icon){
			layer_set_hidden(bitmap_layer_get_layer(bt_icon_layer), !connected);
		}
		settings_just_updated = false;
	}
}

void battery_handler(BatteryChargeState new_state){
    uint8_t last_percent = last_known_charge_state.charge_percent;
    last_known_charge_state = new_state;
    if(last_percent != new_state.charge_percent){
        layer_mark_dirty(graphics_layer);
    }
}

#define WEEKDAY_FILL GColorBlack

void update_ring_2(GContext *ctx, Layer *layer, struct tm *t){
    uint8_t circleSize = 10;

    GRect frame = grect_inset(layer_get_frame(layer), GEdgeInsets(10));
	graphics_context_set_fill_color(ctx, settings.colours[0]);
	graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 10, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(get_angle(ANGLE_TYPE_WEEKDAY, t, true)));

    if(settings.showBorders){
        graphics_context_set_stroke_color(ctx, settings.colours[0]);
        graphics_draw_arc(ctx, frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(180));
        GRect innerFrame = GRect(frame.origin.x+circleSize-1, frame.origin.y, frame.size.w-circleSize*2 + 2, frame.size.h);
        graphics_draw_arc(ctx, innerFrame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(180));
    }
}

#define MINUTE_FILL GColorBlack

void update_ring_1(GContext *ctx, Layer *layer, struct tm *t){
    uint8_t circleSize = 10;

    GRect frame = grect_inset(layer_get_frame(layer), GEdgeInsets(24));
	graphics_context_set_fill_color(ctx, settings.colours[1]);
	graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 10, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(get_angle(ANGLE_TYPE_MINUTE, t, true)));

    if(settings.showBorders){
        graphics_context_set_stroke_color(ctx, settings.colours[1]);
        graphics_draw_arc(ctx, frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(180));
        GRect innerFrame = GRect(frame.origin.x+circleSize-1, frame.origin.y, frame.size.w-circleSize*2 + 2, frame.size.h);
        graphics_draw_arc(ctx, innerFrame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(180));
    }
}

#define BATTERY_FILL GColorBlack

void update_ring_0(GContext *ctx, Layer *layer, struct tm *t){
    uint8_t circleSize = 8;

    GRect frame = grect_inset(layer_get_frame(layer), GEdgeInsets(38));
	graphics_context_set_fill_color(ctx, settings.colours[2]);
	graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, circleSize, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(get_angle(ANGLE_TYPE_BATTERY, t, true)));

    if(settings.showBorders){
        graphics_context_set_stroke_color(ctx, settings.colours[2]);
        graphics_draw_arc(ctx, frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(180));
        GRect innerFrame = GRect(frame.origin.x+circleSize-1, frame.origin.y, frame.size.w-circleSize*2 + 2, frame.size.h);
        graphics_draw_arc(ctx, innerFrame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(180));
    }
}

void graphics_proc(Layer *layer, GContext *ctx){
	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);

  	update_ring_2(ctx, layer, t);
  	update_ring_1(ctx, layer, t);
  	update_ring_0(ctx, layer, t);

    #ifdef SDK_2
    if(settings.showBorders){ //Boarders
    	graphics_context_set_stroke_color(ctx, settings.colours[0]);
    	graphics_draw_circle(ctx, center, 60);
    	graphics_draw_circle(ctx, center, 50);
        graphics_context_set_stroke_color(ctx, settings.colours[1]);
    	graphics_draw_circle(ctx, center, 46);
    	graphics_draw_circle(ctx, center, 36);
        graphics_context_set_stroke_color(ctx, settings.colours[2]);
    	graphics_draw_circle(ctx, center, 32);
    	graphics_draw_circle(ctx, center, 27);
    }
    #endif

	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_context_set_stroke_color(ctx, GColorWhite);
    //The stupid black box
    #ifdef SDK_2
	graphics_fill_rect(ctx, GRect(2, 82, 68, 80), 0, GCornerNone);
    #endif

    //End caps
    #ifdef PBL_ROUND
    /*
	graphics_context_set_fill_color(ctx, settings.colours[2]);
	graphics_fill_rect(ctx, GRect(64, 111, 3, 6), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, settings.colours[1]);
	graphics_fill_rect(ctx, GRect(64, 120, 3, 11), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, settings.colours[0]);
	graphics_fill_rect(ctx, GRect(64, 134, 3, 11), 0, GCornerNone);
    */
    #else
    graphics_context_set_fill_color(ctx, settings.colours[2]);
    graphics_fill_rect(ctx, GRect(64, 111, 3, 6), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, settings.colours[1]);
    graphics_fill_rect(ctx, GRect(64, 120, 3, 11), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, settings.colours[0]);
    graphics_fill_rect(ctx, GRect(64, 134, 3, 11), 0, GCornerNone);
    #endif
}

void main_window_update_settings(Settings new_settings){
	settings = new_settings;
	layer_mark_dirty(graphics_layer);
	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);
  	layer_set_hidden(effect_layer_get_layer(invert_layer), !settings.invert);
  	layer_set_hidden(bitmap_layer_get_layer(bt_icon_layer), !settings.bluetooth_icon);
  	settings_just_updated = true;
  	bluetooth_handler(bluetooth_connection_service_peek());
  	if(settings.boot_animation){
  		animation_timer = app_timer_register(10, animation_callback, NULL);
  	}
  	vibes_double_pulse();
    layer_mark_dirty(graphics_layer);
}

void main_window_load(Window *window){
    Layer *window_root_layer = window_get_root_layer(window);

    GRect frame = layer_get_frame(window_root_layer);
	window_layer = layer_create(frame);
    layer_add_child(window_root_layer, window_layer);

	settings = data_framework_get_settings();
    last_known_charge_state = battery_state_service_peek();

    #ifdef SDK_2
	minute_segment_path = gpath_create(&MINUTE_SEGMENT_PATH_POINTS);
	gpath_move_to(minute_segment_path, grect_center_point(&bounds));

	weekday_segment_path = gpath_create(&WEEKDAY_SEGMENT_PATH_POINTS);
	gpath_move_to(weekday_segment_path, grect_center_point(&bounds));

	battery_path = gpath_create(&BATTERY_SEGMENT_PATH_POINTS);
	gpath_move_to(battery_path, grect_center_point(&bounds));
    #endif

	graphics_layer = layer_create(GRect(0, 0, frame.size.w, frame.size.h));
	layer_set_update_proc(graphics_layer, graphics_proc);
	layer_add_child(window_layer, graphics_layer);

	bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_ICON);

	bt_icon_layer = bitmap_layer_create(GRect(0, 0, frame.size.w, frame.size.h));
	bitmap_layer_set_bitmap(bt_icon_layer, bt_icon_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(bt_icon_layer));
	layer_set_hidden(bitmap_layer_get_layer(bt_icon_layer), !settings.bluetooth_icon || !bluetooth_connection_service_peek());

    #ifdef PBL_ROUND
    GFont timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_30));
    GRect timeFrame = GRect(10, 94, 80, 32);
    GRect dateFrame = GRect(24, timeFrame.origin.y+timeFrame.size.h+2, 54, 50);
    #else
    GFont timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_24));
    GRect timeFrame = GRect(2, 92, 60, 26);
    GRect dateFrame = GRect(4, 116, 54, 50);
    #endif

	time_layer = text_layer_init(timeFrame, GTextAlignmentCenter, timeFont);
	text_layer_set_text(time_layer, "00:00");
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	date_layer = text_layer_init(dateFrame, GTextAlignmentRight, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text(date_layer, "March 20, 2015");
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

    GBitmap *bluetoothIcon = gbitmap_create_with_resource(RESOURCE_ID_LIGNITE_IMAGE_BLUETOOTH_ICON);

    disconnect_notification = notification_create(window);
    notification_set_icon(disconnect_notification, bluetoothIcon);
    notification_set_accent_colour(disconnect_notification, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
    notification_set_contents(disconnect_notification, "Oh boy...", "I lost connection to\nyour phone, sorry!");

    reconnect_notification = notification_create(window);
    notification_set_icon(reconnect_notification, bluetoothIcon);
    notification_set_accent_colour(reconnect_notification, PBL_IF_COLOR_ELSE(GColorBlue, GColorWhite));
    notification_set_contents(reconnect_notification, "Woohoo!", "You are now\nconnected to your\nphone again!");

    notification_register_push_handler(notification_push_handler);

	invert_layer = effect_layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, effect_layer_get_layer(invert_layer));
	effect_layer_add_effect(invert_layer, effect_invert, NULL);
	layer_set_hidden(effect_layer_get_layer(invert_layer), !settings.invert);

    data_framework_register_settings_callback(main_window_update_settings, SETTINGS_CALLBACK_MAIN_WINDOW);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);

  	if(settings.boot_animation){
  		animation_timer = app_timer_register(1000, animation_callback, NULL);
  	}
}

void main_window_unload(Window *window){

}

void main_window_init(){
	main_window = window_create();
	window_set_background_color(main_window, GColorBlack);
	window_set_window_handlers(main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
	tick_timer_service_subscribe(MINUTE_UNIT, main_tick_handler);
    battery_state_service_subscribe(battery_handler);
	bluetooth_connection_service_subscribe(bluetooth_handler);
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_deinit(){
	window_destroy(main_window);
}
