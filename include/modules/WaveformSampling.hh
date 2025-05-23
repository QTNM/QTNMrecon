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

class WaveformSampling
{
    public:
        WaveformSampling(std::string source, std::string inbox, std::string outbox);
        // Configures the module. Could have more config parameters
        // String Key outbox label as input.

        DataPack operator()(DataPack); // this is called by the pipeline
        // Transforming module event map in and out.

        // getter/setter methods for configuration could live here.
        inline void setSampleTime(quantity<ns> st) {sampletime = st;}

    private:
    // include any configuration data members for internal use here.
        quantity<Hz> average_omega(const vec_t& omvec);
        vec_t interpolate(const vec_t& tvals, const vec_t& vvals);
        quantity<ns> sampletime;
        int nantenna;
  
    // these below serve as string keys to access (read/write) the Event map
        std::string originkey;
        std::string inkey;
        std::string outkey;
};
#endif
