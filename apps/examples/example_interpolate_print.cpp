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
#include "CLI11.hpp"
#include <Event.hh>
#include "types.hh"
#include <mp-units/format.h>
#include <mp-units/ostream.h>

class printInterpolator
{
public:
  printInterpolator(std::string inbox) :
    inkey1(std::move(inbox)) {}; // constructor; required
  
  //  void operator()(Event_map<std::any> emap); // this is called by the pipeline
  void operator()(DataPack dp); // this is called by the pipeline
  
private:
    // these below serve as string keys to access (read/write) the Event map
  std::string inkey1;

};

//void printInterpolator::operator()(Event_map<std::any> emap)
void printInterpolator::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
  if (! dp.getRef().count(inkey1)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return; // not found, return unchanged map, no processing
    }
    //    Event_map<std::any> mymap = std::move(emap); // move from buffer copy
    Event<std::any> indata1 = dp.getRef()[inkey1]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey1 label.

    std::cout << "Event_map values:" << std::endl;
    // for values need to know what is stored
    try // casting can go wrong; throws at run-time, catch it.
    {
      // from raw
      std::cout << "evID " << dp.getTruthRef().vertex.eventID << std::endl;
      std::cout << "chirp rate " << dp.getTruthRef().chirp_rate << std::endl;
      std::cout << "beat frequency " << dp.getTruthRef().beat_frequency << std::endl;
      // from interpolator
      std::cout << "Sampling time " << dp.getTruthRef().sampling_time << std::endl;
        // can also cast the container
        auto vvv1 = std::any_cast<std::vector<double>>(indata1["sampled_0_V"]);
        std::cout << "Sampled antenna 1 size = " << vvv1.size() << std::endl;
        auto vvv2 = std::any_cast<std::vector<double>>(indata1["sampled_1_V"]);
        std::cout << "Sampled antenna 2 size = " << vvv2.size() << std::endl;
    }
    catch (const std::bad_any_cast& e)
    {
        std::cout << e.what() << std::endl;
    }

    // action of module template module.
    
    return;
}


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

  // transformer, here interpolation
  auto interpolator = WaveformSampling("raw","","sampled");
  quantity<ns> stime = 0.008 * ns;
  interpolator.setSampleTime(stime);
  
  // add truth
  auto addbeat = OmegaBeatToTruth("sampled","omout");

  // data sink: simply print to screen, take from key
  auto sink   = printInterpolator("sampled");
  
  auto pl = yap::Pipeline{} | source | addchirp | interpolator | addbeat | sink;
  
  pl.consume();
  ff.Close();
}
