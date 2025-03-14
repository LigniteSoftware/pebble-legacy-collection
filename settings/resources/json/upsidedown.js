var settingsJSON = {
  "name": "UpsideDown",
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
        }
      ]
    },
    {
      "title": "pref_header_animations",
      "items": [
        {
          "type": "toggle",
          "label": "minutely",
          "storage_key": "minutely_animations",
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
          "label": "top_colour",
          "storage_key": "top_colour",
          "pebble_key": 6,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "bottom_colour",
          "storage_key": "bottom_colour",
          "pebble_key": 7,
          "default": "GColorBlack"
        }
      ]
    }
  ]
}