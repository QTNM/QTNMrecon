// types for QTNM tools

#ifndef QT_TYPES_H
#define QT_TYPES_H

// mp-units
#include <mp-units/systems/si/si.h>
#include <mp-units/systems/isq/isq.h>
#include <mp-units/systems/hep/hep.h>

// std
#include <complex>
#include <vector>
#include <cstdint>

constexpr auto Pi = 3.141592653589793238462643383279502884;

using namespace mp_units;
using namespace mp_units::si::unit_symbols; // work in SI units
using namespace mp_units::hep::unit_symbols; // work in hep units

// waveform data comes in [Volt]
using waveform_value = quantity<isq::voltage[V]>; // type double in quantity

// FFT direction specifier
enum class fft_dir {DIR_FWD = +1, DIR_BWD = -1};

// FFT argument: std::vector<std::complex>
using fft_arg = std::vector<std::complex<waveform_value>>;
using pattern_arg = std::vector<std::complex<double>>;

// interface FFT output: std::vector<double>
using waveform_t = std::vector<waveform_value>;
using vec_t  = std::vector<double>;
using digi_t = std::vector<std::int16_t>; // and digitizer with 16 bit type

#endif
