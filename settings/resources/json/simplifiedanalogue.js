var settingsJSON = {
  "name": "Simplified Analogue",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "sim_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "sim_btrealert",
          "pebble_key": 1,
          "default": false
        }
      ]
    },
    {
      "title": "pref_header_lookandfeel",
      "items": [
        {
          "type": "toggle",
          "label": "weekday",
          "storage_key": "sim_weekday",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "toggle",
          "label": "advanced_date",
          "storage_key": "sim_advancedday",
          "pebble_key": 4,
          "default": true
        },
        {
          "type": "toggle",
          "label": "digital_time",
          "storage_key": "sim_digitaltime",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "toggle",
          "label": "minute_analogue",
          "storage_key": "sim_minute_hand",
          "pebble_key": 6,
          "default": true
        },
        {
          "type": "toggle",
          "label": "hour_analogue",
          "storage_key": "sim_hour_hand",
          "pebble_key": 7,
          "default": true
        },
        {
          "type": "toggle",
          "label": "battery_bar",
          "storage_key": "sim_battery_bar",
          "pebble_key": 15,
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
          "storage_key": "sim_minute_colour",
          "pebble_key": 9,
          "default": "GColorYellow"
        },
        {
          "type": "colour",
          "label": "hour_colour",
          "storage_key": "sim_hour_colour",
          "pebble_key": 10,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "time_colour",
          "storage_key": "sim_time_colour",
          "pebble_key": 11,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "date_colour",
          "storage_key": "sim_day_colour",
          "pebble_key": 12,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "advanced_date",
          "storage_key": "sim_advanced_date_colour",
          "pebble_key": 13,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "sim_background_colour",
          "pebble_key": 14,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "battery_bar",
          "storage_key": "sim_battery_bar",
          "pebble_key": 16,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}