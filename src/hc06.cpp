#include <Arduino.h>
#include "hc06.hpp"

#if HC06_SOFTWARE_SERIAL
#include <SoftwareSerial.h>
#endif

#if HC06_SOFTWARE_SERIAL
#define HC06_SERIAL_PORT HC06SwSerial
SoftwareSerial HC06_SERIAL_PORT(HC06_SOFTWARE_SERIAL_RX_PIN, HC06_SOFTWARE_SERIAL_TX_PIN);
#else
#define HC06_SERIAL_PORT Serial1
#endif

HC06::HC06() {}

void HC06::send(String s) {
  HC06_SERIAL_PORT.print(s);
}

String HC06::readLine(unsigned int timeout) {
  HC06_SERIAL_PORT.setTimeout(timeout);
  return HC06_SERIAL_PORT.readStringUntil('\n');
}

byte HC06::read() {
  while (HC06_SERIAL_PORT.available() < 1)
    continue;
  return HC06_SERIAL_PORT.read();
}

bool HC06::isAlive() {
  #ifdef HC06_DEBUG_SERIAL
  HC06_DEBUG_SERIAL.println("Sending health check");
  #endif

  // Send health check command
  HC06_SERIAL_PORT.write(getATcommand(HC06AtCommand::HEALTH_CHECK).c_str());

  // Wait for an "OK"
  char replyBuffer[3] = { 'E', 'R', '\0' };
  HC06_SERIAL_PORT.setTimeout(10000);
  HC06_SERIAL_PORT.readBytes(replyBuffer, 2);

  // Check if we received an "OK"
  if ((replyBuffer[0] == 'O') && (replyBuffer[1] == 'K')) {
    #ifdef HC06_DEBUG_SERIAL
    HC06_DEBUG_SERIAL.println("Connected");
    #endif

    return true;
  } else {
    #ifdef HC06_DEBUG_SERIAL
    HC06_DEBUG_SERIAL.print("Something else was received: ");
    HC06_DEBUG_SERIAL.println(replyBuffer);
    #endif

    delay(1000); // See manual
    return false;
  }
}

bool HC06::setPin(String pin) {
  #ifdef HC06_DEBUG_SERIAL
  HC06_DEBUG_SERIAL.println("Setting pin");
  #endif

  HC06_SERIAL_PORT.print(getATcommand(HC06AtCommand::SET_PIN).c_str());
  HC06_SERIAL_PORT.print(pin);

  HC06_SERIAL_PORT.setTimeout(10000);
  const char expectedReply[] = { 'O', 'K', 's', 'e', 't', 'P', 'I', 'N' };
  char reply[9]; reply[8] = '\0';
  HC06_SERIAL_PORT.readBytes(reply, 8);

  bool matches = true;
  for (unsigned int i = 0; i < 8; i++) {
    if (expectedReply[i] != reply[i]) {
      matches = false;
      break;
    }
  }

  if (matches) {
    #ifdef HC06_DEBUG_SERIAL
    HC06_DEBUG_SERIAL.println("Pin set");
    #endif

    return true;
  } else {
    #ifdef HC06_DEBUG_SERIAL
    HC06_DEBUG_SERIAL.print("Something else was received: ");
    HC06_DEBUG_SERIAL.println(reply);
    #endif

    delay(1000); // See manual
    return false;
  }
}

bool HC06::setName(String name) {
  #ifdef HC06_DEBUG_SERIAL
  HC06_DEBUG_SERIAL.println("Setting name");
  #endif

  HC06_SERIAL_PORT.print(getATcommand(HC06AtCommand::SET_NAME).c_str());
  HC06_SERIAL_PORT.print(name);
  
  HC06_SERIAL_PORT.setTimeout(10000);
  const char expectedReply[] = { 'O', 'K', 's', 'e', 't','n', 'a', 'm', 'e' };
  char reply[10]; reply[9] = '\0';
  HC06_SERIAL_PORT.readBytes(reply, 9);

  bool matches = true;
  for (unsigned int i = 0; i < 9; i++) {
    if (expectedReply[i] != reply[i]) {
      matches = false;
      break;
    }
  }

  if (matches) {
    #ifdef HC06_DEBUG_SERIAL
    HC06_DEBUG_SERIAL.println("Name set");
    #endif

    return true;
  } else {
    #ifdef HC06_DEBUG_SERIAL
    HC06_DEBUG_SERIAL.print("Something else was received: ");
    HC06_DEBUG_SERIAL.println(reply);
    #endif

    delay(1000); // See manual
    return false;
  }
}

bool HC06::connect(String pin, String name, unsigned int baudrate) {
  #ifdef HC06_DEBUG_SERIAL
  HC06_DEBUG_SERIAL.println("Starting serial with HC06");
  #endif
  HC06_SERIAL_PORT.begin(9600);

  while(! this->isAlive())
    continue;
  while(! this->setPin(pin))
    continue;
  while(! this->setName(name))
    continue;
 return true;
}

String HC06::getATcommand(HC06AtCommand c) {
  switch(c) {
    case HC06AtCommand::HEALTH_CHECK:
    {
      return String("AT");
      break;
    }
    case HC06AtCommand::SET_NAME:
    {
      return String("AT+NAME");
      break;
    }
    case HC06AtCommand::SET_PIN:
    {
      return String("AT+PIN");
      break;
    }
    case HC06AtCommand::VERSION:
    {
      return String("AT+VERSION");
      break;
    }
    default:
    {
      return String();
      break;
    }
  }
}

int HC06::available() {
  return HC06_SERIAL_PORT.available();
}