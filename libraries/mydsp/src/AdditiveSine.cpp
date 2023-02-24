#include <cmath>

#include "AdditiveSine.h"

#define SINE_TABLE_SIZE 16384

AdditiveSine::AdditiveSine(int SR) : 
sineTable(SINE_TABLE_SIZE),
phasor(SR),
gain(1.0),
samplingRate(SR){}

void AdditiveSine::setFrequency(float f){
  phasor.setFrequency(f);
}
    
void AdditiveSine::setGain(float g){
  gain = g;
}
    
float AdditiveSine::tick(){
  int index1 = phasor.tick()*SINE_TABLE_SIZE;
  int index2 = (int) (index1 * 1.5) % SINE_TABLE_SIZE;
  return (sineTable.tick(index1) + sineTable.tick(index2))*gain * 0.5; //multiplying is more efficient than dividing
}
