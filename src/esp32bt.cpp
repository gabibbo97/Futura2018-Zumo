#include <Arduino.h>
#include "esp32bt.hpp"

#if ESP32_SOFTWARE_SERIAL
  #include <SoftwareSerial.h>
#endif

#if ESP32_SOFTWARE_SERIAL
  #define ESP32_SERIAL_PORT ESP32SwSerial
  SoftwareSerial ESP32_SERIAL_PORT(ESP32_SOFTWARE_SERIAL_RX_PIN, ESP32_SOFTWARE_SERIAL_TX_PIN);
#else
  #define ESP32_SERIAL_PORT Serial
#endif

ESP32BT::ESP32BT() {}

void ESP32BT::send(String s) {
  ESP32_SERIAL_PORT.print(s);
}

String ESP32BT::readLine(unsigned int timeout) {
  ESP32_SERIAL_PORT.setTimeout(timeout);
  return ESP32_SERIAL_PORT.readStringUntil('\n');
}

byte ESP32BT::read() {
  while (ESP32_SERIAL_PORT.available() < 1)
    continue;
  return ESP32_SERIAL_PORT.read();
}

bool ESP32BT::isAlive() {
  return true;
}

bool ESP32BT::setPin(String pin) {
  return true;
}

bool ESP32BT::setName(String name) {
  return true;
}

bool ESP32BT::connect(String pin, String name, unsigned int baudrate) {
  #ifdef ESP32_DEBUG_SERIAL
    ESP32_DEBUG_SERIAL.println("Starting serial with ESP32BT");
  #endif
  ESP32_SERIAL_PORT.begin(9600);

  return true;
}

int ESP32BT::available() {
  return ESP32_SERIAL_PORT.available();
}