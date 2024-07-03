// Add signal and noise test

#include "SineGenerator.hh"
#include "DownConverter.hh"
#include "dsp.hh"

#include <iostream>
#include <fstream>

int main()
{
    DSP dsp; // results in Fourier space
    // data
    quantity<Hz> srate = 4096.0 * Hz; // Hz
    quantity<s>  dur = 10.0 * s; // [s]
    quantity<Hz> freq = 1.0 * kHz;
    SineGenerator sinsig(1.0*V, freq, srate, dur, 0.0*deg);
    freq = 950 * Hz; // near to 1kHz
    // mixer
    DownConverter demod;
    demod.setLOamplitude(10.0*V);
    demod.setLOfrequency(freq - 100*Hz); // 800 Hz
    demod.setFilterCutoff(300.0*Hz);
    demod.setSamplingRate(srate);

    // make data
    waveform_t s1  = sinsig.generate(); // pure tone
    sinsig.setFrequency(freq); // s2 frequency
    waveform_t xin = sinsig.add(s1, 0); // add s1 to s2

    // demodulate
    waveform_t res = demod.mix(xin);

    // check in frequencies
    std::vector<quantity<Hz>> ff = dsp.dft(res, srate); // dft

    std::ofstream ofs("mix.csv", std::ofstream::out);
    for (size_t i=0; i<res.size();++i)
        ofs << ff[i].numerical_value_in(Hz) << ", " << res[i].numerical_value_in(V) << std::endl;
    ofs.close();

    return 0;
}
