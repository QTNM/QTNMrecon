// Test Geant4 Antenna signal generator module implementation

// std
#include <iostream>

// us
#include "TestG4AntGenerator.hh"
#include "yap/pipeline.h"


TestG4AntGenerator::TestG4AntGenerator(std::string out) : 
  outkey(std::move(out)),
  counter(0),
  maxEventNumber(7), // make 7 different signals, to eID=4 for test
  nantenna(1),
  eID(0),
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
  counter++; // first counter=1

  // configure generator
  // first signal
  double sampleinterval = 1.0 / sampling_rate.numerical_value_in(GHz); // [ns]
  sig.setAmplitude(amplitude); // irrelevant for test
  sig.setFrequency(frequency); // assumes tens of GHz
  sig.setSampling_rate(sampling_rate); // at big sampling rate GHz
  sig.setDuration(duration); // for short duration [ns]
  sig.setPhase_rad(phase); // not used in test
  vec_t basesig = sig.generate_pattern(); // waveform 1

  // second signal
  sig.setAmplitude(amplitude/2.0); // smaller signal
  sig.setFrequency(frequency + 1.0*GHz); // shift by 1GHz, low energy generic f shift
  sig.setDuration(duration/2.0); // half length
  vec_t track2 = sig.generate_pattern(); // waveform 2

  // std::cout << "check waveform 2: " << std::endl;
  // for (auto val: track2) std::cout << val << ", ";
  // std::cout << std::endl;
  
  // make signal number 'counter', 4 test cases
  int tID;
  int nsamples = duration.numerical_value_in(ns) / sampleinterval;
  timeshift = (int)(nsamples / 4.0); // initial shift, quarter in
  std::vector<int> aID;
  vec_t vvec, tvec;

  // prepare fake Geant4 output data
  Event_map<std::any> eventmap; // data item for delivery
  Event<std::any> outdata; // to hold all the data items

  if (counter%2) { // odd rows, eID=1,2,3,4
    eID++; // initial = 1, final eID=4 for single track tID=1 only
    tID = 1;
    for (int j=0;j<basesig.size();j++) {
      for (int i=0;i<nantenna;++i) { // interleaving like in G4 output
	aID.push_back(i);
	vvec.push_back(basesig.at(j));
	tvec.push_back(j * sampleinterval); // [ns]
      }
    }
    outdata["AntennaID"] = std::make_any<std::vector<int>>(aID);
    outdata["VoltageVec"] = std::make_any<vec_t>(vvec);
    outdata["TimeVec"] = std::make_any<vec_t>(tvec);
    
    eventmap[outkey] = outdata;
    DataPack dp(eventmap);
    dp.getTruthRef().vertex.eventID = eID; // required for outputs
    dp.getTruthRef().vertex.trackID = tID; // required for outputs
    dp.getTruthRef().nantenna = nantenna; // required for outputs
    dp.getTruthRef().average_omega = 2.0*myPi*frequency; // artificially set for later
    aID.clear();
    vvec.clear();
    tvec.clear();

    return dp;
  }

  else {  // even rows
    tID = 2; // second track, track2 half-length, shift
    timeshift = (counter-1)*timeshift; // duration/4->3d/4->5d/4
    for (int j=0;j<track2.size();j++) {
      for (int i=0;i<nantenna;++i) {
	aID.push_back(i);
	vvec.push_back(track2.at(j));
	tvec.push_back(timeshift*sampleinterval + j * sampleinterval); // [ns]
      } // offset tvec by timeshift
    }
    outdata["AntennaID"] = std::make_any<std::vector<int>>(aID);
    outdata["VoltageVec"] = std::make_any<vec_t>(vvec);
    outdata["TimeVec"] = std::make_any<vec_t>(tvec);

    eventmap[outkey] = outdata;
    DataPack dp(eventmap);
    dp.getTruthRef().vertex.eventID = eID; // required for outputs
    dp.getTruthRef().vertex.trackID = tID; // required for outputs
    dp.getTruthRef().nantenna = nantenna; // required for outputs
    dp.getTruthRef().average_omega = 2.0*myPi*(frequency+1.0*GHz); // artificially set for later
    aID.clear();
    vvec.clear();
    tvec.clear();

    return dp;
  }
}
