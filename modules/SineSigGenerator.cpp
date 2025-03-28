// Sine signal generator module implementation

// std
#include <iostream>

// us
#include "SineSigGenerator.hh"
#include "yap/pipeline.h"


SineSigGenerator::SineSigGenerator(std::string out) : 
    outkey(std::move(out)),
    counter(0),
    maxEventNumber(0),
    frequency(0.0*Hz) // signal incomplete config
    {}

SineSigGenerator::SineSigGenerator(std::string out, quantity<V> amp, quantity<Hz> freq, quantity<Hz> srate, 
                                    quantity<s> dur, quantity<rad> phrad) : 
    outkey(std::move(out)),
    amplitude(amp),
    frequency(freq),
    sampling_rate(srate),
    duration(dur),
    phase(phrad),
    counter(0),
    maxEventNumber(0)
{
  std::cout << "SineSig constructor called." << std::endl;
  sig.setAmplitude(amplitude);
  sig.setFrequency(frequency);
  sig.setSampling_rate(sampling_rate);
  sig.setDuration(duration);
  sig.setPhase_rad(phase);
}

DataPack SineSigGenerator::operator()()
{
  if (frequency<=0.0*Hz) {
    std::cout << "Error, SineSigGenerator: incomplete configuration." << std::endl;
    throw yap::GeneratorExit{};
  }
  if (counter > maxEventNumber)
    throw yap::GeneratorExit{};
  counter++;
  
  Event_map<std::any> eventmap; // data item for delivery
  Event<std::any> outdata; // to hold all the data items
  
  outdata["waveform_V_0"] = std::make_any<waveform_t>(sig.generate()); // use generator
  outdata["amplitude_V_0"] = std::make_any<quantity<V>>(amplitude);
  outdata["frequency_Hz_0"] = std::make_any<quantity<Hz>>(frequency);
  outdata["sampling_Hz_0"] = std::make_any<quantity<Hz>>(sampling_rate);
  outdata["duration_s_0"] = std::make_any<quantity<s>>(duration);
  outdata["phase_rad_0"] = std::make_any<quantity<rad>>(phase);
  std::cout << "sine gen: counter " << counter << ", in key " << outkey << std::endl;
  eventmap[outkey] = outdata;
  DataPack dp(eventmap);
  dp.getTruthRef().nantenna = 1; // required for outputs
  return dp;
}
