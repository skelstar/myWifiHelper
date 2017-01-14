#ifndef myWifiHelper_h
#define myWifiHelper_h

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

typedef void ( *MessageCallbackType )( char* msg );

class MyWifiHelper 
{
    public:

        MyWifiHelper(MessageCallbackType messageCallback);
        int setupWifi();
        IPAddress getWifiIP();
		void setupOTA(char* host);

    private:
    	MessageCallbackType _messageCallback;       // The listener function
};

#endif
