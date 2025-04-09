// Filter for QTNM tools
#ifndef QT_FILTER_H
#define QT_FILTER_H

// std

// us
#include "types.hh"
#include "dsp.hh"


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

        waveform_t LPassFilter(const waveform_t &record); // operation function
        waveform_t BPassFilter(const waveform_t &record); // operation function

    // setter and getter
        void   setSamplingRate(quantity<Hz> sr);
        void   setLowFilterFreq(quantity<Hz> low);
        void   setBPassFilterFreqs(quantity<Hz> low, quantity<Hz> high);
        void   setFilterOrder(int o);

};

class MatchedFilter
{
    // while pure function container,
    // default constructor/destructor
    public:
        waveform_t Filter(waveform_t &record); // operation function

        void setTemplate(vec_t templ);

    private:
        DSP dsp; // FFT internal, holds no data
        void padding(waveform_t&, int); // data padding
        void padding(vec_t&, int); // pattern padding
        vec_t pattern;
};


class MovingAverage {

 private:
  int      fMAwidth = 5;        // Moving Average filter width, default=5
  double   fresponse;
  waveform_t padding(waveform_t &record, int width);

 public:
  MovingAverage() = default;
  virtual  ~MovingAverage() = default;

  waveform_t Filter(waveform_t &record);

  // setter and getter
  void     setMovingAverageWidth(int width);  // set averaging window width

};

#endif
