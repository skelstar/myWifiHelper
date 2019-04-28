#ifndef myWifiHelper_h
#define myWifiHelper_h

#include <stdlib.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

// types
typedef void (*SubscriptionCallbackType)(byte *payload, unsigned int length);

struct subscriptionType
{
  char *topic;
  SubscriptionCallbackType callback;
};

class MyWifiHelper
{
  public:
    bool begin(char *hostname);
    void beginWithMQTT(char *hostname);

    IPAddress getWifiIP();

    void setupOTA(char *host);
    void handleOTA();
    // void setupMqtt(char* baseTopic);
    // void setupMqtt();
    void loopMqtt();
    void mqttPublish(char *topic, char *payload);
    bool mqttAddSubscription(char *topic, SubscriptionCallbackType callback);
    // void mqttCallback(char *topic, byte *payload, unsigned int length);

    #define MAX_SUBSCRIPTIONS 6

  private:
    WiFiClient client;
    // const char * value;

    subscriptionType subscription[MAX_SUBSCRIPTIONS];
};

#endif
