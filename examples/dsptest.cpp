// DSP test
#include "dsp.hh"
#include <mp-units/math.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>


int main()
{
    DSP dsp; // default constructor
    quantity<Hz> srate = 4096.0 * Hz; // [Hz] sampling; Po2 padding sensitive!
    quantity<s> duration = 0.5 * s; // [s]

    // make data
    int entries = (int)(srate.numerical_value_in(Hz) * duration.numerical_value_in(s));
    fft_arg xin(entries); // zero filled
    xin[0] = 1.0 * V; // delta function, waveform_value->complex<waveform_value>

    // full fft test
    fft_arg xo = dsp.rfft1d(xin, fft_dir::DIR_FWD);

    std::ofstream ofs("t.csv", std::ofstream::out);
    for (auto entry : xo)
        ofs << entry.real().numerical_value_in(V) << "," << entry.imag().numerical_value_in(V) << std::endl;
    ofs.close();

    // dft test
    waveform_t dftdata; // takes real values from type Complex
    for (auto entry : xin) dftdata.push_back(entry.real());
    std::vector<quantity<Hz>> freq = dsp.dft(dftdata, srate); // dftin 

    std::ofstream ofs2("t2.csv", std::ofstream::out);
    for (unsigned int i=0;i<dftdata.size();++i)
        ofs2 << freq[i].numerical_value_in(Hz) << "," << dftdata[i].numerical_value_in(V) << std::endl;
    ofs2.close();

        // try inverse fft from complex to real
    fft_arg xo2 = dsp.rfft1d(xo, fft_dir::DIR_BWD);
    waveform_t rout(entries/2);
    for (int i=0;i<entries/2;++i) {
        rout[i] = abs(xo2[i].real()+xo2[entries-i].imag()); // response output
    }
    std::ofstream ofs3("t3.csv", std::ofstream::out);
    for (unsigned int i=0;i<rout.size();++i)
        ofs3 << rout[i].numerical_value_in(V) << std::endl;
    ofs3.close();

    return 0;
}
