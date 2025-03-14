AR = 'arm-none-eabi-ar'
ARFLAGS = 'rcs'
AS = 'arm-none-eabi-gcc'
BINDIR = '/usr/local/bin'
BLOCK_MESSAGE_KEYS = []
BUILD_DIR = 'aplite'
BUILD_TYPE = 'app'
BUNDLE_BIN_DIR = 'aplite'
BUNDLE_NAME = 'Modulite.pbw'
CC = ['arm-none-eabi-gcc']
CCLNK_SRC_F = []
CCLNK_TGT_F = ['-o']
CC_NAME = 'gcc'
CC_SRC_F = []
CC_TGT_F = ['-c', '-o']
CC_VERSION = ('4', '7', '2')
CFLAGS = ['-std=c99', '-mcpu=cortex-m3', '-mthumb', '-ffunction-sections', '-fdata-sections', '-g', '-fPIE', '-Os', '-D_TIME_H_', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter', '-Wno-error=unused-function', '-Wno-error=unused-variable']
CFLAGS_MACBUNDLE = ['-fPIC']
CFLAGS_cshlib = ['-fPIC']
CPPPATH_ST = '-I%s'
DEFINES = ['RELEASE', 'PBL_PLATFORM_APLITE', 'PBL_BW', 'PBL_RECT', 'PBL_COMPASS', 'PBL_DISPLAY_WIDTH=144', 'PBL_DISPLAY_HEIGHT=168', 'PBL_SDK_3']
DEFINES_ST = '-D%s'
DEST_BINFMT = 'elf'
DEST_CPU = 'arm'
DEST_OS = 'darwin'
INCLUDES = ['aplite']
LD = 'arm-none-eabi-ld'
LIBDIR = '/usr/local/lib'
LIBPATH_ST = '-L%s'
LIB_DIR = 'node_modules'
LIB_JSON = []
LIB_ST = '-l%s'
LINKFLAGS = ['-mcpu=cortex-m3', '-mthumb', '-Wl,--gc-sections', '-Wl,--warn-common', '-fPIE', '-Os']
LINKFLAGS_MACBUNDLE = ['-bundle', '-undefined', 'dynamic_lookup']
LINKFLAGS_cshlib = ['-shared']
LINKFLAGS_cstlib = ['-Wl,-Bstatic']
LINK_CC = ['arm-none-eabi-gcc']
MESSAGE_KEYS = {u'gs_date_format': 2000, u'custom_location': 18, u'mod_btrealert': 1, u'mod_accent_colour_left': 9, u'mod_btdisalert': 0, u'mod_data_type_bottom': 6, u'mod_accent_colour_right': 5, u'mod_hour_hand_colour': 11, u'mod_outer_circle_background_colour': 22, u'temperature': 101, u'pebbleAccountToken': 6969, u'use_celsius': 17, u'mod_second_hand': 15, u'accessCode': 1001, u'mod_imperial_measurements': 20, u'mod_hour_hand': 23, u'mod_outer_circle_colour': 13, u'mod_minute_hand_colour': 12, u'mod_background_colour': 10, u'email': 1000, u'mod_accent_colour_bottom': 7, u'mod_data_type_right': 4, u'mod_data_type_top': 2, u'mod_analogue_hands': 14, u'mod_shake_to_hide_hands': 19, u'icon': 100, u'mod_minute_hand': 24, u'checksum': 1002, u'mod_data_type_left': 8, u'request_weather': 200, u'mod_second_hand_colour': 21, u'mod_outer_circle_type': 16, u'mod_accent_colour_top': 3}
MESSAGE_KEYS_DEFINITION = '/Users/edwinfinch/lignite-pebble/Modulite/build/src/message_keys.auto.c'
MESSAGE_KEYS_HEADER = '/Users/edwinfinch/lignite-pebble/Modulite/build/include/message_keys.auto.h'
MESSAGE_KEYS_JSON = '/Users/edwinfinch/lignite-pebble/Modulite/build/js/message_keys.json'
NODE_PATH = '/Users/edwinfinch/Library/Application Support/Pebble SDK/SDKs/current/node_modules'
PEBBLE_SDK_COMMON = '/Users/edwinfinch/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble/common'
PEBBLE_SDK_PLATFORM = '/Users/edwinfinch/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble/aplite'
PEBBLE_SDK_ROOT = '/Users/edwinfinch/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble'
PLATFORM = {'TAGS': ['aplite', 'bw', 'rect', 'compass', '144w', '168h'], 'MAX_FONT_GLYPH_SIZE': 256, 'ADDITIONAL_TEXT_LINES_FOR_PEBBLE_H': [], 'MAX_APP_BINARY_SIZE': 65536, 'MAX_RESOURCES_SIZE': 524288, 'MAX_APP_MEMORY_SIZE': 24576, 'MAX_WORKER_MEMORY_SIZE': 10240, 'NAME': 'aplite', 'BUNDLE_BIN_DIR': 'aplite', 'BUILD_DIR': 'aplite', 'MAX_RESOURCES_SIZE_APPSTORE_2_X': 98304, 'MAX_RESOURCES_SIZE_APPSTORE': 131072, 'DEFINES': ['PBL_PLATFORM_APLITE', 'PBL_BW', 'PBL_RECT', 'PBL_COMPASS', 'PBL_DISPLAY_WIDTH=144', 'PBL_DISPLAY_HEIGHT=168']}
PLATFORM_NAME = 'aplite'
PREFIX = '/usr/local'
PROJECT_INFO = {'appKeys': {u'gs_date_format': 2000, u'custom_location': 18, u'mod_btrealert': 1, u'mod_accent_colour_left': 9, u'mod_btdisalert': 0, u'mod_data_type_bottom': 6, u'mod_accent_colour_right': 5, u'mod_hour_hand_colour': 11, u'mod_outer_circle_background_colour': 22, u'temperature': 101, u'pebbleAccountToken': 6969, u'use_celsius': 17, u'mod_second_hand': 15, u'accessCode': 1001, u'mod_imperial_measurements': 20, u'mod_hour_hand': 23, u'mod_outer_circle_colour': 13, u'mod_minute_hand_colour': 12, u'mod_background_colour': 10, u'email': 1000, u'mod_accent_colour_bottom': 7, u'mod_data_type_right': 4, u'mod_data_type_top': 2, u'mod_analogue_hands': 14, u'mod_shake_to_hide_hands': 19, u'icon': 100, u'mod_minute_hand': 24, u'checksum': 1002, u'mod_data_type_left': 8, u'request_weather': 200, u'mod_second_hand_colour': 21, u'mod_outer_circle_type': 16, u'mod_accent_colour_top': 3}, u'sdkVersion': u'3', u'projectType': u'native', u'uuid': u'4a5b2a1c-2493-416c-ac5a-cf6dc9c3d815', u'messageKeys': {u'gs_date_format': 2000, u'custom_location': 18, u'mod_btrealert': 1, u'mod_accent_colour_left': 9, u'mod_btdisalert': 0, u'mod_data_type_bottom': 6, u'mod_accent_colour_right': 5, u'mod_hour_hand_colour': 11, u'mod_outer_circle_background_colour': 22, u'temperature': 101, u'pebbleAccountToken': 6969, u'use_celsius': 17, u'mod_second_hand': 15, u'accessCode': 1001, u'mod_imperial_measurements': 20, u'mod_hour_hand': 23, u'mod_outer_circle_colour': 13, u'mod_minute_hand_colour': 12, u'mod_background_colour': 10, u'email': 1000, u'mod_accent_colour_bottom': 7, u'mod_data_type_right': 4, u'mod_data_type_top': 2, u'mod_analogue_hands': 14, u'mod_shake_to_hide_hands': 19, u'icon': 100, u'mod_minute_hand': 24, u'checksum': 1002, u'mod_data_type_left': 8, u'request_weather': 200, u'mod_second_hand_colour': 21, u'mod_outer_circle_type': 16, u'mod_accent_colour_top': 3}, 'companyName': u'Lignite', u'enableMultiJS': True, u'watchapp': {u'onlyShownOnCommunication': False, u'hiddenApp': False, u'watchface': True}, u'capabilities': [u'configurable', u'health'], 'versionLabel': u'1.16', u'targetPlatforms': [u'aplite', u'basalt', u'chalk', u'diorite'], 'longName': u'Modulite', u'displayName': u'Modulite', 'shortName': u'Modulite', u'resources': {u'media': [{u'type': u'bitmap', u'name': u'IMAGE_HEART_ICON', u'file': u'icons/heart-icon.png'}, {u'type': u'bitmap', u'name': u'IMAGE_DEEP_SLEEP_ICON', u'file': u'icons/sleep-icon-deep.png'}, {u'type': u'bitmap', u'name': u'IMAGE_TOTAL_SLEEP_ICON', u'file': u'icons/sleep-icon-light.png'}, {u'type': u'bitmap', u'name': u'IMAGE_STEPS_ICON', u'file': u'icons/steps-icon.png'}, {u'type': u'bitmap', u'name': u'IMAGE_DISTANCE_WALKED_ICON', u'file': u'icons/walked-distance.png'}, {u'type': u'bitmap', u'name': u'IMAGE_CALORIES_ICON', u'file': u'icons/calories.png'}, {u'type': u'bitmap', u'name': u'IMAGE_BATTERY_ICON', u'file': u'icons/battery-icon.png'}, {u'type': u'bitmap', u'name': u'IMAGE_BATTERY_CHARGING_ICON', u'file': u'icons/battery-icon-charging.png'}, {u'type': u'bitmap', u'name': u'IMAGE_BLUETOOTH_ICON', u'file': u'icons/bluetooth-new.png'}, {u'targetPlatforms': [u'chalk'], u'type': u'font', u'name': u'FONT_BEBAS_NEUE_REGULAR_32', u'file': u'fonts/bebasneue_regular.ttf'}, {u'targetPlatforms': [u'chalk'], u'type': u'font', u'name': u'FONT_BEBAS_NEUE_REGULAR_42', u'file': u'fonts/bebasneue_regular.ttf'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'font', u'name': u'FONT_BEBAS_NEUE_REGULAR_30', u'file': u'fonts/bebasneue_regular.ttf'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'font', u'name': u'FONT_BEBAS_NEUE_REGULAR_36', u'file': u'fonts/bebasneue_regular.ttf'}, {u'targetPlatforms': [u'chalk'], u'type': u'font', u'name': u'FONT_WEATHER_ICONS_44', u'file': u'fonts/weather_icons.ttf'}, {u'targetPlatforms': [u'chalk'], u'type': u'font', u'name': u'FONT_WEATHER_ICONS_38', u'file': u'fonts/weather_icons.ttf'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'font', u'name': u'FONT_WEATHER_ICONS_40', u'file': u'fonts/weather_icons.ttf'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'font', u'name': u'FONT_WEATHER_ICONS_36', u'file': u'fonts/weather_icons.ttf'}, {u'type': u'bitmap', u'name': u'LIGNITE_IMAGE_BLUETOOTH_ICON', u'file': u'lignite/bluetooth_icon.png'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'bitmap', u'name': u'IMAGE_BACKGROUND_ANALOGUE_TICKS', u'file': u'analogue-background.png'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'bitmap', u'name': u'IMAGE_BACKGROUND_ANALOGUE_DOTS', u'file': u'dots-background.png'}]}, 'name': u'Modulite'}
REQUESTED_PLATFORMS = [u'aplite', u'basalt', u'chalk', u'diorite']
RESOURCES_JSON = [{u'type': u'bitmap', u'name': u'IMAGE_HEART_ICON', u'file': u'icons/heart-icon.png'}, {u'type': u'bitmap', u'name': u'IMAGE_DEEP_SLEEP_ICON', u'file': u'icons/sleep-icon-deep.png'}, {u'type': u'bitmap', u'name': u'IMAGE_TOTAL_SLEEP_ICON', u'file': u'icons/sleep-icon-light.png'}, {u'type': u'bitmap', u'name': u'IMAGE_STEPS_ICON', u'file': u'icons/steps-icon.png'}, {u'type': u'bitmap', u'name': u'IMAGE_DISTANCE_WALKED_ICON', u'file': u'icons/walked-distance.png'}, {u'type': u'bitmap', u'name': u'IMAGE_CALORIES_ICON', u'file': u'icons/calories.png'}, {u'type': u'bitmap', u'name': u'IMAGE_BATTERY_ICON', u'file': u'icons/battery-icon.png'}, {u'type': u'bitmap', u'name': u'IMAGE_BATTERY_CHARGING_ICON', u'file': u'icons/battery-icon-charging.png'}, {u'type': u'bitmap', u'name': u'IMAGE_BLUETOOTH_ICON', u'file': u'icons/bluetooth-new.png'}, {u'targetPlatforms': [u'chalk'], u'type': u'font', u'name': u'FONT_BEBAS_NEUE_REGULAR_32', u'file': u'fonts/bebasneue_regular.ttf'}, {u'targetPlatforms': [u'chalk'], u'type': u'font', u'name': u'FONT_BEBAS_NEUE_REGULAR_42', u'file': u'fonts/bebasneue_regular.ttf'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'font', u'name': u'FONT_BEBAS_NEUE_REGULAR_30', u'file': u'fonts/bebasneue_regular.ttf'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'font', u'name': u'FONT_BEBAS_NEUE_REGULAR_36', u'file': u'fonts/bebasneue_regular.ttf'}, {u'targetPlatforms': [u'chalk'], u'type': u'font', u'name': u'FONT_WEATHER_ICONS_44', u'file': u'fonts/weather_icons.ttf'}, {u'targetPlatforms': [u'chalk'], u'type': u'font', u'name': u'FONT_WEATHER_ICONS_38', u'file': u'fonts/weather_icons.ttf'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'font', u'name': u'FONT_WEATHER_ICONS_40', u'file': u'fonts/weather_icons.ttf'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'font', u'name': u'FONT_WEATHER_ICONS_36', u'file': u'fonts/weather_icons.ttf'}, {u'type': u'bitmap', u'name': u'LIGNITE_IMAGE_BLUETOOTH_ICON', u'file': u'lignite/bluetooth_icon.png'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'bitmap', u'name': u'IMAGE_BACKGROUND_ANALOGUE_TICKS', u'file': u'analogue-background.png'}, {u'targetPlatforms': [u'aplite', u'basalt', u'diorite'], u'type': u'bitmap', u'name': u'IMAGE_BACKGROUND_ANALOGUE_DOTS', u'file': u'dots-background.png'}]
RPATH_ST = '-Wl,-rpath,%s'
SANDBOX = False
SDK_VERSION_MAJOR = 5
SDK_VERSION_MINOR = 78
SHLIB_MARKER = None
SIZE = 'arm-none-eabi-size'
SONAME_ST = '-Wl,-h,%s'
STLIBPATH_ST = '-L%s'
STLIB_MARKER = None
STLIB_ST = '-l%s'
SUPPORTED_PLATFORMS = ['basalt', 'aplite', 'diorite', 'chalk', 'emery']
TARGET_PLATFORMS = ['diorite', 'chalk', 'basalt', 'aplite']
TIMESTAMP = 1533870226
USE_GROUPS = True
VERBOSE = 0
WEBPACK = '/Users/edwinfinch/Library/Application Support/Pebble SDK/SDKs/current/node_modules/.bin/webpack'
cprogram_PATTERN = '%s'
cshlib_PATTERN = 'lib%s.so'
cstlib_PATTERN = 'lib%s.a'
macbundle_PATTERN = '%s.bundle'
