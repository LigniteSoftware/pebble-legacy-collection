var settingsJSON = {
  "name": "Tree of Colours",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "tre_btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "tre_btrealert",
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
          "label": "randomize_colours",
          "storage_key": "tre_randomize",
          "pebble_key": 3,
          "default": true
        },
        {
          "type": "number_picker",
          "label": "default_width",
          "storage_key": "tre_branch_width",
          "pebble_key": 4,
          "default": 2,
          "min_value": 0,
          "max_value": 10
        }
      ]
    },
    {
      "title": "pref_header_colours",
      "items": [
        {
          "type": "colour",
          "label": "custom_colour_1",
          "storage_key": "tre_cust_colour_1",
          "pebble_key": 6,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "custom_colour_2",
          "storage_key": "tre_cust_colour_2",
          "pebble_key": 7,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "custom_colour_3",
          "storage_key": "tre_cust_colour_3",
          "pebble_key": 8,
          "default": "GColorBlack"
        }
      ]
    },
    {
      "title": "pref_header_othercolours",
      "items": [
        {
          "type": "colour",
          "label": "circle_colour",
          "storage_key": "tre_circle_colour",
          "pebble_key": 10,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "time_colour",
          "storage_key": "tre_time_colour",
          "pebble_key": 11,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}
