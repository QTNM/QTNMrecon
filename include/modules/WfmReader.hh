// Intermediate waveform Reader for a pipeline module
#ifndef wfmreader_HH
#define wfmreader_HH 1

// std includes
#include <string>

// ROOT
#include "TTreeReader.h"

// must have include for pipeline
#include <Event.hh>
#include "types.hh"

class WfmReader
{
    public:
        WfmReader(TTreeReader& re, std::string outbox); // constructor; required
        // input file name and new Key outbox label.

        DataPack operator()(); // this is called by the pipeline

    private:
    // include any configuration data members for internal use here.
    // ROOT file access for member functions
    TTreeReader& reader;

    TTreeReaderValue<int> eventID;
    TTreeReaderValue<int> trackID;
    TTreeReaderValue<int> hitevID;
    TTreeReaderValue<int> hittrID;
    TTreeReaderValue<double> edep; // interaction data
    TTreeReaderValue<double> tstamp;
    TTreeReaderValue<double> prek;
    TTreeReaderValue<double> postk;
    TTreeReaderValue<double> preth;
    TTreeReaderValue<double> postth;
    TTreeReaderValue<double> locx; // interaction location
    TTreeReaderValue<double> locy;
    TTreeReaderValue<double> locz;
    TTreeReaderValue<double> posx; // vertex data
    TTreeReaderValue<double> posy;
    TTreeReaderValue<double> posz;
    TTreeReaderValue<double> kine;
    TTreeReaderValue<double> pangle;
    TTreeReaderValue<std::vector<vec_t*>> wfmvec;

    // these below serve as string keys to access (read/write) the Event map
    std::string outkey;

};
#endif
