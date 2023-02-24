#include <cmath>

#include "DX7.h"

#define SINE_TABLE_SIZE 16384

DX7::DX7(int SR) : 
sineTable(SINE_TABLE_SIZE),
phasor(SR),
gain(1.0),
samplingRate(SR),
am1(SR),
am2(SR)
{
  am2.setCFreq(440);
}

void DX7::setFrequency(float f){
  am1.setCFreq(f);
}
    
void DX7::setGain(float g){
  gain = g;
}
    
float DX7::tick(){
  int index1 = am1.tick();
  int index2 = am2.tick();
  return (index1 + index2)*gain * 0.5; //multiplying is more efficient than dividing
}
