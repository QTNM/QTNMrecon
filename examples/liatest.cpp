// lock-in amplifier test

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <random>

#include "LIA.hh"
#include "SineGenerator.hh"

int main()
{
    quantity<Hz> srate = 1000.0 * Hz; // [Hz] sampling
    quantity<s> duration = 1.0 * s; // [s]
    quantity<Hz> ff = 20.0 * Hz;
    quantity<Hz> fcut = 5.0 * Hz;

    LIA lia(fcut, srate);

    // make data
    SineGenerator sinsig(1.0*V, ff, srate, duration, 90.0*deg); // cos
    waveform_t data = sinsig.generate();
    vec_t ref = sinsig.generate_pattern();
    lia.SetReference(ref);

    sinsig.setphase_rad(0.0 * rad);
    vec_t crref = sinsig.generate_pattern();
    lia.SetCrossReference(crref);

    waveform_t output = lia.quadrature(data);
    std::cout << "got signal of size: " << output.size() << std::endl;

    std::ofstream ofs("lia.csv", std::ofstream::out);
    for (unsigned int i=0;i<output.size();++i)
        ofs << output[i].numerical_value_in(V) << std::endl;
    ofs.close();
    return 0;
}
