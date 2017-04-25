#ifndef myWifiHelper_h
#define myWifiHelper_h

#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>            // https://github.com/bblanchon/ArduinoJson

// types
typedef void ( *SubscriptionCallbackType )( byte* payload, unsigned int length );

class MyWifiHelper 
{
    public:
        
        WiFiClient client;

        MyWifiHelper(char* host);
        int setupWifi();
        int setupWifi(char* ssidnObjectame);
        IPAddress getWifiIP();

		void setupOTA(char* host);
        void handleOTA();

		void setupMqtt();
		void loopMqtt();
        bool loopMqttNonBlocking();
        void mqttPublish(char* topic, char* payload);
        void mqttPublish(char* topic, char* payload, bool showDebug);
		bool mqttAddSubscription(char* topic, SubscriptionCallbackType callback);
        const char* mqttGetJsonCommand(byte *payload);
        const char* mqttGetJsonCommandValue();

        const char* value;
};

#endif
