#include <pebble.h>
#include "lignite.h"
#include "gbitmap_color_palette_manipulator.h"
#include "graphics.h"

#define DEBUG_BATTERY

GRect graphics_root_frame;
Layer *graphics_root_layer;
static BitmapLayer *background_layer;
static GBitmap *background_bitmap;
Layer *main_circle_layer, *battery_circle_layer, *hands_layer;

Settings graphics_settings;

BatteryBarState batteryBarState;
ClockHandState bluetoothHandState, hourHandState, minuteHandState;

bool currently_connected = false;
bool is_past_12 = false;
bool graphics_boot = true;
int currentMinute = 0, currentHour = 0;

#ifdef PBL_ROUND
	#define INSET_BASE 4
#else
	#define INSET_BASE 1
#endif

State g;

void main_circle_data_init(State* state) {
    state->rotation = 0;
	state->center = FPointI(graphics_root_frame.size.w / 2, graphics_root_frame.size.h / 2);
	state->origin = g.center;
    state->font = ffont_create_from_resource(RESOURCE_ID_RAW_BEBAS_NEUE_BOLD);
}

static inline FPoint cartesianPoint(fixed_t radius, int32_t angle) {
	FPoint pt;
	int32_t c = cos_lookup(angle);
	int32_t s = sin_lookup(angle);
	pt.x = g.origin.x - s * radius / TRIG_MAX_RATIO;
	pt.y = g.origin.y + c * radius / TRIG_MAX_RATIO;
	return pt;
}

static inline int32_t hourAngle(int32_t hour) {
	int amount = 42;
	int difference = -1;
	if(hour < 4){
		amount = 24;
		difference = -2;
	}
	return (hour+difference) * TRIG_MAX_ANGLE / amount;
}

void main_circle_proc(Layer *layer, GContext *ctx){
	int base = 24;
	#ifdef PBL_ROUND
	int addition = 4;
	#else
	int addition = 0;
	#endif
	graphics_context_set_antialiased(ctx, false);

    //GRect backgroundFrame = grect_inset(layer_get_frame(layer), GEdgeInsets(INSET_BASE));
	GRect foregroundFrame = grect_inset(layer_get_frame(layer), GEdgeInsets(INSET_BASE+4));

	/*
	//Draw the background circle
	graphics_context_set_fill_color(ctx, graphics_settings.outerCircleBackgroundColour);
	graphics_fill_radial(ctx, backgroundFrame, GOvalScaleModeFitCircle, base + addition, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));

	//Draw the foreground circle (content goes on this)
	graphics_context_set_fill_color(ctx, graphics_settings.outerCircleForegroundColour);
	graphics_fill_radial(ctx, foregroundFrame, GOvalScaleModeFitCircle, base - 8 + addition, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
	*/

	#ifdef PBL_ROUND
		//Draw the red "OFF" indicator section
		graphics_context_set_fill_color(ctx, graphics_settings.outerCircleOffColour);
		graphics_fill_radial(ctx, foregroundFrame, GOvalScaleModeFitCircle, base - 8 + addition, DEG_TO_TRIGANGLE(90), DEG_TO_TRIGANGLE(116));

		//Draw the green "ON" indicator section
		graphics_context_set_fill_color(ctx, graphics_settings.outerCircleOnColour);
		graphics_fill_radial(ctx, foregroundFrame, GOvalScaleModeFitCircle, base - 8 + addition, DEG_TO_TRIGANGLE(116), DEG_TO_TRIGANGLE(136));
	#endif

	//Begin drawing text on a path
	#ifdef PBL_ROUND
		fixed_t rt = INT_TO_FIXED(72);
	#else
		fixed_t rt = INT_TO_FIXED(58);
	#endif

	State *state = &g;

	char text[3];
	#ifdef PBL_ROUND
		int32_t text_size = 18;
	#else
		int32_t text_size = 14;
	#endif

	FContext fctx;
	fctx_init_context(&fctx, ctx);
	fctx_set_fill_color(&fctx, graphics_settings.outerCircleTextColour);
	fctx_set_color_bias(&fctx, 0);
	fctx_begin_fill(&fctx);
	fctx_set_text_size(&fctx, state->font, text_size);

	//Draw the numbers 1-12
	for (int h = 1; h < 13; h++) {
		int fixedH = 13-h;
		snprintf(text, sizeof text, "%d", fixedH);
		int32_t a = state->rotation + hourAngle(h);
        fctx_set_rotation(&fctx, ((h/12) * TRIG_MAX_ANGLE / 90) + a);
		fctx_set_offset(&fctx, cartesianPoint(rt, a));
		fctx_draw_string(&fctx, text, state->font, GTextAlignmentCenter, FTextAnchorMiddle);
	}

	//Draw the "OFF" icon
	int32_t a = hourAngle(34);
	fctx_set_rotation(&fctx, a);
	fctx_set_offset(&fctx, cartesianPoint(rt, a));
	fctx_draw_string(&fctx, "OFF", state->font, GTextAlignmentCenter, FTextAnchorMiddle);

	//Draw the "ON" icon
	a = 51 * (TRIG_MAX_ANGLE / 60);
	fctx_set_rotation(&fctx, a);
	fctx_set_offset(&fctx, cartesianPoint(rt, a));
	fctx_draw_string(&fctx, "ON", state->font, GTextAlignmentCenter, FTextAnchorMiddle);

	//Draw the numbers that show minutes (4 of them)
	char number_array[4][4] = {
			{ 00, 05, 10, 15 },
			{ 15, 20, 25, 30 },
			{ 30, 35, 40, 45},
			{ 45, 50, 55, 00 }
	};
	int angles[4] = {
		-TRIG_MAX_ANGLE/8, -TRIG_MAX_ANGLE/16, TRIG_MAX_ANGLE/16, TRIG_MAX_ANGLE/8
	};
	for (int h = 0; h < 4; h++) {
		int32_t b = hourAngle((h*2)-13);
		snprintf(text, sizeof text, "%d", number_array[currentMinute/15][h]);
        fctx_set_rotation(&fctx, angles[h]);
		fctx_set_offset(&fctx, cartesianPoint(rt, b));
		fctx_draw_string(&fctx, text, state->font, GTextAlignmentCenter, FTextAnchorMiddle);
	};

	fctx_end_fill(&fctx);
	fctx_deinit_context(&fctx);
	/*
	//Draw the ticks that go between the numbers
	for(int i = 0; i < 12; i++){
		int angle = ((i*0.75) * (TRIG_MAX_ANGLE / 60)) - (6 * (TRIG_MAX_ANGLE / 60));
		angle += (i/4) * (2 * (TRIG_MAX_ANGLE / 60));
		int halfWidth = graphics_root_frame.size.w/2;
		int halfHeight = graphics_root_frame.size.h/2;
		GPoint innerGPoint = GPoint((sin_lookup(angle) * 60 / TRIG_MAX_RATIO) + halfWidth, (-cos_lookup(angle) * 60 / TRIG_MAX_RATIO) + halfHeight);
		GPoint outerGPoint = GPoint((sin_lookup(angle) * 65 / TRIG_MAX_RATIO) + halfWidth, (-cos_lookup(angle) * 65 / TRIG_MAX_RATIO) + halfHeight);
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_circle(ctx, innerGPoint, 1);
	}
	*/
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
	////APP_LOG(APP_LOG_LEVEL_DEBUG, "Current rotation %d percent %ld", batteryBarState.currentRotation, percent);
    layer_mark_dirty(battery_circle_layer);
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
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Firing charging animation");
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

/*
Loading animation
*/
void graphics_loading_animation_update(Animation *animation, const AnimationProgress distance_normalized){
	long percent = (distance_normalized*100)/ANIMATION_NORMALIZED_MAX;
	long newAngle = batteryBarState.requiredAngle*percent;
	batteryBarState.currentAngle = newAngle/100;
	////APP_LOG(APP_LOG_LEVEL_DEBUG, "Current angle %d (required %d) percent %ld", batteryBarState.currentAngle, batteryBarState.requiredAngle, percent);
    layer_mark_dirty(battery_circle_layer);
	if(percent == 100 && batteryBarState.chargeState.is_charging && !batteryBarState.isAnimatingCharge){
		graphics_fire_charging_animation();
	}
}

void graphics_loading_animation_teardown(Animation *animation){
    AnimationImplementation *implementation = (AnimationImplementation*)animation_get_implementation(animation);
    free(implementation);
	animation_destroy(animation);

	graphics_battery_handler(battery_state_service_peek());
}

void graphics_fire_loading_animation(){
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Firing loading animation");
    Animation *animation = animation_create();
    animation_set_duration(animation, 1500);
    animation_set_delay(animation, 50);
    AnimationImplementation *implementation = malloc(sizeof(AnimationImplementation));
    implementation->setup = graphics_setup_placeholder;
    implementation->update = graphics_loading_animation_update;
    implementation->teardown = graphics_loading_animation_teardown;
    AnimationHandlers handlers = (AnimationHandlers){
        .started = graphics_started_placeholder,
        .stopped = graphics_stopped_placeholder
    };
    animation_set_handlers(animation, handlers, NULL);
    animation_set_implementation(animation, implementation);
    animation_schedule(animation);
}
//End loading animation

void graphics_battery_handler(BatteryChargeState new_state){
	//APP_LOG(APP_LOG_LEVEL_INFO, "Got charge percent of %d", new_state.charge_percent);
	batteryBarState.requiredAngle = (new_state.charge_percent)/10 * 36;

	if(batteryBarState.chargeState.charge_percent != new_state.charge_percent){
		graphics_fire_loading_animation();
	}
	else if(!batteryBarState.chargeState.is_charging && new_state.is_charging){
		graphics_fire_charging_animation();
	}
	else{
		batteryBarState.currentRotation = 360;
	}
	batteryBarState.chargeState = new_state;

	layer_mark_dirty(battery_circle_layer);
}

void battery_circle_proc(Layer *layer, GContext *ctx){
	if(!graphics_settings.batteryBar){
		return;
	}
	#ifdef PBL_ROUND
	int addition = 36;
	#else
	int addition = 32;
	#endif

	GRect backgroundFrame = grect_inset(layer_get_frame(layer), GEdgeInsets(INSET_BASE+addition));
	graphics_context_set_fill_color(ctx, graphics_settings.batteryBarInnerColour);
	graphics_fill_radial(ctx, backgroundFrame, GOvalScaleModeFitCircle, 7, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));

	GRect foregroundFrame = grect_inset(layer_get_frame(layer), GEdgeInsets(INSET_BASE+addition-4));
	graphics_context_set_fill_color(ctx, graphics_settings.batteryBarOuterColour);
	#ifdef DEBUG_BATTERY
	if(batteryBarState.chargeState.is_charging){
		graphics_context_set_fill_color(ctx, GColorGreen);
	}
	#endif
	graphics_fill_radial(ctx, foregroundFrame, GOvalScaleModeFitCircle, 14, DEG_TO_TRIGANGLE(batteryBarState.currentAngle+batteryBarState.currentRotation), DEG_TO_TRIGANGLE(batteryBarState.currentAngle*2 + batteryBarState.currentRotation));

	////APP_LOG(APP_LOG_LEVEL_INFO, "Reloaded (%d to %d).", batteryBarState.currentAngle, batteryBarState.currentRotation);
}

#ifdef PBL_ROUND
int baseCircle = 33;
#else
int baseCircle = 27;
#endif

int16_t connectedRotation = (21 * (TRIG_MAX_ANGLE / 60));
int16_t disconnectedRotation = (17 * (TRIG_MAX_ANGLE / 60));

void hands_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, graphics_settings.handColour);
	graphics_fill_circle(ctx, GPoint(graphics_root_frame.size.w/2, graphics_root_frame.size.h/2), baseCircle);
	//TODO: change this to background colour
	graphics_context_set_fill_color(ctx, graphics_settings.backgroundColour);
	graphics_fill_circle(ctx, GPoint(graphics_root_frame.size.w/2, graphics_root_frame.size.h/2), baseCircle-2);

	#ifdef PBL_COLOR
	graphics_context_set_stroke_width(ctx, graphics_settings.handThickness);
	#endif
	//Draw the bluetooth hand
	{
		graphics_context_set_stroke_color(ctx, graphics_settings.handColour);
		graphics_draw_line(ctx, bluetoothHandState.innerGPoint, bluetoothHandState.outerGPoint);
	}

	//Draw the hour hand
	{
		graphics_context_set_stroke_color(ctx, graphics_settings.handColour);
		graphics_draw_line(ctx, hourHandState.innerGPoint, hourHandState.outerGPoint);
	}

	//Draw the minute hand
	{
		graphics_context_set_stroke_color(ctx, graphics_settings.handColour);
		graphics_draw_line(ctx, minuteHandState.innerGPoint, minuteHandState.outerGPoint);
	}
}

/*
Hand tick animations
*/

GPoint get_inner_gpoint(int32_t newAngle){
	int halfWidth = graphics_root_frame.size.w/2;
	int halfHeight = graphics_root_frame.size.h/2;

	return GPoint((sin_lookup(newAngle) * baseCircle / TRIG_MAX_RATIO) + halfWidth, (-cos_lookup(newAngle) * baseCircle / TRIG_MAX_RATIO) + halfHeight);
}

GPoint get_outer_gpoint(int32_t newAngle){
	int halfWidth = graphics_root_frame.size.w/2;
	int halfHeight = graphics_root_frame.size.h/2;

	return GPoint((sin_lookup(newAngle) * (baseCircle*2) / TRIG_MAX_RATIO) + halfWidth, (-cos_lookup(newAngle) * (baseCircle*2) / TRIG_MAX_RATIO) + halfHeight);
}

//Bluetooth hand tick
void graphics_bluetooth_hand_animation_update(Animation *animation, const AnimationProgress distance_normalized){
	long percent = (distance_normalized*100)/ANIMATION_NORMALIZED_MAX;
	long newAngleLong = bluetoothHandState.distanceToTravel*percent + (currently_connected ? disconnectedRotation : connectedRotation);
	int16_t newAngle = newAngleLong/100;

	////APP_LOG(APP_LOG_LEVEL_DEBUG, "Current angle %d (current %ld) percent %ld", newAngle, bluetoothHandState.currentAngle, percent);

	bluetoothHandState.innerGPoint = get_inner_gpoint(newAngle);
	bluetoothHandState.outerGPoint = get_outer_gpoint(newAngle);

	layer_mark_dirty(hands_layer);
}

void graphics_bluetooth_hand_animation_teardown(Animation *animation){
    AnimationImplementation *implementation = (AnimationImplementation*)animation_get_implementation(animation);
    free(implementation);
	animation_destroy(animation);
}

void graphics_fire_bluetooth_hand_change(){
	bluetoothHandState.currentAngle = currently_connected ? connectedRotation : disconnectedRotation;

	uint16_t distanceBetween = (connectedRotation-disconnectedRotation);
	bluetoothHandState.distanceToTravel = !currently_connected ? (TRIG_MAX_ANGLE+connectedRotation - distanceBetween) : (TRIG_MAX_ANGLE+disconnectedRotation + distanceBetween);

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Firing bluetooth hand change animation");
    Animation *animation = animation_create();
    animation_set_duration(animation, 1500);
    animation_set_delay(animation, 50);
    AnimationImplementation *implementation = malloc(sizeof(AnimationImplementation));
    implementation->setup = graphics_setup_placeholder;
    implementation->update = graphics_bluetooth_hand_animation_update;
    implementation->teardown = graphics_bluetooth_hand_animation_teardown;
    AnimationHandlers handlers = (AnimationHandlers){
        .started = graphics_started_placeholder,
        .stopped = graphics_stopped_placeholder
    };
    animation_set_handlers(animation, handlers, NULL);
    animation_set_implementation(animation, implementation);
    animation_schedule(animation);
}
//End bluetooth hand tick animation

//Hour hand tick animations
void graphics_hour_hand_animation_update(Animation *animation, const AnimationProgress distance_normalized){
	hourHandState.isAnimating = true;
	long percent = (distance_normalized*100)/ANIMATION_NORMALIZED_MAX;
	long newAngleLong = hourHandState.distanceToTravel*percent;
	int16_t newAngle = newAngleLong/100;

	////APP_LOG(APP_LOG_LEVEL_DEBUG, "Current hour angle %d (current %ld) percent %ld", newAngle, hourHandState.currentAngle, percent);

	hourHandState.innerGPoint = get_inner_gpoint(newAngle);
	hourHandState.outerGPoint = get_outer_gpoint(newAngle);

	layer_mark_dirty(hands_layer);
}

void graphics_hour_hand_animation_teardown(Animation *animation){
    AnimationImplementation *implementation = (AnimationImplementation*)animation_get_implementation(animation);
    free(implementation);
	animation_destroy(animation);
	hourHandState.isAnimating = false;
}

void graphics_fire_hour_hand_change(bool boot){
	if(hourHandState.isAnimating){
		return;
	}
	int hourDisplacements[12] = {
		(1.25 * (TRIG_MAX_ANGLE / 60)), (2.5 * (TRIG_MAX_ANGLE / 60)), (3.75 * (TRIG_MAX_ANGLE / 60)), (5.25 * (TRIG_MAX_ANGLE / 60)),
		(6.5 * (TRIG_MAX_ANGLE / 60)), (8 * (TRIG_MAX_ANGLE / 60)), (9.5 * (TRIG_MAX_ANGLE / 60)), (11 * (TRIG_MAX_ANGLE / 60)),
		(12.5 * (TRIG_MAX_ANGLE / 60)), (14.5 * (TRIG_MAX_ANGLE / 60)), (16.75 * (TRIG_MAX_ANGLE / 60)), (19.25 * (TRIG_MAX_ANGLE / 60))
	};
	int angle = (47 * (TRIG_MAX_ANGLE / 60)) - hourDisplacements[currentHour-1];
	int previousHour = currentHour-2;
	if(previousHour < 1){
		previousHour = 12;
	}
	int oldAngle = (47 * (TRIG_MAX_ANGLE / 60)) - hourDisplacements[previousHour];

	uint16_t distanceBetween = (angle-oldAngle);
	hourHandState.distanceToTravel = (TRIG_MAX_ANGLE+angle);
	boot ? (hourHandState.distanceToTravel -= TRIG_MAX_ANGLE) : 2 == 2;

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Firing hour hand change animation (%d)", distanceBetween);
    Animation *animation = animation_create();
    animation_set_duration(animation, 1500);
    animation_set_delay(animation, 50);
    AnimationImplementation *implementation = malloc(sizeof(AnimationImplementation));
    implementation->setup = graphics_setup_placeholder;
    implementation->update = graphics_hour_hand_animation_update;
    implementation->teardown = graphics_hour_hand_animation_teardown;
    AnimationHandlers handlers = (AnimationHandlers){
        .started = graphics_started_placeholder,
        .stopped = graphics_stopped_placeholder
    };
    animation_set_handlers(animation, handlers, NULL);
    animation_set_implementation(animation, implementation);
    animation_schedule(animation);
}
//End hour hand tick animation

//Minute tick animations
void graphics_minute_hand_animation_update(Animation *animation, const AnimationProgress distance_normalized){
	minuteHandState.isAnimating = true;

	long percent = (distance_normalized*100)/ANIMATION_NORMALIZED_MAX;
	long newAngleLong = minuteHandState.distanceToTravel*percent;
	int16_t newAngle = newAngleLong/100;

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Current minute angle %d (current %ld) percent %ld", newAngle, minuteHandState.currentAngle, percent);

	minuteHandState.innerGPoint = get_inner_gpoint(newAngle);
	minuteHandState.outerGPoint = get_outer_gpoint(newAngle);

	layer_mark_dirty(hands_layer);
}

void graphics_minute_hand_animation_teardown(Animation *animation){
    AnimationImplementation *implementation = (AnimationImplementation*)animation_get_implementation(animation);
    free(implementation);
	animation_destroy(animation);
	minuteHandState.isAnimating = false;
}

void graphics_fire_minute_hand_change(bool boot){
	if(minuteHandState.isAnimating){
		return;
	}
	int fixedMinute = currentMinute;
	fixedMinute -= (15*(currentMinute/15));
	int angle = ((fixedMinute*0.80) * (TRIG_MAX_ANGLE / 60)) - (7 * (TRIG_MAX_ANGLE / 60));
	angle += (fixedMinute/5) * (0.75 * (TRIG_MAX_ANGLE / 60));

	minuteHandState.distanceToTravel = (TRIG_MAX_ANGLE+angle);
	boot ? (minuteHandState.distanceToTravel -= TRIG_MAX_ANGLE) : (2==2);

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Firing minute hand change animation (%ld)", minuteHandState.distanceToTravel);
    Animation *animation = animation_create();
    animation_set_duration(animation, 1500);
    animation_set_delay(animation, 50);
    AnimationImplementation *implementation = malloc(sizeof(AnimationImplementation));
    implementation->setup = graphics_setup_placeholder;
    implementation->update = graphics_minute_hand_animation_update;
    implementation->teardown = graphics_minute_hand_animation_teardown;
    AnimationHandlers handlers = (AnimationHandlers){
        .started = graphics_started_placeholder,
        .stopped = graphics_stopped_placeholder
    };
    animation_set_handlers(animation, handlers, NULL);
    animation_set_implementation(animation, implementation);
    animation_schedule(animation);
}
//End minute hand tick animation

/*
End hand tick animations
*/

void graphics_tick_handler(struct tm *t){
	currentMinute = t->tm_min;
	currentHour = t->tm_hour;
	if(t->tm_hour > 12){
		currentHour -= 12;
	}
	is_past_12 = t->tm_hour > 12;
	if(currentHour == 0){
		currentHour = 12;
	}
	layer_mark_dirty(main_circle_layer);
	layer_mark_dirty(hands_layer);
	//APP_LOG(APP_LOG_LEVEL_WARNING, "Boot %d", graphics_boot);
	graphics_fire_hour_hand_change(graphics_boot);
	graphics_fire_minute_hand_change(graphics_boot);
	graphics_boot = false;
}

void graphics_bluetooth_handler(bool connected){
	currently_connected = connected;
	graphics_fire_bluetooth_hand_change();
}

void update_background_layer(){
	if(background_bitmap)
		gbitmap_destroy(background_bitmap);

	background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

	bitmap_layer_set_bitmap(background_layer, background_bitmap);

	replace_gbitmap_color(GColorBlue, graphics_settings.outerCircleBackgroundColour, background_bitmap, background_layer);
	replace_gbitmap_color(GColorVividCerulean, graphics_settings.outerCircleForegroundColour, background_bitmap, background_layer);
	replace_gbitmap_color(GColorBlack, graphics_settings.backgroundColour, background_bitmap, background_layer);
	replace_gbitmap_color(GColorGreen, graphics_settings.outerCircleOnColour, background_bitmap, background_layer);
	replace_gbitmap_color(GColorRed, graphics_settings.outerCircleOffColour, background_bitmap, background_layer);
}

void graphics_settings_callback(Settings new_settings){
	graphics_settings = new_settings;

	layer_mark_dirty(main_circle_layer);
	layer_mark_dirty(battery_circle_layer);
	layer_mark_dirty(hands_layer);
	update_background_layer();
}

void graphics_init(Layer *root_layer){
	graphics_root_frame = layer_get_frame(root_layer);
	graphics_settings = data_framework_get_settings();

	battery_state_service_subscribe(graphics_battery_handler);
	data_framework_register_settings_callback(graphics_settings_callback, SETTINGS_CALLBACK_GRAPHICS);

	graphics_root_layer = layer_create(graphics_root_frame);
	layer_add_child(root_layer, graphics_root_layer);

	background_layer = bitmap_layer_create(graphics_root_frame);
	layer_add_child(graphics_root_layer, bitmap_layer_get_layer(background_layer));
	update_background_layer();

	main_circle_layer = layer_create(graphics_root_frame);
	layer_set_update_proc(main_circle_layer, main_circle_proc);
	layer_add_child(graphics_root_layer, main_circle_layer);

	battery_circle_layer = layer_create(graphics_root_frame);
	layer_set_update_proc(battery_circle_layer, battery_circle_proc);
	layer_add_child(graphics_root_layer, battery_circle_layer);
	graphics_battery_handler(battery_state_service_peek());

	hands_layer = layer_create(graphics_root_frame);
	layer_set_update_proc(hands_layer, hands_proc);
	layer_add_child(graphics_root_layer, hands_layer);
	graphics_bluetooth_handler(bluetooth_connection_service_peek());

	main_circle_data_init(&g);
	layer_mark_dirty(main_circle_layer);
}

void graphics_deinit(){
	battery_state_service_unsubscribe();
}
