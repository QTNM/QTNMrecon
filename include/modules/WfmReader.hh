// Intermediate waveform Reader for a pipeline module
#ifndef wfmreader_HH
#define wfmreader_HH 1

// std includes
#include <string>

// ROOT
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// must have include for pipeline
#include <Event.hh>

class WfmReader
{
public:
  WfmReader(TTreeReader& re, int na, std::string outbox); // constructor; required
  // input file name and new Key outbox label.

  DataPack operator()(); // this is called by the pipeline

private:
  // include any configuration data members for internal use here.
  // ROOT file access for member functions
  TTreeReader& reader;
  
  TTreeReaderValue<int> nantenna;
  TTreeReaderValue<int> eventID;
  TTreeReaderValue<int> trackID;
  TTreeReaderValue<double> samplingtime;
  TTreeReaderValue<double> starttime;
  TTreeReaderValue<double> avomega;
  TTreeReaderValue<double> beatf;
  TTreeReaderValue<double> chirprate;
  TTreeReaderValue<double> bfield;
  // vertex
  TTreeReaderValue<double> posx;
  TTreeReaderValue<double> posy;
  TTreeReaderValue<double> posz;
  TTreeReaderValue<double> kEnergy;
  TTreeReaderValue<double> pangle;
  // hit data
  TTreeReaderValue<std::vector<int>> hitevID;
  TTreeReaderValue<std::vector<int>> hittrID;
  TTreeReaderValue<std::vector<double>> hitedep; // interaction data
  TTreeReaderValue<std::vector<double>> hittime;
  TTreeReaderValue<std::vector<double>> hitposttheta;
  TTreeReaderValue<std::vector<double>> hitx; // interaction location
  TTreeReaderValue<std::vector<double>> hity;
  TTreeReaderValue<std::vector<double>> hitz;
  // waveform data
  std::vector<TTreeReaderArray<double>> wfmarray;

  // these below serve as string keys to access (read/write) the Event map
  std::string outkey;
  int nant; // configure at construction
};
#endif
