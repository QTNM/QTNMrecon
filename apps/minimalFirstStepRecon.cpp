// example application
// how to run a reconstruction pipeline
// make a main program, include and instantiate all the modules
// as required for the intended purpose and send them
// into the pipeline object.
// Needs a data source and a data sink at the least.
// Any data processing needs to be placed in between these bookends.
//

// std
#include<iostream>
#include<string>
#include<vector>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"

// us
#include "yap/pipeline.h"
#include "modules/FullAntennaSimReader.hh"
#include "modules/AddChirpToTruth.hh"
#include "modules/WaveformSampling.hh"
#include "modules/OmegaBeatToTruth.hh"
#include "modules/writeWfmToRoot.hh"

#include "CLI11.hpp"
#include <Event.hh>
#include <mp-units/format.h>
#include <mp-units/ostream.h>


int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Minimal First Step Recon Pipeline"};
  int nevents = -1;
  quantity<T> bfield = 0.7 * T; // constant sim b-field value [T]
  std::string fname = "qtnm.root";
  std::string outfname = "sampled.root";

  app.add_option("-n,--nevents", nevents, "<number of events> Default: -1");
  app.add_option("-i,--input", fname, "<input file name> Default: qtnm.root");
  app.add_option("-o,--output", outfname, "<output file name> Default: sampled.root");

  CLI11_PARSE(app, argc, argv);

  // keys to set
  std::string origin = "raw";
  std::string samp = "sampled";

  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re1("ntuple/Signal", &ff);
  TTreeReader re2("ntuple/Score", &ff);
  auto source = FullAntennaSimReader(re1, re2, origin);
  source.setMaxEventNumber(nevents); // default = all events in file
  source.setSimConstantBField(bfield); // MUST be set

  // add truth
  auto addchirp = AddChirpToTruth(origin); // default antenna number
  int nant = 2;
  addchirp.setAntennaNumber(nant);
  
  // transformer (2)
  auto interpolator = WaveformSampling(origin,"",samp);
  quantity<ns> stime = 0.008 * ns;
  interpolator.setSampleTime(stime);

  // add truth, 'omout' is just for checking
  auto addbeat = OmegaBeatToTruth(samp,"omout");

  // data sink: write to Root, take from key
  TFile* outfile = new TFile(outfname.data(), "RECREATE");
  TTree* tr = new TTree("sampled","sampled data");
  tr->SetDirectory(outfile);

  auto sink = WriterWfmToRoot(samp, tr, nant);
  
  auto pl = yap::Pipeline{} | source | addchirp | interpolator | addbeat | sink;
  
  pl.consume();
  
  std::cout << "app finished. " << std::endl;
  tr->Write();
  outfile->Close(); // free TTree
  ff.Close();
}
