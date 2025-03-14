var settingsJSON = {
  "name": "Chunky",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "ch-btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "ch-btrealert",
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
          "label": "pref_title_invert",
          "storage_key": "ch-invert",
          "pebble_key": 3,
          "default": false
        },
        {
          "type": "toggle",
          "label": "battery_bar",
          "storage_key": "ch-batterybar",
          "pebble_key": 4,
          "default": true
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "minute_colour",
          "storage_key": "ch-minute-colour",
          "pebble_key": 6,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "hour_colour",
          "storage_key": "ch-hour-colour",
          "pebble_key": 7,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "ch-background-colour",
          "pebble_key": 8,
          "default": "GColorBlack"
        }
      ]
    }
  ]
}