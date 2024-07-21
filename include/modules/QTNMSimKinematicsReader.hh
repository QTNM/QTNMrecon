// Sim Antenna Reader for a pipeline module
#ifndef simkinreader_HH
#define simkinreader_HH 1

// std includes
#include <string>

// ROOT
#include "TTreeReader.h"

// must have include for pipeline
#include <Event.hh>

class QTNMSimKinematicsReader
{
    public:
        QTNMSimKinematicsReader(TTreeReader& re, std::string outbox); // constructor; required
        // input file name and new Key outbox label.

        DataPack operator()(); // this is called by the pipeline

        // default = -1 for all events
        inline void setMaxEventNumber(int nend) {maxEventNumber = nend;}
        inline void setSimConstantBField(quantity<T> b) {Bfield = b;}

    private:
    // include any configuration data members for internal use here.
    int maxEventNumber;
    int evcounter;
    quantity<T> Bfield;

    // ROOT file access for member functions
    TTreeReader& reader;
    TTreeReaderValue<int> eventID;
    TTreeReaderValue<int> trackID;
    TTreeReaderValue<double> posx;
    TTreeReaderValue<double> posy;
    TTreeReaderValue<double> posz;
    TTreeReaderValue<double> kine;
    TTreeReaderValue<double> pangle;
    TTreeReaderValue<std::vector<double>> tvec;
    TTreeReaderValue<std::vector<double>> omvec;
    TTreeReaderValue<std::vector<double>> kevec;
    TTreeReaderValue<std::vector<double>> pxvec;
    TTreeReaderValue<std::vector<double>> pyvec;
    TTreeReaderValue<std::vector<double>> pzvec;
    TTreeReaderValue<std::vector<double>> bxvec;
    TTreeReaderValue<std::vector<double>> byvec;
    TTreeReaderValue<std::vector<double>> bzvec;
    TTreeReaderValue<std::vector<double>> axvec;
    TTreeReaderValue<std::vector<double>> ayvec;
    TTreeReaderValue<std::vector<double>> azvec;

    // these below serve as string keys to access (read/write) the Event map
    std::string outkey;

};
#endif
