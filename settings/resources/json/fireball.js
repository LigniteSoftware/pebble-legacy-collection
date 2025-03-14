var settingsJSON = {
  "name": "Fireball",
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
      "title": "look_and_feel",
      "items": [
        {
          "type": "toggle",
          "label": "date",
          "storage_key": "date",
          "pebble_key": 2,
          "default": true
        },
        {
          "type": "toggle",
          "label": "minute_depends_on_minute",
          "storage_key": "minute_depends_on_minute",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "toggle",
          "label": "battery_bar",
          "storage_key": "battery_bar",
          "pebble_key": 4,
          "default": true
        }
      ]
    },
    {
      "title": "colours",
      "items": [
        {
          "type": "colour",
          "label": "minute_colour",
          "storage_key": "minute_colour",
          "pebble_key": 5,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "battery_colour",
          "storage_key": "battery_colour",
          "pebble_key": 6,
          "default": "GColorIslamicGreen"
        },
        {
          "type": "colour",
          "label": "text_colour",
          "storage_key": "text_colour",
          "pebble_key": 7,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "background_colour",
          "pebble_key": 8,
          "default": "GColorBlack"
        }
      ]
    }
  ]
}