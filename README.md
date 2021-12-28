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
