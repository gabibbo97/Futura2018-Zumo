#include <Zumo32U4.h>
#include <PID_v1.h>

#include "Movement.hpp"

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

void Movement::move (int distance, float speed) {
  this->power = speed;

  while (true) {
    update();
    delay(100);
  }
}