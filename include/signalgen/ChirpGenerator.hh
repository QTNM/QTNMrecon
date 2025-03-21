// QTNM signal generator chirp generator
#ifndef QT_CHIRPGEN_H
#define QT_CHIRPGEN_H

// std

// us
#include "VSignal.hh"


class ChirpGenerator : VSignal
{
    public:
        ChirpGenerator(); // constructor
        ChirpGenerator(quantity<V> amp, quantity<Hz> freq, quantity<Hz/s> chrate, quantity<Hz> srate, 
                        quantity<s> dur, quantity<rad> phrad); // full constructor
        virtual ~ChirpGenerator() = default; // default destructor

        waveform_t generate() override; // must have from VSignal
        vec_t generate_pattern() override; // must have from VSignal
        waveform_t add(waveform_t& sig, size_t pos) override; // also required

        inline void setAmplitude(quantity<V> a) {amplitude = a; state_change = true;}
        inline void setFrequency(quantity<Hz> ff) {frequency = ff; state_change = true;}
        inline void setChirprate(quantity<Hz/s> ch) {chirprate = ch; state_change = true;}
        inline void setSampling_rate(quantity<Hz> sr) {sampling_rate = sr; state_change = true;}
        inline void setDuration(quantity<s> d) {duration = d; state_change = true;}
        inline void setPhase_rad(quantity<rad> phr) {phase = phr; state_change = true;}

    private:
        waveform_t tsig; // store tests signal for repeat requests

        quantity<V>     amplitude;
        quantity<Hz>    frequency;
        quantity<Hz/s>  chirprate;
        quantity<Hz>    sampling_rate;
        quantity<s>     duration;
        quantity<rad>   phase;
};

#endif
