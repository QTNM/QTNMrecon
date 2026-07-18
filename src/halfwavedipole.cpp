// Half-Wave Dipole response implementation
#include <iostream>
#include <cmath>

// us
#include "HalfWaveDipole.hh"


HalfWaveDipole::HalfWaveDipole(XYZPoint apos, XYZVector apol, double d) : 
antPos(apos),
antPol(apol),
screen_distance(d)
{}

vec_t HalfWaveDipole::voltage_response(Event<std::any>& ev)
{
    std::vector<XYZVector> efield = calculate_Efield(ev, antPos);
    std::cout << "in half-wave dipole made e-field of size: " << efield.size() << std::endl;
    vec_t response;
    try
      {
	auto omega = std::any_cast<std::vector<double>>(ev["OmVec"]); // ang freq
	if (screen_distance > 0.0) { // screen present
	  auto pz = std::any_cast<std::vector<double>>(ev["pzVec"]); // z-points
	  for (size_t i=0; i<omega.size()-1;++i) { // check units
	    if ((screen_distance - std::fabs(pz[i])) >= 0.0) {
	      double wvlg = c_SI / (omega[i] / (2.0*myPi)); // c in SI units = wvlg in [m]
	      double resp = (wvlg/myPi * efield[i].Dot(antPol)); // unit [V]
	      response.push_back(resp);
	    }
	    else
	      response.push_back(0.0); // cut off antenna beyond screen
	  }
	}
	else {  // no screen
	  for (size_t i=0; i<omega.size()-1;++i) { // check units
	    double wvlg = c_SI / (omega[i] / (2.0*myPi)); // c in SI units = wvlg in [m]
	    double resp = (wvlg/myPi * efield[i].Dot(antPol)); // unit [V]
	    response.push_back(resp);
	  }
	}
      }
    catch (const std::bad_any_cast& e)
      {
	std::cerr << e.what() << std::endl;
      }
    return response;
}
