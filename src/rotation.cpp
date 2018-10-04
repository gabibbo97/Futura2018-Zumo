#include <Wire.h>
#include <Zumo32U4.h>

#include "rotation.hpp"

Rotation::Rotation () {
  Wire.begin();
  this->gyro.init();

  this->gyro.writeReg(L3G::CTRL1, 0b11111111);

  // 2000 dps full scale
  this->gyro.writeReg(L3G::CTRL4, 0b00100000);

  // High-pass filter disabled
  this->gyro.writeReg(L3G::CTRL5, 0b00000000);

  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    // Wait for new data to be available, then read it.
    while(!this->gyro.readReg(L3G::STATUS_REG) & 0x08);
    this->gyro.read();

    // Add the Z axis reading to the total.
    total += this->gyro.g.z;
  }
  this->gyroOffset = total / 1024;
}

void Rotation::reset () {
  this->gyroLastUpdate = micros();
  this->angle = 0;
}

void Rotation::counterclockwise (int speed) {
  this->reset();

  this->motors.setSpeeds(-speed, speed);
}

void Rotation::clockwise (int speed) {
  this->reset();
  
  this->motors.setSpeeds(speed, -speed);
}

void Rotation::update () {
  this->gyro.read();
  this->turnRate = this->gyro.g.z - this->gyroOffset;

  uint16_t m = micros();
  uint16_t dt = m - this->gyroLastUpdate;
  this->gyroLastUpdate = m;

  int32_t d = (int32_t)(this->turnRate) * dt;
  this->angle -= (int64_t)d * ((float)14680064 / 17578125) / this->turnAngle1;
}

void Rotation::rotate (int angle, int speed) {
  //angle = angle - (speed/25);
  this->reset();

  if (angle < 0) {
    speed = -speed;
  }

  this->motors.setSpeeds(speed, -speed);

  while (true) {
    this->update();
    if (angle < 0 && this->angle < (float)angle)
      break;
    if (angle > 0 && this->angle > (float)angle)
      break;
  }

  this->stop();
}

void Rotation::stop () {
  this->motors.setSpeeds(0, 0);
}
