var settingsJSON = {
  "name": "Colours",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "col_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "col_btrealert",
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
          "label": "outline_numbers",
          "storage_key": "col_border",
          "pebble_key": 3,
          "default": false
        },
        {
          "type": "toggle",
          "label": "pref_constant_anim",
          "storage_key": "col_constantanim",
          "pebble_key": 4,
          "default": false
        },
        {
          "type": "toggle",
          "label": "organize_colours",
          "storage_key": "col_organize",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "toggle",
          "label": "randomize_width",
          "storage_key": "col_randomize",
          "pebble_key": 6,
          "default": false
      },
      {
        "type": "number_picker",
        "label":"default_width",
        "storage_key": "col_default_width",
        "pebble_key": 7,
        "default": 6
      }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "minute_colour",
          "storage_key": "ch_minute_colour",
          "pebble_key": 9,
          "default": "GColorWhite"
        },
        {
          "type": "colour",
          "label": "hour_colour",
          "storage_key": "ch_hour_colour",
          "pebble_key": 10,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}
