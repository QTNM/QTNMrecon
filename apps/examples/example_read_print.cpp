// example application
// how to run a reconstruction pipeline
// make a main program, include and instantiate all the modules
// as required for the intended purpose and send them
// into the pipeline object.
// Needs a data source and a data sink at the least.
// Any data processing needs to be placed in between these bookends.
//
// Current data model assumes the exchange of a DataPack from module to module
// with the definition in Event.hh

// std
#include<iostream>
#include<string>

// ROOT
#include "TFile.h"
#include "TTreeReader.h"

// us
#include "yap/pipeline.h"
#include "modules/QTNMSimAntennaReader.hh"
#include "modules/AddChirpToTruth.hh"
#include "modules/printSimReader.hh"
#include "CLI11.hpp"

int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Example Recon Pipeline"};
  int nevents = -1;
  int nant = 2;
  quantity<T> bfield = 0.7 * T; // constant sim b-field value [T]
  std::string fname = "qtnm.root";

  app.add_option("-n,--nevents", nevents, "<number of events> Default: -1");
  app.add_option("-i,--input", fname, "<input file name> Default: qtnm.root");

  CLI11_PARSE(app, argc, argv);

  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re("ntuple/Signal", &ff);
  auto source = QTNMSimAntennaReader(re, "raw");
  source.setMaxEventNumber(nevents); // default = all events in file
  source.setSimConstantBField(bfield); // MUST be set
  source.setAntennaN(nant);

  // add truth
  auto addchirp = AddChirpToTruth("raw");

  // data sink: simply print to screen, take from key 'raw'
  auto sink   = printSimReader("raw");
  
  auto pl = yap::Pipeline{} | source | addchirp | sink;
  
  pl.consume();
  ff.Close();
}
