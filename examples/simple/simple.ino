#include <myWifiHelper.h>
//simple.ino

#define WIFI_HOSTNAME "simple-wifihelper-test"
#define WIFI_OTA_NAME "simple-wifihelper-test-OTA"


MyWifiHelper wifiHelper(WIFI_HOSTNAME);

void setup() {

    Serial.begin(9600);
    delay(200);
    Serial.println("Booting");

    Serial.println("setupWifi()");
    wifiHelper.setupWifi();
    Serial.println("setupOTA(WIFI_OTA_NAME)");
    wifiHelper.setupOTA(WIFI_OTA_NAME);
}

void loop() {

    ArduinoOTA.handle();

    Serial.print(".");

    delay(10);
}
