#include <pebble.h>
#include "lignite.h"
#include "data_framework.h"

//#define DEBUG_DEFAULT_SETTINGS

void settings_setup();
void settings_finish();
Settings data_framework_get_default_settings();

Settings data_framework_local_settings;
SettingsChangeCallback settings_callbacks[AMOUNT_OF_SETTINGS_CALLBACKS];

uint32_t data_framework_checksum = 0;
bool userWas1x, doNotVibrate; //If the user used this face before 2.0

void data_framework_setup(){
    app_message_register_inbox_received(data_framework_inbox);
    app_message_register_inbox_dropped(data_framework_inbox_dropped);
    app_message_open(512, 512);

    settings_setup();
}

void data_framework_finish(){
    settings_finish();
}

void process_tuple(Tuple *t){
    int key = t->key;
    int value = t->value->int32;
    switch (key) {
        case APP_KEY_SECURITY_CHECKSUM:
            data_framework_checksum = (uint32_t)value;
            break;

        case APP_KEY_BTREALERT:
            data_framework_local_settings.btrealert = value;
            break;
        case APP_KEY_BTDISALERT:
            data_framework_local_settings.btdisalert = value;
            break;
        case APP_KEY_WEEK_NUMBER:
            data_framework_local_settings.weekNumber = value;
            break;
        case APP_KEY_SECOND_RADIAL:
            data_framework_local_settings.secondRadial = value;
            break;
        case APP_KEY_HOUR_RADIAL:
            data_framework_local_settings.hourRadial = value;
            break;
        case APP_KEY_MINUTE_RADIAL:
            data_framework_local_settings.minuteRadial = value;
            break;
        case APP_KEY_BATTERY_RADIAL:
            data_framework_local_settings.batteryRadial = value;
            break;
        #ifdef PBL_COLOR
        case APP_KEY_SECOND_RADIAL_COLOUR:
            data_framework_local_settings.secondColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_MINUTE_RADIAL_COLOUR:
            data_framework_local_settings.minuteColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_HOUR_RADIAL_COLOUR:
            data_framework_local_settings.hourColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BATTERY_RADIAL_COLOUR:
            data_framework_local_settings.batteryColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BACKGROUND_COLOUR:
            data_framework_local_settings.backgroundColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BLUETOOTH_ICON_COLOUR:
            data_framework_local_settings.bluetoothIconColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_TEXT_COLOUR:
            data_framework_local_settings.textColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        #endif

        case COMMANDS_KEY:
            if(value == 666){
                data_framework_local_settings = data_framework_get_default_settings();
                vibes_short_pulse();
            }
            break;
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
    for(int i = 0; i < AMOUNT_OF_SETTINGS_CALLBACKS; i++){
        settings_callbacks[i](data_framework_local_settings);
    }
    if(!doNotVibrate){
        vibes_double_pulse();
    }
    doNotVibrate = false;
}

void data_framework_inbox_dropped(AppMessageResult reason, void *context){
    if(LOG_ALL){
        NSLog("Message dropped, reason %d.", reason);
    }
}

void data_framework_register_settings_callback(SettingsChangeCallback callback, SettingsCallback callbackIdentity){
    settings_callbacks[callbackIdentity] = callback;
}

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

Settings data_framework_get_settings(){
    #ifdef DEBUG_DEFAULT_SETTINGS
    return data_framework_get_default_settings();
    #else
    return data_framework_local_settings;
    #endif
}

Settings data_framework_get_default_settings(){
    Settings defaults;

    defaults.btdisalert = true;
    defaults.btrealert = false;
    defaults.weekNumber = false;
    defaults.secondRadial = true;
    defaults.hourRadial = true;
    defaults.minuteRadial = true;
    defaults.batteryRadial = true;

    #ifdef PBL_COLOR
    defaults.bluetoothIconColour = GColorWhite;
    defaults.backgroundColour = GColorBlack;
    defaults.textColour = GColorWhite;
    defaults.secondColour = GColorPictonBlue;
    defaults.hourColour = GColorBlue;
    defaults.minuteColour = GColorDukeBlue;
    defaults.batteryColour = GColorWhite;
    #else
    defaults.bluetoothIconColour = GColorWhite;
    defaults.backgroundColour = GColorBlack;
    defaults.textColour = GColorWhite;
    defaults.secondColour = GColorLightGray;
    defaults.hourColour = GColorWhite;
    defaults.minuteColour = GColorLightGray;
    defaults.batteryColour = GColorWhite;
    #endif

    return defaults;
}

#define CURRENT_STORAGE_VERSION 1

void data_framework_save_settings(bool logResult){
    int result = persist_write_data(SETTINGS_KEY, &data_framework_local_settings, sizeof(data_framework_local_settings));
    int result1 = persist_write_int(STORAGE_VERSION_KEY, CURRENT_STORAGE_VERSION);
    if(logResult){
        NSLog("Wrote %d bytes to %s's settings.", result + result1, APP_NAME);
    }
}

int data_framework_load_settings(bool logResult){
    int result = persist_read_data(SETTINGS_KEY, &data_framework_local_settings, sizeof(data_framework_local_settings));
    if(logResult){
        NSLog("Read %d bytes from %s's settings.", result, APP_NAME);
    }
    if(result < 0){
        data_framework_local_settings = data_framework_get_default_settings();
    }
    return result;
}

void settings_setup() {
    uint8_t version = persist_read_int(STORAGE_VERSION_KEY);

    int result = data_framework_load_settings(LOG_ALL);

    //User was already using the watchface and then upgraded to 2.0
    if(result > 0 && version == 0){
        persist_write_bool(USER_WAS_1X_KEY, true);
        userWas1x = true;
        return;
    }
    //or, the user had a good version in place
    if(version >= CURRENT_STORAGE_VERSION){
        userWas1x = persist_read_bool(USER_WAS_1X_KEY);
        return;
    }

    data_framework_local_settings = data_framework_get_default_settings();
}

void settings_finish(){
    data_framework_save_settings(LOG_ALL);
}
