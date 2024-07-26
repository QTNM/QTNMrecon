// Digitizer
// Turn a waveform vector of doubles into an oscilloscope
// representation with a vector of 16 bit unsigned short
// with vector<N sampling points>.
// Wrap into object holding and reporting configuration.
// Needs voltage range, ADC bits and sampling rate.
// Voltage range can be taken from input max/min. 
// Input data container references (const for read-only).

#ifndef digitizer_HH
#define digitizer_HH 1

// std
#include <vector>
#include <cmath>
#include <cstdint>

// us
#include "types.hh"

class Digitizer
{
    public:
        Digitizer(); // set default values for oscilloscope
        Digitizer(quantity<V> voltrange, int bitrange);
        virtual ~Digitizer() = default;

        // operations, avoid data copy
        waveform_t  digitize(const waveform_t&);

        // config parameter
        inline void setVRange(quantity<V> v) {vrange = v; reset();}
        inline void setADCBits(int b);
        inline void setGainFactor(double g) {gain = g; reset();}

        void dumpInfo(); // report by print

    private:
        // internal operation
        void digitize_raw(const waveform_t&);
        void reset();
        std::int16_t _adc(waveform_value);

        int bitrange; // 16 bit max
        int bmax;     // max number from bitrange
        double gain;
        quantity<V> vrange;

        digi_t darray; // digitizer array
        waveform_t calarray; // scaled array
        digi_t dwave; // digitized waveform vector<std::int16_t>
};

inline void Digitizer::setADCBits(int b)
{
    bitrange = b;
    if (bitrange>16) bitrange = 16; // hard limit at 16 bits
    reset();
}


#endif
