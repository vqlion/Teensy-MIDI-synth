#include "MyDsp.h"

#define AUDIO_OUTPUTS 1

#define MULT_16 32767

MyDsp::MyDsp() :
  AudioStream(AUDIO_OUTPUTS, new audio_block_t*[AUDIO_OUTPUTS]),
  sine(AUDIO_SAMPLE_RATE_EXACT),
  synth(AUDIO_SAMPLE_RATE_EXACT)
{}

MyDsp::~MyDsp() {}

// set sine wave frequency
void MyDsp::setFreq(float freq) {
  sine.setFrequency(freq);
}

void MyDsp::setGain(float g) {
  synth.setGain(g);
}

void MyDsp::setNoteOn(int index) {
  synth.setNoteOn(index);
}

void MyDsp::setNoteOff(int index) {
  synth.setNoteOff(index);
}

void MyDsp::update(void) {
  audio_block_t* outBlock[AUDIO_OUTPUTS];
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++) {
    outBlock[channel] = allocate();
    if (outBlock[channel]) {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float sineSample = sine.tick();
        float currentSample = sineSample * 0.5;
        currentSample = max(-1, min(1, currentSample));
        int16_t val = currentSample * MULT_16;
        outBlock[channel]->data[i] = val;
      }
      transmit(outBlock[channel], channel);
      release(outBlock[channel]);
    }
  }
}
