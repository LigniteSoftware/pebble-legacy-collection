var settingsJSON = {
  "name": "Pulse",
  "items": [
    {
      "title": "pref_header_general",
      "items": [
        {
          "type": "toggle",
          "label": "pref_btdisalert",
          "storage_key": "pul-btdisalert",
          "pebble_key": 0,
          "default": true
        },
        {
          "type": "toggle",
          "label": "pref_btrealert",
          "storage_key": "pul-btrealert",
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
          "label": "date",
          "storage_key": "pul-date",
          "pebble_key": 2,
          "list": [
            "grow",
            "flower"
          ],
          "default": 0
        }
      ]
    },
    {
      "title": "pref_header_animations",
      "items": [
        {
          "type": "toggle",
          "label": "constant_anim",
          "storage_key": "pul-constant_anim",
          "pebble_key": 4,
          "default": false
        },
        {
          "type": "toggle",
          "label": "shake_to_anim",
          "storage_key": "pul-shake_to_anim",
          "pebble_key": 5,
          "default": true
        },
        {
          "type": "list",
          "label": "animation_mode",
          "storage_key": "pul-animation_mode",
          "pebble_key": 6,
          "list": [
            "grow",
            "flower"
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
          "label": "circle_colour",
          "storage_key": "pul-circle-colour",
          "pebble_key": 7,
          "default": "GColorBlack"
        },
        {
          "type": "colour",
          "label": "time_colour",
          "storage_key": "pul-time-colour",
          "pebble_key": 9,
          "default": "GColorWhite"
        }
      ]
    }
  ]
}