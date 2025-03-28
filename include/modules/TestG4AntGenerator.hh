// Sine signal generator for a pipeline module
#ifndef testG4generator_HH
#define testG4generator_HH 1

// std includes
#include <string>

// tool includes

// must have include for pipeline
#include <Event.hh>
#include "SineGenerator.hh"

class TestG4AntGenerator
{
public:
  TestG4AntGenerator(std::string outbox); // constructor; required
  // Configures the module. Could have more config parameters
  // String Key outbox label as input.
  
  DataPack operator()(); // this is called by the pipeline
  // Generators deliver an event map and receive nothing as signature.
  
  // getter/setter methods for configuration could live here.
  inline void setNAntennna(int na) {nantenna = na;}
  inline void setAmplitude(quantity<V>  a) {amplitude = a;}
  inline void setFrequency(quantity<Hz> ff) {frequency = ff;}
  inline void setSampling_rate(quantity<Hz> sr) {sampling_rate = sr;}
  inline void setDuration(quantity<s> d) {duration = d;}
  inline void setPhase_rad(quantity<rad> phr) {phase = phr;}
  
private:
  // include any configuration data members for internal use here.
  int eID;
  int nantenna;
  int timeshift;
  quantity<V>   amplitude;
  quantity<Hz>  frequency;
  quantity<Hz>  sampling_rate;
  quantity<s>   duration;
  quantity<rad> phase;
  SineGenerator sig; // default constructor
  
  // these below serve as string keys to access (read/write) the Event map
  std::string outkey;
  int counter;
  int maxEventNumber; // hard-coded for test operation
};
#endif
