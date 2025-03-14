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
