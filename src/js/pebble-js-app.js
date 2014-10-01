var Bus = ( function () {
    
    var busInfoUrl = "http://bybussen.api.tmn.io/rt/";
    var berg = "16011567";
    var ila = "16011192";
    var dictionary;
    
    return {
        getBusInfo: function() {
            dictionary = {};
            var self = this; // Needed to access parseInfo in callback function
            this.xmlReq(busInfoUrl + berg, "GET", 
                function(responseText) {
                    var json = JSON.parse(responseText);
                    self.parseInfo(json);
                }
            );
            this.xmlReq(busInfoUrl + ila, "GET",
                function(responseText) {
                    var json = JSON.parse(responseText);
                    self.parseInfo(json);
                }
            );
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log("Sent departures to pebble");
                    console.log(Object.keys(dictionary).length);
                    for (var key in dictionary) {
                        console.log(dictionary[key]);
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
            console.log("INFO: parseInfo called");
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
            console.log("INFO: printInfo called");
            var row = "";
            for (var i = 1; i < list.length; i++) {
                console.log(list[i].t.substring(11, 16) + " - " + this.calcTime(list[i].t));
                row += list[i].t.substring(11, 16) + " - " + this.calcTime(list[i].t) + "\n";
            }
            if (list[0] === "Ila") {
                dictionary["1"] = row;
            } else {
                dictionary["0"] = row;                
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
    console.log("showing configmenu");
});

Pebble.addEventListener('webviewclosed', function(e) {
    console.log("config closed");
    var options = JSON.parse(decodeURIComponent(e.response));
    console.log("Options: " + JSON.stringify(options));
});
