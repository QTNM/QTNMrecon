// merge track waveforms for a given event
#ifndef QT_MTR_H
#define QT_MTR_H

// std includes
#include <string>

// ROOT
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// us
#include "Event.hh"

class trackMerger
{
  // trackMerger class
  // can't use WfmReader module: too specific for pipeline use
  // hence read in here. Writing with the writeWfmToRoot module can work.
  
public:
  trackMerger(TTreeReader& re, int na);
  virtual ~trackMerger() = default;
  
  DataPack Loop(); // process file row by row, construct merged data pack
  // if required, otherwise pass incoming data pack through. Writer receives pack.
  
private:
  DataPack read(); // read row in file, construct data pack
  vec_t add(vec_t& sig, double mergetime, vec_t& other); // operation no units in file IO

  DataPack intermediate; // hold one pack
  int nant; // configure at construction
  int prevID;
  
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
};

#endif
