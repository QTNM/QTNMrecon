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
#include "Math/Vector3D.h" // XYZVector
#include "Math/Point3D.h" // XYZPoint

using namespace ROOT::Math;

// us
#include "yap/pipeline.h"
#include "modules/QTNMSimKinematicsReader.hh"
#include "modules/AddChirpToTruth.hh"
#include "modules/AntennaResponse.hh"
#include "receiver/HalfWaveDipole.hh"
#include "modules/WaveformSampling.hh"
#include "modules/OmegaBeatToTruth.hh"
#include "modules/AddNoise.hh"
#include "modules/Mixer.hh"
#include "modules/Digitize.hh"
#include "modules/writeDigitizerToRoot.hh"

#include "CLI11.hpp"
#include <Event.hh>
#include "types.hh"
#include <mp-units/format.h>
#include <mp-units/ostream.h>


int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Example Recon Pipeline"};
  int nevents = -1;
  quantity<T> bfield = 0.7 * T; // constant sim b-field value [T]
  std::string fname = "qtnm.root";
  std::string outfname = "recon.root";

  app.add_option("-n,--nevents", nevents, "<number of events> Default: -1");
  app.add_option("-i,--input", fname, "<input file name> Default: qtnm.root");
  app.add_option("-o,--output", outfname, "<output file name> Default: recon.root");

  CLI11_PARSE(app, argc, argv);

  // keys to set
  std::string origin = "raw";
  std::string resp = "response";
  std::string samp = "sampled";
  std::string noisy = "noisy";
  std::string mixed = "mixer";
  std::string l2noise = "noisy_";
  std::string l2mix = "mixed_";

  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re("ntuple/Signal", &ff);
  auto source = QTNMSimKinematicsReader(re, origin);
  source.setMaxEventNumber(nevents); // default = all events in file
  source.setSimConstantBField(bfield); // MUST be set

  // add truth
  auto addchirp = AddChirpToTruth(origin); // default antenna number

  // transformer (1)
  int nant = 2;
  auto antresponse = AntennaResponse(origin, resp);
  // configure antennae
  std::vector<VReceiver*> allantenna;
  XYZPoint  apos1(0.027, 0.0, 0.0); // fix from geometry, SI unit [m]
  XYZVector apol1(0.0, 1.0, 0.0); // unit vector
  VReceiver* ant1 = new HalfWaveDipole(apos1, apol1); // insert as pointer
  allantenna.push_back(ant1);
  XYZPoint  apos2(0.0, 0.027, 0.0); // fix from geometry
  XYZVector apol2(1.0, 0.0, 0.0); // unit vector
  VReceiver* ant2 = new HalfWaveDipole(apos2, apol2); // insert as pointer
  allantenna.push_back(ant2);

  antresponse.setAntennaCollection(allantenna); // finished antenna configuration

  // transformer (2)
  auto interpolator = WaveformSampling(origin,resp,samp);
  quantity<ns> stime = 0.008 * ns;
  interpolator.setSampleTime(stime);

  // add truth, 'omout' is just for checking
  auto addbeat = OmegaBeatToTruth(samp,"omout");

  // add noise, step (3), fill more truth with units
  auto noiseAdder = AddNoise(samp, noisy, l2noise);
  noiseAdder.setSignalToNoise(1.0);

  // mixer, step (4), waveform in from l2 key, out in l2 key
  auto mixer = Mixer(noisy, mixed, l2noise, l2mix);
  quantity<Hz> tfreq = 50.0 * MHz;
  mixer.setTargetFrequency(tfreq);

  // digitizer, step (5), waveform from l2 key
  auto digitizer = Digitize(mixed, l2mix);
  quantity<Hz> dsampling = 1.0 * GHz;
  quantity<V> vert = 1.0 * V;
  digitizer.setDigiSamplingRate(dsampling);
  digitizer.setVerticalRange(vert);
  digitizer.setGainFactor(10.0);
  digitizer.setBitNumber(12);

  // data sink: simply print to screen, take from key
  TFile* outfile = new TFile(outfname.data(), "RECREATE");
  TTree* tr = new TTree("recon","reconstructed data");
  tr->SetDirectory(outfile);
  auto sink = WriterDigiToRoot(tr, nant);
  
  auto pl = yap::Pipeline{} | source | addchirp |antresponse | interpolator | addbeat |
    noiseAdder | mixer | digitizer | sink;
  
  pl.consume();
  
  std::cout << "in app: " << std::endl;
  tr->Show(1);
  tr->Write();
  outfile->Close(); // free TTree
  ff.Close();

  delete ant1;
  delete ant2;
}
