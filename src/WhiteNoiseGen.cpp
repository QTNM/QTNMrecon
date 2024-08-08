// Noise, white noise generator implementation

// us
#include "WhiteNoiseGen.hh"

// std
#include <cmath>
#include <random>
#include <algorithm>

WhiteNoiseGenerator::WhiteNoiseGenerator() : 
scalefactor(0.0 * V),
sampling_rate(-1.0 * Hz),
duration(0.0 * s),
seed(1234)
{
    // set in a not-ready state; must use setters after this.
    state_change = true; // not used
}


WhiteNoiseGenerator::WhiteNoiseGenerator(quantity<V> scale, quantity<Hz> sr, quantity<s> d, int sd) : 
scalefactor(scale), // Normal distribution std value
sampling_rate(sr),
duration(d),
seed(sd)
{
    state_change = true; // not used
}


vec_t WhiteNoiseGenerator::generate_pattern()
{
    generate(); // tsig exists, strip of units
    vec_t res(tsig.size());
    for (size_t i=0;i<tsig.size();++i) res[i] = tsig[i].numerical_value_in(tsig[i].unit);
    return res;
}


waveform_t WhiteNoiseGenerator::generate()
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


waveform_t WhiteNoiseGenerator::add(waveform_t& sig, size_t pos)
{
    // changes signal but leave state unchanged for repeat requests
    // avoiding repeated addition operations. Reset by any configuration 
    // setter call.
    if (tsig.empty()) generate();
    // add other to existing tsig at position pos, 
    // up to at most end of tsig, irrespective of the length of other
    size_t j = 0;
    for (size_t i=pos;i<tsig.size() && j<sig.size();++i) {
      tsig[i] += sig[j];
      ++j;
    }
    return tsig;
}
