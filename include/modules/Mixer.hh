// Mixer module for a pipeline module
#ifndef mixer_HH
#define mixer_HH 1

// std includes
#include <string>

// tool includes
#include "DownConverter.hh"

// must have include for pipeline
#include <Event.hh>
#include "types.hh"

class Mixer
{
    public:
  Mixer(std::string inbox, std::string outbox, std::string l2in, std::string l2out); // constructor; required
        // Configures the module. Could have more config parameters
        // String Key outbox label as input.

        DataPack operator()(DataPack); // this is called by the pipeline
        // Transforming module event map in and out.

        // getter/setter methods for configuration could live here.
        inline void setTargetFrequency(quantity<Hz> tf) {targetFrequency = tf;}
        inline void setFilterCutFrequency(quantity<Hz> cf) {cutFrequency = cf;}

    private:
    // include any configuration data members for internal use here.
        DownConverter demod;
        quantity<Hz> targetFrequency;
        quantity<Hz> cutFrequency;

    // these below serve as string keys to access (read/write) the Event map
        std::string inkey;
        std::string outkey;
        std::string l2in;
        std::string l2out;
};
#endif
