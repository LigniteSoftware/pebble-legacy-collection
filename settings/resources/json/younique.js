var settingsJSON = {
    "name": "Younique",
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
        "title": "pref_header_photos",
        "items": [{
            "type": "photo",
            "label": "photo_uploader",
            "storage_key": "photo_uploader",
            "pebble_key": 2,
            "default": false
        }, {
          "type": "list",
          "label": "photo_timeout",
          "storage_key": "photo_timeout",
          "pebble_key": 5,
          "list": [
            "never",
            "mins15",
            "mins30",
            "mins60",
            "mins180"
          ],
          "default": 2
        }, {
            "type": "toggle",
            "label": "photo_nightmode",
            "storage_key": "night_mode",
            "pebble_key": 6,
            "default": true
        }, {
            "type": "toggle",
            "label": "show_island_image",
            "storage_key": "show_island_image",
            "pebble_key": 6,
            "default": true
        }]
    }, {
        "title": "pref_header_modules",
        "items": [{
            "type": "modules",
            "label": "module_selector",
            "storage_key": "module_selector",
            "pebble_key": 3,
            "default": false
        }]
    }, {
        "title": "pref_header_weather",
        "items": [{
            "type": "toggle",
            "label": "use_celsius",
            "storage_key": "use_celsius",
            "pebble_key": 1,
            "default": false
        }, {
            "type": "textfield",
            "label": "nothing",
            "storage_key": "custom_location",
            "pebble_key": 18,
            "hint": "enter_custom_location",
            "default": ""
        }]
    }, {
        "title": "pref_header_lookandfeel",
        "items": [{
          "type": "list",
          "label": "younique_shake_mode",
          "storage_key": "younique_shake_mode",
          "pebble_key": 5,
          "list": [
            "ShakeModeShowDateAndBattery",
            "ShakeModeShowEverything",
            "ShakeModeDisabled",
            "ShakeModeHideAll"
          ],
          "default": 1
        }, {
            "type": "toggle",
            "label": "large_time_font",
            "storage_key": "large_time_font",
            "pebble_key": 6,
            "default": false
        }, {
            "type": "toggle",
            "label": "imperial_measurements",
            "storage_key": "imperial_measurements",
            "pebble_key": 1,
            "default": false
        }]
    }, {
      "title": "pref_header_colours",
      "colourOnly":true,
      "items": [
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "background_colour",
          "pebble_key": 6,
          "default": "GColorBlack"
        },
          {
            "type": "colour",
            "label": "module_background_colour",
            "storage_key": "module_colour",
            "pebble_key": 7,
            "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "battery_bar_colour",
          "storage_key": "battery_bar_colour",
          "pebble_key": 7,
          "default": "GColorRed"
        }
      ]
    } ]
}