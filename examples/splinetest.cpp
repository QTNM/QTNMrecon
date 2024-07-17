// Spline test
#include "spline.h"
#include "types.hh"

#include <iostream>

int main()
{
    static vec_t X = {0.1, 0.4, 1.2, 1.8, 2.0};
    static vec_t Y = {0.1, 0.7, 0.6, 1.1, 0.9};

    spline s(X,Y);

    std::cout << s(0.2) << ", " << s(1.2) << ", " << s(1.9) << std::endl;

    return 0;
}
