// Amplifier module for a pipeline module
#ifndef amplifier_HH
#define amplifier_HH 1

// std includes
#include <string>
#include <cmath>

// tool includes
#include "filter.hh"

// must have include for pipeline
#include <Event.hh>

class Amplifier
{
public:
  Amplifier(std::string inbox, std::string outbox, std::string l2in, std::string l2out); // constructor
  // Configures the module. Could have more config parameters
  // String Key outbox label as input.
  
  DataPack operator()(DataPack); // this is called by the pipeline
  // Transforming module event map in and out.
  
  // getter/setter methods for configuration could live here.
  inline void setBandPassWidthOnTruth(quantity<Hz> w) {bpwidth = w;}
  inline void setFilterLowerBoundFrequency(quantity<Hz> lf) {cutlowerbound = lf;}
  inline void setFilterUpperBoundFrequency(quantity<Hz> uf) {cutupperbound = uf;}
  inline void setGainFactor(double g) {gainfactor = g;}
  inline void setGainFactorPowerdB(double pg);
  inline void setGainFactorVoltagedB(double vg);
  
private:
  // include any configuration data members for internal use here.
  Butterworth bw; // band-pass filter here
  quantity<Hz> bpwidth; // use truth value as centre +- bandwidth/2
  quantity<Hz> cutlowerbound; // manual band-pass setting
  quantity<Hz> cutupperbound;
  double gainfactor; // multiplies signal
  
  // these below serve as string keys to access (read/write) the Event map
  std::string inkey;
  std::string outkey;
  std::string l2in;
  std::string l2out;
};

inline void Amplifier::setGainFactorPowerdB(double pg)
{
  // avoid extreme values, arbitrary bound
  if (pg>100.0) pg = 100.0;
  else if (pg<-100.0) pg = -100.0; // unrealistic; attenuation
  else setGainFactor(std::sqrt(std::pow(10,pg/10.0))); // sqrt: multiplies waveform [V]
}

inline void Amplifier::setGainFactorVoltagedB(double vg)
{
  // avoid extreme values, arbitrary bound
  if (vg>100.0) vg = 100.0;
  else if (vg<-100.0) vg = -100.0; // unrealistic; attenuation
  else setGainFactor(std::pow(10,vg/20.0));
}

#endif
