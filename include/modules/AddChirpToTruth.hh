// Simply add the cirp rate to truth data in module

// std includes
#include <string>

// ROOT includes
#include "TLinearFitter.h"

// must have include for pipeline
#include <Event.hh>

class AddChirpToTruth
{
    public:
        AddChirpToTruth(std::string inbox); // constructor; required
        // Configures the module. Could have more config parameters
        // Minimum required are the key labels for input and output 
        // of Event Map data item. String Key inbox label and new Key outbox label.

        DataPack operator()(DataPack dp); // this is called by the pipeline
        // any module in the pipeline should be able to receive an 
        // event map and return an event map. 
        // Only Readers should expect no input argument and return an event map
        // and Writers only receive an event map and return void as signature.

        // getter/setter methods for configuration could live here.
        inline void setAntennaNumber(int n) {nantenna = n;} // default set to 1

    private:
    // include any configuration data members for internal use here.
    TLinearFitter* lft = nullptr;
    int nantenna;
    quantity<Hz> e2f(quantity<keV> energy, quantity<T> bf); // convert keV to Hz for chirp

    // these below serve as string keys to access (read/write) the Event map
    std::string inkey;
};
