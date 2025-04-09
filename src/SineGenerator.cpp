// Test signal sine generator implementation

// us
#include "SineGenerator.hh"
#include <mp-units/math.h>
#include <mp-units/format.h>
#include <mp-units/ostream.h>

// std
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

SineGenerator::SineGenerator() : 
amplitude(0.0 * V),
frequency(-1.0 * Hz),
sampling_rate(-1.0 * Hz),
duration(0.0 * s),
phase(0.0 * rad)
{
    // set in a not-ready state; must use setters after this.
    state_change = true;
}


SineGenerator::SineGenerator(quantity<V> amp, quantity<Hz> ff, quantity<Hz> sr, 
        quantity<s> d, quantity<rad> phr) : 
amplitude(amp),
frequency(ff),
sampling_rate(sr),
duration(d),
phase(phr)
{
    state_change = true;
}

vec_t SineGenerator::generate_pattern()
{
    if (state_change)
        generate(); 
    // tsig exists, strip of units
    vec_t res(tsig.size());
    for (size_t i=0;i<tsig.size();++i) res[i] = tsig[i].numerical_value_in(tsig[i].unit);
    return res;
}

waveform_t SineGenerator::generate()
{
    // check
    if (frequency<0. * Hz || sampling_rate<0. * Hz) {
        tsig.clear();
        std::cout << "unphysical parameters in sine generator: f=" 
                    << frequency << ", sr=" << sampling_rate << std::endl;
        return tsig; // empty for not-ready state
    }

    if (state_change)
    {
        tsig.clear();
        int entries = static_cast<int>((duration * sampling_rate).numerical_value_in(one));
        for (int i=0;i<entries;++i) {
	  quantity<s> tval = i / sampling_rate.numerical_value_in(Hz) * s;
            tsig.push_back(amplitude * sin(2*myPi * (frequency * tval).numerical_value_in(one) + 
                                            phase.numerical_value_in(rad)));
        }
        state_change = false; // recycle signal if nothing changes
    }
    return tsig;
}


waveform_t SineGenerator::add(waveform_t& sig, size_t pos)
{
    // changes signal but leave state unchanged for repeat requests
    // avoiding repeated addition operations. Reset by any configuration 
    // setter call.

    if (state_change)
        generate(); 
    // add other to existing tsig at position pos, 
    // up to at most end of tsig, irrespective of the length of other
    size_t j = 0;
    for (size_t i=pos;i<tsig.size() && j<sig.size();++i) {
      tsig[i] += sig[j];
      ++j;
    }
    return tsig;
}
