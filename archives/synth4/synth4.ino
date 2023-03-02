#include <Audio.h>
#include "myDsp.h"

const int MIDI_RANGE = 128;
const int POLY_RANGE = 3;
const int RECORD_RANGE = 4800;
const int DELAY_VALUE = 5;

myDsp dsp;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(dsp, 0, out, 0);
AudioConnection patchCord1(dsp, 0, out, 1);
bool playing, polyphonic, loopPlaying;
int numberOfNotes;
int currentNoteOrder;
int loop_index;
int recording_size;
int playingNote[MIDI_RANGE];
char freq1[10] = "freq1";
char freq2[10] = "freq2";
char freq3[10] = "freq3";
char freq4[10] = "freq4";
char gate1[10] = "gate1";
char gate2[10] = "gate2";
char gate3[10] = "gate3";
char gate4[10] = "gate4";
char *freqs[POLY_RANGE];
char *gates[POLY_RANGE];
int record1[RECORD_RANGE];
int record2[RECORD_RANGE];
int instrumentsOnLoops[2] = {-1, -1};
int instrumentPlaying = -1;
byte *midiInfo;

void setup()
{
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.5);
  playing = false;
  loopPlaying = false;
  numberOfNotes = 0;
  currentNoteOrder = 0;
  loop_index = 0;
  Serial.begin(9600);
  freqs[0] = freq1;
  freqs[1] = freq2;
  freqs[2] = freq3;
  freqs[3] = freq4;
  gates[0] = gate1;
  gates[1] = gate2;
  gates[2] = gate3;
  gates[3] = gate4;
  polyphonic = true;
  recording_size = 0;
  for (int i = 0; i < MIDI_RANGE; i++)
  {
    playingNote[i] = -1;
  }
  for (int i = 0; i < RECORD_RANGE; i++)
  {
    record1[i] = -1;
  }
  dsp.setParamValue("gate", 1);
}

void loop()
{
  byte type, data1, data2;
  if (usbMIDI.read())
  {
    // Serial.println("reading");
    midiInfo = getMidiInput();
    type = midiInfo[0];
    data1 = midiInfo[1];
    data2 = midiInfo[2];
    processMIDI(type, data1, data2);
  }
  if (loopPlaying)
  {
    playLoop();
    loop_index = (loop_index + 1) % recording_size;
  }
  delay(DELAY_VALUE);
}

void processMIDI(byte type, byte data1, byte data2)
{
  if (type == usbMIDI.NoteOff)
  {
    // Serial.println("Note off");
    int midiNote = int(data1);
    int currentNote = playingNote[midiNote];
    Serial.println(currentNote);
    playingNote[midiNote] = -1;
    bool otherNote = false;
    if (!polyphonic)
    {
      for (int i = 0; i < MIDI_RANGE; i++)
      {
        if (playingNote[i] >= 0)
          otherNote = true;
      }
      if (!otherNote)
      {
        dsp.setParamValue(gates[currentNote], 0);
      }
    }
    else
    {
      dsp.setParamValue(gates[currentNote], 0);
    }
    numberOfNotes--;
  }
  else if (type == usbMIDI.NoteOn)
  {
    // Serial.println("Note on");
    int midiNote = int(data1);
    float noteFrequency = getFrequencyFromMidi(midiNote);
    dsp.setParamValue(freqs[currentNoteOrder], noteFrequency);
    dsp.setParamValue(gates[currentNoteOrder], 1.0);
    playingNote[midiNote] = currentNoteOrder;
    numberOfNotes++;
    Serial.println(currentNoteOrder);
    if (polyphonic)
    {
      currentNoteOrder = (currentNoteOrder + 1) % POLY_RANGE;
    }
    else
    {
      for (int i = 0; i < MIDI_RANGE; i++)
      {
        if (playingNote[i] != 0 && i != midiNote)
          playingNote[i] = -1;
      }
    }
  }
  else if (type == usbMIDI.ControlChange)
  {
    Serial.print("Control change ");
    int control = int(data1);
    int inputValue = int(data2);
    Serial.print(control);
    Serial.print(" ");
    Serial.println(inputValue);
    float outputValue;
    switch (control)
    {
    case 20:
      if (inputValue == 127)
      {
        recordLoop();
      }
      break;
    case 21:
      if (inputValue == 127)
      {
        loop_index = 0;
        loopPlaying = true;
      }
      else if (inputValue == 0)
      {
        loopPlaying = false;
        dsp.setParamValue("gate4", 0);
      }
      break;

    default:
      break;
    }
  }
}

byte *getMidiInput()
{

  // fetch the MIDI message, defined by these 5 numbers (except SysEX)
  //
  byte *res = new byte[3];

  res[0] = usbMIDI.getType();  // which MIDI message, 128-255
  res[1] = usbMIDI.getData1(); // first data byte of message, 0-127
  res[2] = usbMIDI.getData2(); // second data byte of message, 0-127
  return res;
}

void recordLoop()
{
  for (int i = 0; i < recording_size; i++)
  {
    record1[i] = -1;
  }
  polyphonic = false;
  bool playingNote = false;
  bool recording = true;
  int currentNotePlaying;
  int record_index = 0;
  byte *midiInfo;
  while (recording && record_index < RECORD_RANGE)
  {
    if (usbMIDI.read())
    {
      midiInfo = getMidiInput();
      if (midiInfo[0] == usbMIDI.NoteOn)
      {
        playingNote = true;
        currentNotePlaying = int(midiInfo[1]);
      }
      else if (midiInfo[0] == usbMIDI.NoteOff)
      {
        if (int(midiInfo[1]) == currentNotePlaying)
          playingNote = false;
      }
      else if (midiInfo[0] == usbMIDI.ControlChange)
      {
        int control = int(midiInfo[1]);
        int inputValue = int(midiInfo[2]);
        if (control == 20)
        {
          recording_size = record_index;
          recording = false;
        }
      }
    }
    if (playingNote)
    {
      // Serial.print(midiInfo[1]);
      // Serial.print(" ");
      // int midiNote = int(midiInfo[1]);
      Serial.println(currentNotePlaying);
      float noteFrequency = getFrequencyFromMidi(currentNotePlaying);
      dsp.setParamValue("freq4", noteFrequency);
      dsp.setParamValue("gate4", 1.0);

      record1[record_index] = currentNotePlaying;
    }
    else
    {
      dsp.setParamValue("gate4", 0);
      Serial.println(-1);
      record1[loop_index] = -1;
    }
    delay(DELAY_VALUE);
    record_index++;
  }
  dsp.setParamValue("gate4", 0);
  polyphonic = true;
}

void playLoop()
{
  Serial.println(record1[loop_index]);
  if (record1[loop_index] != -1)
  {
    float frequency = getFrequencyFromMidi(record1[loop_index]);
    dsp.setParamValue("freq4", frequency);
    dsp.setParamValue("gate4", 1.0);
  }
  else
  {
    dsp.setParamValue("gate4", 0.0);
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
