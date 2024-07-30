// Waveform mixer module implementation

// std
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

// us
#include "Mixer.hh"
#include <mp-units/ostream.h> // for cout stream


Mixer::Mixer(std::string in, std::string out, std::string l2in, std::string l2out) : 
    inkey(std::move(in)),
    outkey(std::move(out)),
    l2in(std::move(l2in)),
    l2out(std::move(l2out)),
    targetFrequency(1.0 * Hz)
{}


DataPack Mixer::operator()(DataPack dp)
{
    if (! dp.getRef().count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    Event<std::any> indata = dp.getRef()[inkey];
    Event<std::any> outdata; // to hold all the data items
    // tools
    std::cout << "in mixer module." << std::endl;
    // mixer
    demod.setLOamplitude(1.0*V);
    demod.setFilterCutoff(10.0*targetFrequency);

    // get hold of relevant data, transform and store in outdata
    try
    {
        // cast the containers; not the same L1 dictionary!
        dp.getExperimentRef().target_frequency = targetFrequency;
	std::cout << "mixer module " << targetFrequency << " target freq" << std::endl;
        int nantenna = dp.getTruthRef().nantenna; // use data pack
        quantity<ns> stime = dp.getTruthRef().sampling_time;
        demod.setSamplingRate(1.0/stime.numerical_value_in(s) * Hz);
	std::cout << "mixer module " << stime << " sampling time set" << std::endl;

        quantity<Hz> avom = dp.getTruthRef().average_omega;
        quantity<Hz> losc = (avom/(2.0*myPi)) - targetFrequency;
        demod.setLOfrequency(losc); // mixer fully configured
	std::cout << "mixer module " << losc << " losc freq set" << std::endl;

        for (int i=0;i<nantenna;++i) {
            std::string ikey = l2in + std::to_string(i);
            auto pure = std::any_cast<waveform_t>(indata[ikey]);
            // sine wave gen with losc; multiply with pure, LPassFilter, new time interval
            // sample_time_[ns] in event.
            waveform_t mixedsig = demod.mix(pure); // use down converter.
	    std::cout << "mixer module; mixed wfm size " << mixedsig.size() << std::endl;

            std::string okey = l2out + std::to_string(i);
            outdata[okey] = std::make_any<waveform_t>(mixedsig);
            //dp.getRef()[inkey].erase(ikey); // obsolete, have transformed
        }
    }
    catch(const std::bad_any_cast& e)
    {
      std::cerr << "Mixer: " << e.what() << '\n';
    }
    dp.getRef()[outkey] = outdata;
    return dp;
}

