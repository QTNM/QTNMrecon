// Test signal sine generator implementation

// us
#include "ChirpGenerator.hh"
#include <mp-units/math.h>

// std
#include <cmath>
#include <algorithm>
#include <functional>

ChirpGenerator::ChirpGenerator() : 
amplitude(0.0 * V),
frequency(-1.0 * Hz),
chirprate(0.0 * Hz*Hz),
sampling_rate(-1.0 * Hz),
duration(0.0 * s),
phase(0.0 * rad)
{
    // set in a not-ready state; must use setters after this.
    state_change = true;
}


ChirpGenerator::ChirpGenerator(quantity<V> amp, quantity<Hz> ff, quantity<Hz*Hz> ch, 
        quantity<Hz> sr, quantity<s> d, quantity<rad> phr) : 
amplitude(amp),
frequency(ff),
chirprate(ch),
sampling_rate(sr),
duration(d),
phase(phr)
{
    state_change = true;
}


vec_t ChirpGenerator::generate_pattern()
{
    if (state_change)
        generate(); // tsig exists, strip of units
    vec_t res(tsig.size());
    for (size_t i=0;i<tsig.size();++i) res[i] = tsig[i].numerical_value_in(tsig[i].unit);
    return res;
}


waveform_t ChirpGenerator::generate()
{
    // check
    if (frequency<0.*Hz || sampling_rate<0.*Hz) {
        tsig.clear();
        return tsig; // empty for not-ready state
    }

    if (state_change)
    {
        tsig.clear();
        int entries = static_cast<int>((duration * sampling_rate).numerical_value_in(one));
        for (int i=0;i<entries;++i) {
            quantity<s> tval = i / sampling_rate;
            // define linear chirp
            tsig.push_back(amplitude * sin(2*myPi * (frequency * tval + 
                     0.5* chirprate * tval*tval).numerical_value_in(one) + phase.numerical_value_in(rad)));
        }
        state_change = false; // recycle signal if nothing changes
    }
    return tsig;
}


waveform_t ChirpGenerator::add(waveform_t& sig, size_t pos)
{
    // changes signal but leave state unchanged for repeat requests
    // avoiding repeated addition operations. Reset by any configuration 
    // setter call.

    // add other to existing tsig at position pos, 
    // up to at most end of tsig, irrespective of the length of other
    std::transform(tsig.begin()+pos,tsig.end(),sig.begin(),tsig.begin()+pos,std::plus<waveform_value>());
    return tsig;
}
