// --------------------
// LIA methods implementation

// std
#include <algorithm>
#include <functional>
#include <iostream>
// us
#include <mp-units/math.h>
#include <mp-units/format.h>
#include <mp-units/ostream.h>

#include "LIA.hh"

LIA::LIA()
{
    bw.setFilterOrder(10); // fixed
}

LIA::LIA(quantity<Hz> fc, quantity<Hz> sr)
{
    // fully set up filter
    bw.setFilterOrder(10); // fixed
    bw.setLowFilterFreq(fc);
    bw.setSamplingRate(sr);
}

LIA::~LIA() {} // default

waveform_t LIA::quadrature(waveform_t& signal)
{
    waveform_t res;
    // check
    if (reference.empty() || coreference.empty()) {
        std::cout << "ref or cross-ref empty" << std::endl;
        return res; // not set-up, return empty
    }
    waveform_t part1, part2; // temp
    // mix
    for (size_t ii=0;ii<signal.size();++ii)
    {
        part1.push_back(signal[ii] * reference[ii]);
        part2.push_back(signal[ii] * coreference[ii]);
    }
    std::cout << std::endl;

    // filter
    waveform_t f1 = bw.LPassFilter(part1);
    waveform_t f2 = bw.LPassFilter(part2);

    // to quadrature of arrays
    for (size_t i=0;i<f1.size();++i)
        res.push_back(sqrt(f1[i]*f1[i] + f2[i]*f2[i]));
    return res;
}

waveform_t LIA::quadrature(digi_t& dsignal)
{
    waveform_t sig(dsignal.size()); // upcasting to vector<quantity> from vector<short>
    std::transform(dsignal.begin(),dsignal.end(),sig.begin(),
                    [](short x){return static_cast<waveform_value>(x * V);});
    return quadrature(sig);
}
