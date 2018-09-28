#include <Arduino.h>

#include "esp32bt.hpp"
#include "zumo.hpp"

#include <Zumo32U4.h>

#include "movement.hpp"


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

Movement m;

void setup() {

}

void loop() {
  m.move(123, 200);
}