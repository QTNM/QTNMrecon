// Noise, custom noise generator implementation

// us
#include "CustomNoiseGen.hh"
#include "rapidcsv.hh"
#include "spline.h"
#include "dsp.hh"

// std
#include <cmath>
#include <complex>
#include <random>
#include <algorithm>

CustomNoiseGenerator::CustomNoiseGenerator() : 
noiseRMS(1.0 * V),
sampling_rate(-1.0 * Hz),
duration(0.0 * s),
seed(1234)
{
    // set in a not-ready state; must use setters after this.
    state_change = true; // not used
}


CustomNoiseGenerator::CustomNoiseGenerator(const char* fname, quantity<V> rms, quantity<Hz> sr, quantity<s> d, int sd) : 
noiseRMS(rms), // Normal distribution std value
sampling_rate(sr),
duration(d),
seed(sd)
{
    state_change = true; // not used
    // read power spectrum from file
    rapidcsv::Document doc(fname); // should handle errors
    frequencies = doc.GetColumn<double>(0); // column 0
    powerspec   = doc.GetColumn<double>(1); // column 1
}


vec_t CustomNoiseGenerator::generate_pattern()
{
    generate(); // tsig exists, strip of units
    vec_t res(tsig.size());
    for (size_t i=0;i<tsig.size();++i) res[i] = tsig[i].numerical_value_in(tsig[i].unit);
    return res;
}


waveform_t CustomNoiseGenerator::generate()
{
    // check
    if (sampling_rate<0. * Hz) {
        tsig.clear();
        return tsig; // empty for not-ready state
    }
    // tools
    std::mt19937 generator(seed);
    std::normal_distribution<double> normal(0.0,1.0);
    std::uniform_real_distribution<double> urand(0.0, 1.0);
    std::complex<double> j(0.0, 1.0); // complex i
    tsig.clear(); // ignore state_change, always new production

    // interpolation of frequency spectrum
    int entries = static_cast<int>((duration * sampling_rate).numerical_value_in(one));
    if (entries%2) entries = (entries+1) / 2; // make even if odd then half
    else entries /= 2; // half for nyquist

    double step = sampling_rate.numerical_value_in(Hz)/2.0 / entries;
    vec_t interpolatedPowerSpec;
    spline ip(frequencies, powerspec); // interpolator

    // add white noise contribution to averaged power spec at 1dB = scale 1.26
    for (int i=0;i<entries;++i) {
        interpolatedPowerSpec.push_back(ip(i * step) + 1.26*normal(generator));
    }
    
    // prepare the inverse FFT, work without units here, add-on at end
    DSP dsp;
    pattern_arg interpolatedComplexSpec; // vec of complex double
    for (double entry : interpolatedPowerSpec) { // sqrt(power) times random phase
      interpolatedComplexSpec.push_back(std::sqrt(entry) * std::exp(urand(generator) * j*2.0*Pi));
    }
    interpolatedPowerSpec.clear(); // not needed anymore
    pattern_arg interpolatedSpecFull = dsp.rfft1d(interpolatedComplexSpec, fft_dir::DIR_BWD); // inv FFT

    interpolatedComplexSpec.clear(); // not needed anymore
    vec_t temp(entries/2);
    double ssquare = 0.0;
    for (int i=0;i<entries/2;++i) {
        temp[i] = abs(interpolatedSpecFull[i].real() + interpolatedSpecFull[entries-i].imag()); // response output
        ssquare += temp[i]*temp[i];
    }
    double temprms = std::sqrt(2*ssquare/entries);
    for (int i=0;i<entries/2;++i) {
        tsig.push_back(temp[i] * noiseRMS/temprms); // scale to target RMS, carries unit [V]
    }
    return tsig;
}


waveform_t CustomNoiseGenerator::add(waveform_t& sig, size_t pos)
{
    // changes signal but leave state unchanged for repeat requests
    // avoiding repeated addition operations. Reset by any configuration 
    // setter call.

    if (tsig.empty()) generate();

    // add other to existing tsig at position pos, 
    // up to at most end of tsig, irrespective of the length of other
    transform(tsig.begin()+pos,tsig.end(),sig.begin(),tsig.begin()+pos,std::plus<waveform_value>());
    return tsig;
}
