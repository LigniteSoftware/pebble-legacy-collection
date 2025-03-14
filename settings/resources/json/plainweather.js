var settingsJSON = {
  "name": "Plain Weather",
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
        "title":"pref_header_weather",
        "items": [
          {
            "type": "toggle",
            "label": "weather",
            "storage_key": "weather",
            "pebble_key": 2,
            "default": true
          },
          {
            "type": "toggle",
            "label": "use_celsius",
            "storage_key": "use_celsius",
            "pebble_key": 500,
            "default": false
          },
          {
            "type": "textfield",
            "label": "custom_location",
            "storage_key": "custom_location",
            "hint":"enter_custom_location",
            "pebble_key": 501,
            "default": ""
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
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "background_colour",
          "pebble_key": 7,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}