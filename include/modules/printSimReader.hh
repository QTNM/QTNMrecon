// Example boiler-plate code for a pipeline module

#ifndef printsimreader_HH
#define printsimreader_HH 1

// std includes
#include <string>

// tool includes

// must have include for pipeline
#include <Event.hh>

class printSimReader
{
    public:
        printSimReader(std::string inbox); // constructor; required
        // Configures the module. Could have more config parameters
        // Minimum required are the key labels for input and output 
        // of Event Map data item. String Key inbox label and new Key outbox label.

        void operator()(DataPack dp); // this is called by the pipeline
        // Writers/Sinks only receive a DataPack and return void as signature.

        // getter/setter methods for configuration could live here.

    private:
    // include any configuration data members for internal use here.

    // these below serve as string keys to access (read/write) the Event map
    std::string inkey;

};
#endif
