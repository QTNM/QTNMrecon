// Waveform interpolator module implementation

// std
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <numbers>

// tool

// us
#include "AddNoise.hh"


AddNoise::AddNoise(std::string in, std::string out) : 
    inkey(std::move(in)),
    outkey(std::move(out)),
    SNr(1.0),
    nantenna(1)
{}


DataPack AddNoise::operator()(DataPack dp)
{
    if (! dp.getRef().count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    Event<std::any> indata = dp.getRef()[inkey];
    Event<std::any> outdata; // to hold all the data items
    // get hold of relevant data, transform and store in outdata
    try
    {
        // cast the containers
        for (int i=0;i<nantenna;++i) {
            std::string ikey = "sampled_" + std::to_string(i) + "_[V]";
            auto pure = std::any_cast<vec_t>(indata[ikey]);

            // set up the noise generator
            auto stime = std::any_cast<quantity<ns>>(indata["sample_time[ns]"]); // in [ns]
            double maxel = *std::max_element(pure.begin(),pure.end()); // Amplitude A
            quantity<V> nlevel = maxel / std::numbers::sqrt2 / SNr * V; // A/sqrt(2) * noise/sig ratio

            noisegen.setScale(nlevel); // is std.dev of Gaussian = RMS to reach SNR=(A/sqrt2)/RMS
            noisegen.setSampling_rate(1.0/stime.numerical_value_in(ns) * Hz); // inverse time!=frequency
            noisegen.setDuration(pure.size()*stime);
            // seed is default
            // need values with unit from now on
            waveform_t res(pure.size());
            for (size_t i=0;i<pure.size();++i) res[i] = pure[i] * V; // vec_t -> waveform_t

            waveform_t noisy = noisegen.add(res,0); // use the noise generator
            std::string okey = "noisy_" + std::to_string(i);
            outdata[okey] = std::make_any<waveform_t>(noisy);
        }
    }
    catch(const std::bad_any_cast& e)
    {
        std::cerr << e.what() << '\n';
    }
    dp.getRef()[outkey] = outdata;
    return dp;
}
