// --------------------
// filter methods implementation

// std
#include <algorithm>
#include <functional>
#include <iostream>
#include <cmath>

// us
#include <mp-units/math.h>
#include "filter.hh"


// MatchedFilter: inline methods implementation
void MatchedFilter::SetTemplate(vec_t t)
{
    pattern = t;
    std::reverse(pattern.begin(), pattern.end()); // preparation for cross-correlation
}

void MatchedFilter::padding(waveform_t& indata, int zeros)
{
    int count = 0;
    while (count<zeros) {
        indata.push_back(0.0 * V); // casts 0 to complex
        count++;
    }
}

void MatchedFilter::padding(vec_t& indata, int zeros)
{
    int count = 0;
    while (count<zeros) {
        indata.push_back(0.0); // casts 0 to complex
        count++;
    }
}

waveform_t MatchedFilter::Filter(waveform_t &record)
{
    int zeros = record.size() - pattern.size();
    zeros>0 ? padding(pattern,zeros) : padding(record, abs(zeros));

    pattern_arg pin(pattern.size()); // temporary
    for (int i=0; i<pattern.size();++i) pin[i] = pattern[i]; // casting double->complex<double>
    pattern_arg tdat(record.size()); // temporary
    for (int i=0; i<record.size();++i) tdat[i] = record[i].numerical_value_in(record.front().unit); // unit removed

    pattern_arg go = dsp.rfft1d(pin, fft_dir::DIR_FWD);  // pattern
    pattern_arg fo = dsp.rfft1d(tdat, fft_dir::DIR_FWD); // data, without unit
    
    // overwrites pin, complex conjugate of pattern
    std::transform(go.begin(),go.end(),pin.begin(),
                    [this](std::complex<double> d){return std::conj(d);});

    // correlation = multiplication of prepared transforms
    std::transform(pin.begin(),pin.end(), fo.begin(), tdat.begin(), 
                    std::multiplies<std::complex<double>>()); // (hat(g))*f

    pattern_arg xinv = dsp.rfft1d(tdat, fft_dir::DIR_BWD);
    waveform_t res;
    for (auto entry : xinv) res.push_back(entry.real() * record.front().unit); // complex<double>->quantity<V>
    return res;
}

// ------------------
// Butterworth filter
// Buttterworth: methods implementation

void Butterworth::SetSamplingRate(quantity<isq::frequency[Hz]> ff)
{
    if (ff > 0.0 * Hz) ftimebase = 1.0/ff; // take only positive time base
    else ftimebase = 1. * ns; // unit nano seconds
    recalc = true; // recalculate coefficients
}


void Butterworth::SetLowFilterFreq(quantity<Hz> low)
{
    if (low >= 0.0 * Hz) flowfreq = low; // take only positive frequencies
    else flowfreq = 0.0 * Hz;
    recalc = true; // recalculate coefficients
}


void Butterworth::SetBPassFilterFreqs(quantity<Hz> low, quantity<Hz> high)
{
    if (low >= 0.0 * Hz) flowfreq = low; // take only positive frequencies
    else flowfreq = 0.0 * Hz;
    if (high > 0.0 * Hz && high > low) fhighfreq = high; // take only positive frequencies
    else fhighfreq = flowfreq + 1.0 * Hz; // shouldn't happen but right order at least
    recalc = true; // recalculate coefficients
}


void Butterworth::SetFilterOrder(int o)
{
    if (o%2) fOrder = (o>2) ? (o + 1) : 2 ;    // Minimum order here is 2
    else fOrder = (o>2) ? o : 2 ; // take only even orders
    // prepare filter coefficients
    const int n = fOrder/2;
    A.resize(n), d1.resize(n), d2.resize(n); // zero initialize, plain numbers
    d3.resize(n), d4.resize(n); // zero initialize, plain numbers
    w0.resize(n), w1.resize(n), w2.resize(n); // carry unit [V]
    w3.resize(n), w4.resize(n); // carry unit [V]
    recalc = true; // recalculate coefficients
}

void Butterworth::LPassFilter(const waveform_t &record, waveform_t &result)
{
    quantity<Hz> nyfreq;
    double a, a2, r, s;  // naming as in source code, see header.
    const int n = fOrder/2;

    if (recalc) { // coefficients calculated previously
        nyfreq = 1.0 / (2.0 * ftimebase);  // Nyquist frequency
        if (flowfreq < nyfreq) {  // must be smaller than Nyquist frequency
            a  = tan(Pi * (flowfreq * ftimebase).numerical_value_in(one));
            a2 = a*a;
            for (int i=0;i<n; ++i) {
	            r = std::sin(Pi*(2.0*i+1)/(4.0*n));
                s = a2 + 2.0*r*a + 1.0;
                A[i]  = a2/s;
                d1[i] = 2.0*(1.0-a2)/s;
                d2[i] = -(a2-2.0*a*r+1.0)/s;
            }
            recalc = false; // coefficients calculated
        }
        else 
            return;       // empty; no filtering
    }

    if (!record.empty()) {
        result.clear();
        for (waveform_value xdata : record) {
            for (int j=0;j<n;++j) { // for every data item, filter order loop
                w0[j]     = d1[j]*w1[j] + d2[j]*w2[j] + xdata;
                xdata     = A[j] * (w0[j]+2.0*w1[j]+w2[j]);
                w2[j]     = w1[j];
                w1[j]     = w0[j];
            }
            result.push_back(xdata);
        }
    }
    else
        std::cout << "Error data not valid in apply filter" << std::endl;
    w1.clear(); // reset w1,w2 for next call
    w2.clear();
    w1.resize(n); 
    w2.resize(n);
}


void Butterworth::BPassFilter(const waveform_t &record, waveform_t &result)
{
    quantity<Hz> nyfreq;
    double a, a2, b, b2, r, s;  // naming as in source code, see header.
    const int n = (int)std::rint(fOrder/4);

    if (recalc) { // coefficients calculated previously
        nyfreq = 1.0 / (2.0 * ftimebase);  // Nyquist frequency
        if (flowfreq < nyfreq) {  // must be smaller than Nyquist frequency
	  a  = cos(Pi * ((fhighfreq+flowfreq) * ftimebase).numerical_value_in(one)) /
	       cos(Pi * ((fhighfreq-flowfreq) * ftimebase).numerical_value_in(one));
	  a2 = a*a;
	  b  = tan(Pi * ((fhighfreq-flowfreq) * ftimebase).numerical_value_in(one));
	  b2 = b*b;
            for (int i=0;i<n; ++i) {
	        r = std::sin(Pi*(2.0*i+1)/(4.0*n));
                s = b2 + 2.0*r*b + 1.0;
                A[i]  = b2/s;
                d1[i] = 4.0*a*(1.0 + b*r)/s;
                d2[i] = 2.0*(b2 - 2.0*a2 - 1.0)/s;
                d3[i] = 4.0*a*(1.0 - b*r)/s;
                d4[i] = -(b2 - 2.0*b*r + 1.0)/s;
            }
            recalc = false; // coefficients calculated
        }
        else 
            return;       // empty; no filtering
    }

    if (!record.empty()) {
        result.clear();
        for (waveform_value xdata : record) {
            for (int j=0;j<n;++j) { // for every data item, filter order loop
                w0[j]     = d1[j]*w1[j] + d2[j]*w2[j] + d3[j]*w3[j] + d4[j]*w4[j] + xdata;
                xdata     = A[j] * (w0[j]-2.0*w2[j]+w4[j]);
                w4[j]     = w3[j];
                w3[j]     = w2[j];
                w2[j]     = w1[j];
                w1[j]     = w0[j];
            }
            result.push_back(xdata);
        }
    }
    else
        std::cout << "Error data not valid in apply filter" << std::endl;
    w1.clear(); // reset w1-4 for next call
    w2.clear();
    w3.clear();
    w4.clear();
    w1.resize(n); 
    w2.resize(n);
    w3.resize(n); 
    w4.resize(n);
}
