// Test Geant4 Antenna signal generator module implementation

// std
#include <iostream>

// us
#include "TestG4AntGenerator.hh"
#include "yap/pipeline.h"


TestG4AntGenerator::TestG4AntGenerator(std::string out) : 
  outkey(std::move(out)),
  counter(0),
  maxEventNumber(4), // make 4 different signals for test
  nantenna(1),
  frequency(0.0*Hz),
  amplitude(-1.0*V), // signal incomplete config
  sampling_rate(0.0*Hz),
  duration(0.0*s),
  phase(0.0*rad)
{}


DataPack TestG4AntGenerator::operator()()
{
  if (amplitude<=0.0*V) {
    std::cout << "Error, TestG4AntGenerator: incomplete configuration." << std::endl;
    throw yap::GeneratorExit{};
  }
  if (counter >= maxEventNumber)
    throw yap::GeneratorExit{};
  counter++;

  // configure generator
  // first signal
  sig.setAmplitude(amplitude);
  sig.setFrequency(frequency);
  sig.setSampling_rate(sampling_rate);
  sig.setDuration(duration);
  sig.setPhase_rad(phase);
  vec_t basesig = sig.generate_pattern(); // waveform 1

  // second signal
  sig.setFrequency(frequency + 1.0*GHz); // shift by 1GHz
  sig.setDuration(duration/2.0); // half long
  vec_t track2 = sig.generate_pattern(); // waveform 2
  
  // make signal number 'counter', 4 test cases
  int eID, tID;
  switch(counter) {
  case 1:
    std::vector<int> aID;
    vec_t vvec, tvec;
    for (int j;j<basesig.size();j++) {
      for (int i=0;i<nantenna;++i) {
	aID.push_back(i);
	vvec.push_back(basesig.at(j));
	tvec.push_back(j/sampling_rate); // unit, check
      }
    }
    eID = 1;
    tID = 1; // just single waveform
  case 2:

  case 3:

  case 4:
    
  }
  
  // prepare fake Geant4 output data
  Event_map<std::any> eventmap; // data item for delivery
  Event<std::any> outdata; // to hold all the data items
  
  outdata["AntennaID"] = std::make_any<vec_t>(); // interleaved as in G4
  outdata["VoltageVec"] = std::make_any<vec_t>();
  outdata["TimeVec"] = std::make_any<vec_t>();

  std::cout << "sine gen: counter " << counter << ", in key " << outkey << std::endl;
  eventmap[outkey] = outdata;
  DataPack dp(eventmap);
  dp.getTruthRef().vertex.eventID = eID; // required for outputs
  dp.getTruthRef().vertex.trackID = tID; // required for outputs
  dp.getTruthRef().nantenna = nantenna; // required for outputs

  return dp;
}
