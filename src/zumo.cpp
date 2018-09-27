#include "zumo.hpp"
#include <Zumo32U4.h>

template <class C>
String Zumo<C>::generateReply(ZumoReply reply, int payload[], size_t size) {
  String msg;
  
  // Set tag
  if (reply == ZumoReply::ACCELEROMETER)  { msg.concat("AC"); } else
  if (reply == ZumoReply::ACKNOWLEDGE)    { msg.concat("ACK"); } else
  if (reply == ZumoReply::BATTERY)        { msg.concat("BT"); } else
  if (reply == ZumoReply::COMPASS)        { msg.concat("CP"); } else
  if (reply == ZumoReply::DISTANCE_FRONT) { msg.concat("DF"); } else
  if (reply == ZumoReply::DISTANCE_LEFT)  { msg.concat("DL"); } else
  if (reply == ZumoReply::DISTANCE_RIGHT) { msg.concat("DR"); } else
  if (reply == ZumoReply::ERROR)          { msg.concat("ER"); } else
  if (reply == ZumoReply::GYROSCOPE)      { msg.concat("GY"); } else
  if (reply == ZumoReply::LINESENSOR)     { msg.concat("LS"); } else
  if (reply == ZumoReply::BUZZER)         { msg.concat("BZ"); } else
  if (reply == ZumoReply::UNKNOWN)        { msg.concat("UK"); }

  // Add payload
  for (unsigned int i = 0; i < size; i++) {
    msg.concat('|');
    msg.concat(payload[i]);
  }

  // Add linefeed
  msg.concat('\n');

  #ifdef ZUMO_DEBUG_SERIAL
  ZUMO_DEBUG_SERIAL.print("Crafted message: ");
  ZUMO_DEBUG_SERIAL.print(msg);
  #endif

  return msg;
}

template <class C>
int Zumo<C>::generateErrorCode(ZumoError e) {
  if (e == ZumoError::UNKNOWN_ERROR)               { return 1; } else
  if (e == ZumoError::PARAMETER_IS_LESS_THAN_ZERO) { return 2; }
}

template <class C>
void Zumo<C>::parseCommand(String cmd) {
  if (cmd.indexOf('\r') != -1)
    cmd.remove(cmd.indexOf('\r'));
  
  String tag = cmd.substring(0, cmd.indexOf(cmd.indexOf('|') != -1 ? '|' : '\n'));

  String payload = cmd.substring(cmd.indexOf('|') + 1); //Ignore first |
  #ifdef ZUMO_DEBUG_SERIAL
    ZUMO_DEBUG_SERIAL.print("Payload string: ");
    ZUMO_DEBUG_SERIAL.println(payload);
  #endif

  unsigned int payloadCount = 0;
  for ( unsigned int i = 0; i < cmd.length(); i++ )
    if (cmd.charAt(i) == '|')
      payloadCount++;

  #ifdef ZUMO_DEBUG_SERIAL
    ZUMO_DEBUG_SERIAL.print("Found ");
    ZUMO_DEBUG_SERIAL.print(payloadCount);
    ZUMO_DEBUG_SERIAL.println(" data segment/s");
  #endif

  long int payloadData[payloadCount];

  #ifdef ZUMO_DEBUG_SERIAL
    String payloadDebug[payloadCount];
  #endif

  for ( unsigned int i = 0; i < payloadCount; i++ ) {
    if (payload.indexOf('|') == -1) {
      payloadData[i] = payload.toInt();

      #ifdef ZUMO_DEBUG_SERIAL
        payloadDebug[i] = payload;
      #endif
    } else {
      payloadData[i] = payload.substring(0, payload.indexOf('|')).toInt();

      #ifdef ZUMO_DEBUG_SERIAL
        payloadDebug[i] = payload.substring(0, payload.indexOf('|'));
      #endif

      payload = payload.substring(payload.indexOf('|') + 1);
    }
  }

  #ifdef ZUMO_DEBUG_SERIAL
    for ( unsigned int i = 0; i < payloadCount; i++ ) {
      ZUMO_DEBUG_SERIAL.print("Payload[");
      ZUMO_DEBUG_SERIAL.print(i);
      ZUMO_DEBUG_SERIAL.print("] = ");
      ZUMO_DEBUG_SERIAL.print(payloadData[i]);
      ZUMO_DEBUG_SERIAL.print(" parsed from ");
      ZUMO_DEBUG_SERIAL.println(payloadDebug[i]);
    }
  #endif

  executeCommand(tag, payloadData, payloadCount);
}

template <class C>
void Zumo<C>::executeCommand(String tag, long int payloadData[], unsigned int payloadCount) {
  // Dispatch
  if (tag.equals("PM")) {
  } else if (tag.equals("BZ")) {
    // Override too low frequencies
    if (payloadData[0] < 31)
      payloadData[0] = 31;
    if (payloadData[1] <= 0) {
      ZumoConnection.send(this->generateReply(ZumoReply::ERROR, generateErrorCode(ZumoError::PARAMETER_IS_LESS_THAN_ZERO), 1));
    } else {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      unsigned int frequency = payloadData[0];
      unsigned int duration = payloadData[1];
      Zumo32U4Buzzer::playFrequency(frequency, duration, 15);
      delay(duration);
      ZumoConnection.send(this->generateReply(ZumoReply::BUZZER, {}, 0));
    }
  } else if (tag.equals("DF")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    Zumo32U4ProximitySensors sensors;
    sensors.initThreeSensors();
    sensors.read();
    int readings[2] = { sensors.countsFrontWithLeftLeds(), sensors.countsFrontWithRightLeds() };
    ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_FRONT, readings, 2));
  } else if (tag.equals("DL")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    Zumo32U4ProximitySensors sensors;
    sensors.initThreeSensors();
    sensors.read();
    int readings[2] = { sensors.countsLeftWithLeftLeds(), sensors.countsLeftWithRightLeds() };
    ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_LEFT, readings, 2));
  } else if (tag.equals("DR")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    Zumo32U4ProximitySensors sensors;
    sensors.initThreeSensors();
    sensors.read();
    int readings[2] = { sensors.countsRightWithLeftLeds(), sensors.countsRightWithRightLeds() };
    ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_RIGHT, readings, 2));
  } else if (tag.equals("AC")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    LSM303 accelerometer;
    accelerometer.init();
    accelerometer.read();
    int readings[6] = {
      accelerometer.a.x,
      accelerometer.a.y,
      accelerometer.a.z
    };
    ZumoConnection.send(this->generateReply(ZumoReply::ACCELEROMETER, readings, 3));
  } else if (tag.equals("GY")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    L3G gyro;
    gyro.init();
    gyro.read();
    int readings[3] = {
      gyro.g.x,
      gyro.g.y,
      gyro.g.z
    };
    ZumoConnection.send(this->generateReply(ZumoReply::GYROSCOPE, readings, 3));
  } else if (tag.equals("CP")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    LSM303 accelerometer;
    accelerometer.init();
    accelerometer.read();
    int bearing = int(accelerometer.heading());
    ZumoConnection.send(this->generateReply(ZumoReply::COMPASS, &bearing, 1));
  } else if (tag.equals("LS")) {
    Zumo32U4LineSensors sensors;
    sensors.initFiveSensors();
    unsigned int readings[5];
    sensors.read(readings);
    int readingsInt[5] = {
      int(readings[0]),
      int(readings[1]),
      int(readings[2]),
      int(readings[3]),
      int(readings[4]),
    };
    ZumoConnection.send(this->generateReply(ZumoReply::LINESENSOR, readingsInt, 5));
  } else if (tag.equals("RM")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    this->rebootMicrocontroller();
  } else if (tag.equals("BT")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    int battery[2] = {
      int(readBatteryMillivolts()),
      int(usbPowerPresent()),
    };
    ZumoConnection.send(this->generateReply(ZumoReply::BATTERY, battery, 2));
  } else {
    #ifdef ZUMO_DEBUG_SERIAL
    ZUMO_DEBUG_SERIAL.print("Unknown tag ");
    ZUMO_DEBUG_SERIAL.println(tag);
    #endif

    ZumoConnection.send(this->generateReply(ZumoReply::UNKNOWN, {}, 0));
  }
}

/*
template <class C>
void Zumo<C>::parseCommand(String cmd) {
  // Parse the tag
  if (cmd.indexOf('|') != -1) {
    // There is at least one |

    // Parse the tag
    String tag = cmd.substring(0, cmd.indexOf('|'));
    
    // Parse the payload
    String payload = cmd.substring(cmd.indexOf('|'));
    #ifdef ZUMO_DEBUG_SERIAL
    ZUMO_DEBUG_SERIAL.print("Payload string: ");
    ZUMO_DEBUG_SERIAL.println(payload);
    #endif
    unsigned int payloadCount = 0;
    for ( unsigned int i = 0; i < payload.length(); i++ )
      if (payload.charAt(i) == '|')
        payloadCount++;
    #ifdef ZUMO_DEBUG_SERIAL
    ZUMO_DEBUG_SERIAL.print("Found ");
    ZUMO_DEBUG_SERIAL.print(payloadCount);
    ZUMO_DEBUG_SERIAL.println(" data segment/s");
    #endif
    long int payloadData[payloadCount];
    #ifdef ZUMO_DEBUG_SERIAL
    String payloadDebug[payloadCount];
    #endif
    for ( unsigned int i = 0; i < payloadCount; i++ )
      payloadData[i] = 0;
    for ( unsigned int i = 0; i < payloadCount; i++ ) {
      // Remove | header
      if (payload.charAt(0) == '|')
        payload = payload.substring(1);
      // Select substring
      if (payload.indexOf('|') == -1) {
        payloadData[i] = payload.toInt();
        #ifdef ZUMO_DEBUG_SERIAL
        payloadDebug[i] = payload;
        #endif
      } else {
        payloadData[i] = payload.substring(0, payload.indexOf('|')).toInt();
        #ifdef ZUMO_DEBUG_SERIAL
        payloadDebug[i] = payload.substring(0, payload.indexOf('|'));
        #endif
        payload = payload.substring(payload.indexOf('|'));
      }
    }
    #ifdef ZUMO_DEBUG_SERIAL
    for ( unsigned int i = 0; i < payloadCount; i++ ) {
      ZUMO_DEBUG_SERIAL.print("Payload[");
      ZUMO_DEBUG_SERIAL.print(i);
      ZUMO_DEBUG_SERIAL.print("] = ");
      ZUMO_DEBUG_SERIAL.print(payloadData[i]);
      ZUMO_DEBUG_SERIAL.print(" parsed from ");
      ZUMO_DEBUG_SERIAL.println(payloadDebug[i]);
    }
    #endif
    // Dispatch
    if (tag.equals("PM")) {
    } else if (tag.equals("BZ")) {
      // Override too low frequencies
      if (payloadData[0] < 31)
        payloadData[0] = 31;
      if (payloadData[1] <= 0) {
        ZumoConnection.send(this->generateReply(ZumoReply::ERROR, generateErrorCode(ZumoError::PARAMETER_IS_LESS_THAN_ZERO), 1));
      } else {
        ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
        unsigned int frequency = payloadData[0];
        unsigned int duration = payloadData[1];
        Zumo32U4Buzzer::playFrequency(frequency, duration, 15);
      }
    } else if (tag.equals("DF")) {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      Zumo32U4ProximitySensors sensors;
      sensors.initThreeSensors();
      sensors.read();
      int readings[2] = { sensors.countsFrontWithLeftLeds(), sensors.countsFrontWithRightLeds() };
      ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_FRONT, readings, 2));
    } else if (tag.equals("DL")) {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      Zumo32U4ProximitySensors sensors;
      sensors.initThreeSensors();
      sensors.read();
      int readings[2] = { sensors.countsLeftWithLeftLeds(), sensors.countsLeftWithRightLeds() };
      ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_LEFT, readings, 2));
    } else if (tag.equals("DR")) {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      Zumo32U4ProximitySensors sensors;
      sensors.initThreeSensors();
      sensors.read();
      int readings[2] = { sensors.countsRightWithLeftLeds(), sensors.countsRightWithRightLeds() };
      ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_RIGHT, readings, 2));
    } else if (tag.equals("AC")) {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      LSM303 accelerometer;
      accelerometer.init();
      accelerometer.read();
      int readings[6] = {
        accelerometer.a.x,
        accelerometer.a.y,
        accelerometer.a.z
      };
      ZumoConnection.send(this->generateReply(ZumoReply::ACCELEROMETER, readings, 3));
    } else if (tag.equals("GY")) {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      L3G gyro;
      gyro.init();
      gyro.read();
      int readings[3] = {
        gyro.g.x,
        gyro.g.y,
        gyro.g.z
      };
      ZumoConnection.send(this->generateReply(ZumoReply::GYROSCOPE, readings, 3));
    } else if (tag.equals("CP")) {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      LSM303 accelerometer;
      accelerometer.init();
      accelerometer.read();
      int bearing = int(accelerometer.heading());
      ZumoConnection.send(this->generateReply(ZumoReply::COMPASS, &bearing, 1));
    } else if (tag.equals("LS")) {
      Zumo32U4LineSensors sensors;
      sensors.initFiveSensors();
      unsigned int readings[5];
      sensors.read(readings);
      int readingsInt[5] = {
        int(readings[0]),
        int(readings[1]),
        int(readings[2]),
        int(readings[3]),
        int(readings[4]),
      };
      ZumoConnection.send(this->generateReply(ZumoReply::LINESENSOR, readingsInt, 5));
    } else if (tag.equals("RM")) {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      this->rebootMicrocontroller();
    } else if (tag.equals("BT")) {
      ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
      int battery[2] = {
        int(readBatteryMillivolts()),
        int(usbPowerPresent()),
      };
      ZumoConnection.send(this->generateReply(ZumoReply::BATTERY, battery, 2));
    } else {
      #ifdef ZUMO_DEBUG_SERIAL
      ZUMO_DEBUG_SERIAL.print("Unknown tag ");
      ZUMO_DEBUG_SERIAL.println(tag);
      #endif

      ZumoConnection.send(this->generateReply(ZumoReply::UNKNOWN, {}, 0));
    }
  } else {
    // There is not a |
    #ifdef ZUMO_DEBUG_SERIAL
    ZUMO_DEBUG_SERIAL.println("No separator was present in the message");
    #endif

    ZumoConnection.send(this->generateReply(ZumoReply::ERROR, generateErrorCode(ZumoError::MISSING_TAG), 1));
  }
}
*/

template <class C>
void Zumo<C>::rebootMicrocontroller() {
  void (*zero)(void) = 0;
  zero();
}

template <class C>
void Zumo<C>::eventLoop() {
  if (ZumoConnection.available() > 0) {
    ledYellow(true);
    this->parseCommand(ZumoConnection.readLine());
    ledYellow(false);
  }
}

template <class C>
bool Zumo<C>::connectBluetooth(String pin, String name, unsigned int baudrate) {
  ledGreen(false); ledRed(true);
  bool connStatus = ZumoConnection.connect(pin, name, baudrate);
  if (connStatus) {
    ledGreen(true); ledRed(false);
    delay(1000);
    ledGreen(false);
  }
  return connStatus;
}