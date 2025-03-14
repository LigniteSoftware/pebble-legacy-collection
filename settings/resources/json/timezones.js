var settingsJSON = {
  "name": "Timezones",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "tim_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "tim_btrealert",
          "pebble_key": 1,
          "default": false
        }
      ]
    },
    {
      "title": "your_time",
      "items": [
        {
          "type": "colour",
          "label": "colour",
          "storage_key": "tim_colour_1",
          "pebble_key": 4,
          "default": "GColorRed"
        },
        {
          "type": "toggle",
          "label": "analogue",
          "storage_key": "tim_analogue_1",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "textfield",
          "label":"name",
          "storage_key": "tim_name_1",
          "pebble_key": 3,
          "hint": "name",
          "default": "You"
        }
      ]
    },
    {
      "title": "other_time",
      "items": [
        {
          "type": "timezones",
          "label":"timezone",
          "storage_key": "tim_timezone",
          "pebble_key": 7,
          "default": "Europe/Zurich"
        },
        {
          "type": "toggle",
          "label": "subtract_hour",
          "storage_key": "tim_subtract_hour",
          "pebble_key": 8,
          "default": false
        },
        {
          "type": "colour",
          "label": "colour",
          "storage_key": "tim_colour_2",
          "pebble_key": 10,
          "default": "GColorOrange"
        },
        {
          "type": "toggle",
          "label": "analogue",
          "storage_key": "tim_analogue_2",
          "pebble_key": 11,
          "default": true
        },
        {
          "type": "textfield",
          "label":"name",
          "storage_key": "tim_name_2",
          "pebble_key": 9,
          "hint": "name",
          "default": "Other"
        }
      ]
    }
  ]
}
