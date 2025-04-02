// Root Fake Geant4 Antenna Sim Writer module implementation

// std
#include <iostream>

// us
#include "FakeG4AntWriter.hh"


FakeG4AntToRoot::FakeG4AntToRoot(std::string inkey, TTree* tr) : 
  inkey(std::move(inkey)),
  mytree(tr),
  antennaID(nullptr),
  timevec(nullptr),
  voltagevec(nullptr),
  KEdummy(nullptr),
  OMdummy(nullptr)
{
  // can now point branch at dummy addresses; makes header only
  mytree->Branch("EventID",&evID,"EventID/I");
  mytree->Branch("TrackID",&trID,"TrackID/I");
  mytree->Branch("Posx",&dx,"Posx/D");
  mytree->Branch("Posy",&dy,"Posy/D");
  mytree->Branch("Posz",&dz,"Posz/D");
  mytree->Branch("KinEnergy",&dke,"KinEnergy/D");
  mytree->Branch("PitchAngle",&dang,"PitchAngle/D");

  mytree->Branch("AntennaID", &antennaID); // point to vec_t dummy address
  mytree->Branch("TimeVec", &timevec); // point to vec_t dummy address
  mytree->Branch("VoltageVec", &voltagevec); // point to vec_t dummy address
  mytree->Branch("OmVec", &KEdummy); // point to vec_t dummy address
  mytree->Branch("KEVec", &OMdummy); // point to vec_t dummy address

}


void FakeG4AntToRoot::operator()(DataPack dp)
{
  // extract from datapack and assign to output branch variables with the correct address
  // have generator to fill eventID and trackID
  evID = dp.getTruthRef().vertex.eventID;
  mytree->SetBranchAddress("EventID",&evID);
  trID = dp.getTruthRef().vertex.trackID;
  mytree->SetBranchAddress("TrackID",&trID);
  dx    = 0.0; // empty
  mytree->SetBranchAddress("Posx",&dx);
  dy    = 0.0;
  mytree->SetBranchAddress("Posy",&dy);
  dz    = 0.0;
  mytree->SetBranchAddress("Posz",&dz);
  dke   = 0.0;
  mytree->SetBranchAddress("KinEnergy",&dke);
  dang  = 0.0;
  mytree->SetBranchAddress("PitchAngle",&dang);

  // have generator to fill these 3 vectors
  Event<std::any> indata = dp.getRef()[inkey];
  std::vector<int> daID = std::any_cast<std::vector<int>>(indata["AntennaID"]); // vec<int>
  antennaID   = &daID; // vec<int>* copy
  std::cout << " casting test: size=" << antennaID->size() << std::endl;
  mytree->SetBranchAddress("AntennaID",&antennaID);
  vec_t dtvec = std::any_cast<vec_t>(indata["TimeVec"]); // vec_t
  timevec     = &dtvec; // vec_t* copy
  mytree->SetBranchAddress("TimeVec",&timevec);
  vec_t dvvec = std::any_cast<vec_t>(indata["VoltageVec"]); // vec_t
  voltagevec  = &dvvec; // vec_t* copy
  mytree->SetBranchAddress("VoltageVec",&voltagevec);
  mytree->SetBranchAddress("OmVec",&OMdummy); // empty
  mytree->SetBranchAddress("KEVec",&KEdummy);

  // all output collected, write it
  mytree->Fill();
}
