#include <Wire.h>
#include <Zumo32U4.h>

#include "rotation.hpp"

//#define ZUMO_DEBUG_SERIAL Serial

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

void Rotation::update () {
  this->gyro.read();
  this->turnRate = this->gyro.g.z - this->gyroOffset;

  // Figure out how much time has passed since the last update (dt)
  uint16_t m = micros();
  uint16_t dt = m - this->gyroLastUpdate;
  this->gyroLastUpdate = m;

  // Multiply dt by turnRate in order to get an estimation of how
  // much the robot has turned since the last update.
  // (angular change = angular velocity * time)
  int32_t d = (int32_t)(this->turnRate) * dt;

  // The units of d are gyro digits times microseconds.  We need
  // to convert those to the units of turnAngle, where 2^29 units
  // represents 45 degrees.  The conversion from gyro digits to
  // degrees per second (dps) is determined by the sensitivity of
  // the gyro: 0.07 degrees per second per digit.
  //
  // (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
  // = 14680064/17578125 unit/(digit*us)
  this->angle -= (int64_t)d * ((float)14680064 / 17578125) / this->turnAngle1;
}

void Rotation::rotate (int angle, int speed) {
  this->reset();

  if (angle < 0) {
    angle = -angle;
    speed = -speed;
  }

  this->power = speed;

  this->motors.setLeftSpeed(speed);
  this->motors.setRightSpeed(-speed);

  while (true) {
    this->update();
    Serial.println(this->angle);
    if (angle < 0 && (int32_t)this->angle < -angle)
      break;
    if (angle > 0 && (int32_t)this->angle > angle)
      break;
  }

  this->stop();
}

void Rotation::stop () {
  this->motors.setSpeeds(0, 0);
}