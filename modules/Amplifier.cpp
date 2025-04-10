// Waveform amplifier module implementation

// std
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

// us
#include "Amplifier.hh"
#include <mp-units/ostream.h> // for cout stream


Amplifier::Amplifier(std::string in, std::string out, std::string l2in, std::string l2out) : 
    inkey(std::move(in)),
    outkey(std::move(out)),
    l2in(std::move(l2in)),
    l2out(std::move(l2out)),
    bpwidth(-1.0 * Hz),
    cutlowerbound(0.0 * Hz),
    cutupperbound(0.0 * Hz),
    gainfactor(0.0)
{}


DataPack Amplifier::operator()(DataPack dp)
{
  if (! dp.getRef().count(inkey)) { 
    throw std::logic_error("input key not in dictionary!");
  }
  Event<std::any> indata = dp.getRef()[inkey];
  Event<std::any> outdata; // to hold all the data items

  // filter set up
  quantity<Hz> avom = dp.getTruthRef().average_omega;
  quantity<Hz> centre = avom/(2.0*myPi);
  quantity<ns> stime = dp.getTruthRef().sampling_time;
  bw.setSamplingRate(1.0 / stime.numerical_value_in(s) * Hz);
  bw.setFilterOrder(12); // multiple of 4 easiest
  
  if (bpwidth>0.0*Hz) // default changed hence use this method
    bw.setBPassFilterFreqs(centre-bpwidth/2.0, centre+bpwidth/2.0);

  else { // careful with manual settings
    if (cutlowerbound > centre || cutupperbound < centre) {
      std::cout << "WARNING: filter interval excludes simulation mean signal freq." << std::endl;
      std::cout << "======= no Amplifier operation." << std::endl;
      return dp;
    }
    else if (cutlowerbound>=cutupperbound) {
      std::cout << "WARNING filter bounds not set or wrong order." << std::endl;
      std::cout << "======= no Amplifier operation." << std::endl;
      return dp;
    }
    else
      bw.setBPassFilterFreqs(cutlowerbound, cutupperbound); // centre in interval
  }
  
  // get hold of relevant data, transform and store in outdata
  try
    {
      // cast the containers; not the same L1 dictionary!
      int nantenna = dp.getTruthRef().nantenna; // use data pack
      for (int i=0;i<nantenna;++i) {
	std::string ikey = l2in + std::to_string(i);
	auto wfm = std::any_cast<waveform_t>(indata[ikey]);
	waveform_t filtsig = bw.BPassFilter(wfm); // use band-pass filter
	std::cout << "filtered wfm size " << filtsig.size() << std::endl;
	std::transform(filtsig.begin(),filtsig.end(),
		       filtsig.begin(),[](waveform_value x){return x*gainfactor;}); // in-place
	
	std::string okey = l2out + std::to_string(i);
	outdata[okey] = std::make_any<waveform_t>(filtsig);
	dp.getRef()[inkey].erase(ikey); // obsolete, have transformed
      }
    }
  catch(const std::bad_any_cast& e)
    {
      std::cerr << "Mixer: " << e.what() << '\n';
    }
  dp.getRef()[outkey] = outdata;
  return dp;
}

