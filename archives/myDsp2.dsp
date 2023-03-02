import("stdfaust.lib");
// freqs and gains definitions go here
osc(freq) = rdtable(tablesize, os.sinwaveform(tablesize), int(os.phasor(tablesize,freq)))
with{
    tablesize = 1 << 15; // instead of 1 << 16
};

kick(pitch, click, attack, decay, drive, gate) = out
with {
    env = en.adsr(attack, decay, 0.0, 0.1, gate);
    pitchenv = en.adsr(0.005, click, 0.0, 0.1, gate);
    clean = env * osc((1 + pitchenv * 4) * pitch);
    out = ma.tanh(clean * drive);
};

freqSynth1 = hslider("freqSynth1", 440, 20, 5000, 0.1);
freqSynth2 = hslider("freqSynth2", 523.25, 20, 5000, 0.1);
freqSynth3 = hslider("freqSynth3", 349.2, 20, 5000, 0.1);
freqSynth4 = hslider("freqSynth4", 587.33, 20, 5000, 0.1);
freqSynth5 = hslider("freqSynth5", 587.33, 20, 5000, 0.1);
freqSynth6 = hslider("freqSynth6", 587.33, 20, 5000, 0.1);
freqSynthLoop1 = hslider("freqSynthLoop1", 587.33, 20, 5000, 0.1);
freqSynthLoop2 = hslider("freqSynthLoop2", 587.33, 20, 5000, 0.1);
freqSynthLoop3 = hslider("freqSynthLoop3", 587.33, 20, 5000, 0.1);
freqSynthLoop4 = hslider("freqSynthLoop4", 587.33, 20, 5000, 0.1);
freqSynthLoop5 = hslider("freqSynthLoop5", 587.33, 20, 5000, 0.1);
freqSynthLoop6 = hslider("freqSynthLoop6", 587.33, 20, 5000, 0.1);

gateSynth = checkbox("gateSynth");
gateGuitar = checkbox("gateGuitar");
gateDrums = checkbox("gateDrums");

gateSynth1 = checkbox("gateSynth1");
gateSynth2 = checkbox("gateSynth2");
gateSynth3 = checkbox("gateSynth3");
gateSynth4 = checkbox("gateSynth4");
gateSynth5 = checkbox("gateSynth5");
gateSynth6 = checkbox("gateSynth6");
gateSynthLoop1 = checkbox("gateSynthLoop1");
gateSynthLoop2 = checkbox("gateSynthLoop2");
gateSynthLoop3 = checkbox("gateSynthLoop3");
gateSynthLoop4 = checkbox("gateSynthLoop3");
gateSynthLoop5 = checkbox("gateSynthLoop3");
gateSynthLoop6 = checkbox("gateSynthLoop3");

gateDrums1 = checkbox("gateDrums1");
gateDrums2 = checkbox("gateDrums2");
gateDrums3 = checkbox("gateDrums3");
gateDrums4 = checkbox("gateDrums4");
gateDrums5 = checkbox("gateDrums5");
gateDrums6 = checkbox("gateDrums6");

gainSynth = hslider("gainSynth", 0.5, 0, 1, 0.01);
gainDrums = hslider("gainDrums", 0.5, 0, 1, 0.01);
gainGuitar = hslider("gainGuitar", 0.5, 0, 1, 0.01);

pitch = hslider("pitch", 1, 0.5, 1.5, 0.1);

modFreq1 = hslider("modfreq1", 50, 0.1, 100, 0.1);
index1 = hslider("index1", 1, 0, 20, 1);

filter = hslider("filter", 0, 0, 1, 0.01);
filterFreq = hslider("filterFreq", 10, 5, 3000, 0.1);

lowP = fi.lowpass(2, filterFreq);

at = hslider("at", 0, 0, 1, 0.01);
dt = hslider("dt", 0, 0, 1, 0.01);
sl = hslider("sl", 0.5, 0, 1, 0.01);
rt = hslider("rt", 0, 0, 1, 0.01);

kickDecay = hslider("kickDecay", 0.2, 0.2, 1, 0.01);
kickClick = hslider("kickClick", 0.05, 0.005, 0.1, 0.001);

envelope(gate) = en.adsr(at,dt,sl,rt,gate);

highHat = sy.hat(2500, 10000, 0.05, 0.05, gateDrums1);
highHat2 = sy.hat(2500, 10000, 0.05, 0.05, gateDrums2);
kick1 = kick(40, kickClick, 0.001, kickDecay, 5, gateDrums3);
kick2 = kick(40, kickClick, 0.005, kickDecay, 5, gateDrums4);
clap = sy.clap(3500, 0.001, 0.05, gateDrums5);
clap2 = sy.clap(3500, 0.001, 0.05, gateDrums6);

//lowpass to add!
synth = os.saw2ptr(freqSynth1*pitch+osc(modFreq1)*index1)*envelope(gateSynth1), 
    os.saw2ptr(freqSynth2*pitch+osc(modFreq1)*index1)*envelope(gateSynth2), 
    os.saw2ptr(freqSynth3*pitch+osc(modFreq1)*index1)*envelope(gateSynth3),
    os.saw2ptr(freqSynth4*pitch+osc(modFreq1)*index1)*envelope(gateSynth4),
    os.saw2ptr(freqSynth5*pitch+osc(modFreq1)*index1)*envelope(gateSynth5),
    os.saw2ptr(freqSynth6*pitch+osc(modFreq1)*index1)*envelope(gateSynth6),
    os.saw2ptr(freqSynthLoop1*pitch+osc(modFreq1)*index1)*envelope(gateSynthLoop1),
    os.saw2ptr(freqSynthLoop2*pitch+osc(modFreq1)*index1)*envelope(gateSynthLoop2),
    os.saw2ptr(freqSynthLoop3*pitch+osc(modFreq1)*index1)*envelope(gateSynthLoop3),
    os.saw2ptr(freqSynthLoop4*pitch+osc(modFreq1)*index1)*envelope(gateSynthLoop4),
    os.saw2ptr(freqSynthLoop5*pitch+osc(modFreq1)*index1)*envelope(gateSynthLoop5),
    os.saw2ptr(freqSynthLoop6*pitch+osc(modFreq1)*index1)*envelope(gateSynthLoop6)
    :> _ *gateSynth*gainSynth : lowP : _;

guitar = pm.ks_ui_MIDI :> _ *gateGuitar*gainGuitar : fi.lowpass(2, 400) : _;

drums = highHat,
    highHat2,
    kick1,
    kick2,
    clap,
    clap2
    :> _ *gateDrums*gainDrums;

//process = polys0*(mode==0), polys1*(mode==1), polys2*(mode==2);

process = 
    synth, guitar, drums :> _;
     
effect = dm.zita_light; //multiple voices all go to the same effect line
