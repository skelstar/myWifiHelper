#ifndef myWifiHelper_h
#define myWifiHelper_h

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

class MyWifiHelper 
{
    public:
        MyWifiHelper();
        void setupWifi();
		void setupOTA(char* host);
    private:
};

#endif
