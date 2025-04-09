// QTNM noise generator white noise generator
#ifndef QT_WNOISEGEN_H
#define QT_WNOISEGEN_H

// std

// us
#include "VSignal.hh"


class WhiteNoiseGenerator : VSignal
{
    public:
        WhiteNoiseGenerator(); // constructor
        WhiteNoiseGenerator(quantity<V> scale, quantity<Hz> srate, quantity<s> dur, int seed); // full constructor
        virtual ~WhiteNoiseGenerator() = default; // default destructor

        waveform_t generate() override; // must have from VSignal
        vec_t generate_pattern() override; // must have from VSignal
        waveform_t add(waveform_t& sig, size_t pos) override; // also required

        inline void setScale(quantity<V> s) {scalefactor = s;}
        inline void setSampling_rate(quantity<Hz> sr) {sampling_rate = sr;}
        inline void setDuration(quantity<s> d) {duration = d;}
        inline void setSeed(int sd) {seed = sd;}

    private:
        waveform_t tsig; // store tests signal for repeat requests

        int    seed;
        quantity<V>  scalefactor;
        quantity<Hz> sampling_rate;
        quantity<s>  duration;

};

#endif
