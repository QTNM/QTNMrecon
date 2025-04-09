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
#include<fstream>
#include<string>
#include<vector>

// ROOT
#include "TFile.h"
#include "TTreeReader.h"

// us
#include "yap/pipeline.h"
#include "modules/QTNMSimAntennaReader.hh"
#include "modules/AddChirpToTruth.hh"
#include "modules/WaveformSampling.hh"
#include "modules/OmegaBeatToTruth.hh"
#include "modules/AddNoise.hh"
#include "modules/Mixer.hh"
#include "modules/Digitize.hh"
//#include "modules/xCsvWriter.hh"
#include "modules/digiCsvWriter.hh"

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
  std::string fname = "qtnm.root";
  std::string outname = "out.csv";
  quantity<T> bfield = 0.7 * T; // constant sim b-field value [T]

  app.add_option("-n,--nevents", nevents, "<number of events> Default: -1");
  app.add_option("-i,--input", fname, "<input file name> Default: qtnm.root");

  CLI11_PARSE(app, argc, argv);

  // keys to set
  int nant = 2;
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
  auto source = QTNMSimAntennaReader(re, origin);
  source.setMaxEventNumber(nevents); // default = all events in file
  source.setSimConstantBField(bfield); // MUST be set
  source.setAntennaN(nant);

  // add truth
  auto addchirp = AddChirpToTruth(origin); // default antenna number

  // transformer
  auto interpolator = WaveformSampling(origin,resp,samp);
  quantity<ns> stime = 0.008 * ns;
  interpolator.setSampleTime(stime);

  // add truth, 'omout' is just for checking
  auto addbeat = OmegaBeatToTruth(samp,"omout");

  // add noise fill more truth with units
  auto noiseAdder = AddNoise(samp, noisy, l2noise);
  noiseAdder.setSignalToNoise(10.0);
  noiseAdder.setOnsetPercent(10.0);

  // mixer waveform in from l2 key, out in l2 key
  auto mixer = Mixer(noisy, mixed, l2noise, l2mix);
  quantity<Hz> tfreq = 100.0 * MHz;
  mixer.setTargetFrequency(tfreq);
  mixer.setFilterCutFrequency(10*tfreq);

  // digitizer waveform from l2 key
  auto digitizer = Digitize(mixed, l2mix);
  quantity<Hz> dsampling = 1.0 * GHz;
  quantity<V> vert = 1.0 * V;
  digitizer.setDigiSamplingRate(dsampling);
  digitizer.setVerticalRange(vert);
  digitizer.setGainFactor(10.0);
  digitizer.setBitNumber(12);

  // data sink: write to disk, take from key 'wave', L2 key 'waveform[V]'
  // assumes L2 key has a waveform_t
  std::ofstream ofs(outname, std::ofstream::out); // open
  if (! ofs.is_open()) {
    throw std::logic_error("Cannot open output file");
  }
  ofs << "Event ID,Wave value[V]\n" << std::flush; // header
  //auto sink   = xCsvWriter(ofs, mixed, l2mix); // write signals vector
  auto sink   = digiCsvWriter(ofs); // write digitized signals vector
  
  auto pl = yap::Pipeline{} | source | addchirp | interpolator | addbeat |
    noiseAdder | mixer | digitizer | sink;
  
  pl.consume();

  ff.Close();
  ofs.close();
}
