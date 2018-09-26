#include <Arduino.h>

#include "hc06.hpp"
#include "zumo.cpp"

Zumo<HC06> Robot;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Robot.connectBluetooth("1234", "sminotti", 9600);
}

void loop() {
    // put your main code here, to run repeatedly:
    Robot.eventLoop();
}