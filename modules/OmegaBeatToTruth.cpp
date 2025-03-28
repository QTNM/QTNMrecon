// add omega beat frequency to truth implementation

// std
#include <iostream>
#include <algorithm>

// us
#include "OmegaBeatToTruth.hh"
#include <mp-units/ostream.h> // for cout stream


OmegaBeatToTruth::OmegaBeatToTruth(std::string in, std::string out) : 
  inkey(std::move(in)),
  outkey(std::move(out)) // not used beyond testing
{}

DataPack OmegaBeatToTruth::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
    if (! dp.getRef().count(inkey)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return dp; // not found, return unchanged map, no processing
    }
    Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
    Event<std::any> outdata;
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey label.
    quantity<ns> stime = dp.getTruthRef().sampling_time;
    quantity<Hz> srate = 1.0/stime.numerical_value_in(s) * Hz; // from waveform sampling
    // std::cout << "in omega beat: sample rate: " << srate << std::endl;
    // use omega data vector for fitting
    try
    {
        // get hold of truth data from sim
        auto omvec = std::any_cast<vec_t>(indata["omega"]); // sampled in wfmsampling
        // convert to waveform_t with unit, cheat to enable use of dft()
        waveform_t res;
        for (auto entry : omvec) res.push_back(entry/1.e9/(2.0*myPi) * V); // omega/2pi as double->quantity<V>
        std::vector<quantity<Hz>> freq = dsp.dft(res, srate); // FFT sampled mega
        waveform_t::iterator loc = std::max_element(res.begin()+1,res.end()); // find max, remove DC bin
        auto idx = std::distance(res.begin(), loc); // index of max
	std::cout << "loc from distance; " << idx << std::endl;
        std::cout << "frequency peak at " << freq.at(idx) << std::endl; // use index on freq vector
        dp.getTruthRef().beat_frequency = freq.at(idx); // truth output
	// outdata["omfft"] = std::any_cast<waveform_t>(res);   // q<V>, again cheated unit for use of dft()
	// outdata["omfreq"] = std::any_cast<std::vector<quantity<Hz>>>(freq); // q<Hz>
    }
    catch(const std::bad_any_cast& e)
    {
      std::cerr << "OmegaBeatToTruth: " << e.what() << '\n';
    }
    //    dp.getRef()[outkey] = outdata; // not used beyond testing
    return dp;
}
