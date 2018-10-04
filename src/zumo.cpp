#include "zumo.hpp"
#include <Zumo32U4.h>

template <class C>
String Zumo<C>::generateReply(ZumoReply reply, int payload[], size_t size) {
  String msg;
  
  // Set tag
  if (reply == ZumoReply::ACCELEROMETER)  { msg += "AC";  } else
  if (reply == ZumoReply::ACKNOWLEDGE)    { msg += "ACK"; } else
  if (reply == ZumoReply::BATTERY)        { msg += "BT";  } else
  if (reply == ZumoReply::COMPASS)        { msg += "CP";  } else
  if (reply == ZumoReply::DISTANCE_FRONT) { msg += "DF";  } else
  if (reply == ZumoReply::DISTANCE_LEFT)  { msg += "DL";  } else
  if (reply == ZumoReply::DISTANCE_RIGHT) { msg += "DR";  } else
  if (reply == ZumoReply::ERROR)          { msg += "ER";  } else
  if (reply == ZumoReply::GYROSCOPE)      { msg += "GY";  } else
  if (reply == ZumoReply::LINESENSOR)     { msg += "LS";  } else
  if (reply == ZumoReply::BUZZER)         { msg += "BZ";  } else
  if (reply == ZumoReply::MOVE_END)       { msg += "ME";  } else
  if (reply == ZumoReply::UNKNOWN)        { msg += "UK";  }

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
  if (e == ZumoError::PARAMETER_IS_LESS_THAN_ZERO) { return 2; } else
  if (e == ZumoError::NOT_ENOUGH_ARGUMENTS)        { return 3; }
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
void Zumo<C>::checkSpeed (long int &speed) {
  if (speed < 100)
    speed = 100;
  else if (speed > 350)
    speed = 350;
}

template <class C>
void Zumo<C>::stop () {
  this->movement.stop();
  this->rotation.stop();
}

template <class C>
void Zumo<C>::executeCommand (String tag, long int payloadData[], unsigned int payloadCount) {
  // Dispatch
  if (tag.equals("PM")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    checkSpeed(payloadData[1]);
    this->stop();
    this->movement.move(payloadData[0], payloadData[1]);
    ZumoConnection.send(this->generateReply(ZumoReply::MOVE_END, {}, 0));
  } else if (tag.equals("MF")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    checkSpeed(payloadData[0]);
    this->stop();
    this->movement.forward(payloadData[0]);
  } else if (tag.equals("MB")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    checkSpeed(payloadData[0]);
    this->stop();
    this->movement.backward(payloadData[0]);
  } else if (tag.equals("SS")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    this->stop();
  } else if (tag.equals("PR")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    checkSpeed(payloadData[1]);
    this->stop();
    this->rotation.rotate(payloadData[0], payloadData[1]);
    ZumoConnection.send(this->generateReply(ZumoReply::MOVE_END, {}, 0));
  } else if (tag.equals("RL")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    checkSpeed(payloadData[0]);
    this->stop();
    this->rotation.counterclockwise(payloadData[0]);
  } else if (tag.equals("RR")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    checkSpeed(payloadData[0]);
    this->stop();
    this->rotation.clockwise(payloadData[0]);
  } else if (tag.equals("BZ")) {
    // Check arguments
    if (payloadCount < 2) {
      ZumoConnection.send(this->generateReply(ZumoReply::ERROR, generateErrorCode(ZumoError::NOT_ENOUGH_ARGUMENTS), 1));
      return;
    }
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
    int distance = this->distance.distanceCM();
    int d[1] = {distance};
    ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_FRONT, d, 1));
  /*} else if (tag.equals("DL")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    this->proximity_sensors.read();
    int readings[2] = { this->proximity_sensors.countsLeftWithLeftLeds(), this->proximity_sensors.countsLeftWithRightLeds() };
    ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_LEFT, readings, 2));
  } else if (tag.equals("DR")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    this->proximity_sensors.read();
    int readings[2] = { this->proximity_sensors.countsRightWithLeftLeds(), this->proximity_sensors.countsRightWithRightLeds() };
    ZumoConnection.send(this->generateReply(ZumoReply::DISTANCE_RIGHT, readings, 2));
  */} else if (tag.equals("AC")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    this->accelerometer.read();
    int readings[6] = {
      this->accelerometer.a.x,
      this->accelerometer.a.y,
      this->accelerometer.a.z
    };
    ZumoConnection.send(this->generateReply(ZumoReply::ACCELEROMETER, readings, 3));
  } else if (tag.equals("GY")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    this->gyro.read();
    int readings[3] = {
      this->gyro.g.x,
      this->gyro.g.y,
      this->gyro.g.z
    };
    ZumoConnection.send(this->generateReply(ZumoReply::GYROSCOPE, readings, 3));
  } else if (tag.equals("CP")) {
    ZumoConnection.send(this->generateReply(ZumoReply::ACKNOWLEDGE, {}, 0));
    this->accelerometer.read();
    int bearing = int(this->accelerometer.heading());
    ZumoConnection.send(this->generateReply(ZumoReply::COMPASS, &bearing, 1));
  } else if (tag.equals("LS")) {
    unsigned int readings[3];
    this->line_sensors.read(readings);
    int readingsInt[3] = {
      int(readings[0]),
      int(readings[1]),
      int(readings[2])
    };
    ZumoConnection.send(this->generateReply(ZumoReply::LINESENSOR, readingsInt, 3));
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

template <class C>
void Zumo<C>::init() {
  Wire.begin();
  /*
  this->proximity_sensors.initThreeSensors();
  uint16_t levels[] = { 2, 3, 10, 15, 30, 50 };
  this->proximity_sensors.setBrightnessLevels(levels, sizeof(levels)/2);
  */
  this->distance.init();
  this->line_sensors.initThreeSensors();
  this->accelerometer.init();
  this->accelerometer.enableDefault();
  this->gyro.init();
}

template <class C>
void Zumo<C>::rebootMicrocontroller() {
  void (*zero)(void) = 0;
  zero();
}

template <class C>
void Zumo<C>::eventLoop() {
  if (ZumoConnection.available() > 0) {
    ledRed(true);
    this->parseCommand(ZumoConnection.readLine());
    ledRed(false);
  }

  this->movement.update();

  delay(100);
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
