// QTNM signal receiver abstract interface class
#ifndef QT_PANTENNA_H
#define QT_PANTENNA_H

// std
#include <vector>
#include <cmath>

// us
#include "VReceiver.hh"

// ROOT
#include "Math/Vector3D.h" // XYZVector
#include "Math/Point3D.h" // XYZPoint

using namespace ROOT::Math;

class PatchAntenna : public VReceiver
{
public:
  PatchAntenna(XYZPoint antennaPos, XYZVector antennaPlaneNormal, double w, double h, double er); // constructor
  virtual ~PatchAntenna() = default;
  
  vec_t voltage_response(Event<std::any>& event) override;
  // must be implemented by a concrete receiver class
  // numbers in units of [V].
  // specific characteristics of a signal are set in that derived
  // class but the operation to generate is common to all hence
  // this interface
  inline vec_t antenna_local_time(Event<std::any>& event) {return antenna_time(event, antPos);}
  

private:
  XYZPoint  antPos;    // input geometry
  XYZVector antPol;    // input geometry
  double width;
  double height;
  double epsr;
};

#endif
