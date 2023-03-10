#include <Audio.h>
#include "myDsp.h"

/*
  A MIDI controlled synthesizer & looper. Up to 6 polyphonic voices and 2 loops recorded/played.
  3 instruments: a synth, a drumkit (kick, snare, hihat) & a guitar.
  The loops are only monphonic and you can only record loops on the synth & drumkit.

  This code is meant to be uploaded on a teensy 4.0 board, with an audio shield.
  The myDsp.h library is generated via the Faust programming language

  We use a computer to transfer MIDI signals from a keyboard to the teensy, which then can interpret them.
  This solution is not ideal, and we would have wanted the teensy to communicate with the keyboard directly, but couldn't achieve it yet.

  Authors: 2023 Valentin JOSSIC & Sofia EL KHALIFI, feel free to reuse for whatever reason

  https://github.com/vqlion/Teensy-MIDI-synth
*/

const int MIDI_RANGE = 128;
const int POLY_RANGE = 6;
const int RECORD_RANGE = 4800;
const int DELAY_VALUE = 5;

myDsp dsp;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(dsp, 0, out, 0);
AudioConnection patchCord1(dsp, 0, out, 1);
bool playing, polyphonic, loop1Playing, loop2Playing;
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
char gate1[32] = "gateSynth1";
char gate2[32] = "gateSynth2";
char gate3[32] = "gateSynth3";
char gate4[32] = "gateSynth4";
char gate5[32] = "gateSynth5";
char gate6[32] = "gateSynth6";
char *freqs[POLY_RANGE];
char *gates[POLY_RANGE];
int record1[RECORD_RANGE];
int record2[RECORD_RANGE];
int instrumentsOnLoops[2] = {-1, -1};
int instrumentPlaying;
byte *midiInfo;

void setup()
{
  AudioMemory(2); //honesly, I don't know.
  audioShield.enable();
  audioShield.volume(0.5);
  playing = false;
  loop1Playing = false;
  loop2Playing = false;
  currentNoteOrder = 0;
  loop1_index = 0;
  loop2_index = 0;
  Serial.begin(9600);
  freqs[0] = freq1;
  freqs[1] = freq2;
  freqs[2] = freq3;
  freqs[3] = freq4;
  freqs[4] = freq5;
  freqs[5] = freq6;
  gates[0] = gate1;
  gates[1] = gate2;
  gates[2] = gate3;
  gates[3] = gate4;
  gates[4] = gate5;
  gates[5] = gate6;
  polyphonic = true;
  recording1_size = 0;
  recording2_size = 0;
  instrumentPlaying = 0;
  dsp.setParamValue("gateSynth", 1);
  dsp.setParamValue("gateDrums", 1);
  dsp.setParamValue("gateGuitar", 1);
  dsp.setParamValue("gain", 1); //"gain" is the guitar's gain
  dsp.setParamValue("sl", 0.1);
  dsp.setParamValue("filterFreq", 1000);
  for (int i = 0; i < MIDI_RANGE; i++)
  {
    playingNote[i] = -1;
  }
  for (int i = 0; i < RECORD_RANGE; i++)
  {
    record1[i] = -1;
    record2[i] = -1;
  }
}

void loop()
{
  dsp.setParamValue("gateDrums1", 0);
  dsp.setParamValue("gateDrums2", 0);
  dsp.setParamValue("gateDrums3", 0);
  dsp.setParamValue("gateDrums4", 0);
  dsp.setParamValue("gateDrums5", 0);
  dsp.setParamValue("gateDrums6", 0);
  dsp.setParamValue("gate", 0); // the gate parameter is actually only used by the guitar (second instrument)
  // gate parameters are to enable an instrument. It's like when you hit the key or you pinch the string

  byte type, data1, data2;
  if (usbMIDI.read())
  {
    midiInfo = getMidiInput();
    type = midiInfo[0]; //type of midi input
    data1 = midiInfo[1]; //usually represents the button you just pressed
    data2 = midiInfo[2]; //usually represents its value
    processMIDI(type, data1, data2);
  }
  if (loop1Playing)
  {
    playLoop(0);
    loop1_index = (loop1_index + 1) % recording1_size;
  }
  if (loop2Playing)
  {
    playLoop(1);
    loop2_index = (loop2_index + 1) % recording2_size;
  }
  delay(DELAY_VALUE); // delaying because loops can't be saved at processing time due to memory usage
}


/*
Process the midi input and plays the sounds
Also changes the values asked (like the cutoff filter, the gain, ...)
*/
void processMIDI(byte type, byte data1, byte data2)
{
  if (type == usbMIDI.NoteOff)
  {
    int midiNote = int(data1);
    int currentNote = playingNote[midiNote];
    Serial.println(-1 * currentNote);
    playingNote[midiNote] = -1;
    bool otherNote = false;
    if (!polyphonic) // this portion of code might be deprecated, but I don't have time to check whether it breaks if I remove it
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
  }
  else if (type == usbMIDI.NoteOn)
  {
    int midiNote = int(data1);
    float noteFrequency = getFrequencyFromMidi(midiNote);
    if (instrumentPlaying == 0) // synth
    {
      dsp.setParamValue(freqs[currentNoteOrder], noteFrequency);
      dsp.setParamValue(gates[currentNoteOrder], 1.0);
      playingNote[midiNote] = currentNoteOrder;
      currentNoteOrder = (currentNoteOrder + 1) % POLY_RANGE; // loops through a note order to assign each note a different polyphonic voice
    }
    else if (instrumentPlaying == 1) // drums
    {
      switch (midiNote)
      {
      case 60:
        dsp.setParamValue("gateDrums3", 1); // kick
        break;
      case 62:
        dsp.setParamValue("gateDrums5", 1); // snare
        break;
      case 64:
        dsp.setParamValue("gateDrums1", 1); // hihat (bad variable names...)
        break;
      default:
        break;
      }
    }
    else if (instrumentPlaying == 2) // guitar
    {
      dsp.setParamValue("freq", noteFrequency); //"freq" is only used for the guitar
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
    switch (control) // different action based on the knob physically turned
    {
    case 20:
      if (inputValue != 0)
      {
        recordLoop(0);
      }
      break;
    case 24:
      if (inputValue != 0)
      {
        recordLoop(1);
      }
      break;
    case 21:
      loop1_index = 0;
      if (loop1Playing)
      {
        loop1Playing = false;
        dsp.setParamValue("gateSynthLoop2", 0);
      }
      else
      {
        loop1Playing = true;
      }
      break;
    case 25:
      loop2_index = 0;
      if (loop2Playing)
      {
        loop2Playing = false;
        dsp.setParamValue("gateSynthLoop3", 0);
      }
      else
      {
        loop2Playing = true;
      }
      break;
    case 52:
      instrumentPlaying = 0; // change instrument: synth
      break;
    case 53:
      instrumentPlaying = 1; // drums
      break;
    case 54:
      instrumentPlaying = 2; // drums
      break;
    case 93:
      dsp.setParamValue("gainSynth", mapfloat(inputValue, 0, 127, 0, 1));
      break;
    case 18:
      dsp.setParamValue("gainDrums", mapfloat(inputValue, 0, 127, 0, 1));
      break;
    case 19:
      dsp.setParamValue("gainGuitar", mapfloat(inputValue, 0, 127, 0, 2));
      break;
    case 74:
      dsp.setParamValue("filterFreq", mapfloat(inputValue, 0, 127, 500, 5000)); // cutoff filter (lowpass) on the synth
      break;
    case 73:
      dsp.setParamValue("at", mapfloat(inputValue, 0, 127, 0, 1)); // attack (synth)
      break;
    case 75:
      dsp.setParamValue("dt", mapfloat(inputValue, 0, 127, 0, 1)); // decay
      break;
    case 79:
      dsp.setParamValue("sl", mapfloat(inputValue, 0, 127, 0, 1)); // sustain
      break;
    case 72:
      dsp.setParamValue("rt", mapfloat(inputValue, 0, 127, 0, 1)); // release
      break;
    case 76:
      dsp.setParamValue("modfreq1", mapfloat(inputValue, 0, 127, 0.5, 75)); // fm
      break;
    case 77:
      dsp.setParamValue("index1", mapfloat(inputValue, 0, 127, 0, 20)); // modulation multiplier (fm gain)
      break;
    case 1:
      dsp.setParamValue("filterFreq", mapfloat(inputValue, 0, 127, 500, 5000)); // cutoff filter (linked to 2 knobs on our keyboard because why not)
      break;
    case 71:
      dsp.setParamValue("damping", mapfloat(inputValue, 0, 127, 0, 1)); // guitar's resonance
      break;
    case 81:
      dsp.setParamValue("kickDecay", mapfloat(inputValue, 0, 127, 0.1, 1));
      break;
    case 80:
      dsp.setParamValue("kickClick", mapfloat(inputValue, 0, 127, 0.005, 0.1));
      break;
    case 85:
      dsp.setParamValue("globalGain", mapfloat(inputValue, 0, 127, 0, 2));
      break;

    default:
      break;
    }
  }
  else if (type == usbMIDI.PitchBend)
  {
    int pitch1, pitch2;
    pitch1 = int(data1);
    pitch2 = int(data2);
    int pitch = pitch1 + pitch2 * 128; // this is just how pitch is processed in midi. precision I guess
    if (instrumentPlaying == 0)        // synth
    {
      if (pitch == 8192)
      {
        dsp.setParamValue("pitch", 1);
      }
      else
      {
        dsp.setParamValue("pitch", mapfloat(pitch, 0, 16383, 0.1, 2));
      }
    }
    else if (instrumentPlaying == 2) // guitar
    {
      if (pitch == 8192)
      {
        dsp.setParamValue("bend", 0);
      }
      else
      {
        dsp.setParamValue("bend", mapfloat(pitch, 0, 16383, -2, 2));
      }
    }
  }
}

/*
Records the loop of the index number (two loops possible => index is 0 or 1)
*/
void recordLoop(int index)
{
  if (index == 0)
  {
    instrumentsOnLoops[0] = instrumentPlaying;
    for (int i = 0; i < recording1_size; i++)
    {
      record1[i] = -1;
    }
  }
  else if (index == 1)
  {
    instrumentsOnLoops[1] = instrumentPlaying;
    for (int i = 0; i < recording2_size; i++)
    {
      record2[i] = -1;
    }
  }

  bool recording = true;
  bool playingNote = false;
  int record_index = 0;
  int currentNotePlaying;
  byte *midiInput;
  byte type, data1, data2;

  while (recording && record_index < RECORD_RANGE)
  {
    dsp.setParamValue("gateDrums1", 0);
    dsp.setParamValue("gateDrums2", 0);
    dsp.setParamValue("gateDrums3", 0);
    dsp.setParamValue("gateDrums4", 0);
    dsp.setParamValue("gateDrums5", 0);
    dsp.setParamValue("gateDrums6", 0);

    if (loop1Playing)
    {
      playLoop(0);
      loop1_index = (loop1_index + 1) % recording1_size;
    }
    if (loop2Playing)
    {
      playLoop(1);
      loop2_index = (loop2_index + 1) % recording2_size;
    }
    // play loops even when recording the loop!

    if (usbMIDI.read())
    {
      midiInput = getMidiInput();
      type = midiInput[0];
      data1 = midiInput[1];
      data2 = midiInput[2];

      if (type == usbMIDI.NoteOn)
      {
        playingNote = true;
        currentNotePlaying = int(data1);
      }
      else if (type == usbMIDI.NoteOff)
      {
        if (int(data1) == currentNotePlaying)
          playingNote = false;
      }
      else if (type == usbMIDI.ControlChange)
      {
        if (index == 0 && int(data1) == 20 && int(data2) == 0)
        {
          recording1_size = record_index; // store the size of the loop in a global variable
          recording = false;
        }
        else if (index == 1 && int(data1) == 24 && int(data2) == 0)
        {
          recording2_size = record_index;
          recording = false;
        }
      }
    }

    if (playingNote)
    {
      if (instrumentPlaying == 0)
      {
        float frequency = getFrequencyFromMidi(currentNotePlaying);
        dsp.setParamValue("freqSynthLoop1", frequency);
        dsp.setParamValue("gateSynthLoop1", 1);
      }
      else if (instrumentPlaying == 1)
      {
        switch (currentNotePlaying)
        {
        case 60:
          dsp.setParamValue("gateDrums4", 1);
          break;
        case 62:
          dsp.setParamValue("gateDrums6", 1);
          break;
        case 64:
          dsp.setParamValue("gateDrums2", 1);
          break;
        default:
          break;
        }
      }
      if (index == 0)
      {
        record1[record_index] = currentNotePlaying; // actually store the midi information of the loop
      }
      else if (index == 1)
      {
        record2[record_index] = currentNotePlaying;
      }
      Serial.println(currentNotePlaying);
    }
    else
    {
      dsp.setParamValue("gateSynthLoop1", 0);
      if (index == 0)
      {
        record1[record_index] = -1; // if no note is played
      }
      else if (index == 1)
      {
        record2[record_index] = -1;
      }
      Serial.println(-1);
    }
    delay(DELAY_VALUE); // delay (the same as in the loop function to keep the rythm)
    record_index++;
  }
  dsp.setParamValue("gateSynthLoop1", 0); // cut the synth after the loop is over (or it might continue FOREVER!!! )
}

/*
plays the current position of the loop (the index is the loop asked)
*/
void playLoop(int index)
{
  if (index == 0)
  {
    int midiNote = record1[loop1_index]; //loop index is global
    if (midiNote != -1)
    {
      if (instrumentsOnLoops[0] == 0)
      {
        float frequency = getFrequencyFromMidi(midiNote);
        dsp.setParamValue("freqSynthLoop2", frequency);
        dsp.setParamValue("gateSynthLoop2", 1);
      }
      else if (instrumentsOnLoops[0] == 1)
      {
        switch (midiNote)
        {
        case 60:
          dsp.setParamValue("gateDrums4", 1);
          break;
        case 62:
          dsp.setParamValue("gateDrums6", 1);
          break;
        case 64:
          dsp.setParamValue("gateDrums2", 1);
          break;
        default:
          break;
        }
      }
    }
    else
    {
      dsp.setParamValue("gateSynthLoop2", 0);
    }
  }
  else if (index == 1)
  {
    int midiNote = record2[loop2_index];
    if (midiNote != -1)
    {
      if (instrumentsOnLoops[1] == 0)
      {
        float frequency = getFrequencyFromMidi(midiNote);
        dsp.setParamValue("freqSynthLoop3", frequency);
        dsp.setParamValue("gateSynthLoop3", 1);
      }
      else if (instrumentsOnLoops[1] == 1)
      {
        switch (midiNote)
        {
        case 60:
          dsp.setParamValue("gateDrums4", 1);
          break;
        case 62:
          dsp.setParamValue("gateDrums6", 1);
          break;
        case 64:
          dsp.setParamValue("gateDrums2", 1);
          break;
        default:
          break;
        }
      }
    }
    else
    {
      dsp.setParamValue("gateSynthLoop3", 0);
    }
  }
}

/*
Retruns an array with the midi information of the key pressed on the keyboard
*/
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

float getFrequencyFromMidi(float midiNote)
{
  float exponent = (midiNote - 69.0) / 12.0;
  return pow(2.0, exponent) * 440;
}

/*
Like the built in map function on arduino, but with floats
reference: https://forum.arduino.cc/t/map-to-floating-point/3976/3
*/
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
