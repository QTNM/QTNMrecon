// Waveform interpolator module implementation

// std
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <numbers>

// tool

// us
#include "AddNoise.hh"
#include <mp-units/ostream.h> // for cout stream


AddNoise::AddNoise(std::string in, std::string out, std::string l2out) : 
    inkey(std::move(in)),
    outkey(std::move(out)),
    l2out(std::move(l2out)),
    SNr(1.0)
{}


DataPack AddNoise::operator()(DataPack dp)
{
    if (! dp.getRef().count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    Event<std::any> indata = dp.getRef()[inkey];
    Event<std::any> outdata; // to hold all the data items
    std::cout << "in add noise module." << std::endl;
    // get hold of relevant data, transform and store in outdata
    try
    {
        // cast the containers
        int nantenna = dp.getTruthRef().nantenna; // use data pack
        quantity<ns> stime = dp.getTruthRef().sampling_time;
        for (int i=0;i<nantenna;++i) {
            std::string ikey = "sampled_" + std::to_string(i) + "_[V]";
            auto pure = std::any_cast<vec_t>(indata[ikey]);
	    std::cout << "retrieve key " << ikey << " waveform of size " << pure.size() << std::endl;

            // set up the noise generator
            double maxel = *std::max_element(pure.begin(),pure.end()); // Amplitude A
            quantity<V> nlevel = maxel / std::numbers::sqrt2 / SNr * V; // A/sqrt(2) * noise/sig ratio
	    std::cout << "noise level in V " << nlevel << std::endl;
            noisegen.setScale(nlevel); // is std.dev of Gaussian = RMS to reach SNR=(A/sqrt2)/RMS
            noisegen.setSampling_rate(1.0/stime.numerical_value_in(s) * Hz); // inverse time!=frequency
            noisegen.setDuration(pure.size()*stime);
	    std::cout << "duration set at " << pure.size()*stime << std::endl;
            // seed is default
            // need values with unit from now on
            waveform_t res(pure.size());
            for (size_t i=0;i<pure.size();++i) res[i] = pure[i] * V; // vec_t -> waveform_t
            dp.getTruthRef().pure.push_back(res); // copy to truth for storage; with unit

            waveform_t noisy = noisegen.add(res,0); // use the noise generator
            std::string okey = l2out + std::to_string(i);
	    std::cout << "store key " << okey << " waveform of size " << noisy.size() << std::endl;
            outdata[okey] = std::make_any<waveform_t>(noisy);
            dp.getRef()[inkey].erase(ikey); // obsolete, have copy with unit
        }
    }
    catch(const std::bad_any_cast& e)
    {
        std::cerr << e.what() << '\n';
    }
    dp.getRef()[outkey] = outdata;
    dp.getTruthRef().snratio = SNr;
    return dp;
}
