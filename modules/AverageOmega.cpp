// add average Omega frequency to truth implementation

// std
#include <iostream>
#include <algorithm>

// ROOT includes
#include "TLinearFitter.h"

// us
#include "AverageOmega.hh"


AverageOmega::AverageOmega(std::string in) : 
  inkey(std::move(in))
{
}

DataPack AverageOmega::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
    if (! dp.getRef().count(inkey)) { 
        std::cout << "input key not in dictionary! Average Omega" << std::endl;
        return dp; // not found, return unchanged map, no processing
    }
    Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
    if (! indata.count("OmVec")) { 
        std::cout << "Omega vector not in dictionary!" << std::endl;
        return dp; // not found, return unchanged map, no processing
    }
    
    // block Wfms too short for processing
    if (dp.getTruthRef().tooShort) {
      std::cout << "Waveform too short to process: Average Omega." << std::endl;
      return dp;
    }

    // use omega data vector for fitting
    TLinearFitter lft; // line fit
    lft.SetDim(1);
    lft.SetFormula("pol1");
    lft.StoreData(false);
    try
      {
        // get hold of truth data from sim
	auto omvec = std::any_cast<vec_t>(indata["OmVec"]);
	auto tvec = std::any_cast<vec_t>(indata["SourceTime"]);

	int npoints = (int)omvec.size()-2; // not final 2 points from MC
	lft.AssignData(npoints, 1, tvec.data(), omvec.data());
	lft.Eval(); // fit
	double baseom = lft.GetParameter(0);  // intercept of line as average
        std::cout << "average omega [Hz]: " << baseom << std::endl;
	dp.getTruthRef().base_omega = baseom * Hz;
	dp.getTruthRef().base_bfield = e2b(dp.getTruthRef().vertex.kineticenergy, baseom*Hz); // calculate
      }
    catch(const std::bad_any_cast& e)
      {
	std::cerr << "AverageOmega: " << e.what() << '\n';
      }
    lft.Clear();
    return dp;
}

quantity<T> AverageOmega::e2b(quantity<keV> en, quantity<Hz> om)
{
  double gam = 1.0 + en.numerical_value_in(J) / (me_SI*c_SI*c_SI); // all in SI units
  quantity<T> B0 = (me_SI * gam * om.numerical_value_in(Hz) / qe_SI) * T;
  return B0;
}
