// Sim Hit Reader for a pipeline module
#ifndef simhitreader_HH
#define simhitreader_HH 1

// std includes
#include <string>

// ROOT
#include "TFile.h"
#include "TTreeReader.h"

// must have include for pipeline
#include <Event.hh>

class QTNMSimHitReader
{
    public:
        QTNMSimHitReader(std::string fname, std::string outbox); // constructor; required
        // input file name and new Key outbox label.
        ~QTNMSimHitReader(); // destructor to close the file

        Event_map<std::any> operator()(); // this is called by the pipeline

        // default = -1 for all events
        inline void setMaxEventNumber(int nend) {maxEventNumber = nend;}

    private:
    // include any configuration data members for internal use here.
    int maxEventNumber;
    int evcounter;

    // ROOT file access for member functions
    TFile* fin = nullptr;
    TTreeReader* reader = nullptr;

    // these below serve as string keys to access (read/write) the Event map
    std::string outkey;

};
#endif
