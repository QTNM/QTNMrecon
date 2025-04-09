// ------------------------------------
// DownConverter methods implementation

// std
#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>

// us
#include <mp-units/math.h>
#include <mp-units/format.h>
#include <mp-units/ostream.h>

#include "DownConverter.hh"

DownConverter::DownConverter() : 
sampling_rate(-1.0 * Hz)
{}

waveform_t DownConverter::mix(waveform_t& signal)
{
    // minimal check
    if (sampling_rate<0.0 * Hz) {
        throw std::logic_error("tool settings missing, at least on sampling rate!");
    }

    // set up rest of filter
    bw.setSamplingRate(sampling_rate);
    bw.setFilterOrder(10);

    // set up rest of local oscillator signal
    lo.setPhase_rad(0.0 * rad); // sin() with phase 0 rad
    lo.setSampling_rate(sampling_rate);
    lo.setDuration(signal.size() / sampling_rate);

    // multiplication RF with LO
    waveform_t temp(signal.size()); // with unit
    vec_t lo_in = lo.generate_pattern(); // LO generation, no unit
    std::transform(signal.begin(),signal.end(),lo_in.begin(),temp.begin(),
                    [this](quantity<V> x, double y){return x*y;});

    // filter
    //    std::cout << "in demod:mix - filter gets temp size " << temp.size() << std::endl;
    waveform_t res = bw.LPassFilter(temp); // cut frequency set in configuration of down converter

    return res;
}
