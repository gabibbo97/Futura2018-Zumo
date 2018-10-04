#include <Wire.h>
#include <Zumo32U4.h>
#include <PID_v1.h>

#include "movement.hpp"

Movement::Movement () {
}

void Movement::reset () {
  if (this->pPID != NULL)
    free(this->pPID);
  
  this->stop();
  this->pPID = new PID(&(this->Input), &(this->Output), &(this->Setpoint), 0.05,0.03,0.03, DIRECT);

  this->pPID->SetMode(AUTOMATIC);
  this->pPID->SetOutputLimits(-20, 20);
  
  this->encoders.getCountsAndResetLeft();
  this->encoders.getCountsAndResetRight();
}

void Movement::update () {
  if (this->power == 0)
    return;

  int16_t countsLeft = this->encoders.getCountsLeft();
  int16_t countsRight = this->encoders.getCountsRight();

  this->Input = (countsRight - this->encLeft) - (countsLeft - this->encRight);

  this->Setpoint = 0;
  this->pPID->Compute();

  this->encLeft = countsLeft;
  this->encRight = countsRight;
  
  this->motors.setLeftSpeed(this->power - Output/2);
  this->motors.setRightSpeed(this->power + Output/2 + (int)((int)Output^2)/400);
}

void Movement::forward (int speed) {
  this->reset();

  this->power = speed;

  this->motors.setSpeeds(speed, speed);
  delay(25);
}

void Movement::backward (int speed) {
  this->reset();

  this->power = -speed;

  this->motors.setSpeeds(speed, speed);
  delay(25);
}

void Movement::move (int distance, int speed) {
  this->reset();

  if (distance < 0) {
    distance = -distance;
    speed = -speed;
  }

  this->motors.setSpeeds(speed, speed);
  delay(25);

  int encDistance = distance*75;

  this->power = speed;

  while (true) {
    this->update();
    if (speed > 0 && (this->encLeft+this->encRight)/2 > encDistance)
      break;
    if (speed < 0 && (this->encLeft+this->encRight)/2 < -encDistance)
      break;
    delay(200);
  }

  this->stop();
}

void Movement::stop () {
  this->power = 0;
  this->motors.setSpeeds(0, 0);
}