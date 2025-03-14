var APP_NAME = "graph";
var OPTIONS = "options";

var settingURL = ("https://www.lignite.me/pebble/settings/" + APP_NAME);

Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL(settingURL);
});

Pebble.addEventListener("webviewclosed", function(e) {
    if (e.response) {
        options = JSON.parse(decodeURIComponent(e.response));
        if(options){
            localStorage.setItem(OPTIONS, JSON.stringify(options));
            Pebble.sendAppMessage(options);
        }
    }
});