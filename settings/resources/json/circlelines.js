var settingsJSON = {
  "name": "Circle Lines",
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
          "label": "date",
          "storage_key": "date",
          "pebble_key": 2,
          "default": true
        },
        {
          "type": "toggle",
          "label": "time_on_shake",
          "storage_key": "time_on_shake",
          "pebble_key": 3,
          "default": false
        }
      ]
    },
    {
      "title": "spiral",
      "items": [
        {
          "type": "number_picker",
          "label": "spiral_increments",
          "storage_key": "spiral_increments",
          "pebble_key": 4,
          "min_value": 5,
          "max_value": 60,
          "default": 30
        },
        {
          "type": "number_picker",
          "label": "spiral_thickness",
          "storage_key": "spiral_thickness",
          "pebble_key": 5,
          "min_value": 1,
          "max_value": 4,
          "default": 2
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
          "pebble_key": 6,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "text_colour",
          "storage_key": "text_colour",
          "pebble_key": 7,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}