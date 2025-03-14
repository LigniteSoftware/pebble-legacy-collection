var settingsJSON = {
  "name": "Circle Inception",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "btrealert",
          "pebble_key": 1,
          "default": false
        }
      ]
    },
    {
      "title": "day_and_date",
      "items": [
        {
          "type": "toggle",
          "label": "time_instead",
          "storage_key": "time_instead",
          "pebble_key": 2,
          "default": false
        },
        {
          "type": "toggle",
          "label": "day",
          "storage_key": "day",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "toggle",
          "label": "date",
          "storage_key": "date",
          "pebble_key": 4,
          "default": true
        },
        {
          "type": "colour",
          "label": "day_and_date_colour",
          "storage_key": "day_and_date_colour",
          "pebble_key": 5,
          "default": "GColorWhite"
        }
      ]
    },
    {
      "title": "hands",
      "items": [
        {
          "type": "colour",
          "label": "hands_colour",
          "storage_key": "hands_colour",
          "pebble_key": 6,
          "default": "GColorRed"
        },
        {
          "type": "number_picker",
          "label": "hands_thickness",
          "storage_key": "hands_thickness",
          "pebble_key": 7,
          "min_value": 1,
          "max_value": 4,
          "default": 2
        }
      ]
    },
    {
      "title": "battery_bar",
      "items": [
        {
          "type": "toggle",
          "label": "show_battery_bar",
          "storage_key": "show_battery_bar",
          "pebble_key": 8,
          "default": true
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "battery_bar_background_colour",
          "pebble_key": 9,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "foreground_colour",
          "storage_key": "battery_bar_foreground_colour",
          "pebble_key": 10,
          "default": "GColorYellow"
        }
      ]
    },
    {
      "title": "outer_circle",
      "items": [
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "outer_circle_background_colour",
          "pebble_key": 11,
          "default": "GColorBlue"
        },
        {
          "type": "colour",
          "label": "foreground_colour",
          "storage_key": "outer_circle_foreground_colour",
          "pebble_key": 12,
          "default": "GColorVividCerulean"
        },
        {
          "type": "colour",
          "label": "text_colour",
          "storage_key": "outer_circle_text_colour",
          "pebble_key": 13,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "off_colour",
          "storage_key": "outer_circle_off_colour",
          "pebble_key": 14,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "on_colour",
          "storage_key": "outer_circle_on_colour",
          "pebble_key": 15,
          "default": "GColorGreen"
        }
      ]
    },
    {
      "title": "type_other",
      "items": [
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "window_background_colour",
          "pebble_key": 16,
          "default": "GColorBlack"
        }
      ]
    }
  ]
}