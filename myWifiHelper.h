#ifndef myWifiHelper_h
#define myWifiHelper_h

#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

// types
typedef void ( *SubscriptionCallbackType )( byte* payload, unsigned int length );

class MyWifiHelper 
{
    public:

        MyWifiHelper(char* host);
        int setupWifi();
        int setupWifi(char* ssidname);
        IPAddress getWifiIP();
		void setupOTA(char* host);
        void handleOTA();

		void setupMqtt();
		void loopMqtt();
        bool loopMqttNonBlocking();
		void mqttPublish(char* topic, char* payload);
		bool mqttAddSubscription(char* topic, SubscriptionCallbackType callback);
};

#endif
