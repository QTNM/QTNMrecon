// CSV Writer module implementation
// writing a single value; expand for specific outputs.

// std
#include <iostream>

// us
#include "xCsvWriter.hh"
#include "types.hh"

xCsvWriter::xCsvWriter(std::ofstream& ofs, std::string in, std::string l2) : 
    inkey(std::move(in)),
    l2key(std::move(l2)),
    myofs(ofs),
    counter(0) 
{
  std::cout << "constructor called before file open" << std::endl;
}


void xCsvWriter::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
    if (! dp.getRef().count(inkey)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return; // not found, return unchanged map, no processing
    }
    Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey label.
    std::cout << "got indata event size " << indata.size() << std::endl;
    int nant = dp.getTruthRef().nantenna;
    try // casting can go wrong; throws at run-time, catch it.
    {
      for (int i=0;i<nant;i++) {
	std::string l2in = l2key + std::to_string(i);
        auto wave = std::any_cast<waveform_t>(indata[l2in]);
	std::cout << "got wave size " << wave.size() << std::endl;
        for (auto entry : wave)
	  myofs << counter << "," << i << "," << entry.numerical_value_in(V) << "\n" << std::flush;
      }
    }
    catch (const std::bad_any_cast& e)
    {
        std::cout << e.what() << std::endl;
    }
    counter++;
    return;
}
