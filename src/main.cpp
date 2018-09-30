#include <Arduino.h>
#include <Zumo32U4.h>

#include "esp32bt.hpp"
#include "zumo.cpp"

Zumo<ESP32BT>* Robot;

void setup() {
  // put your setup code here, to run once:
  Robot = new Zumo<ESP32BT>();
  Robot->connectBluetooth("1234", "Zumo", 9600);
  Robot->init();
}

void loop() {
  // put your main code here, to run repeatedly:
  Robot->eventLoop();
}
