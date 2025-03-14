var settingsJSON = {
  "name": "Simple Squares",
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
      "title": "rings",
      "items": [
        {
          "type": "toggle",
          "label": "day",
          "storage_key": "day",
          "pebble_key": 2,
          "default": true
        },
        {
          "type": "colour",
          "label": "day_colour",
          "storage_key": "day_colour",
          "pebble_key": 3,
          "default": "GColorWhite"
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
          "label": "date_colour",
          "storage_key": "date_colour",
          "pebble_key": 5,
          "default": "GColorWhite"
        },
        {
          "type": "toggle",
          "label": "battery_bar",
          "storage_key": "battery_bar",
          "pebble_key": 6,
          "default": true
        },
        {
          "type": "colour",
          "label": "battery_bar_colour",
          "storage_key": "battery_bar_colour",
          "pebble_key": 7,
          "default": "GColorWhite"
        }
      ]
    },
    {
      "title": "type_other",
      "items": [
        {
          "type": "colour",
          "label": "hour_colour",
          "storage_key": "hour_colour",
          "pebble_key": 8,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "minute_colour",
          "storage_key": "minute_colour",
          "pebble_key": 9,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "background_colour",
          "pebble_key": 10,
          "default": "GColorBlack"
        }
      ]
    }
  ]
}