// VReceiver member function implementation

#include "VReceiver.hh"


std::vector<XYZVector> VReceiver::calculate_Efield(Event<std::any>& event, XYZPoint& eval_point)
{
  // retrieve the source kinematic data
  std::vector<XYZVector> fields_at_point; // for response calculation, unit [V/m]
  try
    {
      auto px = std::any_cast<std::vector<double>>(event["pxVec"]); // position
      auto py = std::any_cast<std::vector<double>>(event["pyVec"]);
      auto pz = std::any_cast<std::vector<double>>(event["pzVec"]);
      auto bx = std::any_cast<std::vector<double>>(event["bxVec"]); // beta
      auto by = std::any_cast<std::vector<double>>(event["byVec"]);
      auto bz = std::any_cast<std::vector<double>>(event["bzVec"]);
      auto ax = std::any_cast<std::vector<double>>(event["axVec"]); // acceleration
      auto ay = std::any_cast<std::vector<double>>(event["ayVec"]);
      auto az = std::any_cast<std::vector<double>>(event["azVec"]);

      for (size_t i=0; i<px.size();++i) { // check units
	XYZPoint  pos(px[i],py[i],pz[i]);
	XYZVector beta(bx[i],by[i],bz[i]);
	XYZVector acc(ax[i],ay[i],az[i]);

	double fac   = -1*qe_SI / (4.0*myPi*eps0_SI*c_SI);
	double dist  = (eval_point - pos).R();
  
	XYZVector Runit = (eval_point - pos).Unit();
	double dummy = 1.0 - Runit.Dot(beta);
	fac /= dummy*dummy*dummy;
  
	XYZVector relFarEField = fac*(Runit.Cross((Runit-beta).Cross(acc/c_SI))) / dist;    
	XYZVector relNearEField = fac*c_SI*((1.0-beta.Mag2())*(Runit-beta)) / (dist*dist);
	fields_at_point.push_back(relFarEField + relNearEField);
      }

      px.clear(); // clean-up
      py.clear();
      pz.clear();
      bx.clear();
      by.clear();
      bz.clear();
      ax.clear();
      ay.clear();
      az.clear();  

    }
  catch (const std::bad_any_cast& e)
    {
      std::cerr << e.what() << std::endl;
    }

  return fields_at_point;
}
