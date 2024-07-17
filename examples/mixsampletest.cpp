// check mixing and resampling test

#include "SineGenerator.hh"
#include "DownConverter.hh"
#include "spline.h"

#include <iostream>
#include <fstream>

vec_t interpolate(const waveform_t& vvals, const quantity<Hz> srate, const quantity<Hz> targetrate)
{
    vec_t resampled;
    // time axis
    vec_t temptime(vvals.size()); // no unit for spline
    vec_t tempwave(vvals.size()); // no unit for spline
    quantity<s> stime = 1.0 / srate; // time step
    for (size_t i=0; i<vvals.size();++i) { // strip units
        temptime[i] = i * stime.numerical_value_in(s);
        tempwave[i] = vvals[i].numerical_value_in(V);
    }

    spline ip(temptime, tempwave); // interpolator
    int maxpoints = static_cast<int>(temptime.back() * targetrate.numerical_value_in(Hz)); // s * Hz
    for (int i=1;i<=maxpoints;++i)
        resampled.push_back(ip(i / targetrate.numerical_value_in(Hz))); // eval operation
    return resampled;
}

int main()
{
    // data
    quantity<Hz> srate = 10.0 * kHz; // 20 points per osc
    quantity<s>  dur = 1.0 * s; // [s]
    quantity<Hz> freq = 500.0 * Hz;

    SineGenerator sinsig(1.0*V, freq, srate, dur, 0.0*deg);
    // mixer
    DownConverter demod;
    quantity<Hz> targetf = 40 * Hz;
    demod.setLOamplitude(1.0*V);
    demod.setLOfrequency(freq - targetf);
    demod.setFilterCutoff(300.0*Hz);
    demod.setSamplingRate(srate);
    quantity<Hz> resampling = 10.0 * targetf;

    // make data
    waveform_t xin = sinsig.generate(); // pure tone

    // demodulate
    waveform_t res = demod.mix(xin); // still on original srate

    // resample
    vec_t resampled = interpolate(res, srate, resampling); // 10 points per osc sampling

    std::ofstream ofs("mixsamp.csv", std::ofstream::out);
    for (size_t i=0; i<resampled.size();++i)
        ofs << (i+1)/(resampling.numerical_value_in(Hz)) << ", " << resampled[i] << std::endl;
    ofs.close();

    return 0;
}
