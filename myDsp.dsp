import("stdfaust.lib");
// freqs and gains definitions go here
osc(freq) = rdtable(tablesize, os.sinwaveform(tablesize), int(os.phasor(tablesize,freq)))
with{
    tablesize = 1 << 15; // instead of 1 << 16
};
carFreq1 = hslider("freq1", 440, 20, 5000, 0.1);
carFreq2 = hslider("freq2", 523.25, 20, 5000, 0.1);
carFreq3 = hslider("freq3", 349.2, 20, 5000, 0.1);
carFreq4 = hslider("freq4", 587.33, 20, 5000, 0.1);

gate1 = checkbox("gate1");
gate2 = checkbox("gate2");
gate3 = checkbox("gate3");
gate4 = checkbox("gate4");


modFreq1 = hslider("modfreq1", 50, 0.1, 100, 0.1);
index1 = hslider("index1", 1, 0, 20, 1);

filter = hslider("filter", 0, 0, 1, 0.01);
filterFreq = hslider("filterFreq", 10, 0, 400, 0.1);

lowP = fi.lowpass(1, filterFreq);
gate = checkbox("gate");

polys = osc(carFreq1+osc(modFreq1)*index1) *gate1,
    osc(carFreq2+osc(modFreq1)*index1)*gate2,
    osc(carFreq3+osc(modFreq1)*index1)*gate3,
    osc(carFreq4+osc(modFreq1)*index1)*gate4;

process = 
    polys
    :> _ *gate // merging signals here
    : lowP;
    // <: dm.zita_light; // and then splitting them for stereo in