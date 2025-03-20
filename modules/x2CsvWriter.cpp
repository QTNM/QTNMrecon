// CSV Writer module implementation
// writing frequency, amplitude; expand for specific outputs.

// std
#include <iostream>

// us
#include "x2CsvWriter.hh"

x2CsvWriter::x2CsvWriter(std::ofstream& ofs, std::string in, std::string l21, std::string l22) : 
    inkey(std::move(in)),
    l2key1(std::move(l21)), // frequency key
    l2key2(std::move(l22)), // amplitude key
    counter(0),
    myofs(ofs)
{
}


void x2CsvWriter::operator()(DataPack dp)
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
    try // casting can go wrong; throws at run-time, catch it.
      {
	auto vec1 = std::any_cast<std::vector<quantity<Hz>>>(indata[l2key1]);
	auto vec2 = std::any_cast<waveform_t>(indata[l2key2]);
	// std::cout << "got vec1 size " << vec1.size() << std::endl;
	// std::cout << "got vec2 size " << vec2.size() << std::endl;
        for (int i=0;i<vec1.size();i++)
	  myofs << counter << "," << vec1.at(i).numerical_value_in(Hz) << "," << vec2.at(i).numerical_value_in(V) << "\n" << std::flush;
      }
    catch (const std::bad_any_cast& e)
      {
        std::cout << e.what() << std::endl;
      }
    counter++;
    return;
}
