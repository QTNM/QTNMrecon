// Moving Average test

#include "SineGenerator.hh"
#include "WhiteNoiseGen.hh"
#include "filter.hh"

#include <iostream>
#include <fstream>

int main()
{
    quantity<Hz> srate = 25.0 * Hz; // Hz
    quantity<s>  dur = 4.0 * s; // [s]
    quantity<Hz> freq = 1.0 * Hz;
    int seed = 1234;
    SineGenerator sinsig(2.0*V, freq, srate, dur, 0.0*deg);
    WhiteNoiseGenerator noise(0.5*V, srate, dur, seed);
    MovingAverage mav; // default width at 5
    mav.setMovingAverageWidth(3);

    waveform_t temp  = sinsig.generate(); // pure signal
    waveform_t nn = noise.generate();
    waveform_t xo = sinsig.add(nn, 0); // add noise to signal, implicit cast
    waveform_t temp2 = mav.Filter(xo);

    std::ofstream ofs("mav.csv", std::ofstream::out);
    for (size_t i=0; i<xo.size();++i)
        ofs << xo[i].numerical_value_in(V) << ", " << temp[i].numerical_value_in(V) << ", " 
            << temp2[i].numerical_value_in(V) << std::endl;
    ofs.close();

    return 0;
}