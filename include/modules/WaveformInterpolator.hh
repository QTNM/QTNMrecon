// Transformer G4 waveform to sampled waveform for a pipeline module
#ifndef wfminterpolator_HH
#define wfminterpolator_HH 1

// std includes
#include <string>
#include <vector>

// tool includes

// must have include for pipeline
#include <Event.hh>
#include "types.hh"

class WaveformInterpolator
{
    public:
        WaveformInterpolator(std::string inbox, std::string outbox); // constructor; required
        // Configures the module. Could have more config parameters
        // String Key outbox label as input.

        Event_map<std::any> operator()(Event_map<std::any>); // this is called by the pipeline
        // Transforming module event map in and out.

        // getter/setter methods for configuration could live here.
        inline void setSampleTime(quantity<ns> st) {sampletime = st;}
        inline void setAntennaNumber(int n) {nantenna = n;}

    private:
    // include any configuration data members for internal use here.
        vec_t interpolate(const vec_t& tvals, const vec_t& vvals);
        int nantenna;
        quantity<ns> sampletime;

    // these below serve as string keys to access (read/write) the Event map
        std::string inkey;
        std::string outkey;
};
#endif
