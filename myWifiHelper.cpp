#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
#include "PubSubClient.h"
#include "WiFiUdp.h"
#include "ArduinoOTA.h"
#include "myWifiHelper.h"
#include "wificonfig.h"
#include "ArduinoJson.h"            // https://github.com/bblanchon/ArduinoJson
#include "string.h"

//--------------------------------------------------------------------------------

#define SUCCESSFUL_CONNECT  1
#define FAILED_CONNECT      2
#define WIFI_RECONNECT_TIME      5000
#define MQTT_RECONNECT_TIME      1000

#define TOPIC_CONSOLE   "/node/console/log"

/* MQTT */
WiFiClient client;
PubSubClient mqttclient(client);

// Function Signatures
void mqttCallback(char *topic, byte* payload, unsigned int length);
void reconnectMqtt();
bool reconnectMqttNonBlocking();
void resubscribeToTopics();


char* _hostname;
char* _baseTopic;
//--------------------------------------------------------------------------------

MyWifiHelper::MyWifiHelper(char* hostname) {
    _hostname = hostname;
}

int MyWifiHelper::setupWifi() {
    setupWifi((char*)ssid);
}

int MyWifiHelper::setupWifi(char* ssidname) {

    WiFi.hostname(_hostname);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidname, password);
    Serial.print("Connecting to: "); Serial.println(ssidname);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(WIFI_RECONNECT_TIME);
        ESP.restart();
    }

    byte mac[6];
    Serial.print("Success: "); 
    Serial.print(WiFi.localIP());
    Serial.print(", MAC - ");
    Serial.print(mac[5],HEX);
    Serial.print(":");
    Serial.print(mac[4],HEX);
    Serial.print(":");
    Serial.print(mac[3],HEX);
    Serial.print(":");
    Serial.print(mac[2],HEX);
    Serial.print(":");
    Serial.print(mac[1],HEX);
    Serial.print(":");
    Serial.println(mac[0],HEX);
    return SUCCESSFUL_CONNECT;
}

IPAddress MyWifiHelper::getWifiIP() {
    return WiFi.localIP();
}

void MyWifiHelper::setupOTA(char* host) {

    Serial.println("setupOTA()...");
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
    Serial.print("OTA Online (host: "); Serial.print(host); Serial.println(") ");
}

void MyWifiHelper::handleOTA() {

    ArduinoOTA.handle();
}

void MyWifiHelper::setupMqtt(char* baseTopic) {
	_baseTopic = baseTopic;
	Serial.print("BaseTopic: ");	Serial.println(_baseTopic);
    mqttclient.setServer(MQTT_SERVER, 1883);    // ie "192.168.1.105"
    mqttclient.setCallback(mqttCallback);
}

void MyWifiHelper::setupMqtt() {

    mqttclient.setServer(MQTT_SERVER, 1883);    // ie "192.168.1.105"
    mqttclient.setCallback(mqttCallback);
}

void MyWifiHelper::loopMqtt() {
    if (!mqttclient.connected()) {
        reconnectMqtt();
    }
    mqttclient.loop();
}

bool MyWifiHelper::loopMqttNonBlocking() {
    if (!mqttclient.connected()) {
        return reconnectMqttNonBlocking();
    } else {
        mqttclient.loop();
    }
}

int mqttSubHead = 0;
#define MAX_SUBSCRIPTIONS   6

struct subscriptionType {
    char* topic;
    SubscriptionCallbackType callback;
};

subscriptionType subscription[MAX_SUBSCRIPTIONS];

void MyWifiHelper::mqttPublish(char* topic, char* payload) {
    bool retain = false;
    mqttclient.publish(topic, payload, retain);
}

void MyWifiHelper::mqttPublish(char* topic, char* payload, bool showDebug) {
    if (showDebug) {
        Serial.print("Publishing to "); 
        Serial.print(topic);
        Serial.print(", payload: ");
        Serial.println(payload);
    }
    mqttclient.publish(topic, payload);
}

void MyWifiHelper::mqttPublishBaseAnd(char* verb, char* payload) {
	char topic[100];
	sprintf(topic, "%s", "");	// clear topic
	sprintf(topic, "%s/%s", _baseTopic, verb);

    mqttclient.publish(topic, payload);
	// Serial.print("Publishing to "); 
	// Serial.print(topic);
	// Serial.print(", payload: ");
	// Serial.println(payload);
}

bool MyWifiHelper::mqttAddSubscriptionBaseAnd(char* verb, SubscriptionCallbackType callback) {
	char topic[20];
	sprintf(topic, "%s", "");	// clear topic
	sprintf(topic, "%s/%s", _baseTopic, verb);

    if (mqttSubHead != MAX_SUBSCRIPTIONS-1) {

        subscription[mqttSubHead].topic = topic;
        subscription[mqttSubHead].callback = callback;
        Serial.print("Subscribing to: ");
        Serial.println(topic);
        mqttSubHead++;
        return true;
    }
    return false;
}
bool MyWifiHelper::mqttAddSubscription(char* topic, SubscriptionCallbackType callback) {

    if (mqttSubHead != MAX_SUBSCRIPTIONS-1) {
        subscription[mqttSubHead].topic = topic;
        subscription[mqttSubHead].callback = callback;
        Serial.print("Subscribing to: ");
        Serial.println(topic);
        mqttSubHead++;
        return true;
    }
    return false;
}

const char* MyWifiHelper::mqttGetJsonCommand(byte *payload) {
    StaticJsonBuffer<100> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(payload);

    if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println((char*)payload);
        return "";
    }

    payload[0] = '\0';

    value = root["value"];
    return root["command"];
}

const char* MyWifiHelper::mqttGetJsonCommandValue() {
    return value;
}

const char* MyWifiHelper::mqttGetJsonParam(char *payload, const char* param) {
    StaticJsonBuffer<100> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(payload);

    if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println((char*)payload);
        return "";
    }

    return root[param];
}

JsonObject& MyWifiHelper::mqttGetJson(byte *payload) {
    StaticJsonBuffer<100> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(payload);

    if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println((char*)payload);
    }
    
    return root;
}

void MyWifiHelper::mqttSendToConsole(char* payload) {
    mqttclient.publish(TOPIC_CONSOLE, payload);
}

//--------------------------------------------------------------------------------

bool reconnectMqttNonBlocking() {
    if (!mqttclient.connected()) {
        if (mqttclient.connect(_hostname, MQTT_USERNAME, MQTT_PASSWORD)) {
            //Serial.println("connected");
            resubscribeToTopics();
        } 
        return mqttclient.connected();
    }
}

void reconnectMqtt() {

    while (!mqttclient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttclient.connect(_hostname, MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.println("connected");
            resubscribeToTopics();
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttclient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(MQTT_RECONNECT_TIME);
        }
    }
}

void resubscribeToTopics() {
    for (int i=0; i<mqttSubHead; i++) {
        mqttclient.subscribe(subscription[i].topic);
    }
}

void mqttCallback(char *topic, byte* payload, unsigned int length) {

    for (int i=0; i<mqttSubHead; i++) {
        if (strcmp(subscription[i].topic, topic) == 0) {
            subscription[i].callback(payload, length);
        }
    }
}
