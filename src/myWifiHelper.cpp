#include <Arduino.h>
#include <PubSubClient.h>
#include "myWifiHelper.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

//--------------------------------------------------------------------------------

#define SUCCESSFUL_CONNECT 1
#define FAILED_CONNECT 2
#define WIFI_RECONNECT_TIME 5000
#define MQTT_RECONNECT_TIME 1000

/* MQTT */
WiFiClient client;
PubSubClient mqttclient(client);

// Function Signatures

char *_hostname;
bool _addChipId;
char *_baseTopic;
//--------------------------------------------------------------------------------

bool MyWifiHelper::begin(char *hostname)
{
  WiFiManager wm;
  // reset settings - wipe credentials for testing
  // wm.resetSettings();
  return wm.autoConnect("Smart Socket AP"); // anonymous ap
}

int _mqttSubHead = 0;

subscriptionType subscription[MAX_SUBSCRIPTIONS];

void mqttCallback(char *topic, byte *payload, unsigned int length)
{

  for (int i = 0; i < _mqttSubHead; i++)
  {
    if (strcmp(subscription[i].topic, topic) == 0)
    {
      subscription[i].callback(payload, length);
    }
  }
}

void MyWifiHelper::beginWithMQTT(char *hostname)
{
  WiFiManager wm;
  // reset settings - wipe credentials for testing
  // wm.resetSettings();
  WiFiManagerParameter custom_mqtt_password(/*name*/ "server", /*prompt*/ "MQTT password", /*default*/ "", /*length*/ 40);
  wm.addParameter(&custom_mqtt_password);

  bool res;
  res = wm.autoConnect("Smart Socket AP"); // anonymous ap
  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    Serial.println("connected...yeey :)");
  }
  // mqtt
  mqttclient.setServer("192.168.1.105", 1883);
  mqttclient.setCallback(mqttCallback);
  int attempts = 0;
  while (!mqttclient.connected())
  {
    // Attempt to connect
    mqttclient.connect(hostname, "skelstar", custom_mqtt_password.getValue());
    if (attempts++ > 5)
    {
      Serial.printf("Couldn't connect to MQTT!\n");
    }
  }
  Serial.printf("Connected to MQTT! \n");
}

// void MyWifiHelper::setupOTA(char* host) {

//     //Serial.println("setupOTA()...");
//     ArduinoOTA.setHostname(host);

//     ArduinoOTA.onStart([]() {
//         Serial.println("Start");
//     });
//     ArduinoOTA.onEnd([]() {
//         Serial.println("\nEnd");
//     });

//     ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//         Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//     });

//     ArduinoOTA.onError([](ota_error_t error) {
//         Serial.printf("Error[%u]: ", error);
//         if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
// 		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
// 		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
// 		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
// 		else if (error == OTA_END_ERROR) Serial.println("End Failed");
//     });

//     ArduinoOTA.begin();
//     Serial.print("OTA Online (host: "); Serial.print(host); Serial.println(") ");
// }

// void MyWifiHelper::handleOTA() {

//     ArduinoOTA.handle();
// }

void MyWifiHelper::loopMqtt()
{
  if (!mqttclient.connected())
  {
    // reconnectMqtt();
  }
  mqttclient.loop();
}

void MyWifiHelper::mqttPublish(char *topic, char *payload)
{
  bool retain = false;
  mqttclient.publish(topic, payload, retain);
}

bool MyWifiHelper::mqttAddSubscription(char *topic, SubscriptionCallbackType callback)
{

  if (_mqttSubHead != MAX_SUBSCRIPTIONS - 1)
  {
    subscription[_mqttSubHead].topic = topic;
    subscription[_mqttSubHead].callback = callback;
    Serial.print("Subscribing to: ");
    Serial.println(topic);
    _mqttSubHead++;
    return true;
  }
  return false;
}
