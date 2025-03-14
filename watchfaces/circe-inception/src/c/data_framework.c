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
bool userWas1x, doNotVibrate = false; //If the user used this face before 2.0

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

        case APP_KEY_BTREALERT:
            data_framework_local_settings.btrealert = value;
            break;
        case APP_KEY_BTDISALERT:
            data_framework_local_settings.btdisalert = value;
            break;
        case APP_KEY_TIME_INSTEAD:
            data_framework_local_settings.timeInstead = value;
            break;
        case APP_KEY_DAY:
            data_framework_local_settings.day = value;
            break;
        case APP_KEY_DATE:
            data_framework_local_settings.date = value;
            break;
        case APP_KEY_BATTERY_BAR:
            data_framework_local_settings.batteryBar = value;
            break;
        case APP_KEY_HAND_THICKNESS:
            data_framework_local_settings.handThickness = value;
            break;

        case APP_KEY_GS_DATE_FORMAT:
            strncpy(data_framework_local_settings.dateFormat[0], t->value->cstring, sizeof(data_framework_local_settings.dateFormat[0]));
            break;

        #ifdef PBL_COLOR
        case APP_KEY_DAY_DATE_COLOUR:
            data_framework_local_settings.dayDateColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_HAND_COLOUR:
            data_framework_local_settings.handColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BATTERY_BAR_INNER_COLOUR:
            data_framework_local_settings.batteryBarInnerColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BATTERY_BAR_OUTER_COLOUR:
            data_framework_local_settings.batteryBarOuterColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BACKGROUND_OUTER_CIRCLE_COLOUR:
            data_framework_local_settings.outerCircleBackgroundColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_FOREGROUND_OUTER_CIRCLE_COLOUR:
            data_framework_local_settings.outerCircleForegroundColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_OUTER_CIRCLE_TEXT_COLOUR:
            data_framework_local_settings.outerCircleTextColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BLUETOOTH_OFF_COLOUR:
            data_framework_local_settings.outerCircleOffColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BLUETOOTH_ON_COLOUR:
            data_framework_local_settings.outerCircleOnColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BACKGROUND_COLOUR:
            data_framework_local_settings.backgroundColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
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
    defaults.timeInstead = false;
    defaults.day = true;
    defaults.date = true;
    defaults.batteryBar = true;
    defaults.handThickness = 2;

    #ifdef PBL_COLOR
    defaults.dayDateColour = GColorWhite;
    defaults.handColour = GColorRed;
    defaults.batteryBarInnerColour = GColorRed;
    defaults.batteryBarOuterColour = GColorYellow;
    defaults.outerCircleBackgroundColour = GColorBlue;
    defaults.outerCircleForegroundColour = GColorVividCerulean;
    defaults.outerCircleTextColour = GColorWhite;
    defaults.outerCircleOffColour = GColorRed;
    defaults.outerCircleOnColour = GColorGreen;
    defaults.backgroundColour = GColorBlack;
    #else
    defaults.dayDateColour = GColorWhite;
    defaults.handColour = GColorWhite;
    defaults.batteryBarInnerColour = GColorDarkGray;
    defaults.batteryBarOuterColour = GColorWhite;
    defaults.outerCircleBackgroundColour = GColorDarkGray;
    defaults.outerCircleForegroundColour = GColorWhite;
    defaults.outerCircleTextColour = GColorBlack;
    defaults.outerCircleOffColour = GColorLightGray;
    defaults.outerCircleOnColour = GColorLightGray;
    defaults.backgroundColour = GColorBlack;
    #endif

    strcpy(defaults.dateFormat[0], "%x");

    return defaults;
}

#define CURRENT_STORAGE_VERSION 2
#define STORAGE_VERSION_1 1

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

Settings data_framework_port_user_from_settings_1(){
    v1Settings oldSettings;
    persist_read_data(SETTINGS_KEY, &oldSettings, sizeof(oldSettings));
    //NSLog("Read %d bytes from v1 settings, now porting", result);

    Settings newSettings;
    newSettings.btdisalert = oldSettings.btrealert;
    newSettings.btrealert = oldSettings.btrealert;
    newSettings.timeInstead = oldSettings.timeInstead;
    newSettings.day = oldSettings.day;
    newSettings.date = oldSettings.date;
    newSettings.batteryBar = oldSettings.batteryBar;
    newSettings.handThickness = oldSettings.handThickness;
    newSettings.dayDateColour = oldSettings.dayDateColour;
    newSettings.handColour = oldSettings.handColour;
    newSettings.batteryBarInnerColour = oldSettings.batteryBarInnerColour;
    newSettings.batteryBarOuterColour = oldSettings.batteryBarOuterColour;
    newSettings.outerCircleBackgroundColour = oldSettings.outerCircleBackgroundColour;
    newSettings.outerCircleForegroundColour = oldSettings.outerCircleForegroundColour;
    newSettings.outerCircleTextColour = oldSettings.outerCircleTextColour;
    newSettings.outerCircleOffColour = oldSettings.outerCircleOffColour;
    newSettings.outerCircleOnColour = oldSettings.outerCircleOnColour;
    newSettings.backgroundColour = oldSettings.backgroundColour;

    //Settings below are new
    strcpy(newSettings.dateFormat[0], "%x");
    //Settings above are new

    return newSettings;
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
    else if(version == STORAGE_VERSION_1){
        data_framework_local_settings = data_framework_port_user_from_settings_1();
        data_framework_save_settings(LOG_ALL);
        return;
    }

    data_framework_local_settings = data_framework_get_default_settings();
}

void settings_finish(){
    data_framework_save_settings(LOG_ALL);
}
