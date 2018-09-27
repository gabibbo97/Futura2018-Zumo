#include <Arduino.h>

#include "esp32bt.hpp"
#include "zumo.cpp"

Zumo<ESP32BT> Robot;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Robot.connectBluetooth("1234", "Zumo", 9600);
}

void loop() {
    // put your main code here, to run repeatedly:
    Robot.eventLoop();
}