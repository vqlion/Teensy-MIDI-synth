#include <Audio.h>
#include "test.h"

test test;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(test, 0, out, 0);
AudioConnection patchCord1(test, 0, out, 1);
bool playing;
int numberOfNotes;

void setup()
{
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.1);
  playing = false;
  numberOfNotes = 0;
  Serial.begin(9600);
  test.setParamValue("ctfreq", 500.0);
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
  test.setParamValue("gate", gateValue);
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
    numberOfNotes--;
  }
  else if (type == usbMIDI.NoteOn)
  {
    Serial.println("Note on");
    int midiNote = int(data1);
    float noteFrequency = getFrequencyFromMidi(midiNote);
    test.setParamValue("freq", noteFrequency);
    numberOfNotes++;
  }
  else if (type == usbMIDI.ControlChange)
  {
    Serial.println("Control change ");
    int control = int(data1);
    int inputValue = int(data2);
    float outputValue;
    switch (control)
    {
    case 74:
      outputValue = mapfloat(inputValue, 0, 127, 50, 5000);
      test.setParamValue("ctfreq", outputValue);
      break;
    case 17:
      outputValue = mapfloat(inputValue, 0, 127, 0, 1);
      test.setParamValue("gain", outputValue);
      break;

    default:
      break;
    }
  }

  // switch (type)
  // {
  // case usbMIDI.NoteOff: // 0x80
  //   Serial.print("Note Off, ch=");
  //   Serial.print(channel, DEC);
  //   Serial.print(", note=");
  //   Serial.print(data1, DEC);
  //   Serial.print(", velocity=");
  //   Serial.println(data2, DEC);
  //   numberOfNotes--;
  //   break;

  // case usbMIDI.NoteOn: // 0x90
  //   Serial.print("Note On, ch=");
  //   Serial.print(channel, DEC);
  //   Serial.print(", note=");
  //   Serial.print(data1, DEC);
  //   Serial.print(", velocity=");
  //   Serial.println(data2, DEC);
  //   int midiNote = int(data1);
  //   float noteFrequency = getFrequencyFromMidi(midiNote);
  //   test.setParamValue("freq", 440);
  //   numberOfNotes++;
  //   break;

  // case usbMIDI.AfterTouchPoly: // 0xA0
  //   Serial.print("AfterTouch Change, ch=");
  //   Serial.print(channel, DEC);
  //   Serial.print(", note=");
  //   Serial.print(data1, DEC);
  //   Serial.print(", velocity=");
  //   Serial.println(data2, DEC);
  //   break;

  // case usbMIDI.ControlChange: // 0xB0
  //   Serial.print("Control Change, ch=");
  //   Serial.print(channel, DEC);
  //   Serial.print(", control=");
  //   Serial.print(data1, DEC);
  //   Serial.print(", value=");
  //   Serial.println(data2, DEC);
  //   // int control = int(data1);
  //   // int inputValue = int(data2);
  //   // float outputValue;
  //   // switch (control)
  //   // {
  //   // case 74:
  //   //   outputValue = mapfloat(inputValue, 0, 127, 50, 5000);
  //   //   test.setParamValue("ctfreq", outputValue);
  //   //   break;
  //   // case 17:
  //   //   outputValue = mapfloat(inputValue, 0, 127, 0, 1);
  //   //   test.setParamValue("gain", outputValue);
  //   //   break;

  //   // default:
  //   //   break;
  //   // }
  //   // break;

  // case usbMIDI.ProgramChange: // 0xC0
  //   Serial.print("Program Change, ch=");
  //   Serial.print(channel, DEC);
  //   Serial.print(", program=");
  //   Serial.println(data1, DEC);
  //   break;

  // case usbMIDI.AfterTouchChannel: // 0xD0
  //   Serial.print("After Touch, ch=");
  //   Serial.print(channel, DEC);
  //   Serial.print(", pressure=");
  //   Serial.println(data1, DEC);
  //   break;

  // case usbMIDI.PitchBend: // 0xE0
  //   Serial.print("Pitch Change, ch=");
  //   Serial.print(channel, DEC);
  //   Serial.print(", pitch=");
  //   Serial.println(data1 + data2 * 128, DEC);
  //   break;

  // case usbMIDI.SystemExclusive: // 0xF0
  //   // Messages larger than usbMIDI's internal buffer are truncated.
  //   // To receive large messages, you *must* use the 3-input function
  //   // handler.  See InputFunctionsComplete for details.
  //   Serial.print("SysEx Message: ");
  //   printBytes(usbMIDI.getSysExArray(), data1 + data2 * 256);
  //   Serial.println();
  //   break;

  // case usbMIDI.TimeCodeQuarterFrame: // 0xF1
  //   Serial.print("TimeCode, index=");
  //   Serial.print(data1 >> 4, DEC);
  //   Serial.print(", digit=");
  //   Serial.println(data1 & 15, DEC);
  //   break;

  // case usbMIDI.SongPosition: // 0xF2
  //   Serial.print("Song Position, beat=");
  //   Serial.println(data1 + data2 * 128);
  //   break;

  // case usbMIDI.SongSelect: // 0xF3
  //   Serial.print("Sond Select, song=");
  //   Serial.println(data1, DEC);
  //   break;

  // case usbMIDI.TuneRequest: // 0xF6
  //   Serial.println("Tune Request");
  //   break;

  // case usbMIDI.Clock: // 0xF8
  //   Serial.println("Clock");
  //   break;

  // case usbMIDI.Start: // 0xFA
  //   Serial.println("Start");
  //   break;

  // case usbMIDI.Continue: // 0xFB
  //   Serial.println("Continue");
  //   break;

  // case usbMIDI.Stop: // 0xFC
  //   Serial.println("Stop");
  //   break;

  // case usbMIDI.ActiveSensing: // 0xFE
  //   Serial.println("Actvice Sensing");
  //   break;

  // case usbMIDI.SystemReset: // 0xFF
  //   Serial.println("System Reset");
  //   break;

  // default:
  //   Serial.println("Opps, an unknown MIDI message type!");
  // }
}

void printBytes(const byte *data, unsigned int size)
{
  while (size > 0)
  {
    byte b = *data++;
    if (b < 16)
      Serial.print('0');
    Serial.print(b, HEX);
    if (size > 1)
      Serial.print(' ');
    size = size - 1;
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
