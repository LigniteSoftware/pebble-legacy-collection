var settingsJSON = {
  "name": "Personal",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "per_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "per_btrealert",
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
          "label": "pref_boot_animation",
          "storage_key": "per_boot_anim",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_bluetooth_icon",
          "storage_key": "per_bluetooth_indicator",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "textfield",
          "label":"name",
          "storage_key": "per_name_1",
          "pebble_key": 6,
          "hint": "name",
          "default": "friend"
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "accent",
          "storage_key": "per_accent_colour",
          "pebble_key": 8,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "background_colour",
          "storage_key": "per_background_colour",
          "pebble_key": 9,
          "default": "GColorBlack"
        }
      ]
    }
  ]
}
