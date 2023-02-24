#include <Audio.h>
#include "MyDsp.h"

MyDsp myDsp;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(myDsp,0,out,0);
AudioConnection patchCord1(myDsp,1,out,1);

void setup() {
  Serial.begin(9600);
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.5);
}

void loop() {
  Serial.println("running");
  myDsp.setGain(1);
  myDsp.setFreq(440);
  delay(1000);
  myDsp.setFreq(261);
  delay(1000);  
}

float getFrequencyFromMidi(float d) {
  float exponent = (d - 69.0) / 12.0;
  return pow(2.0, exponent) * 440;
}
