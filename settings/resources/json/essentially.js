var settingsJSON = {
  "name": "Essentially",
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
      "title": "pref_title_lookandfeel",
      "items": [
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
          "type": "toggle",
          "label": "battery_bar",
          "storage_key": "battery_bar",
          "pebble_key": 5,
          "default": true
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "accent_colour",
          "storage_key": "accent",
          "pebble_key": 6,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "background_colour",
          "pebble_key": 7,
          "default": "GColorBlack"
        }
      ]
    }
  ]
}