#include <pebble.h>
#include "lignite.h"
#include "data.h"
#include "graphics.h"
#ifndef PBL_PLATFORM_APLITE
#include "_resources/gbitmap_color_palette_manipulator.h"
#endif

DataLayer *data_layers[DATA_MAX_AMOUNT_OF_LAYERS];
uint8_t data_layer_stack_count = 0;
static const char *climacon_condition_codes[14] = {
	"B", //CLEAR_DAY
	"I", //CLEAR_NIGHT
	"Z", //WINDY
	"O", //COLD
	"C", //PARTLY_CLOUDY_DAY
	"J", //PARTLY_CLOUDY_NIGHT
	"N", //HAZE
	"P", //CLOUD
	"Q", //RAIN
	"W", //SNOW
	"X", //HAIL
	"P", //CLOUDY
	"V", //STORM
	"P", //NA
};
static const uint32_t icon_resources[9] = {
    RESOURCE_ID_IMAGE_STEPS_ICON,
    RESOURCE_ID_IMAGE_HEART_ICON,
    RESOURCE_ID_IMAGE_DISTANCE_WALKED_ICON,
    RESOURCE_ID_IMAGE_TOTAL_SLEEP_ICON,
    RESOURCE_ID_IMAGE_DEEP_SLEEP_ICON,
	RESOURCE_ID_IMAGE_CALORIES_ICON,
	RESOURCE_ID_IMAGE_CALORIES_ICON,
	RESOURCE_ID_IMAGE_BATTERY_ICON,
	RESOURCE_ID_IMAGE_BLUETOOTH_ICON
};
static GBitmap *app_icons[4];
Settings data_settings;
GFont weatherFontLarge;

DataLayer *data_find_data_layer_from_root(Layer *layer){
    for(uint8_t i = 0; i < DATA_MAX_AMOUNT_OF_LAYERS; i++){
        if(data_layers[i]){
            if(data_layers[i]->root_layer == layer){
                return data_layers[i];
            }
        }
    }
    return NULL;
}

bool data_location_is_side(DataLocation dataLocation){
    return (dataLocation == DataLocationLeft) || (dataLocation == DataLocationRight);
}

#ifndef PBL_ROUND
GRect data_adjust_grect(GRect frame, DataLocation location){
	uint8_t adjust_amount = 8;
	switch(location){
		case DataLocationBottom:
            frame.origin.y += adjust_amount/2;
            break;
        case DataLocationTop:
            frame.origin.y -= adjust_amount;
            break;
        case DataLocationLeft:
            frame.origin.x -= adjust_amount/2;
            break;
        case DataLocationRight:
            frame.origin.x += adjust_amount/2;
            break;
	}
	return frame;
}
#endif

GRect data_get_time_frame(TimeData *timeData, DataLocation location){
    GRect frame = GRect(0, 0, WINDOW_SIZE.w, WINDOW_SIZE.h/2 - 20);

    if(data_location_is_side(location)){
        frame.size.w = frame.size.w/2;
    }

    switch(location){
        case DataLocationBottom:
            frame.origin.y = WINDOW_SIZE.h/2 + 15;
            break;
        case DataLocationTop:
            frame.origin.y = 20;
            break;
        case DataLocationLeft:
            frame.origin.x = 4;
            frame.origin.y = WINDOW_SIZE.h/2 - 15;
            break;
        case DataLocationRight:
            frame.origin.x = WINDOW_SIZE.w/2 - 2;
            frame.origin.y = WINDOW_SIZE.h/2 - 15;
            break;
        default:
            //APP_LOG(APP_LOG_LEVEL_WARNING, "Unknown data location %d for %p!", location, timeData);
            break;
    }

	#ifndef PBL_ROUND
	frame = data_adjust_grect(frame, location);
	#endif

    return frame;
}

GRect data_get_date_frame(TimeData *timeData, DataLocation location){
    GRect frame = GRect(0, 0, WINDOW_SIZE.w, WINDOW_SIZE.h/2 - 20);

    if(data_location_is_side(location)){
        frame.size.w = frame.size.w/2;
    }

    switch(location){
        case DataLocationBottom:
            frame.origin.y = (WINDOW_SIZE.h/3 * 2) + 17;
            break;
        case DataLocationTop:
            frame.origin.y = 50;
            break;
        case DataLocationLeft:
            frame.origin.x = 4;
            frame.origin.y = WINDOW_SIZE.h/2 + 7;
            break;
        case DataLocationRight:
            frame.origin.x = WINDOW_SIZE.w/2 - 2;
            frame.origin.y = WINDOW_SIZE.h/2 + 7;
            break;
        default:
            //APP_LOG(APP_LOG_LEVEL_WARNING, "Unknown data location %d for %p!", location, timeData);
            break;
    }

    if(timeData->timeFormat == TIME_FORMAT_DATE){
        if(location == DataLocationBottom){
            frame.origin.y -= 12;
        }
        else{
            frame.origin.y -= 20;
        }
    }

	#ifndef PBL_ROUND
	frame = data_adjust_grect(frame, location);
	#endif

    return frame;
}

void data_update_colours_for_ctx_and_location(GContext *ctx, DataLocation location){
    graphics_context_set_fill_color(ctx, data_settings.dataColours[location]);
    graphics_context_set_stroke_color(ctx, data_settings.dataColours[location]);
    graphics_context_set_text_color(ctx, data_settings.dataColours[location]);
}

void data_time_update_proc(Layer *layer, GContext *ctx){
	//NSLog("U there");
    DataLayer *dataLayer = data_find_data_layer_from_root(layer);
    TimeData *timeData = (TimeData*)dataLayer->data;

    data_update_colours_for_ctx_and_location(ctx, dataLayer->dataLocation);

    //NSLog("Updating for %d font %p time format %d bytes left %d", dataLayer->dataLocation, &timeData->timeFont, timeData->timeFormat, heap_bytes_free());

    strftime(timeData->timeBuffer[0], sizeof(timeData->timeBuffer[0]), timeData->timeFormatBuffer[0], timeData->timeT);
//NSLog("1");
    if(timeData->timeFormat < TIME_FORMAT_DATE){
        GRect timeFrame = data_get_time_frame(timeData, dataLayer->dataLocation);
        GTextAlignment timeAlignment = GTextAlignmentCenter;
        GTextOverflowMode timeOverFlow = GTextOverflowModeWordWrap;
//NSLog("2");
        graphics_draw_text(ctx, timeData->timeBuffer[0], timeData->timeFont, timeFrame, timeOverFlow, timeAlignment, NULL);
    }

    if(timeData->timeFormat > TIME_FORMAT_TIME){
        GRect dateFrame = data_get_date_frame(timeData, dataLayer->dataLocation);
		//NSLog("2.1");
		strftime(timeData->dateBuffer[0], sizeof(timeData->dateBuffer[0]), data_settings.dateFormat[0], timeData->timeT);
		//NSLog("2.2");
        graphics_draw_text(ctx, timeData->dateBuffer[0], timeData->dateFont, dateFrame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
		//NSLog("2.3");
    }
}

void data_time_updated_callback(void *layer, struct tm *t){
    DataLayer *dataLayer = (DataLayer*)layer;
    TimeData *layerTimeData = (TimeData*)dataLayer->data;
    size_t size = sizeof(tm);
    if(layerTimeData->timeT){
        free(layerTimeData->timeT);
    }
    layerTimeData->timeT = malloc(size);
    memcpy(layerTimeData->timeT, t, size);
	//NSLog("Updated timet for %d", dataLayer->dataLocation);
    layer_mark_dirty(dataLayer->root_layer);
	//NSLog("Marked as dirty");
}

GRect data_get_temperature_frame(WeatherData *weatherData, DataLocation location){
    GRect temperatureFrame = GRect(0, 30, WINDOW_SIZE.w/2, WINDOW_SIZE.h/2);

    switch(location){
        case DataLocationBottom:
            temperatureFrame.origin.y = WINDOW_SIZE.h/3 * 2 + 10;
        case DataLocationTop:
            temperatureFrame.origin.x = WINDOW_SIZE.w/2 - 20;
            break;
        case DataLocationRight:
            temperatureFrame.origin.x = WINDOW_SIZE.w/2;
        case DataLocationLeft:
            temperatureFrame.origin.y = WINDOW_SIZE.h/2 - 12;
            break;
        default:
            break;
    }

	#ifndef PBL_ROUND
	temperatureFrame = data_adjust_grect(temperatureFrame, location);
	temperatureFrame.origin.x += 6;
	#endif

    //NSLog("{%d, %d, %d, %d} for %d", temperatureFrame.origin.x, temperatureFrame.origin.y, temperatureFrame.size.w, temperatureFrame.size.h, location);

    return temperatureFrame;
}

GRect data_get_condition_frame(WeatherData *weatherData, DataLocation location){
    GRect conditionFrame = GRect(0, 10, WINDOW_SIZE.w/1.5, WINDOW_SIZE.h/2);

    switch(location){
        case DataLocationBottom:
            conditionFrame.origin.y = WINDOW_SIZE.h/2 + 20;
        case DataLocationTop:
            conditionFrame.origin.x = 18;
            break;
        case DataLocationRight:
            conditionFrame.origin.x = WINDOW_SIZE.w/2;
        case DataLocationLeft:
            conditionFrame.origin.y = WINDOW_SIZE.h/2 - 10;
            break;
        default:
            break;
    }

	#ifndef PBL_ROUND
	conditionFrame = data_adjust_grect(conditionFrame, location);
	conditionFrame.origin.x -= 6;
	#endif

    return conditionFrame;
}

void data_weather_update_proc(Layer *layer, GContext *ctx){
    DataLayer *dataLayer = data_find_data_layer_from_root(layer);
    WeatherData *weatherData = (WeatherData*)dataLayer->data;
	bool isSide = data_location_is_side(dataLayer->dataLocation);

	data_update_colours_for_ctx_and_location(ctx, dataLayer->dataLocation);
    snprintf(weatherData->temperatureBuffer[0], sizeof(weatherData->temperatureBuffer[0]), "%d°", weatherData->currentWeather.temperature);
    graphics_draw_text(ctx, weatherData->temperatureBuffer[0], isSide ? fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD) : fonts_get_system_font(FONT_KEY_GOTHIC_18), data_get_temperature_frame(weatherData, dataLayer->dataLocation), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

	if(!isSide){
        graphics_draw_text(ctx, climacon_condition_codes[weatherData->currentWeather.icon], weatherFontLarge, data_get_condition_frame(weatherData, dataLayer->dataLocation), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
}

GRect data_get_health_value_frame(DataLocation location){
    GRect healthFrame = GRect(0, 40, WINDOW_SIZE.w, WINDOW_SIZE.h/2);

    switch(location){
        case DataLocationBottom:
            healthFrame.origin.y = WINDOW_SIZE.h/3 * 2 - 5;
        case DataLocationTop:
            healthFrame.origin.x = 0;
            break;
        case DataLocationRight:
            healthFrame.origin.x = WINDOW_SIZE.w/2;
        case DataLocationLeft:
            healthFrame.size.w = WINDOW_SIZE.w/2;
            healthFrame.origin.y = WINDOW_SIZE.h/2;
            break;
        default:
            break;
    }

	#ifndef PBL_ROUND
	healthFrame = data_adjust_grect(healthFrame, location);
	#endif

    ////NSLog("{%d, %d, %d, %d} for %d", temperatureFrame.origin.x, temperatureFrame.origin.y, temperatureFrame.size.w, temperatureFrame.size.h, location);

    return healthFrame;
}

GRect data_get_health_icon_frame(DataLocation location){
    GRect healthFrame = GRect(WINDOW_SIZE.w/2 - 10, 20, 20, 20);

    switch(location){
        case DataLocationBottom:
            healthFrame.origin.y = WINDOW_SIZE.h - 40;
            break;
        case DataLocationRight:
            healthFrame.origin.x = (WINDOW_SIZE.w/4 * 3) - 10;
            healthFrame.origin.y = WINDOW_SIZE.h/2 - 20;
            break;
        case DataLocationLeft:
            healthFrame.origin.x = WINDOW_SIZE.w/4 - 10;
            healthFrame.origin.y = WINDOW_SIZE.h/2 - 20;
            break;
        default:
            break;
    }

	#ifndef PBL_ROUND
	healthFrame = data_adjust_grect(healthFrame, location);
	#endif

    ////NSLog("{%d, %d, %d, %d} for %d", temperatureFrame.origin.x, temperatureFrame.origin.y, temperatureFrame.size.w, temperatureFrame.size.h, location);

    return healthFrame;
}

void data_health_format_value(HealthData *healthData){
    uint32_t thousands = 0, hundreds = 0;

    switch(healthData->healthMetric){
        case HealthMetricStepCount:
            if(healthData->value > 9999){
                thousands = healthData->value/1000;
                hundreds = (healthData->value % 1000)/100;
                snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ld.%ldk", thousands, hundreds);
            }
            else{
                snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ld", healthData->value);
            }
            break;
        case HealthMetricSleepSeconds:
        case HealthMetricSleepRestfulSeconds:
        case HealthMetricActiveSeconds:
		//APP_LOG(APP_LOG_LEVEL_INFO, "%ld", healthData->value);
            if(healthData->value < 60){
                snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%lds", healthData->value);
            }
            else if(healthData->value > 59 && healthData->value < 3600){
                thousands = healthData->value/60;
                hundreds = (healthData->value % 60);
				if(hundreds > 0){
                	snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ldm %lds", thousands, hundreds);
				}
				else{
					snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ldm", thousands);
				}
            }
            else{
                thousands = healthData->value/3600;
                hundreds = (healthData->value % 3600)/60;
                snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ldh %ldm", thousands, hundreds);
            }
            break;
        case HealthMetricWalkedDistanceMeters:
			//NSLog("Got an imperial set of %d", data_settings.imperialMeasurements);
			if(data_settings.imperialMeasurements){
				uint32_t freakValue = healthData->value*3.28;
				if(freakValue < 528){
	                snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ldft", freakValue);
	            }
	            else{
	                thousands = freakValue/5280;
	                hundreds = (freakValue % 5280)/528;
	                snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ld.%ldmi", thousands, hundreds);
	            }
			}
			else{
	            if(healthData->value <= 999){
	                snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ldm", healthData->value);
	            }
	            else{
	                thousands = healthData->value/1000;
	                hundreds = (healthData->value % 1000)/100;
	                snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ld.%ldkm", thousands, hundreds);
	            }
			}
            break;
		case HealthMetricActiveKCalories:
		case HealthMetricRestingKCalories:
			snprintf(healthData->healthBuffer[0], sizeof(healthData->healthBuffer[0]), "%ld", healthData->value);
			break;
		default:
			break;
    }
    //NSLog(healthData->healthBuffer[0]);
}

void data_load_app_icons(){
    for(int i = 0; i < 4; i++){
        if(app_icons[i]){
			//NSLog("Destroying icon %d, exists %d", i, !(app_icons[i] == NULL));
            gbitmap_destroy(app_icons[i]);
			//NSLog("Destroyed.");
        }
		//NSLog("Checking %d", data_settings.dataTypes[i]);
		switch(data_settings.dataTypes[i]){
		    case DataTypeStepCount:
		    case DataTypeActiveSeconds:
		    case DataTypeWalkedDistance:
		    case DataTypeSleepSeconds:
		    case DataTypeSleepRestfulSeconds:
			case DataTypeActiveKCalories:
			case DataTypeRestingKCalories:;
				HealthData *healthData = (HealthData*)data_layers[i]->data;
				app_icons[i] = gbitmap_create_with_resource(icon_resources[healthData->healthMetric]);
				//NSLog("Creating");
				break;
		    case DataTypeBatteryLevel:
				app_icons[i] = gbitmap_create_with_resource(icon_resources[7]);
				break;
		    case DataTypeBluetoothStatus:
				//NSWarn("Creating a bluetooth one for %d", i);
				app_icons[i] = gbitmap_create_with_resource(icon_resources[8]);
				break;

			default:
				//This doesn't use an icon
				//NSLog("This doesn't use an icon");
				break;
		}
		#ifndef PBL_PLATFORM_APLITE
		//NSLog("Replacing colour for %d, exists %d", i, (app_icons[i] ? 1 : 0));
		//NSLog("%s", get_gcolor_text(data_settings.dataColours[i]));
        replace_gbitmap_color(GColorBlack, data_settings.dataColours[i], app_icons[i], NULL);
		//NSLog("Replaced for %d", i);
		#endif
    }
}

void data_health_update_proc(Layer *layer, GContext *ctx){
	// NSLog("Spook!");
	// #ifndef PBL_PLATFORM_APLITE
    DataLayer *dataLayer = data_find_data_layer_from_root(layer);
    HealthData *healthData = (HealthData*)dataLayer->data;

    data_update_colours_for_ctx_and_location(ctx, dataLayer->dataLocation);

    data_health_format_value(healthData);

	// NSLog("Ctx %s, buffer %s, location %d, datalayer exists %d", ctx ? "exists" : "doesn't exist", healthData->healthBuffer[0], dataLayer->dataLocation, dataLayer ? 1 : 0);

    graphics_draw_text(ctx, healthData->healthBuffer[0], fonts_get_system_font(FONT_KEY_GOTHIC_18), data_get_health_value_frame(dataLayer->dataLocation), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    graphics_context_set_compositing_mode(ctx, GCompOpSet);
	if(app_icons[dataLayer->dataLocation]){
	    graphics_draw_bitmap_in_rect(ctx, app_icons[dataLayer->dataLocation], data_get_health_icon_frame(dataLayer->dataLocation));
	}
	// #endif
}

void data_number_update_proc(Layer *layer, GContext *ctx){
	DataLayer *dataLayer = data_find_data_layer_from_root(layer);
    NumericData *numericData = (NumericData*)dataLayer->data;

    data_update_colours_for_ctx_and_location(ctx, dataLayer->dataLocation);

	if(!numericData->useBufferInstead){
		snprintf(numericData->buffer[0], sizeof(numericData->buffer[0]), "%d", numericData->value);
	}

	graphics_draw_text(ctx, numericData->buffer[0], fonts_get_system_font(FONT_KEY_GOTHIC_18), data_get_health_value_frame(dataLayer->dataLocation), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

	if(app_icons[dataLayer->dataLocation]){
	    graphics_context_set_compositing_mode(ctx, GCompOpSet);
	    graphics_draw_bitmap_in_rect(ctx, app_icons[dataLayer->dataLocation], data_get_health_icon_frame(dataLayer->dataLocation));
	}
	else{
		NSError("No icon was provided for layer %d", dataLayer->dataLocation);
	}
}

void data_settings_update_callback(Settings new_settings){
	data_settings = new_settings;
	data_load_app_icons();
	for(int i = 0; i < 4; i++){
		//NSLog("3.%d (exists: %s) (bytes free: %d)", i, (data_layers[i]->root_layer) ? "Yeah" : "Nah", heap_bytes_free());
		if(data_layers[i]->dataType != DataTypeHidden){
			layer_mark_dirty(data_layers[i]->root_layer);
		}
	}
}

GeneralDataType data_get_general_type(DataType dataType){
	if(dataType > DataTypeHidden && dataType < DataTypeWeather){
		return GeneralDataTypeHealth;
	}
	else if(dataType == DataTypeWeather){
		return GeneralDataTypeWeather;
	}
	else if(dataType > DataTypeWeather && dataType < DataTypeBatteryLevel){
		return GeneralDataTypeTime;
	}
	else if(dataType > DataTypeDate){
		return GeneralDataTypeNumieric;
	}
	return GeneralDataTypeHidden;
}

void data_reload_time_fonts(DataLayer *dataLayer){
	TimeData *data = dataLayer->data;
	if(data->timeFont){
		//fonts_unload_custom_font(data->timeFont);
		//NSWarn("I'd love to destroy this font but then I'd crash %d", heap_bytes_free());
	}
	else{
		//NSLog("FONT OESN");
	}
	data->timeFont = fonts_load_custom_font(resource_get_handle(TIME_FONT_SMALL));
	if(data->timeFormat == TIME_FORMAT_TIME && !data_location_is_side(dataLayer->dataLocation)){
		data->timeFont = fonts_load_custom_font(resource_get_handle(TIME_FONT_LARGE));
	}
	data->dateFont = fonts_get_system_font(FONT_KEY_GOTHIC_18);
}

DataLayer *data_create_data_layer(DataType dataType, DataLocation dataLocation){
    DataLayer *new_layer = malloc(sizeof(DataLayer));
    new_layer->root_layer = layer_create(GRect(0, 0, WINDOW_SIZE.w, WINDOW_SIZE.h));
    new_layer->dataLocation = dataLocation;
    new_layer->dataType = dataType;

	//NSLog("Creating data layer with type %d for location %d bytes free %d", dataType, dataLocation, heap_bytes_free());

    switch(dataType){
        case DataTypeTime:
		case DataTypeTimeAndDate:
		case DataTypeDate:
            {
                new_layer->data = malloc(sizeof(TimeData));

                TimeData *data = new_layer->data;
                data->updateCallback = data_time_updated_callback;
                data->timeT = malloc(1);
                strcpy(data->timeFormatBuffer[0], clock_is_24h_style() ? "%H:%M" : "%I:%M");

				//TODO Fix this for when the actual timeformat is set
				data_reload_time_fonts(new_layer);

                layer_set_update_proc(new_layer->root_layer, data_time_update_proc);
            }
            break;
        case DataTypeWeather:
            {
				WeatherData *weatherData = malloc(sizeof(WeatherData));;
				weatherData->currentWeather.temperature = 0;
                new_layer->data = weatherData;

				if(!weatherFontLarge){
					weatherFontLarge = fonts_load_custom_font(resource_get_handle(WEATHER_FONT_LARGE));
				}

                layer_set_update_proc(new_layer->root_layer, data_weather_update_proc);
            }
            break;
        case DataTypeStepCount:
		case DataTypeActiveSeconds:
		case DataTypeWalkedDistance:
		case DataTypeSleepSeconds:
		case DataTypeSleepRestfulSeconds:
		case DataTypeActiveKCalories:
		case DataTypeRestingKCalories:
            {
				HealthData *healthData = malloc(sizeof(HealthData));
				healthData->value = 0;
                new_layer->data = healthData;

                layer_set_update_proc(new_layer->root_layer, data_health_update_proc);
            }
            break;
		case DataTypeBatteryLevel:
			{
				NumericData *numericData = malloc(sizeof(NumericData));
				numericData->value = 0;
				numericData->useBufferInstead = true;
				numericData->icon = app_icons[dataLocation];

				new_layer->data = numericData;

				layer_set_update_proc(new_layer->root_layer, data_number_update_proc);
			}
			break;
		case DataTypeBluetoothStatus:
			{
				NumericData *numericData = malloc(sizeof(NumericData));
				numericData->value = 0;
				numericData->useBufferInstead = true;
				numericData->icon = app_icons[dataLocation];

				new_layer->data = numericData;

				layer_set_update_proc(new_layer->root_layer, data_number_update_proc);
			}
			break;
        default:
            //NSLog("Unrecognized data type %d", dataLocation);
			new_layer->data = malloc(1);
            break;
    }

    data_layers[dataLocation] = new_layer;
	//NSLog("Returning new layer %d", dataLocation);
    return new_layer;
}

void data_destroy_data_layer(DataLayer *layer){
    if(layer->root_layer){
		//NSLog("Destroying root layer");
        layer_destroy(layer->root_layer);
    }
    else{
        //APP_LOG(APP_LOG_LEVEL_ERROR, "Error: root layer for %p is null! Not destroying, sorry", layer);
    }

    if(layer->data){
		//NSLog("Destroying layer data");
        free(layer->data);
    }
    else{
        //APP_LOG(APP_LOG_LEVEL_ERROR, "Error: data for %p is null! Not destroying, sorry", layer);
    }
	//NSLog("Destroying it all");
	free(layer);
}