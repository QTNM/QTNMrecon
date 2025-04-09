// QTNM signal generator sine generator
#ifndef QT_SINEGEN_H
#define QT_SINEGEN_H

// std

// us
#include "VSignal.hh"


class SineGenerator : VSignal
{
    public:
        SineGenerator(); // constructor
        SineGenerator(quantity<V> amp, quantity<Hz> freq, quantity<Hz> srate, 
                        quantity<s> dur, quantity<rad> phrad); // full constructor
        virtual ~SineGenerator() = default; // default destructor

        waveform_t generate() override; // must have from VSignal
        vec_t generate_pattern() override; // must have from VSignal
        waveform_t add(waveform_t& sig, size_t pos) override; // also required

        inline void setAmplitude(quantity<V>  a) {amplitude = a; state_change = true;}
        inline void setFrequency(quantity<Hz> ff) {frequency = ff; state_change = true;}
        inline void setSampling_rate(quantity<Hz> sr) {sampling_rate = sr; state_change = true;}
        inline void setDuration(quantity<s> d) {duration = d; state_change = true;}
        inline void setPhase_rad(quantity<rad> phr) {phase = phr; state_change = true;}

    private:
        waveform_t tsig; // store tests signal for repeat requests

        quantity<V>   amplitude;
        quantity<Hz>  frequency;
        quantity<Hz>  sampling_rate;
        quantity<s>   duration;
        quantity<rad> phase;
};

#endif
