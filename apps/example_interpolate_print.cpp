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
//
// This example includes a printing sink on purpose since printing
// is for debugging and requires a lot of flexibility. Better to include
// that directly in the app, like here, instead as a separate module in
// the module library.
// Here we print from two L1 level keys, showing how to access previous
// processing outcomes with their keys.

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
#include "modules/WaveformInterpolator.hh"
#include "CLI11.hpp"
#include <Event.hh>
#include "types.hh"
#include <mp-units/format.h>
#include <mp-units/ostream.h>

class printInterpolator
{
public:
  printInterpolator(std::string inbox, std::string inbox2) :
    inkey1(std::move(inbox)),
    inkey2(std::move(inbox2)) {}; // constructor; required
  
  void operator()(Event_map<std::any> emap); // this is called by the pipeline
  
private:
    // these below serve as string keys to access (read/write) the Event map
  std::string inkey1;
  std::string inkey2;

};

void printInterpolator::operator()(Event_map<std::any> emap)
{
    // example getting hold of requested input data for processing
    if (! emap.count(inkey1)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return; // not found, return unchanged map, no processing
    }
    Event_map<std::any> mymap = std::move(emap); // move from buffer copy
    Event<std::any> indata1 = mymap[inkey1]; // access L1 dictionary
    Event<std::any> indata2 = mymap[inkey2]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey1 label.

    std::cout << "Event_map values:" << std::endl;
    // for values need to know what is stored
    try // casting can go wrong; throws at run-time, catch it.
    {
      // from raw
      std::cout << "evID " << std::any_cast<int>(indata2["eventID"]) << std::endl;

      // from interpolator
      std::cout << "Sampling " << std::any_cast<quantity<ns>>(indata1["sample_time[ns]"]) << std::endl;
        // can also cast the container
        auto vvv1 = std::any_cast<std::vector<double>>(indata1["sampled_0_[V]"]);
        std::cout << "Sampled antenna 1 size = " << vvv1.size() << std::endl;
        auto vvv2 = std::any_cast<std::vector<double>>(indata1["sampled_1_[V]"]);
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
  std::string fname = "qtnm.root";

  app.add_option("-n,--nevents", nevents, "<number of events> Default: -1");
  app.add_option("-i,--input", fname, "<input file name> Default: qtnm.root");

  CLI11_PARSE(app, argc, argv);

  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re("ntuple/Signal", &ff);
  auto source = QTNMSimAntennaReader(re, "raw");
  source.setMaxEventNumber(nevents); // default = all events in file

  // transformer, here interpolation
  auto interpolator = WaveformInterpolator("raw","sampled");
  quantity<ns> stime = 0.004 * ns;
  int nant = 2;
  interpolator.setSampleTime(stime);
  interpolator.setAntennaNumber(nant);
  
  // data sink: simply print to screen, take from keys, see above
  auto sink   = printInterpolator("sampled","raw");
  
  auto pl = yap::Pipeline{} | source | interpolator | sink;
  
  pl.consume();
  ff.Close();
}
