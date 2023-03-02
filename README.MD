# Teensy MIDI controlled looping synthesizer

A MIDI controlled polyphonic synthesizer capable of looping sounds, with 3 instruments preloaded.

## Details

The Synthesizer is controlled via a MIDI keyboard. We used an arturia keylab essential 49, but any would do.

### Features:

 - Three instruments available: a synth, a drumkit (kick, snare, hihat), and a guitar
 - 6 polyphonic voices on the synth (play chords!)
 - Ability to create and play monophonic loops on the synth and the drumkit
 - Control instruments' parameters (via knobs on the keyboard): 
    - Synth: cutoff filter, attack, decay, sustain, release, pitchbend, fm frequency and fm intensity
    - Kick: attack and click
    - Guitar: resonance and pitchbend
 - Control the gain of each instrument separately, and the overall gain

All the sounds were generated using the [Faust programming language](https://faust.grame.fr/). The source code is available in [myDspFinal.dsp](myDspFinal.dsp).

The [archives folder](archives/) contains our early stages of development. It can be interesting if you're looking for very basic scripts to begin with, feel free to check it out.

### Possible improvments
As of right now, the keyboard sends MIDI information to a PC, which in turn sends it to the teensy. This is not ideal, and it would be preferable to have the teensy and the keyboard linked directly.

The loops are only monophonic, and the guitar can't be looped, which is a bit sad. This can cause memory issues on the teensy thought.

## Usage

Connect your teensy 4.0 to your computer and upload the "synth" script to it. Don't forget to select ```USB type: "MIDI"``` and ```Optimize: "Smallest Code"``` in the tools menu.

Then plug your MIDI keyboard into your computer, and link its input to the teensy. You can achieve this via a DAW (Ableton for example).

You might have to change a few settings in the control section, because not every MIDI keyboard is built with the same control knobs numbers. You can either reconfigure your MIDI device, or change the code (the ```usbMIDI.ControlChange``` parts). You just have to link the control knob of your choice to the parameter of your choice.

## Team

[Sofia EL KHALIFI](https://github.com/Soubzar) & [Valentin JOSSIC](https://github.com/vqlion)

This project was done for the SON project in 3TC, at INSA Lyon. 