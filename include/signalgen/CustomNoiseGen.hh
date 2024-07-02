// QTNM noise generator custom noise generator

// std

// us
#include "VSignal.hh"

#ifndef QT_CNOISEGEN_H
#define QT_CNOISEGEN_H

class CustomNoiseGenerator : VSignal
{
    public:
        CustomNoiseGenerator(); // constructor
        CustomNoiseGenerator(const char* fname, quantity<V> rms, quantity<Hz> srate, quantity<s> dur, int seed);
        virtual ~CustomNoiseGenerator() = default; // default destructor

        waveform_t generate() override; // must have from VSignal
        vec_t generate_pattern() override; // must have from VSignal
        waveform_t add(waveform_t& sig, size_t pos) override; // also required

        inline void setTargetRMS(quantity<V> s) {noiseRMS = s;}
        inline void setSampling_rate(quantity<Hz> sr) {sampling_rate = sr;}
        inline void setDuration(quantity<s> d) {duration = d;}
        inline void setSeed(int sd) {seed = sd;}

    private:
        waveform_t tsig; // store tests signal for repeat requests
        vec_t powerspec;   // store from file
        vec_t frequencies; // from file

        int    seed;
        quantity<V>  noiseRMS;
        quantity<Hz> sampling_rate;
        quantity<s>  duration;

};

#endif
