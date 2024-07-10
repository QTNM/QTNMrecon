// ROOT Digitizer writer for a pipeline module

#ifndef writerRootDigi_HH
#define writerRootDigi_HH 1

// std includes
#include <string>
#include <vector>

// ROOT
#include "TTree.h"

// must have include for pipeline
#include <Event.hh>

class WriterDigiToRoot
{
    public:
        WriterDigiToRoot(TTree* tr, int na, std::string inbox); // constructor; required
        // Configures the module. Could have more config parameters
        // Minimum required are the key labels for input and output 
        // of Event Map data item. String Key inbox label and new Key outbox label.

        void operator()(DataPack dp); // this is called by the pipeline
        // Writers/Sinks only receive a DataPack and return void as signature.

        // getter/setter methods for configuration could live here.

    private:
    // include any configuration data members for internal use here.
      int nantenna; // how many waveforms to store separately
      TTree* mytree;
      std::vector<std::vector<double>> scopedata; // no unit storage in ROOT file

    // these below serve as string keys to access (read/write) the Event map
      std::string inkey;
};
#endif
