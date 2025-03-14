var settingsJSON = {
  "name": "Equalizer",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "equ_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "equ_btrealert",
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
          "label": "organize_colours",
          "storage_key": "equ_organize_colours",
          "pebble_key": 4,
          "default": true
        },
        {
          "type": "toggle",
          "label": "show_day",
          "storage_key": "equ_show_day",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "toggle",
          "label": "show_date",
          "storage_key": "bea_show_date",
          "pebble_key": 6,
          "default": true
        }
      ]
    },
    {
      "title": "pref_header_animations",
      "items": [
        {
          "type": "toggle",
          "label": "shake_to_animate",
          "storage_key": "equ_shake",
          "pebble_key": 2,
          "default": false
        },
        {
          "type": "list",
          "label": "equalizer_animation_mode",
          "storage_key": "equ_animation_mode",
          "pebble_key": 3,
          "list": [
            "animation_roll",
            "animation_music",
            "animation_expand_and_slam"
          ],
          "default": 0
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "time_colour",
          "storage_key": "equ_time_colour",
          "pebble_key": 7,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "equ_background_colour",
          "pebble_key": 8,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "battery_bar_colour",
          "storage_key": "equ_battery_bar_colour",
          "pebble_key": 9,
          "default": "GColorRed"
        }
      ]
    }
  ]
}