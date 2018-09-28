#pragma once

#include <Zumo32U4.h>

class Rotation {
  private:
  float angle = 0;
  Zumo32U4Motors motors;
  L3G gyro;
  int16_t turnRate;
  int16_t gyroOffset;
  uint16_t gyroLastUpdate = 0;
  int32_t turnAngle45 = 0x20000000;
  int32_t turnAngle1 = (turnAngle45 + 22) / 45;

  public:
  Rotation ();
  void update ();
  void reset ();
  void rotate (int angle, int speed);
  void stop ();
};