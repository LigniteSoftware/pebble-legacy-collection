var settingsJSON = {
  "name": "Lines",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "li_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "li_btrealert",
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
          "label": "show_date",
          "storage_key": "li_showdate",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "toggle",
          "label": "alt_date_format",
          "storage_key": "li_altdateformat",
          "pebble_key": 4,
          "default": true
        },
        {
          "type": "list",
          "label": "battery_bar_mode",
          "storage_key": "li_battery_bar_mode",
          "pebble_key": 9,
          "default": 1,
          "list": [
            "no_battery",
            "bar_only",
            "text_only",
            "bar_and_text"
          ]
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "date_colour",
          "storage_key": "li_hour_colour",
          "pebble_key": 7,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}