// Root Digitizer Writer module implementation
// writing a single value; expand for specific outputs.

// std
#include <iostream>

// us
#include "writeDigitizerToRoot.hh"
#include "types.hh"

WriterDigiToRoot::WriterDigiToRoot(TTree* tr, int na, std::string in) : 
  inkey(std::move(in)),
  mytree(tr),
  nantenna(na)
{
  // N antennae, one for each waveform; need to know at construction for writing
  // construct scopedata entries
  for (int i=0;i<nantenna;++i) {
    std::vector<double> vv;
    scopedata.push_back(vv); // vector in scopedata initialized
  }
  // can now point branch at address
  for (int i=0;i<nantenna;++i) {
    std::string brname = "signal_" + std::to_string(i) + "_V"; // unit in name
    mytree->Branch(brname.data(), &scopedata.at(i)); // point to std::vector<double>
  }
}


void WriterDigiToRoot::operator()(DataPack dp)
{
  // example getting hold of requested input data for processing
  if (! dp.getRef().count(inkey)) { 
    std::cout << "input key not in dictionary!" << std::endl;
    return; // not found, return unchanged map, no processing
  }
  Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
  // yields a L2 unordered map called Event<std::any> with the 
  // help of the inkey label.
  try // casting can go wrong; throws at run-time, catch it.
    {
      for (int i=0;i<nantenna;++i) {
	std::string ikey = "signal_" + std::to_string(i); // from Digitizer, fixed
	auto sig = std::any_cast<waveform_t>(indata[ikey]); // has unit
	std::cout << "got wave size " << sig.size() << std::endl;
	for (auto entry : sig) // strip units from vector entries
	  scopedata.at(i).push_back(entry.numerical_value_in(V));
        mytree->Fill();
        // output streamed, clear memory
	scopedata.at(i).clear();
      }
    }
  catch (const std::bad_any_cast& e)
    {
      std::cout << e.what() << std::endl;
    }
  return;
}
