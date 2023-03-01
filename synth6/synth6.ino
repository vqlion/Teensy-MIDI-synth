#include <Audio.h>
#include "myDsp.h"

const int MIDI_RANGE = 128;
const int POLY_RANGE = 4;
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
int loop1_index;
int loop2_index;
int recording1_size;
int recording2_size;
int playingNote[MIDI_RANGE];
char freq1[32] = "freqSynth1";
char freq2[32] = "freqSynth2";
char freq3[32] = "freqSynth3";
char freq4[32] = "freqSynth4";
char freq5[32] = "freqSynth5";
char freq6[32] = "freqSynth6";
char freq7[32] = "freqSynth7";
char gate1[32] = "gateSynth1";
char gate2[32] = "gateSynth2";
char gate3[32] = "gateSynth3";
char gate4[32] = "gateSynth4";
char gate5[32] = "gateSynth5";
char gate6[32] = "gateSynth6";
char gate7[32] = "gateSynth7";
char *freqs[POLY_RANGE];
char *gates[POLY_RANGE];
int record1[RECORD_RANGE];
int record2[RECORD_RANGE];
int instrumentsOnLoops[2] = {-1, -1};
int instrumentPlaying;
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
  loop1_index = 0;
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
  recording1_size = 0;
  recording2_size = 0;
  instrumentPlaying = 0;
  dsp.setParamValue("gateSynth", 1);
  dsp.setParamValue("sl", 0.1);
  for (int i = 0; i < MIDI_RANGE; i++)
  {
    playingNote[i] = -1;
  }
  for (int i = 0; i < RECORD_RANGE; i++)
  {
    record1[i] = -1;
    record2[i] = -1;
  }
  // Serial.println("Setup done");
}

void loop()
{
  dsp.setParamValue("gateDrums1", 0);
  dsp.setParamValue("gateDrums2", 0);
  dsp.setParamValue("gateDrums3", 0);
  dsp.setParamValue("gateDrums4", 0);
  dsp.setParamValue("gateDrums5", 0);
  dsp.setParamValue("gateDrums6", 0);
  dsp.setParamValue("gate", 0);
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
    loop1_index = (loop1_index + 1) % recording1_size;
    loop2_index = (loop2_index + 1) % recording2_size;
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
    if (instrumentPlaying == 0)
    {
      Serial.println(currentNoteOrder);
      Serial.println(freqs[currentNoteOrder]);
      dsp.setParamValue(freqs[currentNoteOrder], noteFrequency);
      Serial.println(currentNoteOrder);
      dsp.setParamValue(gates[currentNoteOrder], 1.0);
      Serial.println(currentNoteOrder);
      playingNote[midiNote] = currentNoteOrder;
      numberOfNotes++;
      Serial.println(currentNoteOrder);
      currentNoteOrder = (currentNoteOrder + 1) % POLY_RANGE;
    }
    else if (instrumentPlaying == 1)
    {
      switch (midiNote)
      {
      case 60:
        dsp.setParamValue("gateDrums3", 1);
        break;
      case 62:
        dsp.setParamValue("gateDrums5", 1);
        break;
      case 64:
        dsp.setParamValue("gateDrums1", 1);
        break;
      default:
        break;
      }
    }
    else if (instrumentPlaying == 2)
    {
      dsp.setParamValue("freq", noteFrequency);
      dsp.setParamValue("gate", 1);
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
        recordLoop(0);
      }
    case 24:
      if (inputValue == 127)
      {
        recordLoop(1);
      }
      break;
    case 21:
      if (inputValue == 127)
      {
        loop1_index = 0;
        loopPlaying = true;
      }
      else if (inputValue == 0)
      {
        loopPlaying = false;
        dsp.setParamValue("gateSynth5", 0);
      }
      break;
    case 25:
      if (inputValue == 127) {
        loop2_index = 0;
        loopPlaying = true;
      } else if (inputValue == 0) {
        loopPlaying = false;
        dsp.setParamValue("gateSynth5", 0);
      }
    case 52:
      instrumentPlaying = 0;
      dsp.setParamValue("gateSynth", 1);
      dsp.setParamValue("gateDrums", 0);
      dsp.setParamValue("gateGuitar", 0);
      break;
    case 53:
      instrumentPlaying = 1;
      dsp.setParamValue("gateSynth", 0);
      dsp.setParamValue("gateDrums", 1);
      dsp.setParamValue("gateGuitar", 0);
      break;
    case 54:
      instrumentPlaying = 2;
      dsp.setParamValue("gateSynth", 0);
      dsp.setParamValue("gateDrums", 0);
      dsp.setParamValue("gateGuitar", 1);
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

void recordLoop(int index)
{
  for (int i = 0; i < recording1_size; i++)
  {
    if (index == 0)
    {
      record1[i] = -1;
    }
    else
    {
      record2[i] = -1;
    }
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
          recording1_size = record_index;
          recording = false;
        } else if (control == 24) {
          recording2_size = record_index;
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
      dsp.setParamValue("freqSynth5", noteFrequency);
      dsp.setParamValue("gateSynth5", 1.0);

      if (index == 0)
      {
        record1[record_index] = currentNotePlaying;
      }
      else
      {
        record2[record_index] = currentNotePlaying;
      }
    }
    else
    {
      dsp.setParamValue("gateSynth5", 0);
      Serial.println(-1);
      if (index == 0)
      {
        record1[record_index] = -1;
      }
      else
      {
        record2[record_index] = -1;
      }
    }
    delay(DELAY_VALUE);
    record_index++;
  }
  dsp.setParamValue("gateSynth5", 0);
  polyphonic = true;
}

void playLoop()
{
  Serial.println(record1[loop1_index]);
  if (record1[loop1_index] != -1)
  {
    float frequency = getFrequencyFromMidi(record1[loop1_index]);
    dsp.setParamValue("freqSynth5", frequency);
    dsp.setParamValue("gatesynth5", 1.0);
  }
  else
  {
    dsp.setParamValue("gateSynth5", 0.0);
  }
  if (record2[loop2_index] != -1)
  {
    float frequency = getFrequencyFromMidi(record2[loop2_index]);
    dsp.setParamValue("freqSynth6", frequency);
    dsp.setParamValue("gatesynth6", 1.0);
  }
  else
  {
    dsp.setParamValue("gateSynth6", 0.0);
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
