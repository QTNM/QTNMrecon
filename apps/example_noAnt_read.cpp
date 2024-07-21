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

// std
#include<iostream>
#include<string>
#include<vector>

// ROOT
#include "TFile.h"
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
  quantity<T> bfield = 0.7 * T; // constant sim b-field value [T]
  std::string fname = "qtnm.root";

  app.add_option("-n,--nevents", nevents, "<number of events> Default: -1");
  app.add_option("-i,--input", fname, "<input file name> Default: qtnm.root");

  CLI11_PARSE(app, argc, argv);

  // keys to set
  std::string origin = "raw";
  std::string resp = "response";
  std::string samp = "sampled";

  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re("ntuple/Signal", &ff);
  auto source = QTNMSimKinematicsReader(re, origin);
  source.setMaxEventNumber(nevents); // default = all events in file
  source.setSimConstantBField(bfield); // MUST be set

  // add truth
  auto addchirp = AddChirpToTruth(origin); // default antenna number

  // transformer
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

  // transformer
  auto interpolator = WaveformSampling(origin,resp,samp);
  quantity<ns> stime = 0.008 * ns;
  interpolator.setSampleTime(stime);
  
  // data sink: simply print to screen, take from key
  auto sink   = printInterpolator(samp);
  
  auto pl = yap::Pipeline{} | source | addchirp | antresponse | interpolator | sink;
  
  pl.consume();
  ff.Close();
  delete ant1;
  delete ant2;
}
