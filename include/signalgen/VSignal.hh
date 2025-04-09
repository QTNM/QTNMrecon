// QTNM signal generator abstract interface class
#ifndef QT_VSIGNAL_H
#define QT_VSIGNAL_H

// us
#include "types.hh"


class VSignal
{
    public:
    // abstract class -> no constructor
    virtual ~VSignal() = default;

    virtual vec_t generate_pattern() = 0;
    virtual waveform_t generate() = 0;
    // must be implemented by a concrete signal generator class
    // specific characteristics of a signal are set in that derived
    // class but the operation to generate is common to all hence
    // this interface

    virtual waveform_t add(waveform_t& other, size_t position = 0) = 0;
    // any signal generator should be able to add a generated
    // signal from another (other) to its own signal at any
    // permitted position in the container (default = 0).
    // That other must inherit from VSignal.


    protected:
    // state change signals a required re-calculation for
    // any signal generator. Short-cuts repeated requests
    // when nothing has changed and identical signals should
    // be emitted. Default is to always re-calculate.
    // Access from inherited classes directly.
    bool state_change = true;

};

#endif
