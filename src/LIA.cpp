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
    bw.SetFilterOrder(10); // fixed
}

LIA::LIA(quantity<Hz> fc, quantity<isq::frequency[Hz]> sr)
{
    // fully set up filter
    bw.SetFilterOrder(10); // fixed
    bw.SetLowFilterFreq(fc);
    bw.SetSamplingRate(sr);
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
    waveform_t f1, f2; // temp
    // filter
    bw.LPassFilter(part1, f1);
    bw.LPassFilter(part2, f2);

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
