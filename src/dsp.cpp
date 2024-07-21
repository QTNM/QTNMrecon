// --------------------
// DSP: methods implementation

// std
#include <cmath>
#include <cstdint>

// us
#include <mp-units/math.h>
#include "dsp.hh"


// interface to main fft for real DFT
std::vector<quantity<Hz>> DSP::dft(waveform_t &xi, quantity<Hz> srate)
{
    fft_arg tin(xi.size()); // temporary
    for (int i=0; i<xi.size();++i) tin[i] = xi[i]; // casting double->complex<double>
    fft_arg tout = rfft1d(tin, fft_dir::DIR_FWD); // convert time->frequency

    unsigned int nyqlength = tout.size(); // tout length Po2 certain
    std::vector<quantity<Hz>> freq(nyqlength/2); //  Nyquist limit length
    xi.resize(nyqlength/2);  // modifies input! Same limit in size as frequency

    for (int i=0; i<nyqlength/2;++i) {
        xi[i] = abs(tout[i].real() + tout[nyqlength-i].imag()); // response output
        freq[i] = i*(srate/nyqlength); // frequency array explicit output
    }
    return freq;
}

/*
 * Computes a Real Discrete Fourier transform, i.e.,
 * xo[k] = 2/N sum(j=0 -> N-1) xi[j] exp(i 2pi j k / N)
 * with O(N log N) complexity using the butterfly technique
 *
 * NOTE: Only works for arrays whose size is a power-of-two
 */
pattern_arg DSP::rfft1d(pattern_arg &xi, const fft_dir &dir) {
    int s = (int)xi.size();
    int cnt = nearestPo2(s); // only Po2 data size possible
    padding(xi, cnt-s); // input prepared

    int msb = findMSB(cnt);
    double nrm = double(dir)>0 ? 2.0 / double(cnt) : 0.5; // RFFT: 2/N forward, 1/2 reverse
    pattern_arg tempout(cnt); // complex<> doesn't work well with mp-units

    // pre-process the input data
    for (int j = 0; j < cnt; ++j) tempout[j] = nrm * xi[bitr(j, msb)];

    // fft passes
    for (int i = 0; i < msb; ++i) {
        int bm = 1 << i; // butterfly mask
        int bw = 2 << i; // butterfly width
        double ang = double(dir) * myPi / double(bm); // precomputation

        // fft butterflies
        for (int j = 0; j < (cnt/2); ++j) {
            int i1 = ((j >> i) << (i + 1)) + j % bm; // left wing
            int i2 = i1 ^ bm;                        // right wing
            std::complex<double> z1 = std::polar(double(1), ang * double(i1 ^ bw)); // left wing rotation
            std::complex<double> z2 = std::polar(double(1), ang * double(i2 ^ bw)); // right wing rotation
            std::complex<double> tmp = tempout[i1];

            tempout[i1]+= z1 * tempout[i2];
            tempout[i2] = tmp + z2 * tempout[i2];
        }
    }
    return tempout;
}


fft_arg DSP::rfft1d(fft_arg &xi, const fft_dir &dir)
{
    pattern_arg tempin(xi.size()); // complex<> doesn't work well with mp-units
    std::transform(xi.begin(), xi.end(), tempin.begin(),
		   [](std::complex<waveform_value> x){return std::complex<double>{x.real().numerical_value_in(V),
		       x.imag().numerical_value_in(V)};});
    return hidden_rfft1d(tempin, dir);
}


fft_arg DSP::hidden_rfft1d(pattern_arg &xi, const fft_dir &dir) {
  pattern_arg tempin = rfft1d(xi, dir);

  // copy/cast to complex<quantity<V>>
  fft_arg xo(tempin.size()); // filled from tempout in units
    std::transform(tempin.begin(),tempin.end(),xo.begin(),
		   [](std::complex<double> x){return std::complex<waveform_value>{x.real()*V,x.imag()*V};});
    return xo;
}


