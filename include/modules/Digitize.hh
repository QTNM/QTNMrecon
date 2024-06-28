// Transformer G4 waveform to digitized waveform for a pipeline module
#ifndef digi_HH
#define digi_HH 1

// std includes
#include <string>
#include <vector>

// tool includes
#include "digitizer.hh"

// must have include for pipeline
#include <Event.hh>
#include "types.hh"

class Digitize
{
    public:
        Digitize(std::string inbox, std::string outbox); // constructor; required
        // Configures the module. Could have more config parameters
        // String Key outbox label as input.

        DataPack operator()(DataPack); // this is called by the pipeline
        // Transforming module event map in and out.

        // getter/setter methods for configuration could live here.
        inline void setVerticalRange(quantity<V> vr) {vrange = vr;}
        inline void setBitNumber(int bi) {bits = bi;}
        inline void setNAntenna(int na) {nantenna = na;}

    private:
    // include any configuration data members for internal use here.
        Digitizer adc; // default constructor
        quantity<V> vrange;
        int bits;
        int nantenna;

    // these below serve as string keys to access (read/write) the Event map
        std::string inkey;
        std::string outkey;
};
#endif
