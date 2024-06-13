// Half-Wave Dipole response implementation

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
    vec_t response;
    try
    {
      auto omega = std::any_cast<std::vector<double>>(ev["omega"]); // ang freq
      for (size_t i=0; i<omega.size();++i) { // check units
	double wvlg = TMath::C() / (omega[i] / TMath::TwoPi()); // c in SI units = wvlg in [m]
	double resp = (wvlg/TMath::Pi() * efield[i].Dot(antPol)); // unit [V]
        response.push_back(resp);
      }
      omega.clear();
    }
    catch (const std::bad_any_cast& e)
    {
      std::cerr << e.what() << std::endl;
    }
    return response;
}
