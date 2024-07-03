// Band Pass test
#include "dsp.hh"
#include "WhiteNoiseGen.hh"
#include "filter.hh"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>


int main()
{
    DSP dsp; // default constructor
    Butterworth bw;
    // data creation
    quantity<Hz> srate = 4096.0 * Hz; // [Hz] sampling; Po2 padding sensitive!
    quantity<s> duration = 1.0 * s; // [s]
    // filter setup
    quantity<Hz> lf = 1.0 * kHz;
    quantity<Hz> hf = 1.5*lf;
    bw.setSamplingRate(srate);
    bw.setBPassFilterFreqs(lf, hf);
    bw.setFilterOrder(12); // multiple of 4 easiest

    // make data
    WhiteNoiseGenerator noise(1.0*V, srate, duration, 123);
    waveform_t xin = noise.generate();

    // filter
    waveform_t res = bw.BPassFilter(xin);

    // dft test
    std::vector<quantity<Hz>> freq = dsp.dft(res, srate); // dft filtered noise

    std::ofstream ofs2("bp.csv", std::ofstream::out);
    for (unsigned int i=0;i<res.size();++i)
        ofs2 << freq[i].numerical_value_in(Hz) << "," << res[i].numerical_value_in(V) << std::endl;
    ofs2.close();

    return 0;
}
