// Add signal and noise test

#include "SineGenerator.hh"
#include "WhiteNoiseGen.hh"

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

    waveform_t temp  = sinsig.generate(); // pure signal
    waveform_t temp2 = noise.generate();  // pure noise
    waveform_t xo = sinsig.add(temp2, 0); // add noise to signal, implicit cast

    std::ofstream ofs("saddn.csv", std::ofstream::out);
    for (size_t i=0; i<xo.size();++i)
        ofs << xo[i].numerical_value_in(V) << ", " << temp[i].numerical_value_in(V) << ", " 
            << temp2[i].numerical_value_in(V) << std::endl;
    ofs.close();

    return 0;
}