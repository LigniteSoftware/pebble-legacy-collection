var settingsJSON = {
  "name": "Slot Machine",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "slo_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "slo_btrealert",
          "pebble_key": 1,
          "default": false
      },
      {
        "type": "toggle",
        "label": "slotmachine_nightmode",
        "storage_key": "slo_nightmode",
        "pebble_key": 8,
        "default": true
      }
      ]
    },
    {
      "title": "pref_header_lookandfeel",
      "items": [
        {
          "type": "toggle",
          "label": "battery_bar",
          "storage_key": "slo_batterybar",
          "pebble_key": 2,
          "default": true
        },
        {
          "type": "toggle",
          "label": "date",
          "storage_key": "slo_date",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "list",
          "label": "mode",
          "storage_key": "slo_mode",
          "pebble_key": 5,
          "list": [
            "secondly",
            "minutely"
          ],
          "default": 0
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "accent",
          "storage_key": "slo_accent",
          "pebble_key": 6,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "time",
          "storage_key": "slo_time",
          "pebble_key": 7,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}