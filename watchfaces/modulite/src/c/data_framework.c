#include <pebble.h>
#include "lignite.h"
#include "data_framework.h"
#include "_resources/gbitmap_color_palette_manipulator.h"
#include "data.h"

//#define DEBUG_DEFAULT_SETTINGS

void settings_setup();
void settings_finish();
Settings data_framework_get_default_settings();
void data_framework_save_settings(bool logResult);

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
    // NSLog("Settings: Got key %d with value %d", key, value);
    switch (key) {
        case APP_KEY_ICON:
            doNotVibrate = true;
            data_framework_local_settings.currentWeather.icon = value;
            break;
        case APP_KEY_TEMPERATURE:
            data_framework_local_settings.currentWeather.temperature = value;
            break;

        case APP_KEY_BTREALERT:
            data_framework_local_settings.btrealert = value;
            break;
        case APP_KEY_BTDISALERT:
            data_framework_local_settings.btdisalert = value;
            break;

        case APP_KEY_ANALOGUE_HANDS:
            data_framework_local_settings.analogueHands = value;
            break;
        case APP_KEY_SECOND_HAND:
            data_framework_local_settings.secondHand = value;
            break;
        case APP_KEY_SHAKE_TO_HIDE_HANDS:
            data_framework_local_settings.hideArmsOnShake = value;
            break;

        case APP_KEY_OUTER_CIRCLE_TYPE:
            data_framework_local_settings.outerCircleType = value;
            break;

        case APP_KEY_IMPERIAL_MEASUREMENTS:
            data_framework_local_settings.imperialMeasurements = value;
            //NSLog("Imperial set tot %d", data_framework_local_settings.imperialMeasurements);
            break;

        case APP_KEY_DATA_TYPE_TOP:
        case APP_KEY_DATA_TYPE_RIGHT:
        case APP_KEY_DATA_TYPE_BOTTOM:
        case APP_KEY_DATA_TYPE_LEFT:
            data_framework_local_settings.dataTypes[(key-APP_KEY_DATA_TYPE_TOP)/2] = value;
            // NSLog("Set data type %d to %d", (key-APP_KEY_DATA_TYPE_TOP)/2, value);
            break;

        case APP_KEY_HOUR_HAND:
            data_framework_local_settings.hourHand = value;
            break;
        case APP_KEY_MINUTE_HAND:
            data_framework_local_settings.minuteHand = value;
            break;

        case APP_KEY_GS_DATE_FORMAT:
            strncpy(data_framework_local_settings.dateFormat[0], t->value->cstring, sizeof(data_framework_local_settings.dateFormat[0]));
                // NSLog("Got value %s for date format", data_framework_local_settings.dateFormat[0]);
            break;

        #ifdef PBL_COLOR
        case APP_KEY_ACCENT_COLOUR_TOP:
        case APP_KEY_ACCENT_COLOUR_RIGHT:
        case APP_KEY_ACCENT_COLOUR_BOTTOM:
        case APP_KEY_ACCENT_COLOUR_LEFT:;
            GColor colour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));;
            //NSLog("Setting accent colour for icon %d to %s", (key-APP_KEY_ACCENT_COLOUR_TOP)/2, get_gcolor_text(colour));
            data_framework_local_settings.dataColours[(key-APP_KEY_ACCENT_COLOUR_TOP)/2] = colour;
            break;

        case APP_KEY_HOUR_HAND_COLOUR:
            data_framework_local_settings.hourHandColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_MINUTE_HAND_COLOUR:
            data_framework_local_settings.minuteHandColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_SECOND_HAND_COLOUR:
            data_framework_local_settings.secondHandColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_OUTER_CIRCLE_COLOUR:
            data_framework_local_settings.outerCircleColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_OUTER_CIRCLE_BACKGROUND_COLOUR:
            data_framework_local_settings.outerCircleBackgroundColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        case APP_KEY_BACKGROUND_COLOUR:
            data_framework_local_settings.backgroundColour = GColorFromHEX(data_framework_hex_string_to_uint(t->value->cstring));
            break;
        #endif

        default:
            doNotVibrate = true;
            break;

        //Pebble.openURL("https://web.lignite.me:8443/settings/?pebbleAccountToken=04d29190d174877478b3043b948ddc19&app=modulite&email=edwin@lignite.io&accessCode=WOCAOZHONG");
    }
}

void data_framework_log_int(char *keyname, int value){
    // NSLog("\"%s\": %d,", keyname, value);
}

void data_framework_log_string(char *keyname, char *value){
    // NSLog("\"%s\": \"%s\",", keyname, value);
}

void data_framework_log_settings_struct(Settings settings){
    data_framework_log_int("btdisalert", settings.btdisalert);
    data_framework_log_int("btrealert", settings.btrealert);
    //NSLog("\"data\":[");
    for(int i = 0; i < 4; i++){
        data_framework_log_int("type", settings.dataTypes[i]);
        #ifdef PBL_COLOR
        data_framework_log_string("colour", (char*)get_gcolor_text(settings.dataColours[i]));
        #endif
    }
    //NSLog("]");

    #ifdef PBL_COLOR
    data_framework_log_string("backgroundColour", (char*)get_gcolor_text(settings.backgroundColour));
    data_framework_log_string("hourHandColour", (char*)get_gcolor_text(settings.hourHandColour));
    data_framework_log_string("minuteHandColour", (char*)get_gcolor_text(settings.minuteHandColour));
    data_framework_log_string("secondHandColour", (char*)get_gcolor_text(settings.secondHandColour));
    data_framework_log_string("outerCircleColour", (char*)get_gcolor_text(settings.outerCircleColour));
    #endif

    data_framework_log_int("analogueHands", settings.analogueHands);
    data_framework_log_int("secondHand", settings.secondHand);
    data_framework_log_int("outerCircleType", settings.outerCircleType);
    data_framework_log_int("currentWeather.temperature", settings.currentWeather.temperature);
    data_framework_log_int("currentWeather.icon", settings.currentWeather.icon);

    data_framework_log_int("imperialMeasurements", settings.imperialMeasurements);
    data_framework_log_int("hideArmsOnShake", settings.hideArmsOnShake);
}

void data_framework_inbox(DictionaryIterator *iter, void *context){
    //NSLog("Got a message");
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

    data_framework_save_settings(LOG_ALL);

    for(int i = 0; i < AMOUNT_OF_SETTINGS_CALLBACKS; i++){
        // data_framework_log_settings_struct(data_framework_local_settings);
        // NSLog("\n---Calling back for %d---\n", i);
        if(settings_callbacks[i]){
            settings_callbacks[i](data_framework_local_settings);
        }
    }
    if(!doNotVibrate){
        //NSLog("Got actual settings");
        vibes_double_pulse();
    }
    doNotVibrate = false;
    //04d29190d174877478b3043b948ddc19
}

void data_framework_inbox_dropped(AppMessageResult reason, void *context){
    if(LOG_ALL){
        NSLog("Message dropped, reason %d.", reason);
    }
}

void data_framework_register_settings_callback(SettingsChangeCallback callback, SettingsCallback callbackIdentity){
    //NSLog("Registered callback for %d", callbackIdentity);
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
        #pragma message("Default settings are being used")
        return data_framework_get_default_settings();
    #else
        return data_framework_local_settings;
    #endif
}

#ifndef PBL_PLATFORM_APLITE
DataType defaultDataTypes[4] = {
    DataTypeActiveSeconds, DataTypeStepCount, DataTypeTimeAndDate, DataTypeWalkedDistance
};
#else
DataType defaultDataTypes[4] = {
    DataTypeWeather, DataTypeBluetoothStatus, DataTypeTimeAndDate, DataTypeBatteryLevel
};
#endif

Settings data_framework_get_default_settings(){
    Settings defaults;

    for(int i = 0; i < 4; i++){
        defaults.dataTypes[i] = defaultDataTypes[i];
        defaults.dataColours[i] = GColorLightGray;
    }

    defaults.backgroundColour = GColorBlack;
    defaults.hourHandColour = GColorWhite;
    defaults.minuteHandColour = GColorRed;
    defaults.secondHandColour = GColorWhite;
    defaults.outerCircleColour = GColorRed;
    defaults.outerCircleBackgroundColour = GColorBulgarianRose;

    defaults.analogueHands = true;
    defaults.hideArmsOnShake = false;
    defaults.secondHand = false;

    defaults.outerCircleType = OuterCircleTypeBattery;

    defaults.imperialMeasurements = false;
    defaults.hideArmsOnShake = false;

    strcpy(defaults.dateFormat[0], "%x");

    return defaults;
}

#define CURRENT_STORAGE_VERSION 4
#define STORAGE_VERSION_2 3
#define STORAGE_VERSION_1 2

void data_framework_save_settings(bool logResult){
    int result = persist_write_data(SETTINGS_KEY, &data_framework_local_settings, sizeof(data_framework_local_settings));
    int result1 = persist_write_int(STORAGE_VERSION_KEY, CURRENT_STORAGE_VERSION);
    if(logResult){
        APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes to %s's settings.", result + result1, APP_NAME);
    }
}

int data_framework_load_settings(bool logResult){
    int result = persist_read_data(SETTINGS_KEY, &data_framework_local_settings, sizeof(data_framework_local_settings));
    if(logResult){
        APP_LOG(APP_LOG_LEVEL_INFO, "Read %d bytes from %s's settings.", result, APP_NAME);
    }
    if(result < 0){
        data_framework_local_settings = data_framework_get_default_settings();
    }
    return result;
}

/*
v1DataTypeHidden,
v1DataTypeStepCount,
v1DataTypeActiveSeconds,
v1DataTypeWalkedDistance,
v1DataTypeSleepSeconds,
v1DataTypeSleepRestfulSeconds,
v1DataTypeWeather,
v1DataTypeTime,
v1DataTypeTimeAndDate,
v1DataTypeDate,
v1DataTypeDateTwoLines,
v1DataTypeBatteryLevel,
v1DataTypeBluetoothStatus

DataTypeHidden,
DataTypeStepCount,
DataTypeActiveSeconds,
DataTypeWalkedDistance,
DataTypeSleepSeconds,
DataTypeSleepRestfulSeconds,
DataTypeRestingKCalories,
DataTypeActiveKCalories,
DataTypeWeather,
DataTypeTime,
DataTypeTimeAndDate,
DataTypeDate,
DataTypeBatteryLevel,
DataTypeBluetoothStatus
*/

DataType get_new_datatype_from_v1_data_type(v1DataType oldDataType){
    if(oldDataType <= v1DataTypeSleepRestfulSeconds){
        //NSLog("Returning %d for %d", (DataType)oldDataType, oldDataType);
        return (DataType)oldDataType;
    }
    switch(oldDataType){
        case v1DataTypeWeather:
            return DataTypeWeather;
        case v1DataTypeTime:
            return DataTypeTime;
        case v1DataTypeTimeAndDate:
            return DataTypeTimeAndDate;
        case v1DataTypeDate:
        case v1DataTypeDateTwoLines:
            return DataTypeDate;
        case v1DataTypeBatteryLevel:
            return DataTypeBatteryLevel;
        case v1DataTypeBluetoothStatus:
            return DataTypeBluetoothStatus;
        default:
            return DataTypeHidden;
    }

    return DataTypeHidden;
}

Settings data_framework_port_user_from_settings_2(){
    v2Settings oldSettings;
    int result = persist_read_data(SETTINGS_KEY, &oldSettings, sizeof(oldSettings));
    //NSLog("Read %d bytes from v1 settings, now porting", result);

    Settings newSettings;
    newSettings.btdisalert = oldSettings.btrealert;
    newSettings.btrealert = oldSettings.btrealert;
    newSettings.backgroundColour = oldSettings.backgroundColour;
    newSettings.hourHandColour = oldSettings.hourHandColour;
    newSettings.minuteHandColour = oldSettings.minuteHandColour;
    newSettings.outerCircleColour = oldSettings.outerCircleColour;
    newSettings.analogueHands = oldSettings.analogueHands;
    newSettings.secondHand = oldSettings.secondHand;
    newSettings.outerCircleType = oldSettings.outerCircleType;
    newSettings.currentWeather.temperature = oldSettings.currentWeather.temperature;
    newSettings.currentWeather.icon = oldSettings.currentWeather.icon;
    newSettings.imperialMeasurements = oldSettings.imperialMeasurements;
    newSettings.hideArmsOnShake = oldSettings.hideArmsOnShake;
    newSettings.secondHandColour = oldSettings.secondHandColour;
    for(int i = 0; i < 4; i++){
        newSettings.dataTypes[i] = get_new_datatype_from_v1_data_type((v1DataType) oldSettings.dataTypes[i]);
        newSettings.dataColours[i] = oldSettings.dataColours[i];
    }

    //New to v3
    strcpy(newSettings.dateFormat[0], "%x");
    newSettings.outerCircleBackgroundColour = oldSettings.backgroundColour; // This isn't a mistake, it's to make sure the new settings blend like before this was added
    newSettings.minuteHand = true;
    newSettings.hourHand = true;
    //New to v3

    //NSLog("Settings successfully ported to v3 and new settings included!");

    return newSettings;
}

Settings data_framework_port_user_from_settings_1(){
    v1Settings oldSettings;
    persist_read_data(SETTINGS_KEY, &oldSettings, sizeof(oldSettings));
    ////NSLog("Read %d bytes from v1 settings, now porting", result);

    Settings newSettings;
    newSettings.btdisalert = oldSettings.btrealert;
    newSettings.btrealert = oldSettings.btrealert;
    newSettings.backgroundColour = oldSettings.backgroundColour;
    newSettings.hourHandColour = oldSettings.hourHandColour;
    newSettings.minuteHandColour = oldSettings.minuteHandColour;
    newSettings.outerCircleColour = oldSettings.outerCircleColour;
    newSettings.analogueHands = oldSettings.analogueHands;
    newSettings.secondHand = oldSettings.secondHand;
    //TODO this?
    newSettings.outerCircleType = OuterCircleTypeDefault;
    newSettings.currentWeather.temperature = oldSettings.currentWeather.temperature;
    newSettings.currentWeather.icon = oldSettings.currentWeather.icon;
    for(int i = 0; i < 4; i++){
        newSettings.dataTypes[i] = oldSettings.dataTypes[i];
        newSettings.dataColours[i] = oldSettings.dataColours[i];
    }

    //Settings below are new to 1.2
    newSettings.imperialMeasurements = false;
    newSettings.hideArmsOnShake = false;
    newSettings.secondHandColour = GColorWhite;
    //Settings above are new to 1.2

    //NSLog("Settings successfully ported and new settings included!");

    return newSettings;
}

void settings_setup() {
    uint8_t version = persist_read_int(STORAGE_VERSION_KEY);

    //or, the user had a good version in place
    if(version >= CURRENT_STORAGE_VERSION){
        NSLog("Good version");
        data_framework_load_settings(LOG_ALL);
        return;
    }
    else if(version == STORAGE_VERSION_1){
        NSLog("First version");
        data_framework_local_settings = data_framework_port_user_from_settings_1();
        data_framework_save_settings(LOG_ALL);
        return;
    }
    else if(version == STORAGE_VERSION_2){
        NSLog("Second version");
        data_framework_local_settings = data_framework_port_user_from_settings_2();
        data_framework_save_settings(LOG_ALL);
        return;
    }

    NSLog("Default version");
    data_framework_local_settings = data_framework_get_default_settings();
}

void settings_finish(){
    data_framework_save_settings(LOG_ALL);
}