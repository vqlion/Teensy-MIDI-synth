#include <cmath>

#include "Synth1.h"

#define SINE_TABLE_SIZE 16384
#define MIDI_RANGE 128

Synth1::Synth1(int SR) : sineTable(SINE_TABLE_SIZE),
                         phasor(SR),
                         gain(1.0),
                         samplingRate(SR)
{
  midiNotes = new bool[MIDI_RANGE];
  for (int i = 0; i < MIDI_RANGE; i++)
  {
    midiNotes[i] = false;
  }
}

void Synth1::setFrequency(float f)
{
  phasor.setFrequency(f);
}

void Synth1::setNoteOn(int index)
{
  midiNotes[index] = true;
}

void Synth1::setNoteOff(int index)
{
  midiNotes[index] = false;
}

void Synth1::setGain(float g)
{
  gain = g;
}

float Synth1::getFrequencyFromMidi(int d)
{
  float exponent = (d - 69.0) / 12.0;
  return pow(2.0, exponent) * 440;
}

float Synth1::tick()
{
  float res = 0;
  int numberOfNotes = 0;
  float freq;
  for (int i = 0; i < MIDI_RANGE; i++)
  {
    if (midiNotes[i])
    {
      freq = getFrequencyFromMidi(i);
      phasor.setFrequency(freq);
      int index = phasor.tick() * SINE_TABLE_SIZE;
      res += sineTable.tick(index);
      numberOfNotes++;
    }
  }
  if (numberOfNotes != 0)
  {
    return res * gain * (1.0 / numberOfNotes);
  }
  return 0;
}
