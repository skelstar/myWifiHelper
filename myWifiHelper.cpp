#include "Arduino.h"
#include "EventManager.h"
#include "myPushButton.h"

EventManager evM;
#define EVENT_CODE EventManager::kEventUser9

myPushButton::myPushButton(uint8_t pin, bool pullUp, uint16_t heldDurationMs, uint8_t lowState, EventListener listenerCallback) {

    _pin = pin;
    _pullUp = pullUp;
    _lowState = lowState;
    _heldDurationMillis = heldDurationMs;

    _state = ST_NOT_HELD;

    evM.addListener(EVENT_CODE, listenerCallback);

    init2();
}

bool myPushButton::singleButtonPush() {
    return true;    // _state == HELD_FOR_LESS_THAN_HELD_TIME
}

void myPushButton::serviceEvents() {
    bool buttonPressed = isPressed();

    if (_state == ST_NOT_HELD) {
        if (buttonPressed) {
            _state = EV_BUTTON_PRESSED;
        }

    } else if (_state == EV_BUTTON_PRESSED ) {
        _heldBeginMillis = millis();
        evM.queueEvent(EVENT_CODE, _state);
        _state = ST_WAIT_FOR_HELD_TIME;

    } else if (_state == ST_WAIT_FOR_HELD_TIME) {
        if (buttonPressed && isHeldForLongEnough()) {
            _state = EV_HELD_FOR_LONG_ENOUGH;
        } else if (!buttonPressed) {
            _state = EV_RELEASED;
        }

    } else if (_state == EV_HELD_FOR_LONG_ENOUGH) {
        evM.queueEvent(EVENT_CODE, _state);
        _state = ST_WAITING_FOR_RELEASE;

    } else if (_state == ST_WAITING_FOR_RELEASE) {
        if (!buttonPressed) {
            _state = EV_RELEASED;
        }

    } else if (_state == EV_RELEASED) {
        evM.queueEvent(EVENT_CODE, _state);
        _state = ST_NOT_HELD;
    }

    evM.processEvent();
}

void myPushButton::init2() 
{
    if (_pullUp) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }

    delayMicroseconds(5);
}

bool myPushButton::isPressed() 
{
    init2();
    return digitalRead(_pin) != _lowState;
}

bool myPushButton::isHeldForLongEnough() 
{
    return uint16_t(_heldBeginMillis + _heldDurationMillis) <= millis();
}