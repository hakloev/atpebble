atpebble
===================

### atpebble WatchApp for Pebble

This is a WatchApp for the Pebble Watch. It shows realtime bus schedules for two freely chosen bus stops in Trondheim.

By default the app uses the two stops "Gløshaugen Nord - til midtbyen" and "Kongens Gate K2". To change this settings open the settings page in the Pebble App on your phone. For now it's only possible to select bus stops using the location id, but I hope to implement support for text search. It's also possible to select a prefered route. If a prefered route is selected the stops will only display departures by this route. The settings for the app are configurable using the new PebbleKit JS configuration page. 

### Licensing

All the realtime is gathered from the public api [bybussen.api.tmn.io](http://bybussen.api.tmn.io).

The app is based upon [tmn](https://github.com/tmn/)'s [Bybussen-Pebble](https://github.com/tmn/Bybussen-Pebble/).

Be patient, it's work in progress...
