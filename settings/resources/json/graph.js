var settingsJSON = {
  "name": "Graph",
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
      "title": "time",
      "items": [
        {
          "type": "toggle",
          "label": "time",
          "storage_key": "time",
          "pebble_key": 2,
          "default": true
        },
        {
          "type": "toggle",
          "label": "date",
          "storage_key": "date",
          "pebble_key": 3,
          "default": true
        }
      ]
    },
    {
      "title": "bars",
      "items": [
        {
          "type": "toggle",
          "label": "battery",
          "storage_key": "battery",
          "pebble_key": 4,
          "default": true
        },
        {
          "type": "toggle",
          "label": "hour",
          "storage_key": "hour",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "toggle",
          "label": "minute",
          "storage_key": "minute",
          "pebble_key": 6,
          "default": true
        }
      ]
    },
    {
      "title": "colours",
      "items": [
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "background_colour",
          "pebble_key": 7,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "accent_colour",
          "storage_key": "accent_colour",
          "pebble_key": 8,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "text_colour",
          "storage_key": "text_colour",
          "pebble_key": 9,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}