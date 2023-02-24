#include <Audio.h>
#include "myDsp.h"

const int MIDI_RANGE = 128;

myDsp dsp;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(dsp, 0, out, 0);
AudioConnection patchCord1(dsp, 0, out, 1);
bool playing;
int numberOfNotes;
int currentNoteOrder;
int playingNote[MIDI_RANGE];
char freq1[10] = "freq1";
char freq2[10] = "freq2";
char freq3[10] = "freq3";
char freq4[10] = "freq4";
char gate1[10] = "gate1";
char gate2[10] = "gate2";
char gate3[10] = "gate3";
char gate4[10] = "gate4";
char *freqs[4];
char *gates[4];

void setup()
{
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.1);
  playing = false;
  numberOfNotes = 0;
  currentNoteOrder = 0;
  Serial.begin(9600);
  freqs[0] = freq1;
  freqs[1] = freq2;
  freqs[2] = freq3;
  freqs[3] = freq4;
  gates[0] = gate1;
  gates[1] = gate2;
  gates[2] = gate3;
  gates[3] = gate4;
  for (int i = 0; i < MIDI_RANGE; i++) {
    playingNote[i] = 0;
  }
}

void loop()
{
  if (usbMIDI.read())
  {
    // Serial.println("reading");
    processMIDI();
  }
  playing = numberOfNotes > 0 ? true : false;
  float gateValue = playing ? 1.0 : 0.0;
  dsp.setParamValue("gate", gateValue);
  // Serial.println(numberOfNotes);
  // delay(50);
}

void processMIDI(void)
{
  byte type, channel, data1, data2, cable;

  // fetch the MIDI message, defined by these 5 numbers (except SysEX)
  //
  type = usbMIDI.getType();       // which MIDI message, 128-255
  channel = usbMIDI.getChannel(); // which MIDI channel, 1-16
  data1 = usbMIDI.getData1();     // first data byte of message, 0-127
  data2 = usbMIDI.getData2();     // second data byte of message, 0-127
  cable = usbMIDI.getCable();     // which virtual cable with MIDIx8, 0-7
  // uncomment if using multiple virtual cables
  // Serial.print("cable ");
  // Serial.print(cable, DEC);
  // Serial.print(": ");

  // print info about the message
  //

  if (type == usbMIDI.NoteOff)
  {
    Serial.println("Note off");
    int midiNote = int(data1);
    int currentNote = playingNote[midiNote];
    Serial.println(currentNote);
    dsp.setParamValue(gates[currentNote], 0.0);
    playingNote[midiNote] = 0;
    numberOfNotes--;
  }
  else if (type == usbMIDI.NoteOn)
  {
    Serial.println("Note on");
    int midiNote = int(data1);
    float noteFrequency = getFrequencyFromMidi(midiNote);
    dsp.setParamValue(freqs[currentNoteOrder], noteFrequency);
    dsp.setParamValue(gates[currentNoteOrder], 1.0);
    playingNote[midiNote] = (currentNoteOrder + 1) % 4;
    numberOfNotes++;
    currentNoteOrder = (currentNoteOrder + 1) % 4;
    Serial.println(currentNoteOrder);
  }
  else if (type == usbMIDI.ControlChange)
  {
    Serial.println("Control change ");
    int control = int(data1);
    int inputValue = int(data2);
    float outputValue;
    // switch (control)
    // {
    // case 74:
    //   outputValue = mapfloat(inputValue, 0, 127, 50, 5000);
    //   dsp.setParamValue("ctfreq", outputValue);
    //   break;
    // case 17:
    //   outputValue = mapfloat(inputValue, 0, 127, 0, 1);
    //   test.setParamValue("gain", outputValue);
    //   break;

    // default:
    //   break;
    // }
  }

}

float getFrequencyFromMidi(float d)
{
  float exponent = (d - 69.0) / 12.0;
  return pow(2.0, exponent) * 440;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
