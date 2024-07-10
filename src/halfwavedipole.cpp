// Half-Wave Dipole response implementation
#include <iostream>

// us
#include "HalfWaveDipole.hh"

// ROOT
#include "TMath.h"

HalfWaveDipole::HalfWaveDipole(XYZPoint apos, XYZVector apol) : 
antPos(apos),
antPol(apol)
{}

vec_t HalfWaveDipole::voltage_response(Event<std::any>& ev)
{
    std::vector<XYZVector> efield = calculate_Efield(ev, antPos);
    std::cout << "in half-wave dipole made e-field of size: " << efield.size() << std::endl;
    vec_t response;
    try
    {
      quantity<Hz> avomega;
      auto omega = std::any_cast<std::vector<double>>(ev["OmVec"]); // ang freq
      double temp=0.0;
      for (size_t i=0; i<omega.size();++i) { // check units
  	    double wvlg = TMath::C() / (omega[i] / TMath::TwoPi()); // c in SI units = wvlg in [m]
	      double resp = (wvlg/TMath::Pi() * efield[i].Dot(antPol)); // unit [V]
        response.push_back(resp);
        temp += omega[i];
      }
      avomega = temp/omega.size() * Hz;
      ev["avomega_[Hz]"] = std::make_any<quantity<Hz>>(avomega);
      omega.clear();
    }
    catch (const std::bad_any_cast& e)
    {
      std::cerr << e.what() << std::endl;
    }
    return response;
}
