#pragma once
#include "lignite.h"

#ifdef PBL_ROUND
#define TIME_FONT_SMALL RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_32
#define TIME_FONT_LARGE RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_42

#define WEATHER_FONT_LARGE RESOURCE_ID_FONT_WEATHER_ICONS_44
#define WEATHER_FONT_SMALL RESOURCE_ID_FONT_WEATHER_ICONS_38
#else
#define TIME_FONT_SMALL RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_30
#define TIME_FONT_LARGE RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_36

#define WEATHER_FONT_LARGE RESOURCE_ID_FONT_WEATHER_ICONS_40
#define WEATHER_FONT_SMALL RESOURCE_ID_FONT_WEATHER_ICONS_36
#endif

#define DATA_MAX_AMOUNT_OF_LAYERS 10
#define DATA_TIME_BUFFER_LENGTH 16

typedef enum {
    GeneralDataTypeHidden = 0,
    GeneralDataTypeTime,
    GeneralDataTypeWeather,
    GeneralDataTypeHealth,
    GeneralDataTypeNumieric
} GeneralDataType;

typedef enum {
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
} v1DataType;

typedef enum {
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
} DataType;

typedef enum {
    DataLocationTop = 0,
    DataLocationRight,
    DataLocationBottom,
    DataLocationLeft
} DataLocation;

typedef void (*DataTimeUpdateCallback)(void *layer, struct tm *t);

typedef enum {
    TIME_FORMAT_TIME = 0,
    TIME_FORMAT_TIME_AND_DATE,
    TIME_FORMAT_DATE
} TimeFormat;

typedef struct {
    TimeFormat timeFormat;
    struct tm *timeT;
    GFont timeFont;
    GFont dateFont;
    char timeFormatBuffer[1][DATA_TIME_BUFFER_LENGTH];
    char timeBuffer[1][DATA_TIME_BUFFER_LENGTH];
    char dateBuffer[1][DATA_TIME_BUFFER_LENGTH*2];
    uint16_t offset;
    DataTimeUpdateCallback updateCallback;
} TimeData;

typedef struct {
    Weather currentWeather;
    char temperatureBuffer[1][6];
} WeatherData;

/*
enum HealthMetric
HealthMetricStepCount
HealthMetricActiveSeconds
HealthMetricWalkedDistanceMeters
HealthMetricSleepSeconds
HealthMetricSleepRestfulSeconds
*/

typedef struct {
    HealthMetric healthMetric;
    uint32_t value;
    GBitmap *icon;
    char healthBuffer[1][10];
} HealthData;

typedef struct {
    int value;
    GBitmap *icon;
    char buffer[1][14];
    bool useBufferInstead;
} NumericData;

typedef struct {
    DataType dataType;
    Layer *root_layer;
    DataLocation dataLocation;
    void *data;
} DataLayer;

DataLayer *data_find_data_layer_from_root(Layer *layer);
GeneralDataType data_get_general_type(DataType dataType);
void data_settings_update_callback(Settings new_settings);
void data_load_app_icons();
void data_reload_time_fonts(DataLayer *dataLayer);
DataLayer *data_create_data_layer(DataType dataType, DataLocation dataLocation);
void data_destroy_data_layer(DataLayer *layer);