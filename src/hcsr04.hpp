#pragma once

class HCSR04 {
  public:
  HCSR04 ();
  void init ();
  int distanceCM ();

  private:
  int trigPin;
  int echoPin;
};