#ifndef ADDSINE_H_INCLUDED
#define ADDSINE_H_INCLUDED

#include "SineTable.h"
#include "Phasor.h"

class AdditiveSine{
public:
  AdditiveSine(int SR);
  
  void setFrequency(float f);
  void setGain(float g);
  float tick();
private:
  SineTable sineTable;
  Phasor phasor;
  float gain;
  int samplingRate;
};

#endif  // SINE_H_INCLUDED