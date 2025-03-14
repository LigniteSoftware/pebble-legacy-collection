var settingsJSON = {
  "name": "Rings",
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
        },
        {
          "type": "toggle",
          "label": "week_number",
          "storage_key": "week_number",
          "pebble_key": 2,
          "default": false
        }
      ]
    },
    {
      "title": "rings",
      "items": [
        {
          "type": "toggle",
          "label": "second_radial",
          "storage_key": "second_radial",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "colour",
          "label": "second_radial_colour",
          "storage_key": "second_radial_colour",
          "pebble_key": 4,
          "default": "GColorPictonBlue"
        },
        {
          "type": "toggle",
          "label": "hour_radial",
          "storage_key": "hour_radial",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "colour",
          "label": "hour_radial_colour",
          "storage_key": "hour_radial_colour",
          "pebble_key": 6,
          "default": "GColorBlue"
        },
        {
          "type": "toggle",
          "label": "minute_radial",
          "storage_key": "minute_radial",
          "pebble_key": 7,
          "default": true
        },
        {
          "type": "colour",
          "label": "minute_radial_colour",
          "storage_key": "minute_radial_colour",
          "pebble_key": 8,
          "default": "GColorDukeBlue"
        },
        {
          "type": "toggle",
          "label": "battery_radial",
          "storage_key": "battery_radial",
          "pebble_key": 9,
          "default": true
        },
        {
          "type": "colour",
          "label": "battery_radial_colour",
          "storage_key": "battery_radial_colour",
          "pebble_key": 10,
          "default": "GColorWhite"
        }
      ]
    },
    {
      "title": "type_other",
      "items": [
        {
          "type": "colour",
          "label": "bluetooth_icon_colour",
          "storage_key": "bluetooth_icon_colour",
          "pebble_key": 11,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "window_background_colour",
          "pebble_key": 12,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "text_colour",
          "storage_key": "text_colour",
          "pebble_key": 13,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}