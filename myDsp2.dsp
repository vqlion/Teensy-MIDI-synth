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
freqSynth7 = hslider("freqSynth7", 587.33, 20, 5000, 0.1);

gateSynth = checkbox("gateSynth");
gateGuitar = checkbox("gateGuitar");
gateDrums = checkbox("gateDrums");

gateSynth1 = checkbox("gateSynth1");
gateSynth2 = checkbox("gateSynth2");
gateSynth3 = checkbox("gateSynth3");
gateSynth4 = checkbox("gateSynth4");
gateSynth5 = checkbox("gateSynth5");
gateSynth6 = checkbox("gateSynth6");
gateSynth7 = checkbox("gateSynth7");

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

envelope(gate) = en.adsr(at,dt,sl,rt,gate);

highHat = kick(3000, 0.05, 0.005, 0.005, 5, gateDrums1);
highHat2 = kick(3000, 0.05, 0.005, 0.005, 5, gateDrums2);
kick1 = sy.clap(500, 0.001, 0.002, gateDrums3);
kick2 = sy.clap(500, 0.001, 0.002, gateDrums4);
clap = kick(200, 0.05, 0.005, 0.005, 5, gateDrums5),
sy.hat(3170, 18000, 0.05, 0.05, gateDrums5);
clap2 = kick(200, 0.05, 0.005, 0.005, 5, gateDrums6),
sy.hat(3170, 18000, 0.05, 0.05, gateDrums6);

//lowpass to add!
synth = os.saw2ptr(freqSynth1*pitch+osc(modFreq1)*index1)*envelope(gateSynth1)*gateSynth1, 
    os.saw2ptr(freqSynth2+osc(modFreq1)*index1)*envelope(gateSynth2)*gateSynth2, 
    os.saw2ptr(freqSynth3+osc(modFreq1)*index1)*envelope(gateSynth3)*gateSynth3,
    os.saw2ptr(freqSynth4+osc(modFreq1)*index1)*envelope(gateSynth4)*gateSynth4,
    os.saw2ptr(freqSynth5+osc(modFreq1)*index1)*envelope(gateSynth5)*gateSynth5,
    os.saw2ptr(freqSynth6+osc(modFreq1)*index1)*envelope(gateSynth6)*gateSynth6,
    os.saw2ptr(freqSynth7+osc(modFreq1)*index1)*envelope(gateSynth7)*gateSynth7
    :> _ *gateSynth*gainSynth : lowP : _;

guitar = pm.guitar_ui_MIDI :> _ *gateGuitar*gainGuitar;

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
