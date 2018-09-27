#include <Arduino.h>

#define ESP32_SOFTWARE_SERIAL false
#define ESP32_SOFTWARE_SERIAL_RX_PIN 8
#define ESP32_SOFTWARE_SERIAL_TX_PIN 10

// Comment out to disable debugging
#define ESP32_DEBUG_SERIAL Serial

class ESP32BT {
  public:
    // Constructor
    ESP32BT();
    // Methods
    bool connect(String pin, String name, unsigned int baudrate);
    void send(String s);
    String readLine(unsigned int timeout = 10000);
    byte read();
    int available();

    bool isAlive();
    bool setPin(String pin);
    bool setName(String pin);
};