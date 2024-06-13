// Transformer G4 source to antenna response for a pipeline module
#ifndef aresponse_HH
#define aresponse_HH 1

// std includes
#include <string>
#include <vector>

// tool includes

// must have include for pipeline
#include <Event.hh>
#include <VReceiver.hh>
#include "types.hh"

class AntennaResponse
{
    public:
        AntennaResponse(std::string inbox, std::string outbox); // constructor; required
        // Configures the module. Could have more config parameters
        // String Key outbox label as input.

        DataPack operator()(DataPack); // this is called by the pipeline
        // Transforming module event map in and out.

        // getter/setter methods for configuration could live here.
        inline void setAntennaCollection(std::vector<VReceiver*> ant) {receiver = ant;}

    private:
    // include any configuration data members for internal use here.
        std::vector<VReceiver*> receiver;

    // these below serve as string keys to access (read/write) the Event map
        std::string inkey;
        std::string outkey;
};
#endif
