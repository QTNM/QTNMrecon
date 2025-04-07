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
#include "modules/WfmReader.hh"
#include "modules/AddNoise.hh"
#include "modules/Mixer.hh"
#include "modules/Digitize.hh"
#include "modules/writeHitDigiToRoot.hh"
#include "Event.hh"
#include "CLI11.hpp"


int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Example Test Read Wfm Pipeline"};
  std::string fname = "merged.root";
  std::string outfname = "recon.root";

  app.add_option("-i,--input", fname, "<input file name> Default: merged.root");
  app.add_option("-o,--output", outfname, "<output file name> Default: recon.root");

  CLI11_PARSE(app, argc, argv);

  // keys to set
  std::string origin = "wave";
  std::string noisy = "noisy";
  std::string mixed = "mixer";
  std::string l2noise = "noisy_";
  std::string l2mix = "mixed_";
  
  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re("merged", &ff);
  auto source = WfmReader(re, origin);

  // add noise, step (3), fill more truth with units
  auto noiseAdder = AddNoise(origin, noisy, l2noise);
  noiseAdder.setSignalToNoise(10.0);
  noiseAdder.setOnsetPercent(10.0);

  // mixer, step (4), waveform in from l2 key, out in l2 key
  auto mixer = Mixer(noisy, mixed, l2noise, l2mix);
  quantity<Hz> tfreq = 100.0 * MHz;
  mixer.setTargetFrequency(tfreq);

  // digitizer, step (5), waveform from l2 key
  auto digitizer = Digitize(mixed, l2mix);
  quantity<Hz> dsampling = 1.0 * GHz;
  quantity<V> vert = 1.0 * V;
  digitizer.setDigiSamplingRate(dsampling);
  digitizer.setVerticalRange(vert);
  digitizer.setGainFactor(10.0);
  digitizer.setBitNumber(12);

  // data sink: write to Root, take from key
  TFile* outfile = new TFile(outfname.data(), "RECREATE");
  TTree* tr = new TTree("recon","reconstructed data");
  tr->SetDirectory(outfile);
  auto sink = WriterHitDigiToRoot(tr);
  
  auto pl = yap::Pipeline{} | source | noiseAdder | mixer | digitizer | sink;
  
  pl.consume();

  std::cout << "app finished. " << std::endl;
  tr->Write();
  outfile->Close(); // free TTree
  ff.Close();
}
