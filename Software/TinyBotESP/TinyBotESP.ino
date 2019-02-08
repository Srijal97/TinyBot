#include "Arduino.h"
/*
 * ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Subscribe Example
 * Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
 * Made as part of my MQTT Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
 */
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

const int ledPin = 2; // This code uses the built-in led for visual feedback that a message has been received

// WiFi
// Make sure to update this for your own WiFi network!
const char* ssid = "SSID";
const char* wifi_password = "password";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "192.168.1.106";
const char* mqtt_topic = "led_status";
const char* mqtt_username = "Your MQTT Username";
const char* mqtt_password = "Your MQTT Password";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "ESP_1";

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

void ReceivedMessage(char* topic, byte* payload, unsigned int length) {
  // Output the first character of the message to serial (debug)
  Serial.println((char)payload[0]);

  // Handle the message we received
  // Here, we are only looking at the first character of the received message (payload[0])
  // If it is 0, turn the led off.
  // If it is 1, turn the led on.
  if ((char)payload[0] == '0') {
    digitalWrite(ledPin, LOW);
  }
  if ((char)payload[0] == '1') {
    digitalWrite(ledPin, HIGH);
  }
}

bool Connect() {
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect(clientID)) {
      client.subscribe(mqtt_topic);
      return true;
    }
    else {
      return false;
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);

  // Switch the on-board LED off to start with
  digitalWrite(ledPin, LOW);

  // Begin Serial on 115200
  // Remember to choose the correct Baudrate on the Serial monitor!
  // This is just for debugging purposes
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // setCallback sets the function to be called when a message is received.
  client.setCallback(ReceivedMessage);
  if (Connect()) {
    Serial.println("Connected Successfully to MQTT Broker!");
  }
  else {
    Serial.println("Connection Failed!");
  }
}

void loop() {
  // If the connection is lost, try to connect again
  if (!client.connected()) {
    Connect();
  }
  // client.loop() just tells the MQTT client code to do what it needs to do itself (i.e. check for messages, etc.)
  client.loop();
  // Once it has done all it needs to do for this cycle, go back to checking if we are still connected.
}
