// CSV writer for a pipeline module

#ifndef dcsvwriter_HH
#define dcsvwriter_HH 1

// std includes
#include <string>
#include <fstream>

// tool includes

// must have include for pipeline
#include <Event.hh>

class digiCsvWriter
{
    public:
        digiCsvWriter(std::ofstream& ofs); // constructor; required
        // Configures the module. Could have more config parameters
        // Minimum required are the key labels for input and output 
        // of Event Map data item. String Key inbox label and new Key outbox label.

        void operator()(DataPack dp); // this is called by the pipeline
        // Writers/Sinks only receive a DataPack and return void as signature.

        // getter/setter methods for configuration could live here.

    private:
    // include any configuration data members for internal use here.
  std::ofstream& myofs;
  int counter;

    // these below serve as string keys to access (read/write) the Event map
};
#endif
