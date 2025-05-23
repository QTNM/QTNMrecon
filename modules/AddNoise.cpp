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
    onset_percent(10.0),
    SNr(1.0)
{}


DataPack AddNoise::operator()(DataPack dp)
{
    if (! dp.getRef().count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    Event<std::any> indata = dp.getRef()[inkey];
    Event<std::any> outdata; // to hold all the data items
    //    std::cout << "in add noise module." << std::endl;
    // get hold of relevant data, transform and store in outdata
    try
    {
        int nantenna = dp.getTruthRef().nantenna; // use data pack
        quantity<ns> stime = dp.getTruthRef().sampling_time;
	quantity<ns> sample_length;
        
        for (int i=0;i<nantenna;++i) {
          std::string ikey = "sampled_" + std::to_string(i) + "_V";
          auto pure = std::any_cast<vec_t>(indata[ikey]);
	  //          std::cout << "retrieve key " << ikey << " waveform of size " << pure.size() << std::endl;

          // set up the noise generator
	  double maxel, minel;
	  if (pure.size()>=50) { // expected signals are much longer, tests maybe not
	    maxel = *std::max_element(pure.begin(),pure.begin()+50); // max from initial few oscillations
	    minel = *std::min_element(pure.begin(),pure.begin()+50); // min
	  }
	  else {
	    maxel = *std::max_element(pure.begin(),pure.end()); // max
	    minel = *std::min_element(pure.begin(),pure.end()); // min
	  }
          // noise level
	  double amp = 0.5*(maxel - minel);
          quantity<V> nlevel = amp / std::numbers::sqrt2 / SNr * V; // A/sqrt(2) * noise/sig ratio
	  //	  std::cout << "amplitude in V: " << amp << " noise level in V " << nlevel << std::endl;
          noisegen.setScale(nlevel); // is std.dev of Gaussian = RMS to reach SNR=(A/sqrt2)/RMS
          noisegen.setSampling_rate(1.0/stime.numerical_value_in(s) * Hz); // inverse time!=frequency

	  // trigger onset position
	  int onset = static_cast<int>(onset_percent/100.0 * pure.size());
	  sample_length = pure.size() * stime + 2*onset * stime; // [ns], extend sample by 2*onset
          noisegen.setDuration(sample_length);
	  //	  std::cout << "add Noise - duration " << sample_length << std::endl;

          // seed is default
          // need values with unit from now on
          waveform_t res(pure.size());
          for (size_t i=0;i<pure.size();++i) res[i] = pure[i] * V; // vec_t -> waveform_t
          dp.getTruthRef().pure.push_back(pure); // copy to truth for storage; no unit

          waveform_t noisy = noisegen.add(res,onset); // use the noise generator
          std::string okey = l2out + std::to_string(i);
	  std::cout << "store key " << okey << " waveform of size " << noisy.size() << std::endl;
          outdata[okey] = std::make_any<waveform_t>(noisy);
          dp.getRef()[inkey].erase(ikey); // obsolete, have copy with unit
        }
    }
    catch(const std::bad_any_cast& e)
    {
      std::cerr << "AddNoise: " << e.what() << '\n';
    }
    dp.getRef()[outkey] = outdata;
    dp.getTruthRef().snratio = SNr;
    return dp;
}
