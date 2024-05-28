// QTNM noise generator white noise generator

// std

// us
#include "VSignal.hh"

#ifndef QT_CNOISEGEN_H
#define QT_CNOISEGEN_H

class CustomNoiseGenerator : VSignal
{
    public:
        CustomNoiseGenerator(); // constructor
        CustomNoiseGenerator(const char* fname, quantity<V> scale, quantity<Hz> srate, quantity<s> dur, int seed); // full constructor
        virtual ~CustomNoiseGenerator() = default; // default destructor

        waveform_t generate() override; // must have from VSignal
        vec_t generate_pattern() override; // must have from VSignal
        waveform_t add(waveform_t& sig, size_t pos) override; // also required

        inline void setscale(quantity<V> s) {scalefactor = s;}
        inline void setsampling_rate(quantity<Hz> sr) {sampling_rate = sr;}
        inline void setduration(quantity<s> d) {duration = d;}
        inline void setseed(int sd) {seed = sd;}

    private:
        waveform_t tsig; // store tests signal for repeat requests
        vec_t powerspec;   // store from file
        vec_t frequencies; // from file

        int    seed;
        quantity<V>  scalefactor;
        quantity<Hz> sampling_rate;
        quantity<s>  duration;

};

#endif
