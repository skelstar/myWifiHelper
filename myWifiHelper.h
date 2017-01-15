#ifndef myWifiHelper_h
#define myWifiHelper_h

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
// #include <Adafruit_MQTT.h>
// #include <Adafruit_MQTT_Client.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

// types
typedef void ( *MessageCallbackType )( char* msg );
typedef void ( *SubscriptionCallbackType )( byte* payload, unsigned int length );

class MyWifiHelper 
{
    public:

    	MyWifiHelper(MessageCallbackType messageCallback);
        int setupWifi();
        IPAddress getWifiIP();
		void setupOTA(char* host);

		void setupMqtt();
		void loopMqtt();
		void mqttPublish(char* topic, char* payload);
		bool mqttAddSubscription(char* topic, SubscriptionCallbackType callback);

    private:
    	MessageCallbackType _messageCallback;       // The listener function
};

#endif
