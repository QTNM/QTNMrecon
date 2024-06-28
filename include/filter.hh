// Filter for QTNM tools

// std

// us
#include "types.hh"
#include "dsp.hh"

#ifndef QT_FILTER_H
#define QT_FILTER_H


class Butterworth
{
  // Low-pass Butterworth filter, any order
  // Follows https://github.com/adis300/filter-c

    private:
        quantity<s>  ftimebase;
        quantity<Hz> flowfreq;
        quantity<Hz> fhighfreq;
        int    fOrder;
        bool   recalc = true;
        // filter coefficients
        vec_t A, d1, d2; // naming as in source code, see header.  
        vec_t d3, d4;    // naming as in source code, see header.  
        // filter application
        waveform_t w0, w1, w2; // coefficients with unit [V]
        waveform_t w3, w4;     // coefficients with unit [V]

    protected:

    public:
        Butterworth() = default;
        virtual   ~Butterworth() = default;

        void LPassFilter(const waveform_t &record, waveform_t &result); // operation function
        void BPassFilter(const waveform_t &record, waveform_t &result); // operation function

    // setter and getter
        void   SetSamplingRate(quantity<isq::frequency[Hz]> sr);
        void   SetLowFilterFreq(quantity<Hz> low);
        void   SetBPassFilterFreqs(quantity<Hz> low, quantity<Hz> high);
        void   SetFilterOrder(int o);

};

class MatchedFilter
{
    // while pure function container,
    // default constructor/destructor
    public:
        waveform_t Filter(waveform_t &record); // operation function

        void SetTemplate(vec_t templ);

    private:
        DSP dsp; // FFT internal, holds no data
        void padding(waveform_t&, int); // data padding
        void padding(vec_t&, int); // pattern padding
        vec_t pattern;
};

#endif
