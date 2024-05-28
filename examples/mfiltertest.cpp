// matched filter test

#include <iostream>
#include <fstream>

#include "filter.hh"
#include "SineGenerator.hh"
#include "WhiteNoiseGen.hh"


int main()
{
    quantity<Hz> srate = 512.0 * Hz; // Hz
    quantity<s>  dur = 1.0 * s; // [s]
    quantity<Hz> freq  = 15.0 * Hz;
    quantity<Hz> freq2 = 16.0 * Hz;

    // filter test
    // make data
    SineGenerator sinsig(1.0*V, freq, srate, dur, 90.0*deg);
    WhiteNoiseGenerator noise(0.5*V, srate, dur, 1234);
    waveform_t s1  = sinsig.generate(); // pure signal with unit
    waveform_t temp2 = noise.generate();  // pure noise with unit
    waveform_t xin2 = sinsig.add(temp2, 0); // add noise to signal, implicit cast
    sinsig.setfrequency(freq2);
    vec_t s2  = sinsig.generate_pattern(); // pure signal, no unit

    MatchedFilter mf;

    mf.SetTemplate(s2);
    //mf.SetTemplate(xin2); // autocorrelate
    waveform_t out = mf.Filter(xin2); // filter action

    std::ofstream ofs2("mf.csv", std::ofstream::out);
    for (unsigned int i=0;i<out.size();++i)
        ofs2 << out[i].numerical_value_in(V) << std::endl;
    ofs2.close();
    return 0;
}
