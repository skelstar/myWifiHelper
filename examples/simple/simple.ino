#include <myPushButton.h>
//simple.ino

void listener_Button(int eventCode, int eventParam);

myPushButton button(5, true, 2000, 1, listener_Button);

void listener_Button(int eventCode, int eventParam) {
    
    switch (eventParam) {
        
        case button.EV_BUTTON_PRESSED:     
            Serial.println("EV_BUTTON_PRESSED");
            break;          
        
        case button.EV_HELD_FOR_LONG_ENOUGH:
            Serial.println("EV_HELD_FOR_LONG_ENOUGH");
            break;
        
        case button.EV_RELEASED:
            Serial.println("EV_RELEASED");
            break;
    }
}

void setup() {

    Serial.begin(9600);
    delay(200);
    Serial.println("Booting");
}

void loop() {
	button.serviceEvents();
}
