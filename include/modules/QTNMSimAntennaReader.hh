// Sim Antenna Reader for a pipeline module
#ifndef simantreader_HH
#define simantreader_HH 1

// std includes
#include <string>

// ROOT
#include "TTreeReader.h"

// must have include for pipeline
#include <Event.hh>

class QTNMSimAntennaReader
{
    public:
        QTNMSimAntennaReader(TTreeReader& re, std::string outbox); // constructor; required
        // input file name and new Key outbox label.

        Event_map<std::any> operator()(); // this is called by the pipeline

        // default = -1 for all events
        inline void setMaxEventNumber(int nend) {maxEventNumber = nend;}

    private:
    // include any configuration data members for internal use here.
    int maxEventNumber;
    int evcounter;

    // ROOT file access for member functions
    TTreeReader& reader;

    // these below serve as string keys to access (read/write) the Event map
    std::string outkey;

};
#endif
