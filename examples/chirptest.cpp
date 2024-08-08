// Add signal and noise test

#include "ChirpGenerator.hh"

#include <iostream>
#include <fstream>

int main()
{
    quantity<Hz> srate = 100.0 * Hz; // Hz
    quantity<s>  dur = 2.0 * s; // [s]
    quantity<Hz> freq = 1.0 * Hz;
    quantity<Hz/s> chrate = 1.0 * Hz/s;
    int seed = 1235;
    ChirpGenerator chirpsig(2.0*V, freq, chrate, srate, dur, 0.0*deg);

    waveform_t temp  = chirpsig.generate(); // pure signal

    std::ofstream ofs("chirp.csv", std::ofstream::out);
    for (size_t i=0; i<temp.size();++i)
        ofs << temp[i].numerical_value_in(V) << std::endl;
    ofs.close();

    return 0;
}
