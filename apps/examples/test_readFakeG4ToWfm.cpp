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
#include "TTree.h"
#include "TTreeReader.h"

// us
#include "yap/pipeline.h"
#include "modules/QTNMSimAntennaReader.hh"
#include "modules/WaveformSampling.hh"
#include "modules/writeWfmToRoot.hh"
#include "Event.hh"
#include "CLI11.hpp"


int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Example Test Recon Pipeline"};
  int nevents = -1;
  quantity<T> bfield = 0.7 * T; // constant sim b-field value [T]
  std::string fname = "qtnm.root";
  std::string outfname = "sampled.root";

  app.add_option("-n,--nevents", nevents, "<number of events> Default: -1");
  app.add_option("-i,--input", fname, "<input file name> Default: qtnm.root");
  app.add_option("-o,--output", outfname, "<output file name> Default: sampled.root");

  CLI11_PARSE(app, argc, argv);

  // keys to set
  std::string origin = "wave";
  std::string samp   = "sampled";
  int nant = 2;
  
  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re("test", &ff);
  auto source = QTNMSimAntennaReader(re, origin);
  source.setMaxEventNumber(nevents); // default = all events in file
  source.setSimConstantBField(bfield); // MUST be set
  source.setAntennaN(nant);

  // transformer
  auto interpolator = WaveformSampling(origin,"",samp);
  quantity<ns> stime = 0.008 * ns;
  interpolator.setSampleTime(stime);
  
  // data sink: write to Root, take from key
  TFile* outfile = new TFile(outfname.data(), "RECREATE");
  TTree* tr = new TTree("sampled","sampled data");
  tr->SetDirectory(outfile);

  auto sink = WriterWfmToRoot(samp, tr);
  
  auto pl = yap::Pipeline{} | source | interpolator | sink;
  
  pl.consume();

  std::cout << "app finished. " << std::endl;
  tr->Write();
  outfile->Close(); // free TTree
  ff.Close();
}
