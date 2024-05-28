// Digitizer test
#include "digitizer.hh"
#include "SineGenerator.hh"

#include <iostream>
#include <fstream>

int main()
{
    quantity<Hz> srate = 25.0 * Hz; // sampling Hz
    quantity<s>  dur = 4.0 * s; // [s]
    quantity<Hz> freq = 1.0 * Hz;

    Digitizer digi(1.0 * V, srate, 8); // range +- 1V
    SineGenerator sinsig(2.0 * V, freq, srate, dur, 0.0*deg); // 2V amplitude

    digi.dumpInfo();
    waveform_t xo = sinsig.generate();
    waveform_t dout = digi.digitize(xo);

    std::ofstream ofs("digi.csv", std::ofstream::out);
    for (size_t i=0; i<xo.size();++i)
        ofs << xo[i].numerical_value_in(V) << "," << dout[i].numerical_value_in(V) << std::endl;
    ofs.close();

    return 0;
}