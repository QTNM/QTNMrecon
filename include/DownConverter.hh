// Down-converting Mixer for QTNM tools
#ifndef QT_DCONV_H
#define QT_DCONV_H

// us
#include "types.hh"
#include "filter.hh"
#include "SineGenerator.hh"


class DownConverter
{
    // DownConverter class

    public:
    DownConverter();
    virtual ~DownConverter() = default;

    waveform_t mix(waveform_t&  sig); // operation

    // local oscillator and filter settings; duration from waveform, phase=0, filter order=10
    inline void setLOamplitude(quantity<V> amp) {lo.setAmplitude(amp);}
    inline void setLOfrequency(quantity<Hz> lof) {lo.setFrequency(lof);}
    inline void setFilterCutoff(quantity<Hz> fc) {bw.setLowFilterFreq(fc);}
    inline void setSamplingRate(quantity<Hz> sr) {sampling_rate = sr;}

    private:
    Butterworth bw; // low-pass filter, order is fixed at 10
    SineGenerator lo;

    quantity<Hz> sampling_rate;
};

#endif
