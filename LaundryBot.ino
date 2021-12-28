/*
 This example connects to an unencrypted WiFi network.
 Then it prints the MAC address of the WiFi module,
 the IP address obtained, and other network details.

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <Arduino_LSM6DS3.h>
#include <ArduinoOTA.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
char mqtt_host[] = MQTT_HOST; // your mqtt host address
char mqtt_username[] = MQTT_USERNAME; // your mqtt username
char mqtt_password[] = MQTT_PASSWORD; // your mqtt password
char ota_password[] = OTA_PASSWORD; // your mqtt password

int status = WL_IDLE_STATUS;  // the WiFi radio's status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

int        port     = 1883;

const char topic_x[]  = "laundry_bot/acceleration_x";
const char topic_y[]  = "laundry_bot/acceleration_y";
const char topic_z[]  = "laundry_bot/acceleration_z";

const char topic_total_force[]  = "laundry_bot/total_force";

const char topic_rssi[]  = "laundry_bot/rssi";

float initial_x, initial_y, initial_z;

float accelleration_threshold = 0.05;

int set_last_accellerations = 1;

unsigned long elapsed_rssi_update;

void setup() {
  //Initialize serial
  Serial.begin(19200);
  // while (!Serial) {
  //   ;  // wait for serial port to connect. Needed for native USB port only
  // }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  while (!IMU.accelerationAvailable()) {
    ; // wait for initial accellerometer data
  }

  IMU.readAcceleration(initial_x, initial_y, initial_z);
  Serial.print("Initial position - x:");
  Serial.print(initial_x);
  Serial.print(" y:");
  Serial.print(initial_y);
  Serial.print(" z:");
  Serial.print(initial_z);
  Serial.println();

  // attempt to initially connect to WiFi network:
  connectToWifi();

  connectToMQTT();

  // start the WiFi OTA library with internal (flash) based storage
  ArduinoOTA.begin(WiFi.localIP(), "Arduino", ota_password, InternalStorage);

  elapsed_rssi_update = millis();
}

void loop() {
  ArduinoOTA.poll();

  if (IMU.accelerationAvailable()) {    
    status = WiFi.status();
    if (status != WL_CONNECTED) {
      connectToWifi();
    }

    uint8_t mqttStatus = mqttClient.connected();

    if (!mqttStatus) {
      connectToMQTT();      
    }
      
    float x, y, z;

    IMU.readAcceleration(x, y, z);

    float delta_x, delta_y, delta_z;

    delta_x = initial_x - x;
    delta_y = initial_y - y;
    delta_z = initial_z - z;

    if (millis() - elapsed_rssi_update > 10000) {
      elapsed_rssi_update = millis();

      long rssi = WiFi.RSSI();

      mqttClient.beginMessage(topic_rssi);
      mqttClient.print(rssi);
      mqttClient.endMessage();
    }

    float total_force = abs(delta_x) + abs(delta_y) + abs(delta_z);
    
    if (set_last_accellerations > 0 || total_force > accelleration_threshold) {
      Serial.println("Acceleration");

      mqttClient.beginMessage(topic_total_force);
      mqttClient.print(total_force);
      mqttClient.endMessage();

      mqttClient.beginMessage(topic_x);
      mqttClient.print(delta_x);
      mqttClient.endMessage();

      mqttClient.beginMessage(topic_y);
      mqttClient.print(delta_y);
      mqttClient.endMessage();

      mqttClient.beginMessage(topic_z);
      mqttClient.print(delta_z);
      mqttClient.endMessage();

      if (set_last_accellerations > 0) {
        set_last_accellerations--;
      } else {
        set_last_accellerations = 20;
      }
    }
  }
}

void connectToMQTT() {
  status = WiFi.status();
  if (status != WL_CONNECTED) {
    connectToWifi();
  }

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(mqtt_host);

  mqttClient.setUsernamePassword(mqtt_username, mqtt_password);
  
  int mqttStatus = 0;
  
  while (!mqttStatus) {
    mqttStatus = mqttClient.connect(mqtt_host, port);
    Serial.println(mqttStatus);
    
    if (!mqttStatus) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
    }

    delay(10000);
  }  

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void connectToWifi() {
    // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    Serial.println(status);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
