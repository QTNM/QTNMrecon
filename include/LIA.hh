// Lock-in Amplifier (LIA) for QTNM tools
// operating in quadrature only, no phase output

// us
#include "types.hh"
#include "filter.hh"

#ifndef QT_LIA_H
#define QT_LIA_H

class LIA
{
    // LIA class

    public:
    LIA();
    LIA(quantity<Hz> fc, quantity<Hz> sr); // init with filter constants
    virtual ~LIA();

    waveform_t quadrature(waveform_t&  sig); // default run on type double
    waveform_t quadrature(digi_t& sig); // overload for type short

    // setter/getter; move leaves origin undefined
    inline void setReference(vec_t ref) {reference = ref;}
    inline void setCrossReference(vec_t coref) {coreference = coref;} // 90 phase reference (e.g. cos)

    // filter info setting
    inline void setFilterCutoff(quantity<Hz> fc) {bw.setLowFilterFreq(fc);}
    inline void setSamplingRate(quantity<Hz> sr) {bw.setSamplingRate(sr);}

    private:
    Butterworth bw; // low-pass filter, order is fixed at 10

    vec_t reference; // patterns, no unit
    vec_t coreference;
};

#endif
