var settingsJSON = {
  "name": "Speedometer",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "spe_btdisalert",
          "pebble_key": 1,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "spe_btrealert",
          "pebble_key": 2,
          "default": false
        }
      ]
    },
    {
      "title": "pref_title_lookandfeel",
      "items": [
        {
          "type": "toggle",
          "label": "pref_title_invert",
          "storage_key": "spe_invert",
          "pebble_key": 0,
          "default": false
        },
        {
          "type": "toggle",
          "label": "show_borders",
          "storage_key": "spe_showborders",
          "pebble_key": 4,
          "default": false
        },
        {
          "type": "toggle",
          "label": "pref_boot_animation",
          "storage_key": "spe_bootanim",
          "pebble_key": 5,
          "default": false
        },
        {
          "type": "toggle",
          "label": "pref_bluetooth_icon",
          "storage_key": "spe_bticon",
          "pebble_key": 6,
          "default": false
        },
        {
          "type": "toggle",
          "label": "pref_antialiased",
          "storage_key": "spe_dithering",
          "pebble_key": 7,
          "default": true
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "pref_title_1_colour",
          "storage_key": "spe_outer_colour",
          "pebble_key": 9,
          "default": "GColorBlue"
        },
        {
          "type": "colour",
          "label": "pref_title_2_colour",
          "storage_key": "spe_middle_colour",
          "pebble_key": 10,
          "default": "GColorRed"
        },
        {
          "type": "colour",
          "label": "pref_title_3_colour",
          "storage_key": "spe_inner_colour",
          "pebble_key": 11,
          "default": "GColorIslamicGreen"
        }
      ]
    }
  ]
}