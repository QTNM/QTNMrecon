// matched filter test

#include <iostream>
#include <fstream>

#include "filter.hh"
#include "ChirpGenerator.hh"
#include "WhiteNoiseGen.hh"


int main()
{
    quantity<Hz> srate = 1024.0 * Hz; // Hz
    quantity<s>  dur = 1.0 * s; // [s]
    quantity<Hz> freq  = 15.0 * Hz;
    quantity<Hz/s> chrate = 60.0 * Hz/s;
    waveform_t pause(512, 0.0*V); // = 0.5 s

    // filter test
    // make data
    ChirpGenerator chirpsig(2.0*V, freq, chrate, srate, dur, 0.0*deg);
    WhiteNoiseGenerator noise(0.5*V, srate, dur, 1235);
    waveform_t s1  = chirpsig.generate(); // pure signal with unit
    vec_t templ  = chirpsig.generate_pattern(); // pure signal, no unit

    waveform_t temp = noise.generate();  // pure noise with unit
    waveform_t xin = chirpsig.add(temp, 0); // add noise to signal, implicit cast

    // multiple signals
    waveform_t multiple = xin; // copy
    multiple.insert(multiple.end(),pause.begin(),pause.end()); // insert pause
    multiple.insert(multiple.end(),xin.begin(),xin.end()); // second signal

    MatchedFilter mf;

    mf.setTemplate(&templ);
    quantity<s> timestep = 1.0/srate; // for time lag axis

    waveform_t out = mf.Filter(xin); // filter action
    std::vector<quantity<s>> ta = mf.getTimeLag(timestep);

    waveform_t out2 = mf.Filter(multiple); // filter action
    std::vector<quantity<s>> ta2 = mf.getTimeLag(timestep);

    std::ofstream ofs("mf.csv", std::ofstream::out);
    for (int i=0;i<out.size();++i)
        ofs << ta[i].numerical_value_in(s) << "," << out[i].numerical_value_in(V) << std::endl;
    ofs.close();

    std::ofstream ofs2("mf2.csv", std::ofstream::out);
    for (int i=0;i<out2.size();++i)
        ofs2 << ta2[i].numerical_value_in(s) << "," << out2[i].numerical_value_in(V) << std::endl;
    ofs2.close();
    return 0;
}
