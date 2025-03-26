// merge track waveforms for a given event
#ifndef QT_MTR_H
#define QT_MTR_H

// std includes
#include <string>
#include <vector>

// ROOT
#include "TTree.h"
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
  trackMerger(TTreeReader& re, TTree* output, int na);
  virtual ~trackMerger() = default;
  
  void Loop(); // process file row by row, construct merged data pack
  // if required, otherwise pass incoming data pack through. Writer receives pack.

  
private:
  // member functions
  DataPack readRow(); // read row in file, construct data pack
  void writeRow(DataPack dp); // write row in file from Datapack
  void add(vec_t& other, int whichAntenna); // operation no units in file IO
  inline void clearLocal() {localWfm.clear();}

  // local copies for potential merger
  int nant; // configure at construction
  int prevID;
  int prevTrackID;
  double localStart;
  vec_t<vec_t> localWfm;
  
  // ROOT file read access for member functions
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

  // ROOT file write access
  TTree* mytree;
  std::vector<vec_t*> purewave; // no unit storage in ROOT file
  // doubles/int for values without unit
  int evID, trID;
  double samplingtimeOut; // from quantity<ns>
  double starttimeOut; // from quantity<ns>
  double avomegaOut; // quantity<Hz>
  double beatfOut; // quantity<Hz>
  double chirprateOut; // quantity<Hz/s>
  double bfieldOut; // quantity<T>
  // vertex
  double posxOut; // quantity<m>
  double posyOut; // quantity<m>
  double poszOut; // quantity<m>
  double kEnergyOut; // quantity<eV>
  double pangleOut; // quantity<deg>
  std::vector<int>* trackHistory;
  // hit data
  std::vector<int>* hitevIDOut;
  std::vector<int>* hittrIDOut;
  std::vector<double>* hitxOut;
  std::vector<double>* hityOut;
  std::vector<double>* hitzOut;
  std::vector<double>* hitedepOut;
  std::vector<double>* hittimeOut;
  std::vector<double>* hitpostthetaOut;
};

#endif
