// Waveform digitizer module implementation

// std
#include <iostream>
#include <stdexcept>

// us
#include "Digitize.hh"
// tool
#include "spline.h"
#include <mp-units/ostream.h> // for cout stream                                                                                  


Digitize::Digitize(std::string in, std::string l2in) : 
    inkey(std::move(in)),
    l2in(std::move(l2in)),
    vrange(1.0 * V),
    gain(1.0),
    bits(12),
    digisampling(1.0 * GHz)
{}


DataPack Digitize::operator()(DataPack dp)
{
    if (! dp.getRef().count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    Event<std::any> indata = dp.getRef()[inkey];
    quantity<ns> stime = dp.getTruthRef().sampling_time;

    // get hold of relevant data, transform and store in outdata
    try
    {
        // set up the digitizer
        adc.setVRange(vrange);
        adc.setADCBits(bits);
        adc.setGainFactor(gain);

        int nantenna = dp.getTruthRef().nantenna; // use data pack
        dp.getExperimentRef().gain = gain; // store for output
        dp.getExperimentRef().digi_sampling_rate = digisampling; // store for output
	std::cout << "in digitizer " << digisampling << " digital sampling rate." << std::endl;
        // cast the containers
        for (int i=0;i<nantenna;++i) {
            std::string ikey = l2in + std::to_string(i);
            auto sig = std::any_cast<waveform_t>(indata[ikey]);
	    std::cout << "digi input got " << sig.size() << " signal size." << std::endl;
            // new sampling with digi sampling
	    waveform_t resampled = interpolate(sig, stime); // N points per osc sampling
	    std::cout << "digi interpolate gives " << resampled.size() << " resampled size." << std::endl;
            // digitize
            waveform_t dsig = adc.digitize(resampled); // use the digitizer
	    std::cout << "digitize gives " << dsig.size() << " dsig size." << std::endl;
            dp.getExperimentRef().signals.push_back(dsig); // copy to experiment_t for storage
            dp.getRef()[inkey].erase(ikey); // obsolete, have transformed
        }
    }
    catch(const std::bad_any_cast& e)
    {
        std::cerr << e.what() << '\n';
    }

    return dp;
}


waveform_t Digitize::interpolate(const waveform_t& vvals, const quantity<s> stime)
{
    waveform_t resampled;
    // time axis as it arrives
    vec_t temptime(vvals.size()); // no unit for spline
    vec_t tempwave(vvals.size()); // no unit for spline
    for (size_t i=0; i<vvals.size();++i) { // strip units
        temptime[i] = i * stime.numerical_value_in(s);
        tempwave[i] = vvals[i].numerical_value_in(V);
    }

    spline ip(temptime, tempwave); // interpolator, default constructor
    int maxpoints = static_cast<int>(temptime.back() * digisampling.numerical_value_in(Hz)); // s * Hz
    for (int i=1;i<=maxpoints;++i)
        resampled.push_back(ip(i / digisampling.numerical_value_in(Hz)) * V); // eval operation
    return resampled;
}

