#pragma once
#include "lignite.h"

typedef struct DataLayer {
    uint8_t mode;
    Layer *root_layer;
    Layer *graphics_layer;
    TextLayer *title_layer;
    TextLayer *subtitle_layer;
    BitmapLayer *side_layer;
    GBitmap *side_icon;
} DataLayer;

typedef struct DataLayerUpdate {
    bool bluetooth_connected;
    char new_title[1][30];
    char new_subtitle[1][30];
} DataLayerUpdate;

DataLayer *data_layer_get_layer();
void data_layer_mark_dirty(bool animated);
bool data_layer_set_update(DataLayerUpdate update);
void data_layer_temporarily_disable_animation();
void data_layer_create(Layer *window_layer);
void data_layer_destroy();
void data_layer_set_settings(Settings settings);
