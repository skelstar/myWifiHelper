// #ifndef myPushButton_h
// #define myPushButton_h

// #include "Arduino.h"

// class Morse
// {
//   public:
//     Morse(int pin);
//     void dot();
//     void dash();
//   private:
//     int _pin;
// };

// #endif


#ifndef myPushButton_h
#define myPushButton_h

#include <Arduino.h>
#include <EventManager.h>

class myPushButton 
{
    public:
        enum StateCode {
            ST_NOT_HELD,
            EV_BUTTON_PRESSED,
            ST_WAIT_FOR_HELD_TIME,
            EV_HELD_FOR_LONG_ENOUGH,
            ST_WAITING_FOR_RELEASE,
            EV_RELEASED
        };

        typedef void ( *EventListener )( int eventCode, int eventParam );
        myPushButton(uint8_t pin, bool pullUp, uint16_t heldDurationMs, uint8_t lowState, EventListener listenerCallback);
        bool isPressed();
        void serviceEvents();
        bool singleButtonPush();
        bool isHeld();
        bool isReleased();
    private:
        EventManager evM;
        uint8_t _pin;
        bool _pullUp;
        uint8_t _state;
        uint8_t _lowState;
        uint16_t _heldDurationMillis;
        uint16_t _heldBeginMillis;

        bool isHeldForLongEnough();
        void init2();
};

#endif
