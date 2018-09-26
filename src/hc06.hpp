#include <Arduino.h>

#define HC06_SOFTWARE_SERIAL false
#define HC06_SOFTWARE_SERIAL_RX_PIN 8
#define HC06_SOFTWARE_SERIAL_TX_PIN 10

// Comment out to disable debugging
#define HC06_DEBUG_SERIAL Serial

enum class HC06AtCommand {
  HEALTH_CHECK,
  SET_NAME,
  SET_PIN,
  VERSION
};

class HC06 {
  private:
    String getATcommand(HC06AtCommand c);
  public:
    // Constructor
    HC06();
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