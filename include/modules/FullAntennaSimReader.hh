// Sim Antenna Reader for a pipeline module
#ifndef fullantreader_HH
#define fullantreader_HH 1

// std includes
#include <string>

// ROOT
#include "TTreeReader.h"

// must have include for pipeline
#include <Event.hh>

class FullAntennaSimReader
{
    public:
        FullAntennaSimReader(TTreeReader& re1, TTreeReader& re2, std::string outbox); // constructor; required
        // input file name and new Key outbox label.

        DataPack operator()(); // this is called by the pipeline

        // default = -1 for all events
        inline void setMaxEventNumber(int nend) {maxEventNumber = nend;}
        inline void setAntennaN(int na) {nantenna = na;} // for following modules
        inline void setSimConstantBField(quantity<T> b) {Bfield = b;}

    private:
    // include any configuration data members for internal use here.
    int maxEventNumber;
    int evcounter;
    int nantenna;
    quantity<T> Bfield;

    // ROOT file access for member functions
    TTreeReader& reader1;
    TTreeReader& reader2;

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
    TTreeReaderValue<std::vector<int>> aID;
    TTreeReaderValue<std::vector<double>> kevec;
    TTreeReaderValue<std::vector<double>> omvec;
    TTreeReaderValue<std::vector<double>> tvec;
    TTreeReaderValue<std::vector<double>> vvec;

    // these below serve as string keys to access (read/write) the Event map
    std::string outkey;

};
#endif
