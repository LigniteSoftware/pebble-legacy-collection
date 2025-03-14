var settingsJSON = {
  "name": "TimeDock",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "weather_format",
          "storage_key": "timedock-weather_format",
          "pebble_key": 1,
          "default": true
        },
        {
          "type": "toggle",
          "label": "open_on_charging",
          "storage_key": "bea-btrealert",
          "pebble_key": 3,
          "default": false
        }
      ]
    },
    {
      "title": "pref_header_lookandfeel",
      "items": [
        {
          "type": "toggle",
          "label": "alt_date_format",
          "storage_key": "timedock-date_format",
          "pebble_key": 4,
          "default": true
        }
      ]
    },
    {
      "title": "pref_header_animations",
      "items": [
        {
          "type": "toggle",
          "label": "pref_header_animations",
          "storage_key": "timedock-animations",
          "pebble_key": 2,
          "default": true
        },
        {
          "type": "toggle",
          "label": "reduced",
          "storage_key": "timedock-reduced",
          "pebble_key": 8,
          "default": false
        }
      ]
    }
  ]
}