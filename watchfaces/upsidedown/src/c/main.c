#include <pebble.h>
#include "lignite.h"
#include "main_window.h"

int main() {
    data_framework_setup();
    main_window_init();
    window_stack_push(main_window_get_window(), true);
    app_event_loop();
    data_framework_finish();
    main_window_deinit();
}

/*
 * Upside down numbers
 * Lost in translation
 *
void render_layer_update(Layer* layer, GContext *ctx) {
	// Capture the graphics context framebuffer
	GBitmap *framebuffer = graphics_capture_frame_buffer(ctx);

	//backup old framebuffer format data
	uint8_t *orig_addr = gbitmap_get_data(framebuffer);
	GBitmapFormat orig_format = gbitmap_get_format(framebuffer);
	uint16_t orig_stride = gbitmap_get_bytes_per_row(framebuffer);

	//Release the framebuffer now that we are free to modify it
	graphics_release_frame_buffer(ctx, framebuffer);

	GRect windowRect = layer_get_frame(window_get_root_layer(main_window));

	GSize renderSize = GSize(windowRect.size.w/2 - 2, windowRect.size.h);
	#if 0 // BUG: Currently not working
		GBitmap *render_bitmap = gbitmap_create_blank(bounds.size, GBitmapFormat8BitCircular);
	#else
		GBitmap *render_bitmap = gbitmap_create_blank(renderSize, GBitmapFormat8Bit);
	#endif

	//replace screen bitmap with our offscreen render bitmap
	gbitmap_set_data(framebuffer, gbitmap_get_data(render_bitmap),
	gbitmap_get_format(render_bitmap), gbitmap_get_bytes_per_row(render_bitmap), false);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);

	static char minute_buffer[] = "MM";
	#ifdef DEBUG_FONTS
	strftime(minute_buffer, sizeof(minute_buffer), "%S", t);
	#else
	strftime(minute_buffer, sizeof(minute_buffer), "%M", t);
	#endif

	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_draw_text(ctx, minute_buffer, simplifica, GRect(0, 0, renderSize.w, renderSize.h), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

	//restore original context bitmap
	gbitmap_set_data(framebuffer, orig_addr, orig_format, orig_stride, false);

	//draw the bitmap to the screen
	//Make the render_bitmap transparent
	#ifdef PBL_ROUND
	const GPoint location = GPoint(15, 86);
	#else
	const GPoint location = GPoint(24, 84);
	#endif
	graphics_draw_rotated_bitmap(ctx, render_bitmap, GPoint(renderSize.w/2, renderSize.h/2), TRIG_MAX_ANGLE/2, location);

	gbitmap_destroy(render_bitmap);
}
*/
