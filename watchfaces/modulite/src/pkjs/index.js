var CLEAR_DAY = 0;
var CLEAR_NIGHT = 1;
var WINDY = 2;
var COLD = 3;
var PARTLY_CLOUDY_DAY = 4;
var PARTLY_CLOUDY_NIGHT = 5;
var HAZE = 6;
var CLOUD = 7;
var RAIN = 8;
var SNOW = 9;
var HAIL = 10;
var CLOUDY = 11;
var STORM = 12;
var NA = 13;

var imageId = {
  0 : STORM, //tornado
  1 : STORM, //tropical storm
  2 : STORM, //hurricane
  3 : STORM, //severe thunderstorms
  4 : STORM, //thunderstorms
  5 : HAIL, //mixed rain and snow
  6 : HAIL, //mixed rain and sleet
  7 : HAIL, //mixed snow and sleet
  8 : HAIL, //freezing drizzle
  9 : RAIN, //drizzle
  10 : HAIL, //freezing rain
  11 : RAIN, //showers
  12 : RAIN, //showers
  13 : SNOW, //snow flurries
  14 : SNOW, //light snow showers
  15 : SNOW, //blowing snow
  16 : SNOW, //snow
  17 : HAIL, //hail
  18 : HAIL, //sleet
  19 : HAZE, //dust
  20 : HAZE, //foggy
  21 : HAZE, //haze
  22 : HAZE, //smoky
  23 : WINDY, //blustery
  24 : WINDY, //windy
  25 : COLD, //cold
  26 : CLOUDY, //cloudy
  27 : CLOUDY, //mostly cloudy (night)
  28 : CLOUDY, //mostly cloudy (day)
  29 : PARTLY_CLOUDY_NIGHT, //partly cloudy (night)
  30 : PARTLY_CLOUDY_DAY, //partly cloudy (day)
  31 : CLEAR_NIGHT, //clear (night)
  32 : CLEAR_DAY, //sunny
  33 : CLEAR_NIGHT, //fair (night)
  34 : CLEAR_DAY, //fair (day)
  35 : HAIL, //mixed rain and hail
  36 : CLEAR_DAY, //hot
  37 : STORM, //isolated thunderstorms
  38 : STORM, //scattered thunderstorms
  39 : STORM, //scattered thunderstorms
  40 : STORM, //scattered showers
  41 : SNOW, //heavy snow
  42 : SNOW, //scattered snow showers
  43 : SNOW, //heavy snow
  44 : CLOUD, //partly cloudy
  45 : STORM, //thundershowers
  46 : SNOW, //snow showers
  47 : STORM, //isolated thundershowers
  3200 : NA, //not available
};

var options = JSON.parse(localStorage.getItem('options'));
//console.log('read options: ' + JSON.stringify(options));
if (options === null) options = { "custom_location" : "",
                                  "use_celsius" : false};

var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function getWeatherFromLatLong(latitude, longitude) {
/*
 * RIP OpenStreetMap weather querys
 * Dec. 2015-Feb. 2016
 * You will never be forgotten and will always be missed
 *
 var url = 'http://nominatim.openstreetmap.org/reverse?format=json&lat=' + latitude + '&lon=' + longitude;
*/

    var query = encodeURI('select woeid from geo.places where text="(' + latitude + ',' + longitude + ')" limit 1');
    var url = "http://query.yahooapis.com/v1/public/yql?q=" + query + "&format=json";

    // console.log("Reverse geocoding url: " + url);

    xhrRequest(url, 'GET',
        function(responseText) {
            var json = JSON.parse(responseText);
            var location = json.query.results.place.woeid;

            getWeatherFromWoeid(location);
        }
    );
}

function getWeatherFromLocation(location_name) {
  var response;
  var woeid = -1;

  var query = encodeURI("select woeid from geo.places(1) where text=\"" + location_name + "\"");
  var url = "http://query.yahooapis.com/v1/public/yql?q=" + query + "&format=json";
  console.log(url);
  var req = new XMLHttpRequest();
  req.open('GET', url, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if (req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        if (response) {
          woeid = response.query.results.place.woeid;
        //   console.log("Got WOEID of " + woeid);
          getWeatherFromWoeid(woeid);
        }
      } else {
        console.log("Error in getting weather from location: " + e);
      }
    }
  };
  req.send(null);
}

function getWeatherFromWoeid(woeid) {
  var query = encodeURI("select item.condition from weather.forecast where woeid = " + woeid +
                        " and u = " + (false ? "\"c\"" : "\"f\""));
  var url = "http://query.yahooapis.com/v1/public/yql?q=" + query + "&format=json";

  var response;
  var req = new XMLHttpRequest();
  console.log("Fetching weather from " + url);
  req.open('GET', url, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if (req.status == 200) {
        //   console.log("got weather data " + req.responseText);
        response = JSON.parse(req.responseText);
        if (response) {
          var condition = response.query.results.channel.item.condition;
          var icon = imageId[condition.code];
        //   console.log("temp " + condition.temp);
        //   console.log("icon " + condition.code);
        //   console.log("condition " + condition.text);
          var fixedTemp = parseInt(condition.temp);
          if(options.use_celsius){
              console.log("User is using the best temperature scale");
              fixedTemp -= 32;
              fixedTemp = Math.ceil(fixedTemp*(5/9));
          }
          var message = {
            "icon" : icon,
            "temperature" : fixedTemp
          };
       console.log("Sending message through WOEID " + JSON.stringify(message));
          Pebble.sendAppMessage(message);
        }
      } else {
        console.log("Error from getting through WOEID: " + e);
      }
    }
  };
  req.send(null);
}

var locationOptions = {
  "timeout": 15000,
  "maximumAge": 60000
};

function updateWeather() {
  if (options.custom_location.length < 1) {
    navigator.geolocation.getCurrentPosition(locationSuccess,
                                                    locationError,
                                                    locationOptions);
  } else {
    getWeatherFromLocation(options.custom_location);
  }
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  getWeatherFromLatLong(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "icon":11,
    "temperature":""
  });
}

var APP_NAME = "modulite";
var OPTIONS = "options";
var APP_UUID = "4a5b2a1c-2493-416c-ac5a-cf6dc9c3d815";
var GLOBAL_SETTINGS = [
    "dateFormat"
];

var settingURL = ("https://www.lignite.me/pebble/settings/" + APP_NAME);

function getGlobalSettingsString(){
    if(GLOBAL_SETTINGS.length < 1 || !GLOBAL_SETTINGS){
        return undefined;
    }
    var string = "";
    for(var i = 0; i < GLOBAL_SETTINGS.length; i++){
        string += GLOBAL_SETTINGS[i] + "-";
    }
    return string.substring(0, string.length-1);
}


Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL(settingURL);
});

Pebble.addEventListener("webviewclosed", function(e) {
    if (e.response) {
        // console.log("Got " + e.response);
        options = JSON.parse(e.response);
        if(options){
            // console.log(JSON.stringify(options));
            localStorage.setItem(OPTIONS, JSON.stringify(options));
            Pebble.sendAppMessage(options);

            updateWeather();
        }
    }
});

Pebble.addEventListener("appmessage", function(e) {
    if(e.payload.request_weather) {
        updateWeather();
    }
});

Pebble.addEventListener("ready", function(e) {
    updateWeather();
});

/*
Hi!

Can you please not bother our APIs or anything?
It's ok to snoop around, though we kindly ask that you don't try to do any harm.
And if any exploits are found, email us: contact@lignite.io, we'd love to talk
and maybe even work something out!

Thank you :)
-Team Lignite
*/