#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
#include "WiFiUdp.h"
#include "ArduinoOTA.h"
#include "myWifiHelper.h"
#include "wificonfig.h"


#define SUCCESSFUL_CONNECT  1
#define FAILED_CONNECT      2


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