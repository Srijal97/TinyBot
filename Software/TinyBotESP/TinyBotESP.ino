/*
 * ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Subscribe Example
 * Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
 * Made as part of my MQTT Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
 */


#include "Arduino.h"
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <string>

const int ledPin = 2; // This code uses the built-in led for visual feedback that a message has been received

// WiFi
// Make sure to update this for your own WiFi network!
const char* SSID = "A501";
const char* WIFI_PASS = "spoo1989";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* MQTT_SERVER = "192.168.1.106";
const char* MQTT_TOPIC = "command";
const char* MQTT_USERNAME = "Your MQTT Username";
const char* MQTT_PASS = "Your MQTT Password";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* CLIENT_ID = "001";
const char* DEFAULT_ID = "000";

// Initialise the WiFi and MQTT Client objects
WiFiClient wifi_client;
PubSubClient client(MQTT_SERVER, 1883, wifi_client); // 1883 is the listener port for the Broker

void MQTT_sub_callback(char* recv_topic, byte* payload, unsigned int msg_length) {
	if(!strcmp(recv_topic, MQTT_TOPIC) && msg_length == 6){
		// payload = "000-00" ==> "device_id-command"
		String recv_client_id = "000";
		recv_client_id[0] = (char)payload[0];
		recv_client_id[1] = (char)payload[1];
		recv_client_id[2] = (char)payload[2];

		unsigned int recv_command = (payload[4] - 48)*10 + (payload[5] - 48);

		if( recv_client_id == CLIENT_ID || recv_client_id == DEFAULT_ID ){
			switch(recv_command){
			case 0:
				Serial.println("Stop");
				break;
			case 1:
				Serial.println("Forward");
				break;
			case 2:
				Serial.println("Backward");
				break;
			case 3:
				Serial.println("Left");
				break;
			case 4:
				Serial.println("Right");
				break;
			default:
				Serial.println("Error: Command not defined!");

			}
		}
	}
}

bool Connect() {
	// Connect to MQTT Server and subscribe to the topic
	if (client.connect(CLIENT_ID)) {
		client.subscribe(MQTT_TOPIC);
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
	Serial.println(SSID);

	// Connect to the WiFi
	WiFi.begin(SSID, WIFI_PASS);

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
	client.setCallback(MQTT_sub_callback);

}

void loop() {
	if (!client.connected()) {
		if (Connect()) {
			Serial.println("Connected Successfully to MQTT Broker!");
		}
		else {
			Serial.println("Connection Failed!");
			Serial.println("Retrying...");
			delay(3000);
		}
	}
	// client.loop() just tells the MQTT client code to do what it needs to do itself (i.e. check for messages, etc.)
	client.loop();
	// Once it has done all it needs to do for this cycle, go back to checking if we are still connected.
}
