var settingsJSON = {
  "name": "RightNighter",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "rn_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "rn_btrealert",
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
          "label": "pref_boot_animation",
          "storage_key": "rn_bootanim",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_antialiased",
          "storage_key": "rn_antialiasing",
          "pebble_key": 4,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_constant_anim",
          "storage_key": "rn_constantanim",
          "pebble_key": 5,
          "default": false
        },
        {
          "type": "toggle",
          "label": "pref_title_invert",
          "storage_key": "rn_invert",
          "pebble_key": 6,
          "default": false
        }
      ]
    }
  ]
}