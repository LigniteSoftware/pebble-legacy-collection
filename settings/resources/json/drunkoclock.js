var settingsJSON = {
    "name": "Drunk O' Clock",
    "items": [{
        "title": "pref_header_general",
        "items": [{
            "type": "toggle",
            "label": "pref_btdisalert",
            "storage_key": "btdisalert",
            "pebble_key": 0,
            "default": true
        }, {
            "type": "toggle",
            "label": "pref_btrealert",
            "storage_key": "btrealert",
            "pebble_key": 1,
            "default": false
        }]
    }, {
        "title": "pref_header_functionality",
        "items": [{
            "type": "toggle",
            "label": "sober_mode",
            "storage_key": "doc_sober_mode",
            "pebble_key": 2,
            "default": false
        }, {
            "type": "toggle",
            "label": "battery_bar",
            "storage_key": "doc_battery_bar",
            "pebble_key": 10,
            "default": false
        }, {
            "type": "list",
            "label": "shake_use",
            "storage_key": "doc_shake_use",
            "pebble_key": 3,
            "list": [
                "ShakeUseExtraScreen",
                "ShakeUseChangeSwear",
                "ShakeUseHide",
                "ShakeUseDisabled"
            ],
            "default": 0
        }, {
            "type": "list",
            "label": "slot_use",
            "storage_key": "doc_slot_use",
            "pebble_key": 4,
            "list": [
                "SlotUseDate",
                "SlotUseBluetoothStatus",
                "SlotUseTemperature",
                "SlotUseBatteryLevel",
                "SlotUseDigitalTime",
                "SlotUseNothing"
            ],
            "default": 0
        }]
    }, {
        "title": "pref_header_colours",
        "items": [{
            "type": "colour",
            "label": "time_colour",
            "storage_key": "doc_time_colour",
            "pebble_key": 5,
            "default": "GColorWhite"
        }, {
            "type": "colour",
            "label": "swear_colour",
            "storage_key": "doc_swear_colour",
            "pebble_key": 6,
            "default": "GColorWhite"
        }, {
            "type": "colour",
            "label": "slot_colour",
            "storage_key": "doc_slot_colour",
            "pebble_key": 7,
            "default": "GColorWhite"
        }, {
            "type": "colour",
            "label": "graphics_colour",
            "storage_key": "doc_graphics_colour",
            "pebble_key": 8,
            "default": "GColorWhite"
        }, {
            "type": "colour",
            "label": "background_colour",
            "storage_key": "doc_background_colour",
            "pebble_key": 9,
            "default": "GColorBlack"
        }]
    }, {
        "title": "pref_header_weather",
        "items": [{
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