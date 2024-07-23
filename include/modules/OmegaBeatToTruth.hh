// Add the beat frequency from omega to truth data in module
#ifndef addbeat_HH
#define addbeat_HH 1

// std includes
#include <string>

// must have include for pipeline
#include <Event.hh>
#include <dsp.hh>

class OmegaBeatToTruth
{
    public:
  OmegaBeatToTruth(std::string inbox, std::string outbox); // constructor; required
        // Configures the module. Could have more config parameters
        // Minimum required are the key labels for input and output 
        // of Event Map data item. String Key inbox label and new Key outbox label.

        DataPack operator()(DataPack dp); // this is called by the pipeline
        // any module in the pipeline should be able to receive an 
        // event map and return an event map. 
        // Only Readers should expect no input argument and return an event map
        // and Writers only receive an event map and return void as signature.

        // getter/setter methods for configuration could live here.

    private:
    // include any configuration data members for internal use here.
    DSP dsp;

    // these below serve as string keys to access (read/write) the Event map
    std::string inkey;
    std::string outkey;
};
#endif
