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
  //Serial.begin(9600);
}

void loop() {
  // int buttonPressed = digitalRead(0);
  // if (buttonPressed)  {
  //   myDsp.setGain(1);
  // } else {
  //   myDsp.setGain(0);
  // }
  float freq = analogRead(A0) * 100;
  myDsp.setGain(3);
  myDsp.setFreq(random(50,1000));
  delay(500);
}
