#include <Arduino.h>
#include "hcsr04.hpp"

HCSR04::HCSR04 () {
  this->trigPin = 13;
  this->echoPin = 14;
}

void HCSR04::init () {
  pinMode(this->trigPin, OUTPUT);
  pinMode(this->echoPin, INPUT);
}

int HCSR04::distanceCM () {
  long duration;
  digitalWrite(this->trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(this->trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(this->trigPin, LOW);
  
  pinMode(this->echoPin, INPUT);
  duration = pulseIn(this->echoPin, HIGH);
 
  return (duration/2) / 29.1;
}
