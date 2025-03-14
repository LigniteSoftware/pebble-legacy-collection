#include <pebble.h>
#include "lignite.h"
#include "data_framework.h"

Settings data_framework_local_settings;
SettingsChangeCallback settings_callbacks[AMOUNT_OF_SETTINGS_CALLBACKS];
bool was_trial_key = false;

//Cheers to http://forums.getpebble.com/discussion/comment/132931#Comment_132931
unsigned int data_framework_hex_string_to_uint(char const* hexstring){
    unsigned int result = 0;
    char const *c = hexstring;
    unsigned char thisC;
    while((thisC = *c) != 0){
        thisC = toupper(thisC);
        result <<= 4;
        if(isdigit(thisC)){
            result += thisC - '0';
        }
        else if(isxdigit(thisC)){
            result += thisC - 'A' + 10;
        }
        else{
            APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: Unrecognised hex character \"%c\"", thisC);
            return 0;
        }
        ++c;
    }
    return result;
}

void process_tuple(Tuple *t){
    int key = t->key;
    int value = t->value->int32;
    switch (key) {
        case APP_KEY_BTDISALERT:
            data_framework_local_settings.btdisalert = value;
            break;
    	case APP_KEY_BTREALERT:
            data_framework_local_settings.btrealert = value;
            break;
    	case APP_KEY_INVERT:
            data_framework_local_settings.invert = value;
            break;
    	case APP_KEY_BATTERY_BAR:
            data_framework_local_settings.battery_bar = value;
            break;
        #ifdef PBL_COLOR
    	case APP_KEY_MINUTE_COLOUR:
            data_framework_local_settings.minute_colour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
    	case APP_KEY_HOUR_COLOUR:
            data_framework_local_settings.hour_colour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
    	case APP_KEY_BACKGROUND_COLOUR:
            data_framework_local_settings.background_colour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        #endif
    }
}

void data_framework_inbox(DictionaryIterator *iter, void *context){
    Tuple *t = dict_read_first(iter);
    if(t){
        process_tuple(t);
    }
    while(t != NULL){
        t = dict_read_next(iter);
        if(t){
            process_tuple(t);
        }
    }
    if(!was_trial_key){
        for(int i = 0; i < AMOUNT_OF_SETTINGS_CALLBACKS; i++){
            settings_callbacks[i](data_framework_local_settings);
        }
        vibes_double_pulse();
    }
}

void data_framework_load_default_settings(){
    data_framework_local_settings.invert = false;
	data_framework_local_settings.btdisalert = true;
	data_framework_local_settings.btrealert = false;
	data_framework_local_settings.battery_bar = true;
    #ifdef PBL_COLOR
	data_framework_local_settings.hour_colour = GColorRed;
	data_framework_local_settings.background_colour = GColorBlack;
	data_framework_local_settings.minute_colour = GColorRed;
    #else
    data_framework_local_settings.hour_colour = GColorWhite;
	data_framework_local_settings.background_colour = GColorBlack;
	data_framework_local_settings.minute_colour = GColorWhite;
    #endif
}

Settings data_framework_get_settings() {
    data_framework_load_default_settings();
    return data_framework_local_settings;
}