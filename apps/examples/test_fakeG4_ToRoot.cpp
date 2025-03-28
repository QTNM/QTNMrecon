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

// us
#include "yap/pipeline.h"
#include "modules/TestG4AntGenerator.hh"
#include "modules/FakeG4AntWriter.hh"
#include "CLI11.hpp"

int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Example Recon Pipeline"};
  std::string fname = "testG4.root";

  app.add_option("-o,--output", fname, "<output file name> Default: testG4.root");

  CLI11_PARSE(app, argc, argv);

  // data source: sine signal generator, store under key 'wave'
  quantity<V> amp = 1.0 * V;
  quantity<Hz> srate = 155.0 * GHz;
  quantity<s>  dur = 1.0 * ns;
  quantity<Hz> freq = 20.0 * GHz;
  quantity<rad> ph = 0.0 * deg;

  auto source = TestG4AntGenerator("wave");
  source.setNAntenna(2);
  source.setAmplitude(amp);
  source.setFrequency(freq);
  source.setSampling_rate(srate);
  source.setDuration(dur);
  source.setPhase_rad(ph);

  // data sink: write to Root, take from key
  TFile* outfile = new TFile(outfname.data(), "RECREATE");
  TTree* tr = new TTree("test","test data");
  tr->SetDirectory(outfile);
  auto sink = ("wave", tr);
  
  auto pl = yap::Pipeline{} | source | sink;
  
  pl.consume();

  std::cout << "in app: " << std::endl;
  tr->Write();
  outfile->Close(); // free TTree
}
