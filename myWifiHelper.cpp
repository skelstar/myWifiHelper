#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
// #include "Adafruit_MQTT.h"
// #include "Adafruit_MQTT_Client.h"
#include "PubSubClient.h"
#include "WiFiUdp.h"
#include "ArduinoOTA.h"
#include "myWifiHelper.h"
#include "wificonfig.h"


#define SUCCESSFUL_CONNECT  1
#define FAILED_CONNECT      2

/* MQTT */
WiFiClient espClient;
PubSubClient client(espClient);

// Function Signatures
void mqttCallback(char *topic, byte* payload, unsigned int length);
void reconnectMqtt();

/* ------------------------------------------------------------------------------------------ */

MyWifiHelper::MyWifiHelper(MessageCallbackType messageCallback) {

    _messageCallback = messageCallback;
}

int MyWifiHelper::setupWifi() {

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        _messageCallback("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    Serial.print("Success: "); Serial.println(WiFi.localIP());
    //_messageCallback("Success!");
    return SUCCESSFUL_CONNECT;
}

IPAddress MyWifiHelper::getWifiIP() {
    return WiFi.localIP();
}

void MyWifiHelper::setupOTA(char* host) {

    ArduinoOTA.setHostname(host);
    
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial.print("OTA Online.. listening for: "); Serial.println(host);
}

void MyWifiHelper::setupMqtt() {

    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(mqttCallback);
}

void MyWifiHelper::loopMqtt() {
    if (!client.connected()) {
        reconnectMqtt();
    }
    client.loop();
}

void MyWifiHelper::mqttPublish(char* topic, char* payload) {
    Serial.print("Publishing to "); 
    Serial.print(topic);
    Serial.print(", payload: ");
    Serial.println(payload);
    client.publish(topic, payload);
}

int mqttSubHead = 0;
#define MAX_SUBSCRIPTIONS   3

struct subscriptionType {
    char* topic;
    SubscriptionCallbackType callback;
};

subscriptionType subscription[MAX_SUBSCRIPTIONS];

bool MyWifiHelper::mqttAddSubscription(char* topic, SubscriptionCallbackType callback) {

    if (mqttSubHead != MAX_SUBSCRIPTIONS-1) {
        subscription[mqttSubHead].topic = topic;
        subscription[mqttSubHead].callback = callback;
        mqttSubHead++;
        return true;
    }
    return false;
}

void reconnectMqtt() {

    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(MQTT_CLIENTNAME, MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.println("connected");
            for (int i=0; i<mqttSubHead; i++) {
                client.subscribe(subscription[i].topic);
            }
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte* payload, unsigned int length) {

    for (int i=0; i<mqttSubHead; i++) {
        if (strcmp(subscription[i].topic, topic) == 0) {
            subscription[i].callback();
        }
    }
}
