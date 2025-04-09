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
//#include "modules/QTNMSimAntennaReader.hh"
#include "modules/FullAntennaSimReader.hh"
#include "modules/AddChirpToTruth.hh"
#include "modules/WaveformSampling.hh"
#include "modules/OmegaBeatToTruth.hh"
#include "modules/writeWfmToRoot.hh"
#include "trackMerger.hh"
#include "modules/WfmReader.hh"
#include "modules/AddNoise.hh"
#include "modules/Mixer.hh"
#include "modules/Digitize.hh"
#include "modules/writeHitDigiToRoot.hh"

#include "CLI11.hpp"
#include <Event.hh>
#include <mp-units/format.h>
#include <mp-units/ostream.h>


int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Recon Pipeline with track merger"};
  int nevents = -1;
  quantity<T> bfield = 0.7 * T; // constant sim b-field value [T]
  std::string fname = "qtnm.root";
  std::string samfname = "sampled.root";
  std::string merfname = "merged.root";
  std::string outfname = "recon.root";

  app.add_option("-n,--nevents", nevents, "<number of events> Default: -1");
  app.add_option("-i,--input", fname, "<input file name> Default: qtnm.root");
  app.add_option("-o,--output", outfname, "<output file name> Default: recon.root");

  CLI11_PARSE(app, argc, argv);

  // keys to set
  int nant = 2;
  std::string origin = "raw";
  std::string samp = "sampled";
  std::string intermediate = "wave";
  std::string noisy = "noisy";
  std::string mixed = "mixer";
  std::string l2noise = "noisy_";
  std::string l2mix = "mixed_";

  // -------------
  // Stage 1 recon
  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re1("ntuple/Signal", &ff);
  TTreeReader re2("ntuple/Score", &ff);
  auto source1 = FullAntennaSimReader(re1, re2, origin);
  // TTreeReader re("test", &ff); // read test file
  // auto source1 = QTNMSimAntennaReader(re, origin);
  source1.setMaxEventNumber(nevents); // default = all events in file
  source1.setSimConstantBField(bfield); // MUST be set
  source1.setAntennaN(nant);

  // add truth
  auto addchirp = AddChirpToTruth(origin); // default antenna number
  
  // transformer (2)
  auto interpolator = WaveformSampling(origin,"",samp);
  quantity<ns> stime = 0.008 * ns;
  interpolator.setSampleTime(stime);

  // add truth, 'omout' is just for checking
  auto addbeat = OmegaBeatToTruth(samp,"");

  // data sink: write to Root, take from key
  TFile* out1file = new TFile(samfname.data(), "RECREATE");
  TTree* tr1 = new TTree("sampled","sampled data");
  tr1->SetDirectory(out1file);

  auto sink1 = WriterWfmToRoot(samp, tr1);
  
  auto pl1 = yap::Pipeline{} | source1 | addchirp | interpolator | addbeat | sink1;
  //  auto pl1 = yap::Pipeline{} | source1 | interpolator | sink1; // test only
  
  pl1.consume();
  
  std::cout << "Stage 1: minimal recon finished. " << std::endl;
  tr1->Write();
  out1file->Close(); // free TTree
  ff.Close();

  // ---------------------
  // Track Merging stage 2
  // data source: read from ROOT file, store under key 'raw'
  TFile ff2(samfname.data(),"READ");
  TTreeReader re3("sampled", &ff2);

  // data sink: write to Root, take from key
  TFile* out2file = new TFile(merfname.data(), "RECREATE");
  TTree* tr2 = new TTree("merged","merged data");
  tr2->SetDirectory(out2file);

  // set up the track merger object
  auto merge = trackMerger(re3, tr2);

  // run
  merge.Loop();
  
  std::cout << "Track Merger finished. " << std::endl;
  tr2->Write();
  out2file->Close(); // free TTree
  ff2.Close();

  // -------------------
  // Final stage 3 recon
  // data source: read from ROOT file, store under key 'raw'
  TFile ff3(merfname.data(),"READ");
  TTreeReader re4("merged", &ff3);
  auto source3 = WfmReader(re4, intermediate);

  // add noise, step (3), fill more truth with units
  auto noiseAdder = AddNoise(intermediate, noisy, l2noise);
  noiseAdder.setSignalToNoise(10.0);
  noiseAdder.setOnsetPercent(10.0);

  // mixer, step (4), waveform in from l2 key, out in l2 key
  auto mixer = Mixer(noisy, mixed, l2noise, l2mix);
  quantity<Hz> tfreq = 100.0 * MHz;
  //  quantity<Hz> tfreq = 5.0 * GHz; // test case setting; 100 MHz normal
  mixer.setTargetFrequency(tfreq);
  mixer.setFilterCutFrequency(10*tfreq);
  //  mixer.setFilterCutFrequency(tfreq); // test case setting; 10 x tfreq normal

  // digitizer, step (5), waveform from l2 key
  auto digitizer = Digitize(mixed, l2mix);
  //  quantity<Hz> dsampling = 10.0 * GHz; // test case setting
  quantity<Hz> dsampling = 1.0 * GHz;
  quantity<V> vert = 1.0 * V;
  digitizer.setDigiSamplingRate(dsampling);
  digitizer.setVerticalRange(vert);
  digitizer.setGainFactor(1.0);
  digitizer.setBitNumber(12);

  // data sink: write to Root, take from key
  TFile* out3file = new TFile(outfname.data(), "RECREATE");
  TTree* tr3 = new TTree("recon","reconstructed data");
  tr3->SetDirectory(out3file);
  auto sink3 = WriterHitDigiToRoot(tr3);
  
  auto pl2 = yap::Pipeline{} | source3 | noiseAdder | mixer | digitizer | sink3;
  
  pl2.consume();

  std::cout << "app finished. " << std::endl;
  tr3->Write();
  out3file->Close(); // free TTree
  ff3.Close();
  
}
