var Bus = ( function () {
    
    var busInfoUrl = "http://bybussen.api.tmn.io/rt/";

    return {
        getBusInfo: function(place) {
            var self = this; // Needed to access parseInfo in callback function
            this.xmlReq(busInfoUrl + place, "GET", 
                function(responseText) {
                    var json = JSON.parse(responseText);
                    self.parseInfo(json);
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
            xhr.open(type, url, true);
            xhr.send();
        }, 
        parseInfo: function(place) {
            console.log("INFO: parseInfo called");
            var departureList = [];
            for (var i = 0; i < place.next.length; i++) {
                if (place.next[i].l == "5") {
                    departureList.push(place.next[i]);
                }
                if (departureList.length == 3) {
                    break;
                }
            }
            this.printInfo(departureList);
        }, 
        printInfo: function(list) {
            console.log("INFO: printInfo called");
            var dictionary = {}; 
            for (var i = 0; i < list.length; i++) {
                console.log(list[i].t.substring(11, 16) + " - " + this.calcTime(list[i].t));
                dictionary[i] = list[i].t.substring(11, 16) + " - " + this.calcTime(list[i].t);
            }
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log("Sent departures to pebble");
                },
                function(e) {
                     console.log("Error while sending departures to pebble");   
                }
            );
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
        Bus.getBusInfo("16011567");
        Bus.getBusInfo("16011192");
        
    }        
);

Pebble.addEventListener('appmessage', 
    function(e) {
        console.log("AppMessage received from Pebble");
        // fetch here
        Bus.getBusInfo("16011567");
        Bus.getBusInfo("16011192");
    }      
);
