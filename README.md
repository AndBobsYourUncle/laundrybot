# laundrybot
Flash it to an Arduino that has built-in Wifi and an accelerometer, and you can detect when the laundry is done.

Tested with an Arduino Nano 33 IoT.

It publishes to the following MQTT topics out of the box:
* laundry_bot/acceleration_x
  * Acceleration from the onboard accelerometer for the X axis 
* laundry_bot/acceleration_y
  * Acceleration from the onboard accelerometer for the Y axis
* laundry_bot/acceleration_z
  * Acceleration from the onboard accelerometer for the Z axis
* laundry_bot/total_force
  * The absolute value of all the acceleration values added together for each axis
* laundry_bot/rssi
  * The Wifi signal strength

There is also an OTA update baked in so that flashing it after updating the code is made a lot easier.

By default, it only publishes the accelerometer values when the total force exceeds a threshold of 0.05. It also publishes the signal strength every 10 seconds.

Handling of the raw acceleration data can be processed by some upstream consumer, like NodeRED.

Using the NodeRED package here:
https://flows.nodered.org/node/node-red-contrib-statistics

Here is a sample NodeRED flow that takes in sensor values for the total force, and constantly computes and republishes an MQTT value for the standard deviation:
```
[{"id":"9bca418296526db4","type":"tab","label":"Laundry","disabled":false,"info":"","env":[]},{"id":"6dfbfc5eb54dec8c","type":"server-state-changed","z":"9bca418296526db4","name":"Laundry Total Force","server":"269d68e7.0836b8","version":3,"exposeToHomeAssistant":false,"haConfig":[{"property":"name","value":""},{"property":"icon","value":""}],"entityidfilter":"sensor.laundry_total_force","entityidfiltertype":"exact","outputinitially":false,"state_type":"str","haltifstate":"","halt_if_type":"str","halt_if_compare":"is","outputs":1,"output_only_on_state_change":true,"for":0,"forType":"num","forUnits":"minutes","ignorePrevStateNull":false,"ignorePrevStateUnknown":false,"ignorePrevStateUnavailable":false,"ignoreCurrentStateUnknown":false,"ignoreCurrentStateUnavailable":false,"outputProperties":[{"property":"payload","propertyType":"msg","value":"","valueType":"entityState"},{"property":"data","propertyType":"msg","value":"","valueType":"eventData"},{"property":"topic","propertyType":"msg","value":"","valueType":"triggerId"},{"property":"parameter","propertyType":"msg","value":"standardDeviation","valueType":"str"}],"x":130,"y":60,"wires":[["f9a79f6ff95fae39"]]},{"id":"f9a79f6ff95fae39","type":"join","z":"9bca418296526db4","name":"group as array","mode":"custom","build":"array","property":"payload","propertyType":"msg","key":"topic","joiner":"\\n","joinerType":"str","accumulate":false,"timeout":"1","count":"100","reduceRight":false,"reduceExp":"","reduceInit":"","reduceInitType":"","reduceFixup":"","x":140,"y":140,"wires":[["047d451cce9ed7d8"]]},{"id":"c0bfacd6fd65a955","type":"statistics","z":"9bca418296526db4","name":"","dataSetSize":"100","inputField":"payload","inputFieldType":"msg","resultField":"payload","resultFieldType":"msg","parameterField":"","parameterFieldType":"payload","stripFunction":false,"resultOnly":true,"x":500,"y":140,"wires":[["9223d83bee19d0c1"]]},{"id":"047d451cce9ed7d8","type":"change","z":"9bca418296526db4","name":"topic = func","rules":[{"t":"set","p":"topic","pt":"msg","to":"standardDeviation","tot":"str"}],"action":"","property":"","from":"","to":"","reg":false,"x":330,"y":140,"wires":[["c0bfacd6fd65a955"]]},{"id":"9223d83bee19d0c1","type":"mqtt out","z":"9bca418296526db4","name":"home/laundry/standard_deviation","topic":"home/laundry/standard_deviation","qos":"","retain":"true","respTopic":"","contentType":"","userProps":"","correl":"","expiry":"","broker":"f8d229e8.e776e8","x":740,"y":140,"wires":[]},{"id":"269d68e7.0836b8","type":"server","name":"Home Assistant","version":2,"rejectUnauthorizedCerts":true,"ha_boolean":"y|yes|true|on|home|open","connectionDelay":true,"cacheJson":true,"heartbeat":false,"heartbeatInterval":30},{"id":"f8d229e8.e776e8","type":"mqtt-broker","name":"Home Assistant","broker":"localhost","port":"1883","clientid":"nodered","usetls":false,"compatmode":true,"keepalive":"60","cleansession":true,"birthTopic":"","birthQos":"0","birthPayload":"","closeTopic":"","closeQos":"0","closePayload":"","willTopic":"","willQos":"0","willPayload":""}]
```
