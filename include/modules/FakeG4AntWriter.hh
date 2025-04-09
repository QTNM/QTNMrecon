// ROOT fake Geant4 Antenna Sim writer for a pipeline module

#ifndef writerFG4_HH
#define writerFG4_HH 1

// std includes
#include <string>
#include <vector>

// ROOT
#include "TTree.h"

// must have include for pipeline
#include <Event.hh>

class FakeG4AntToRoot
{
    public:
  FakeG4AntToRoot(std::string in, TTree* tr); // constructor; required
  // Configures the module. Could have more config parameters
  // Minimum required are the key labels for input and output 
  // of Event Map data item.

  void operator()(DataPack dp); // this is called by the pipeline
  // Writers/Sinks only receive a DataPack and return void as signature.

  // getter/setter methods for configuration could live here.

    private:
  // include any configuration data members for internal use here.
  std::string inkey;
  TTree* mytree;

  std::vector<int>* antennaID; // interleaved like G4 output
  vec_t* timevec; // may change in QTNMSim to non-interleaved vector
  vec_t* voltagevec;
  vec_t* KEdummy; // empty for consistency with reader 
  vec_t* OMdummy; // empty
  // doubles/int for values without unit
  int evID, trID;
  // no fake vertex
  double dx, dy, dz, dke, dang; // empty dummy numbers for reader
  // no hits in this fake writer
};
#endif
