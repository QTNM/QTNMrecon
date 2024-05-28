#include "dsp.hh"

#include <chrono>
#include <iostream>

int main()
{
    DSP dsp;
    size_t entrySize{8*1024}; // 2^13

    while (entrySize < 1024 * 1024 + 1) // 2^20
    {
        std::cout << std::endl;
        std::cout << "signal of size " << entrySize << std::endl;

        fft_arg xin(entrySize);
        xin.at(0) = 1.0 * V;

        auto begin = std::chrono::steady_clock::now();
        fft_arg xo = dsp.rfft1d(xin, fft_dir::DIR_FWD);

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin);
        std::cout << " milliseconds needed: " << elapsed.count() << std::endl;

        entrySize *= 2;
    }

    return 0;
}
