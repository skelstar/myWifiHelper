#ifndef myWifiHelper_h
#define myWifiHelper_h

#include <stdlib.h>
#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#elif defined(ESP32)
    #include <WiFi.h>
#endif
// #include <ESP8266mDNS.h>
// #include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>         // https://github.com/bblanchon/ArduinoJson
// #include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

// types
typedef void ( *SubscriptionCallbackType )( byte* payload, unsigned int length );

class MyWifiHelper 
{
    public:
        
        WiFiClient client;

        MyWifiHelper(char* hostname);
        MyWifiHelper(char* hostname, bool addChipId);
        int setupWifi();
        int setupWifi(char* ssidnObjectame);
        int setupWifiManagerOnDemand();
        int setupWifiManagerAutoConnect();

        IPAddress getWifiIP();

		void setupOTA(char* host);
        void handleOTA();

        void setupMqtt(char* baseTopic);
		void setupMqtt();

		void loopMqtt();
        bool loopMqttNonBlocking();
        void mqttPublish(char* topic, char* payload);
        void mqttPublish(char* topic, char* payload, bool showDebug);
        void mqttPublishWithId(char* topic, char* payload);
        // void mqttPublishBaseAnd(char* verb, char* payload);
        // bool mqttAddSubscriptionBaseAnd(char* verb, SubscriptionCallbackType callback);
		bool mqttAddSubscription(char* topic, SubscriptionCallbackType callback);
        // bool mqttAddSubscriptionWithId(char* topic, SubscriptionCallbackType callback);

        const char* mqttGetJsonCommand(byte *payload);
        const char* mqttGetJsonCommandValue();
        const char* mqttGetJsonParam(char *payload, const char* param);
        JsonObject& mqttGetJson(byte *payload);
        void mqttSendToConsole(char* payload);

        const char* value;
};

#endif
