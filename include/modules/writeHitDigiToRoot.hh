// ROOT Hit and Digitizer writer for a pipeline module

#ifndef writerRootHitDigi_HH
#define writerRootHitDigi_HH 1

// std includes
#include <string>
#include <vector>
#include <array>

// ROOT
#include "TTree.h"

// must have include for pipeline
#include <Event.hh>

class WriterHitDigiToRoot
{
    public:
        WriterHitDigiToRoot(TTree* tr, int na); // constructor; required
        // Need antenna number as input for setting up output file
        // That truth data is in DataPack but not available at construction.
        // Configures the module. Could have more config parameters
        // Minimum required are the key labels for input and output 
        // of Event Map data item.

        void operator()(DataPack dp); // this is called by the pipeline
        // Writers/Sinks only receive a DataPack and return void as signature.

        // getter/setter methods for configuration could live here.

    private:
    // include any configuration data members for internal use here.
      int nantenna; // needed for construction of output file
      TTree* mytree;
      std::vector<vec_t*> scopedata; // no unit storage in ROOT file
      std::vector<vec_t*> purewave; // no unit storage in ROOT file
      // doubles/int for values without unit
      int nant, evID, trID;
      double snratio;
      double samplingtime; // from quantity<ns>
      double avomega; // quantity<Hz>
      double beatf; // quantity<Hz>
      double chirprate; // quantity<Hz/s>
      double bfield; // quantity<T>
      // vertex
      double posx; // quantity<m>
      double posy; // quantity<m>
      double posz; // quantity<m>
      double kEnergy; // quantity<eV>
      double pangle; // quantity<deg>
      // experiment
      double gain;
      double tfrequency; // quantity<Hz>
      double digisamplingrate; // quantity<Hz>
      // hit data
      std::vector<int>* hitevID;
      std::vector<int>* hittrID;
      std::vector<double>* hitx;
      std::vector<double>* hity;
      std::vector<double>* hitz;
      std::vector<double>* hitedep;
      std::vector<double>* hittime;
      std::vector<double>* hitposttheta;
};
#endif
