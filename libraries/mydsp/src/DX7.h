#ifndef DX7_H_INCLUDED
#define DX7_H_INCLUDED

#include "SineTable.h"
#include "Phasor.h"
#include "Am.h"

class DX7{
public:
  DX7(int SR);
  
  void setFrequency(float f);
  void setGain(float g);
  float tick();
private:
  SineTable sineTable;
  Phasor phasor;
  Am am1, am2;
  float gain;
  int samplingRate;
};

#endif  // SINE_H_INCLUDED