// Adding Noise module for a pipeline
#ifndef addnoise_HH
#define addnoise_HH 1

// std includes
#include <string>
#include <vector>
#include <cmath>

// tool includes
#include "WhiteNoiseGen.hh"

// must have include for pipeline
#include <Event.hh>
#include "types.hh"

class AddNoise
{
    public:
  AddNoise(std::string inbox, std::string outbox, std::string l2out); // constructor; required
        // Configures the module. Could have more config parameters
        // String Key outbox label as input.

        DataPack operator()(DataPack); // this is called by the pipeline
        // Transforming module event map in and out.

        // getter/setter methods for configuration could live here.
        inline void setSignalToNoise(double snr) {SNr = snr;}
        inline void setSampleLength(quantity<s> sl) {sample_length = sl;}
        inline void setOnsetPercent(double tr) {onset_percent = tr;}

    private:
    // include any configuration data members for internal use here.
        WhiteNoiseGenerator noisegen; // default constructor
        quantity<s> sample_length;
        double SNr;
        double onset_percent;

    // these below serve as string keys to access (read/write) the Event map
        std::string inkey;
        std::string outkey;
        std::string l2out;
};
#endif
