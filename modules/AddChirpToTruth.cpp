// add chirp rate to truth implementation

// std
#include <iostream>
#include <algorithm>

// ROOT includes
#include "TLinearFitter.h"

// us
#include "AddChirpToTruth.hh"


AddChirpToTruth::AddChirpToTruth(std::string in) : 
    inkey(std::move(in))
{
}

DataPack AddChirpToTruth::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
    if (! dp.getRef().count(inkey)) { 
        std::cout << "input key not in dictionary! Add Chirp" << std::endl;
        return dp; // not found, return unchanged map, no processing
    }
    Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey label.
    if (! indata.count("KEVec")) { 
        std::cout << "KE vector not in dictionary!" << std::endl;
        return dp; // not found, return unchanged map, no processing
    }
    // block Wfms too short for processing
    if (dp.getTruthRef().tooShort) {
      std::cout << "Waveform too short to process: flag set by reader." << std::endl;
      return dp;
    }
    
    // use KEvec data vector for fitting
    TLinearFitter lft;
    lft.SetDim(1);
    lft.SetFormula("pol1");
    lft.StoreData(false);
    try
    {
        // get hold of truth data from sim
        auto temptiv = std::any_cast<vec_t>(indata["SourceTime"]); // [ns] from file
        auto ov = std::any_cast<vec_t>(indata["KEVec"]); // KE vector in keV
	// std::cout << "ST size: " << temptiv.size() << " KEV size: " << ov.size() << std::endl;

	int npoints = std::min((int)ov.size(), 100000); // don't fit too many points for a line
        lft.AssignData(npoints, 1, temptiv.data(), ov.data());
        lft.EvalRobust(0.9); // allow 10% outlier data
        double tslope = lft.GetParameter(1); // fit result for chirp rate [keV/ns]
        double tinter = lft.GetParameter(0); // fit result for intercept [keV]
	quantity<keV> i0 = tinter * keV; // manual unit
	quantity<keV> i1 = i0 + tslope * 1.e3 * keV; // KE after 1 mus
	//        std::cout << "loss/mus: " << (i1-i0).numerical_value_in(eV) << " from " << i0 << std::endl;

	quantity<Hz> fslope = e2f(i1, dp.getTruthRef().vertex.vertex_bfield)
	  - e2f(i0, dp.getTruthRef().vertex.vertex_bfield);
        std::cout << "frequency chirp per mus: " << fslope << std::endl;
        dp.getTruthRef().chirp_rate = fslope*1.e6 / s; // store truth Hz/s
    }
    catch(const std::bad_any_cast& e)
    {
      std::cerr << "AddChirpTo Truth: " << e.what() << '\n';
    }
    lft.Clear();
    dp.getRef()[inkey].erase("KEVec"); // used, obsolete
    return dp;
}

quantity<Hz> AddChirpToTruth::e2f(quantity<keV> en, quantity<T> bf)
{
  double gam = 1.0 + en.numerical_value_in(J)/(me_SI*c_SI*c_SI); // SI units
  quantity<Hz> f0 = bf.numerical_value_in(T) * qe_SI / (2.0*myPi) / (gam * me_SI) * Hz;
  return f0;
}
