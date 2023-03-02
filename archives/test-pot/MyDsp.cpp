#include "MyDsp.h"

#define AUDIO_OUTPUTS 2

#define MULT_16 32767

MyDsp::MyDsp() :
  AudioStream(AUDIO_OUTPUTS, new audio_block_t*[AUDIO_OUTPUTS]),
  sine(AUDIO_SAMPLE_RATE_EXACT),
  echo0(AUDIO_SAMPLE_RATE_EXACT, 10000),
  echo1(AUDIO_SAMPLE_RATE_EXACT, 10000)
{
  echo1.setDel(10000);
  echo1.setFeedback(0.7);
  echo1.setDel(7000);
  echo1.setFeedback(0.4);
}

MyDsp::~MyDsp() {}

// set sine wave frequency
void MyDsp::setFreq(float freq) {
  sine.setFrequency(freq);
}

void MyDsp::update(void) {
  audio_block_t* outBlock[AUDIO_OUTPUTS];
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++) {
    outBlock[channel] = allocate();
    if (outBlock[channel]) {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float sineSample = sine.tick();
        float currentSampleL = echo0.tick(sineSample) * 0.5;
        float currentSampleR = echo1.tick(sineSample) * 0.5;
        currentSampleL = max(-1, min(1, currentSampleL));
        currentSampleR = max(-1, min(1, currentSampleR));
        int16_t val = channel == 0 ? currentSampleL * MULT_16 : currentSampleR * MULT_16;
        outBlock[channel]->data[i] = val;
      }
      transmit(outBlock[channel], channel);
      release(outBlock[channel]);
    }
  }
}
