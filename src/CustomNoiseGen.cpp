// Noise, white noise generator implementation

// us
#include "CustomNoiseGen.hh"
#include "rapidcsv.hh"

// std
#include <cmath>
#include <random>
#include <algorithm>

CustomNoiseGenerator::CustomNoiseGenerator() : 
scalefactor(0.0 * V),
sampling_rate(-1.0 * Hz),
duration(0.0 * s),
seed(1234)
{
    // set in a not-ready state; must use setters after this.
    state_change = true; // not used
}


CustomNoiseGenerator::CustomNoiseGenerator(const char* fname, quantity<V> scale, quantity<Hz> sr, quantity<s> d, int sd) : 
scalefactor(scale), // Normal distribution std value
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

    std::mt19937 generator(seed);
    // enforce white noise from normal distribution
    std::normal_distribution<double> distribution(0.0,1.0);

    tsig.clear(); // ignore state_change, always new production
    int entries = static_cast<int>((duration * sampling_rate).numerical_value_in(one));
    for (int i=0;i<entries;++i) {
        tsig.push_back(scalefactor * distribution(generator));
    }
    return tsig;
}


waveform_t CustomNoiseGenerator::add(waveform_t& sig, size_t pos)
{
    // changes signal but leave state unchanged for repeat requests
    // avoiding repeated addition operations. Reset by any configuration 
    // setter call.

    // add other to existing tsig at position pos, 
    // up to at most end of tsig, irrespective of the length of other
    transform(tsig.begin()+pos,tsig.end(),sig.begin(),tsig.begin()+pos,std::plus<waveform_value>());
    return tsig;
}
