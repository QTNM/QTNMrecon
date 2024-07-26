// CSV Writer module implementation
// writing a single value; expand for specific outputs.

// std
#include <iostream>

// us
#include "digiCsvWriter.hh"
#include "types.hh"

digiCsvWriter::digiCsvWriter(std::ofstream& ofs) : 
    myofs(ofs),
    counter(0) 
{
  std::cout << "constructor called before file open" << std::endl;
}


void digiCsvWriter::operator()(DataPack dp)
{
    int nant = dp.getTruthRef().nantenna;
    try // casting can go wrong; throws at run-time, catch it.
    {
      for (int i=0;i<nant;i++) {
	vec_t dwave = dp.getExperimentRef().signals.at(i);
	std::cout << "got digi wave size " << dwave.size() << std::endl;
        for (auto entry : dwave)
	  myofs << counter << "," << i << "," << entry << "\n" << std::flush;
      }
    }
    catch (const std::bad_any_cast& e)
    {
        std::cout << e.what() << std::endl;
    }
    counter++;
    return;
}
