var settingsJSON = {
    "name": "Mosaic",
    "items": [{
        "title": "pref_header_general",
        "items": [{
            "type": "toggle",
            "label": "pref_btdisalert",
            "storage_key": "btdisalert",
            "pebble_key": 0,
            "default": true
        }, {
            "type": "toggle",
            "label": "pref_btrealert",
            "storage_key": "btrealert",
            "pebble_key": 1,
            "default": false
        }]
    }, {
        "title": "pref_header_lookandfeel",
        "items": [
        {
          "type": "list",
          "label": "animation_type",
          "storage_key": "mos_animation_type",
          "pebble_key": 2,
          "list": [
              "AnimationTypeNone",
              "AnimationTypeDazzle",
              "AnimationTypeSpiralIn",
              "AnimationTypeSnakeFromTop"
          ],
          "default": 1
        }, {
          "type": "list",
          "label": "shake_action",
          "storage_key": "mos_shake_action",
          "pebble_key": 3,
          "list": [
              "ShakeTypeNone",
              "ShakeTypeAnimate",
              "ShakeTypeShuffle",
              "ShakeTypeDisplayNumbers"
          ],
          "default": 0
        }, {
            "type": "toggle",
            "label": "old_style",
            "storage_key": "mos_old_style",
            "pebble_key": 7,
            "default": false
        }]
    }, {
        "title": "pref_header_colours",
        "items": [{
            "type": "colour",
            "label": "number_colour",
            "storage_key": "mos_number_colour",
            "pebble_key": 5,
            "default": "GColorBlack"
        }, {
            "type": "colour",
            "label": "background_colour",
            "storage_key": "mos_background_colour",
            "pebble_key": 6,
            "default": "GColorWhite"
        }]
    }]
}