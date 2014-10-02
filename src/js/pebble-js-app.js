var locationOptions = {timeout: 15000, maximumAge: 60000},
    stopId1 = window.localStorage.getItem('stopId1') ? window.localStorage.getItem('stopId1') : '16011567', // Østre Berg
    stopId2 = window.localStorage.getItem('stopId2') ? window.localStorage.getItem('stopId2') : '16011192'; // Ila

var dictionary;

var Bus = ( function () {
    
    var busInfoUrl = "http://bybussen.api.tmn.io/rt/";
    var currentRequest;
    
    return {
        getBusInfo: function() {
            dictionary = {};
            currentRequest = stopId1;
            var self = this; // Needed to access parseInfo in callback function
            this.xmlReq(busInfoUrl + stopId1, "GET", 
                function(responseText) {
                    var json = JSON.parse(responseText);
                    self.parseInfo(json);
                    currentRequest = stopId2;
                }
            );
            this.xmlReq(busInfoUrl + stopId2, "GET",
                function(responseText) {
                    var json = JSON.parse(responseText);
                     self.parseInfo(json);
                }
            );          
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log("Sent departures to Pebble");
                    console.log("Dict length: " + Object.keys(dictionary).length);
                    for (var key in dictionary) {
                        console.log("Dict key: " + key + " | Dict value: " + dictionary[key]);
                    }
                },
                function(e) {
                     console.log("Error while sending departures to pebble");   
                }
            );
        },
        xmlReq: function(url, type, callback) {
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
        }, 
        parseInfo: function(place) {
            var departureList = [];
            departureList.push(place.name);
            for (var i = 0; i < place.next.length; i++) {
                if (place.next[i].l === "5") {
                    departureList.push(place.next[i]);
                }
                if (departureList.length === 4) {
                    break;
                }
            }
            this.printInfo(departureList);
        }, 
        printInfo: function(list) {
            var row = "";
            for (var i = 1; i < list.length; i++) {
                console.log(list[i].t.substring(11, 16) + " - " + this.calcTime(list[i].t));
                row += list[i].l + ": " + list[i].t.substring(11, 16) + " - " + this.calcTime(list[i].t) + "\n";
            }
            console.log("List name: " + list[0]);
            console.log("If check: " + currentRequest + " and " + stopId1);
            if (currentRequest === stopId1) { 
                dictionary["0"] = list[0];
                dictionary["2"] = row;
            } else {
                dictionary["1"] = list[0];
                dictionary["3"] = row;                
            }
        },
        calcTime: function(time) {
            console.log("INFO: calcTime called");
            var d = time.match(/^(\d{2}).(\d{2}).(\d{4}) (\d{2}):(\d{2})$/);
            var today = new Date();
            var departure = new Date(d[3], d[2] - 1, d[1], d[4], d[5]);
            var diff = Math.floor((departure.getTime() - today.getTime()) / (1000 * 60));
            if (diff <= -1 || diff <= 0) {
                return "ca nå";
            } else {
                return "ca " + diff + " min";
            }   
        }
    };
}());

Pebble.addEventListener('ready',
    function(e) {
        console.log("JS ready to recieve");
        // initial fetch here
        Bus.getBusInfo();
        
    }        
);

Pebble.addEventListener('appmessage', 
    function(e) {
        console.log("AppMessage received from Pebble");
        // fetch here
        Bus.getBusInfo();
    }      
);

Pebble.addEventListener('showConfiguration', function() {
    console.log("Showing Config Menu");
    var url = "https://hakloev.no/static/files/config.html?stopId1=" + stopId1 + "&stopId2=" + stopId2;
    console.log("Config Menu url: " + url);
    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
    console.log("Config Closed");
    var options = JSON.parse(decodeURIComponent(e.response));
    console.log("Options: " + JSON.stringify(options));
    stopId1 = encodeURIComponent(options.stopId1);
    stopId2 = encodeURIComponent(options.stopId2);
    if (stopId1 == 'undefined') {
        stopId1 = '16011567'; // Berg
    }
    if (stopId2 == 'undefined') {
        stopId2 = '16011192'; // Ila
    }
    window.localStorage.setItem('stopId1', stopId1);
    window.localStorage.setItem('stopId2', stopId2);
    console.log("stopId1: " + stopId1 + " stopId2: " + stopId2);
    Bus.getBusInfo();
});
