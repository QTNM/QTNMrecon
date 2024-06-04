// Sine signal generator module implementation

// std
#include <iostream>

// us
#include "SineSigGenerator.hh"
#include "yap/pipeline.h"


SineSigGenerator::SineSigGenerator(std::string out) : 
    outkey(std::move(out)),
    counter(0),
    maxEventNumber(0)
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
  sig.setamplitude(amplitude);
  sig.setfrequency(frequency);
  sig.setsampling_rate(sampling_rate);
  sig.setduration(duration);
  sig.setphase_rad(phase);
}

DataPack SineSigGenerator::operator()()
{
    if (counter > maxEventNumber)
        throw yap::GeneratorExit{};
    counter++;

    Event_map<std::any> eventmap; // data item for delivery
    Event<std::any> outdata; // to hold all the data items

    outdata["waveform[V]"] = std::make_any<waveform_t>(sig.generate()); // use generator
    outdata["amplitude[V]"] = std::make_any<quantity<V>>(amplitude);
    outdata["frequency[Hz]"] = std::make_any<quantity<Hz>>(frequency);
    outdata["sampling[Hz]"] = std::make_any<quantity<Hz>>(sampling_rate);
    outdata["duration[s]"] = std::make_any<quantity<s>>(duration);
    outdata["phase[rad]"] = std::make_any<quantity<rad>>(phase);
    std::cout << "sine gen: counter " << counter << ", in key " << outkey << std::endl;
    eventmap[outkey] = outdata;
    DataPack dp(eventmap);
    return dp;
}
