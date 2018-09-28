#include <Zumo32U4.h>
#include <PID_v1.h>

#include "movement.hpp"

Movement::Movement () {
  this->pPID = new PID(&(this->Input), &(this->Output), &(this->Setpoint), 0.8,0.7,0.05, DIRECT);
  this->pPID->SetMode(AUTOMATIC);
  this->pPID->SetOutputLimits(-20, 20);
}

void Movement::update () {
  int16_t countsLeft = this->encoders.getCountsLeft();
  int16_t countsRight = this->encoders.getCountsRight();

  this->Input = (countsLeft - this->encLeft) - (countsRight - this->encRight);

  this->pPID->Compute();

  this->encLeft = countsLeft;
  this->encRight = countsRight;

  this->motors.setLeftSpeed(this->power + Output);
  this->motors.setRightSpeed(this->power);
}

void Movement::forward (int power) {
  //Used for InfiniteMovement, manuale trigger of update
}

void Movement::move (int distance, int speed) {
  if (distance < 0) {
    distance = -distance;
    speed = -speed;
  }

  this->power = speed;

  int encDistance = distance*75;

  while (true) {
    this->update();
    if ((this->encLeft+this->encRight)/2 > encDistance)
      break;
    delay(100);
  }

  this->stop();
}

void Movement::stop () {
  this->motors.setSpeeds(0, 0);
}