#include <Audio.h>
#include "MyDsp.h"

MyDsp myDsp;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(myDsp,0,out,0);
AudioConnection patchCord1(myDsp,0,out,1);

void setup() {
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.5);
  pinMode(0, INPUT);
  myDsp.setFreq(440);
  myDsp.setAmFreq(15);
  myDsp.setGain(1);
  //Serial.begin(9600);
}

void loop() {
  myDsp.setGain(1);
  // int buttonPressed = digitalRead(0);
  // if (buttonPressed)  {
  //   myDsp.setGain(1);
  // } else {
  //   myDsp.setGain(0);
  // }
  float freq = analogRead(A0) * 20 / 1024;
  myDsp.setAmFreq(freq);
  delay(1000);
  myDsp.setGain(0);
  delay(1000);
}
