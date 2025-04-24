// QTNM signal receiver abstract interface class
#ifndef QT_VRECEIVER_H
#define QT_VRECEIVER_H

// std
#include <vector>

// us
#include "Event.hh"

// ROOT
#include "Math/Vector3D.h" // XYZVector
#include "Math/Point3D.h" // XYZPoint

using namespace ROOT::Math;

class VReceiver
{
public:
  // abstract class -> no constructor
  virtual ~VReceiver() = default;
  
  virtual vec_t voltage_response(Event<std::any>& event) = 0;
  // must be implemented by a concrete receiver class
  // type is in units of [V], not explicitly a quantityV<> yet.
  // specific characteristics of a signal are set in that derived
  // class but the operation to generate is common to all hence
  // this interface
  virtual vec_t antenna_local_time(Event<std::any>& event) = 0;
  

protected:
  // derived classes can use these concrete member functions.

  // Electric field calculation required for all receiver responses
  // Input the source kinematics. Second is receiver location.
  // Important to clear the event map from source data since not needed afterwards.
  std::vector<XYZVector> calculate_Efield(Event<std::any>& event, XYZPoint& eval_point);
  vec_t antenna_time(Event<std::any>& event, XYZPoint& eval_point);
  static constexpr double c_m_per_ns = c_SI * 1.0e-9; // m/ns
  static constexpr double eps0_SI = 8.854187819e-12; // [F/m]
  
};
#endif
