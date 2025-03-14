var settingsJSON = {
  "name": "Beat",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "bea_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "bea_btrealert",
          "pebble_key": 1,
          "default": false
        }
      ]
    },
    {
      "title": "pref_header_lookandfeel",
      "items": [
        {
          "type": "toggle",
          "label": "shake_to_animate",
          "storage_key": "bea_shake",
          "pebble_key": 2,
          "default": false
        },
        {
          "type": "list",
          "label": "beatline_data",
          "storage_key": "bea_data",
          "pebble_key": 3,
          "list": [
            "battery",
            "bluetooth",
            "random"
          ],
          "default": 0
        },
        {
          "type": "list",
          "label": "animation_type",
          "storage_key": "bea_animation_type",
          "pebble_key": 4,
          "list": [
            "apple_watch_mode",
            "minutely",
            "on_change"
          ],
          "default": 0
        },
        {
          "type": "toggle",
          "label": "show_date",
          "storage_key": "bea_show_date",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "toggle",
          "label": "show_battery",
          "storage_key": "bea_show_battery",
          "pebble_key": 6,
          "default": true
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "time_colour",
          "storage_key": "bea_time_colour",
          "pebble_key": 7,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "bea_background_colour",
          "pebble_key": 8,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "front_colour",
          "storage_key": "bea_front_colour",
          "pebble_key": 9,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "back_colour",
          "storage_key": "bea_back_colour",
          "pebble_key": 10,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}