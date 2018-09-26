#include <Arduino.h>

#include <Wire.h>

// Comment out to disable debugging
#define ZUMO_DEBUG_SERIAL Serial

enum class ZumoError {
  UNKNOWN_ERROR,
  PARAMETER_IS_LESS_THAN_ZERO
};

enum class ZumoReply {
  ACCELEROMETER,
  ACKNOWLEDGE,
  BATTERY,
  COMPASS,
  DISTANCE_FRONT,
  DISTANCE_LEFT,
  DISTANCE_RIGHT,
  ERROR,
  GYROSCOPE,
  LINESENSOR,
  UNKNOWN,
};

template <class C>
class Zumo {
  private:
    String generateReply(ZumoReply reply, int payload[], size_t size);
    int generateErrorCode(ZumoError e);
    C ZumoConnection;
  public:
    Zumo();
    // CONNECTION
    void parseCommand(String cmd);
    void executeCommand(String tag, long int payloadData[], unsigned int payloadCount);
    void rebootMicrocontroller();
    bool connectBluetooth(String pin, String name, unsigned int baudrate);
    void eventLoop();
    // MOVEMENT
    // SENSORS

};

template <class C>
Zumo<C>::Zumo() {};
