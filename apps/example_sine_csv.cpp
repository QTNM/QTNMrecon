// example application
// how to run a reconstruction pipeline
// make a main program, include and instantiate all the modules
// as required for the intended purpose and send them
// into the pipeline object.
// Needs a data source and a data sink at the least.
// Any data processing needs to be placed in between these bookends.
//
// Current data model assumes the exchange of an Event_map from module to module
// with the definition in Event.hh

// std
#include<iostream>
#include<fstream>
#include<string>
#include<stdexcept>

// us
#include "yap/pipeline.h"
#include "modules/SineSigGenerator.hh"
#include "modules/xCsvWriter.hh"
#include "CLI11.hpp"

int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Example Recon Pipeline"};
  int nevents = 0;
  std::string fname = "out.csv";

  app.add_option("-n,--nevents", nevents, "<number of events> Default: 0");
  app.add_option("-o,--output", fname, "<output file name> Default: out.csv");

  CLI11_PARSE(app, argc, argv);

  // data source: sine signal generator, store under key 'wave'
  quantity<V> amp = 1.0 * V;
  quantity<Hz> srate = 25.0 * Hz;
  quantity<s>  dur = 2.0 * s;
  quantity<Hz> freq = 1.0 * Hz;
  quantity<rad> ph = 0.0 * deg;

  auto source = SineSigGenerator("wave",amp,freq,srate,dur,ph);
  source.setMaxEventNumber(nevents); // default = one event

  // data sink: write to disk, take from key 'wave', L2 key 'waveform[V]'
  // assumes L2 key has a waveform_t
  std::ofstream ofs(fname, std::ofstream::out); // open
  if (! ofs.is_open()) {
    throw std::logic_error("Cannot open output file");
  }
  ofs << "Event ID,Wave value[V]\n" << std::flush; // header
  auto sink   = xCsvWriter(ofs, "wave", "waveform[V]");
  
  auto pl = yap::Pipeline{} | source | sink;
  
  pl.consume();
  ofs.close();
}
