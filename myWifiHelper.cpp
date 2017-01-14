#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "WiFiUdp.h"
#include "ArduinoOTA.h"
#include "myWifiHelper.h"
#include "wificonfig.h"


#define SUCCESSFUL_CONNECT  1
#define FAILED_CONNECT      2

/* MQTT */
WiFiClient client;

//Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_PASSWORD);

int head = 0;
#define MAX_SUBSCRIPTIONS   3
Adafruit_MQTT_Subscribe *subscriptions[MAX_SUBSCRIPTIONS];

Adafruit_MQTT_Subscribe timefeed = Adafruit_MQTT_Subscribe(&mqtt, "time/seconds");
Adafruit_MQTT_Subscribe devtestfeed = Adafruit_MQTT_Subscribe(&mqtt, "dev/test");

void timecallback(uint32_t current);
// void devtestcallback(char* message);

int timeZone = -13; // UTC - 4 eastern daylight time (nyc)


void devtestcallback(uint32_t message) {
    Serial.print("devtest: "); Serial.println(message);
}

/* ------------------------------------------------------------------------------------------ */

MyWifiHelper::MyWifiHelper(MessageCallbackType messageCallback) {
    _messageCallback = messageCallback;

    timefeed.setCallback(timecallback);
    devtestfeed.setCallback(devtestcallback);

    mqtt.subscribe(&timefeed);

    for (int i=0; i<3; i++) {
        subscriptions[i] = NULL;
    }

    mqtt.ping();
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

bool MyWifiHelper::MQTT_connect() {
    int8_t ret;

    // Stop if already connected.
    if (mqtt.connected()) {
        return true;
    }

    Serial.print("Connecting to MQTT... ");

    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 10 seconds...");
        mqtt.disconnect();
        return false;
    }
    Serial.println("MQTT Connected!");
    
    mqtt.ping();

    mqtt.processPackets(5000);

    return true;
}

// int MyWifiHelper::addSubscription(char* feed, SubscriptionCallbackType callback) {

//     int idx = head;
//     if (head == MAX_SUBSCRIPTIONS - 1) {
//         // can't add more subscriptions
//         return -1;
//     }
//     head++;
//     subscriptions[idx] = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME , feed, MQTT_QOS_1);
    
// }

void timecallback(uint32_t current) {
    Serial.print("Time: "); Serial.println(current);
}
