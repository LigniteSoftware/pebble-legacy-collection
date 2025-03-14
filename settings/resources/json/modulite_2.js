var settingsJSON = {
    "name": "Healthologue",
    "items": [{
        "title": "pref_header_general",
        "items": [{
            "type": "toggle",
            "label": "pref_btdisalert",
            "storage_key": "mod-btdisalert",
            "pebble_key": 0,
            "default": true
        }, {
            "type": "toggle",
            "label": "pref_btrealert",
            "storage_key": "mod-btrealert",
            "pebble_key": 1,
            "default": false
        }]
    }, {
        "title": "top",
        "items": [{
            "type": "list",
            "label": "data_type",
            "storage_key": "mod-data_type_top",
            "pebble_key": 2,
            "list": [
                "hidden",
                "health_type_step_count",
                "health_type_active_seconds",
                "health_type_walked_distance",
                "health_type_sleep_seconds",
                "health_type_sleep_restful_seconds",
                "health_type_resting_calories",
                "health_type_active_calories",
                "weather_type_icon_and_temp",
                "time_type_time",
                "time_type_time_and_date",
                "time_type_date",
                "watch_battery_level",
                "bluetooth_status"
            ],
            "default": 2
        }, {
            "type": "colour",
            "label": "accent_colour",
            "storage_key": "mod-accent_colour_top",
            "pebble_key": 3,
            "default": "GColorLightGray"
        }]
    }, {
        "title": "right",
        "items": [{
            "type": "list",
            "label": "data_type",
            "storage_key": "mod-data_type_right",
            "pebble_key": 4,
            "list": [
                "hidden",
                "health_type_step_count",
                "health_type_active_seconds",
                "health_type_walked_distance",
                "health_type_sleep_seconds",
                "health_type_sleep_restful_seconds",
                "health_type_resting_calories",
                "health_type_active_calories",
                "weather_type_icon_and_temp",
                "time_type_time",
                "time_type_time_and_date",
                "time_type_date",
                "watch_battery_level",
                "bluetooth_status"
            ],
            "default": 1
        }, {
            "type": "colour",
            "label": "accent_colour",
            "storage_key": "mod-accent_colour_right",
            "pebble_key": 5,
            "default": "GColorLightGray"
        }]
    }, {
        "title": "bottom",
        "items": [{
            "type": "list",
            "label": "data_type",
            "storage_key": "mod-data_type_bottom",
            "pebble_key": 6,
            "list": [
                "hidden",
                "health_type_step_count",
                "health_type_active_seconds",
                "health_type_walked_distance",
                "health_type_sleep_seconds",
                "health_type_sleep_restful_seconds",
                "health_type_resting_calories",
                "health_type_active_calories",
                "weather_type_icon_and_temp",
                "time_type_time",
                "time_type_time_and_date",
                "time_type_date",
                "watch_battery_level",
                "bluetooth_status"
            ],
            "default": 9
        }, {
            "type": "colour",
            "label": "accent_colour",
            "storage_key": "mod-accent_colour_bottom",
            "pebble_key": 7,
            "default": "GColorLightGray"
        }]
    }, {
        "title": "left",
        "items": [{
            "type": "list",
            "label": "data_type",
            "storage_key": "mod-data_type_left",
            "pebble_key": 8,
            "list": [
                "hidden",
                "health_type_step_count",
                "health_type_active_seconds",
                "health_type_walked_distance",
                "health_type_sleep_seconds",
                "health_type_sleep_restful_seconds",
                "health_type_resting_calories",
                "health_type_active_calories",
                "weather_type_icon_and_temp",
                "time_type_time",
                "time_type_time_and_date",
                "time_type_date",
                "watch_battery_level",
                "bluetooth_status"
            ],
            "default": 3
        }, {
            "type": "colour",
            "label": "accent_colour",
            "storage_key": "mod-accent_colour_left",
            "pebble_key": 9,
            "default": "GColorLightGray"
        }]
    }, {
        "title": "colours",
        "items": [{
            "type": "colour",
            "label": "background_colour",
            "storage_key": "mod-background_colour",
            "pebble_key": 10,
            "default": "GColorBlack"
        }, {
            "type": "colour",
            "label": "outer_circle_colour",
            "storage_key": "mod-outer_circle_colour",
            "pebble_key": 13,
            "default": "GColorRed"
        }, {
            "type": "colour",
            "label": "outer_circle_background_colour",
            "storage_key": "mod-outer_circle_background_colour",
            "pebble_key": 22,
            "default": "GColorBulgarianRose"
        }]
    }, {
        "title":"analogue_hands",
        "items":[
            {
                "type": "toggle",
                "label": "analogue_hands",
                "storage_key": "mod-analogue_hands",
                "pebble_key": 14,
                "default": true
            }, {
                "type": "toggle",
                "label": "hour_hand",
                "storage_key": "mod-hour_hand",
                "pebble_key": 23,
                "default": true
            }, {
                "type": "colour",
                "label": "hour_hand_colour",
                "storage_key": "mod-hour_hand_colour",
                "pebble_key": 11,
                "default": "GColorWhite"
            }, {
                "type": "toggle",
                "label": "minute_hand",
                "storage_key": "mod-minute_hand",
                "pebble_key": 24,
                "default": true
            }, {
                "type": "colour",
                "label": "minute_hand_colour",
                "storage_key": "mod-minute_hand_colour",
                "pebble_key": 12,
                "default": "GColorRed"
            }, {
                "type": "toggle",
                "label": "second_hand",
                "storage_key": "mod-second_hand",
                "pebble_key": 15,
                "default": false
            }, {
                "type": "colour",
                "label": "second_hand_colour",
                "storage_key": "mod-second_hand_colour",
                "pebble_key": 21,
                "default": "GColorWhite"
            }, {
                "type": "toggle",
                "label": "shake_to_hide_hands",
                "storage_key": "mod-shake_to_hide_hands",
                "pebble_key": 19,
                "default": false
            }
        ]
    }, {
        "title": "other_options",
        "items": [{
            "type": "toggle",
            "label": "imperial_measurements",
            "storage_key": "mod-imperial_measurements",
            "pebble_key": 20,
            "default": false
        }, {
            "type": "list",
            "label": "outer_circle_type",
            "storage_key": "mod-outer_circle_type",
            "pebble_key": 16,
            "list": [
                "OuterCircleTypeBattery",
                "OuterCircleTypeHidden",
                "OuterCircleTypeMinute",
                "OuterCircleTypeAnalogueLines",
                "OuterCircleTypeAnalogueCircles"
            ],
            "default": 0
        }, {
            "type": "toggle",
            "label": "temperature_format_celcius",
            "storage_key": "use_celsius",
            "pebble_key": 17,
            "default": false
        }, {
            "type": "textfield",
            "label": "custom_location",
            "storage_key": "custom_location",
            "pebble_key": 18,
            "hint": "enter_custom_location",
            "default": ""
        }]
    }]
}