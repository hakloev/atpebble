// Module Bus, containing logic for parsing information from API
var Bus = ( function () {
    
    var busInfoUrl = "http://bybussen.api.tmn.io/rt/";
    var dictionary;
    
    var parseInfo = function(place) {
        var departureList = [];
        departureList.push(place.name);
        if (window.localStorage.getItem('route').length > 0) {
            routeList = window.localStorage.getItem('route').split(',')
        }
        for (var i = 0; i < place.next.length; i++) {
            if (window.localStorage.getItem('route').length === 0) {
                departureList.push(place.next[i]);    
            } else if (routeList.indexOf(place.next[i].l) !== -1) {
                departureList.push(place.next[i]); 
            } 
            if (departureList.length === 5) {
               break;
            }
        }
        printInfo(departureList);
    };
    
    var printInfo = function(list) {
        var row = "";
        if (list.length > 1) {
           for (var i = 1; i < list.length; i++) {
                row += (list[i].l + ": " + list[i].t.substring(11, 16) + " - " + calcTime(list[i].t, list[i].rt) + "\n");
            }
        } else {
            row += (window.localStorage.getItem('route').split(',').join(', ') + "\ningen avganger"); 
        }   
        dictionary[(Object.keys(dictionary).length).toString()] = list[0];
        dictionary[(Object.keys(dictionary).length).toString()] = row;
    };
    
    var calcTime = function(time, realtime) {
        var d = time.match(/^(\d{2}).(\d{2}).(\d{4}) (\d{2}):(\d{2})$/);
        var today = new Date();
        var departure = new Date(d[3], d[2] - 1, d[1], d[4], d[5]);
        var diff = Math.floor((departure.getTime() - today.getTime()) / (1000 * 60));
        if (diff <= -1 || diff <= 0) {
            return realtime ? "nå (S)" : "ca nå";
        } else {
            return (realtime ? (diff + " min (S)") : "ca " + diff + " min");
        }   
    };
    
    return {
        getBusInfo: function() {
            dictionary = {};
            xmlReq(busInfoUrl + window.localStorage.getItem('stopId1'), "GET", 
                function(responseText) {
                    var json = JSON.parse(responseText);
                    parseInfo(json);
                }
            );
            xmlReq(busInfoUrl + window.localStorage.getItem('stopId2'), "GET",
                function(responseText) {
                    var json = JSON.parse(responseText);
                    parseInfo(json);
                }
            );          
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log("Sent departures to Pebble");
                    for (var key in dictionary) {
                        console.log("Key: " + key + " --> Value: " + dictionary[key]);
                    }
                },
                function(e) {
                     console.log("Error while sending departures to pebble");   
                }
            );
        }
    };
})();

// AJAX-function
function xmlReq(url, type, callback) {
        var xhr = new XMLHttpRequest();
        xhr.onload = function () {
            if (xhr.readyState == 4 && xhr.status == 200) {
                console.log('INFO: Callback in XMLHttpRequest');
                callback(this.responseText);
            } else {
                console.log('ERROR: XMLHttpRequest failed, nothing to display in DOM');
            }
        };
        xhr.open(type, url, false);
        xhr.send();
} 

// Pebble ready to receive, does the initial fetch. Will not be able to fetch before localStorage is set
Pebble.addEventListener('ready',
    function(e) {
        Bus.getBusInfo();
    }        
);

// Pebble wants new information
Pebble.addEventListener('appmessage', 
    function(e) {
        Bus.getBusInfo();
    }      
);

// Show settings menu
Pebble.addEventListener('showConfiguration', function() {
    var url = "https://navi.hakloev.no/static/files/atpebble/config.html?";
    
    for (var i = 0; i < window.localStorage.length; i++) {
        var key = window.localStorage.key(i);
        var value = window.localStorage.getItem(key);
        if (value != null) {
            if (i === 0) {
                url += key + "=" + value;
            } else {
                url += "&" + key + "=" + value;
            }
        }
    }
    console.log("Settings-URL: ", url);
    Pebble.openURL(url);
});

// Save new settings
Pebble.addEventListener('webviewclosed', function(e) {
    console.log("Config Closed");
    if (e.response) {
        var values = JSON.parse(decodeURIComponent(e.response));
        console.log("Values: ", JSON.stringify(values));
        for (var key in values) {
            window.localStorage.setItem(key, values[key]);
        }   
    }
    Bus.getBusInfo();
});