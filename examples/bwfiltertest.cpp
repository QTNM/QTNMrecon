// W filter test
#include "filter.hh"
#include "SineGenerator.hh"

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>


int main()
{
    quantity<Hz> srate = 1000.0 * Hz; // [Hz] sampling
    quantity<s> duration = 1.0 * s; // [s]
    quantity<Hz> ff1 = 10.0 * Hz;
    quantity<Hz> ff2 = 20.0 * Hz;
    quantity<Hz> fcut = 15.0 * Hz;

    Butterworth lp;
    lp.SetFilterOrder(10);
    lp.SetSamplingRate(srate);
    lp.SetLowFilterFreq(fcut);

    // filter test
    // make data
    SineGenerator sinsig(1.0*V, ff1, srate, duration, 0.0*deg); // sin
    waveform_t wave1 = sinsig.generate();

    sinsig.setfrequency(ff2);
    waveform_t sig = sinsig.add(wave1, 0);

    // filter
    int entries = (int)(srate.numerical_value_in(Hz) * duration.numerical_value_in(s));
    waveform_t out(entries);
    lp.LPassFilter(sig, out); // filter action

    std::ofstream ofs2("f.csv", std::ofstream::out);
    for (unsigned int i=0;i<out.size();++i)
        ofs2 << sig[i].numerical_value_in(V) << ", " << out[i].numerical_value_in(V) << std::endl;
    ofs2.close();
    return 0;
}
