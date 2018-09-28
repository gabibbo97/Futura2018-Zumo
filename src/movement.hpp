#pragma once

#include <Zumo32U4.h>
#include <PID_v1.h>

class Movement {
  private:
  int power;
  double Input, Output, Setpoint;
  Zumo32U4Motors motors;
  Zumo32U4Encoders encoders;
  int16_t encLeft = 0, encRight = 0;
  PID* pPID;

  public:
  Movement ();
  void update ();
  void forward (int power);
  void move (int distance, int speed);
  void stop ();
};