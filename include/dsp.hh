// DSP library based on
// dj_fft.h - public domain FFT library
// by Jonathan Dupuy; https://github.com/jdupuy/dj_fft
// edited and features added by YR, University of Warwick (2024)
//

#ifndef DJ_DSP_H
#define DJ_DSP_H

// us
#include "types.hh"

class DSP
{
    // while pure function container,
    // default constructor/destructor
    public:
        // main FFT routine, includes zero-padding if required
        pattern_arg rfft1d(pattern_arg &xi, const fft_dir &dir);
        fft_arg rfft1d(fft_arg &xi, const fft_dir &dir);

        // convenience DFT interface to FFT routine
        // output limited to Nyquist max frequency
        std::vector<quantity<Hz>> dft(waveform_t &xi, quantity<isq::frequency[Hz]> srate); 

    private:
        fft_arg hidden_rfft1d(pattern_arg &xi, const fft_dir &dir);
        inline void padding(pattern_arg& indata, int zeros);
        inline int findMSB(int x);
        inline int bitr(uint32_t x, int nb);
        inline size_t nearestPo2(const size_t s);

};

// --------------
// inline methods
inline void DSP::padding(pattern_arg& indata, int zeros)
{
    int count = 0;
    std::complex<double> czero{0., 0.};
    while (count<zeros) {
      indata.push_back(czero);
        count++;
    }
}
/*
* Returns offset to most significant bit
* NOTE: only works for positive power of 2s
* examples:
* 1b      -> 0d
* 100b    -> 2d
* 100000b -> 5d
*/
inline int DSP::findMSB(int x)
{
    int p = 0;

    while (x > 1) {
        x>>= 1;
        ++p;
    }
    return p;
}
/*
*  Bit reverse an integer given a word of nb bits
*  NOTE: Only works for 32-bit words max
*  examples:
*  10b      -> 01b
*  101b     -> 101b
*  1011b    -> 1101b
*  0111001b -> 1001110b
*/
inline int DSP::bitr(uint32_t x, int nb)
{
    x = ( x               << 16) | ( x               >> 16);
    x = ((x & 0x00FF00FF) <<  8) | ((x & 0xFF00FF00) >>  8);
    x = ((x & 0x0F0F0F0F) <<  4) | ((x & 0xF0F0F0F0) >>  4);
    x = ((x & 0x33333333) <<  2) | ((x & 0xCCCCCCCC) >>  2);
    x = ((x & 0x55555555) <<  1) | ((x & 0xAAAAAAAA) >>  1);

    return ((x >> (32 - nb)) & (0xFFFFFFFF >> (32 - nb)));
}

inline size_t DSP::nearestPo2(const size_t s)
{
    size_t counter = 1;
    while (s > std::pow(2,counter)) counter++;
    return (size_t) (std::pow(2,counter));
}


#endif // DJ_DSP_H
