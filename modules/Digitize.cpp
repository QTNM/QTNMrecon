// Waveform interpolator module implementation

// std
#include <iostream>
#include <stdexcept>

// tool

// us
#include "Digitize.hh"

Digitize::Digitize(std::string in, std::string out) : 
    inkey(std::move(in)),
    outkey(std::move(out)),
    vrange(1.0 * V),
    bits(12),
    nantenna(1)
{}


DataPack Digitize::operator()(DataPack dp)
{
    if (! dp.getRef().count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    Event<std::any> indata = dp.getRef()[inkey];
    Event<std::any> outdata; // to hold all the data items
    // get hold of relevant data, transform and store in outdata
    try
    {
        // set up the digitizer
        adc.setVRange(vrange);
        adc.setADCBits(bits);
        // cast the containers
        for (int i=0;i<nantenna;++i) {
            std::string ikey = "noisy_" + std::to_string(i);
            auto sig = std::any_cast<waveform_t>(indata[ikey]);

            waveform_t dsig = adc.digitize(sig); // use the digitizer
            std::string okey = "signal_" + std::to_string(i);
            outdata[okey] = std::make_any<waveform_t>(dsig);
        }
    }
    catch(const std::bad_any_cast& e)
    {
        std::cerr << e.what() << '\n';
    }
    dp.getRef()[outkey] = outdata;
    return dp;
}
