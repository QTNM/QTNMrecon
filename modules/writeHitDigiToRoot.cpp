// Root Digitizer Writer module implementation
// writing a single value; expand for specific outputs.

// std
#include <iostream>
#include <algorithm>
#include <memory>

// us
#include "writeHitDigiToRoot.hh"
#include "types.hh"

WriterHitDigiToRoot::WriterHitDigiToRoot(TTree* tr, int na) : 
  mytree(tr),
  nantenna(na)
{
  // N antennae, one for each waveform; need to know at construction for writing
  // construct scopedata entries
  for (int i=0;i<nantenna;++i) {
    vec_t* vv;
    scopedata.push_back(vv); // vector in scopedata initialized
    vec_t* p;
    purewave.push_back(p); // vector in purewave initialized
  }
  // can now point branch at dummy addresses; makes header only
  mytree->Branch("truth.nantenna",&nant,"truth.nantenna/I");
  mytree->Branch("truth.snratio",&snratio,"truth.snratio/D");
  mytree->Branch("truth.samplingtime_s",&samplingtime,"truth.samplingtime/D");
  mytree->Branch("truth.avomega_Hz",&avomega,"truth.avomega/D");
  mytree->Branch("truth.beatf_Hz",&beatf,"truth.beatf/D");
  mytree->Branch("truth.chirp_Hz_s",&chirprate,"truth.chirp_rate/D");
  mytree->Branch("vertex.evID",&evID,"vertex.evID/I");
  mytree->Branch("vertex.trackID",&trID,"vertex.trackID/I");
  mytree->Branch("vertex.posx_m",&posx,"vertex.posx/D");
  mytree->Branch("vertex.posy_m",&posy,"vertex.posy/D");
  mytree->Branch("vertex.posz_m",&posz,"vertex.posz/D");
  mytree->Branch("vertex.kinenergy_eV",&kEnergy,"vertex.kinenergy/D");
  mytree->Branch("vertex.pitchangle_deg",&pangle,"vertex.pitchangle/D");
  mytree->Branch("digi.gain",&gain,"digi.gain/D");
  mytree->Branch("digi.tfrequency_Hz",&tfrequency,"digi.tfrequency/D");
  mytree->Branch("digi.samplingrate_Hz",&digisamplingrate,"digi.samplingrate/D");
  for (int i=0;i<nantenna;++i) {
    // std::cout << "in write: sptr address for " << i << ", " << &scopedata.at(i) << std::endl;
    // std::cout << "in write: pptr address for " << i << ", " << &purewave.at(i) << std::endl;
    std::string brname = "signal_" + std::to_string(i) + "_V"; // unit in name
    mytree->Branch(brname.data(), &scopedata.at(i)); // point to vec_t dummy address
    brname = "pure_" + std::to_string(i) + "_V"; // unit in name
    mytree->Branch(brname.data(), &purewave.at(i)); // point to vec_t dummy address
  }
  // hit data
  mytree->Branch("hit.eventID", &hitevID); // point to vec<int>* dummy address
  mytree->Branch("hit.trackID", &hittrID); // point to vec<int>* dummy address
  mytree->Branch("hit.locx_m", &hitx); // point to vec<double>* dummy address
  mytree->Branch("hit.locy_m", &hity); // point to vec<double>* dummy address
  mytree->Branch("hit.locz_m", &hitz); // point to vec<double>* dummy address
  mytree->Branch("hit.time_ns", &hittime); // point to vec<double>* dummy address
  mytree->Branch("hit.edep_eV", &hitedep); // point to vec<double>* dummy address
  mytree->Branch("hit.posttheta_deg", &hitposttheta); // point to vec<double>* dummy address

}


void WriterHitDigiToRoot::operator()(DataPack dp)
{
  // extract from datapack and assign to output branch variables with the correct address
  nant    = dp.getTruthRef().nantenna;
  mytree->SetBranchAddress("truth.nantenna",&nant);
  snratio     = dp.getTruthRef().snratio;
  mytree->SetBranchAddress("truth.snratio",&snratio);
  samplingtime = dp.getTruthRef().sampling_time.numerical_value_in(s); // from quantity<ns> no unit for output
  mytree->SetBranchAddress("truth.samplingtime_s",&samplingtime);
  avomega      = dp.getTruthRef().average_omega.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("truth.avomega_Hz",&avomega);
  beatf        = dp.getTruthRef().beat_frequency.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("truth.beatf_Hz",&beatf);
  chirprate    = dp.getTruthRef().chirp_rate.numerical_value_in(Hz/s); // quantity<Hz>
  mytree->SetBranchAddress("truth.chirp_Hz_s",&chirprate);
  // vertex
  evID = dp.getTruthRef().vertex.eventID;
  mytree->SetBranchAddress("vertex.evID",&evID);
  trID = dp.getTruthRef().vertex.trackID;
  mytree->SetBranchAddress("vertex.trackID",&trID);
  posx    = dp.getTruthRef().vertex.posx.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex.posx_m",&posx);
  posy    = dp.getTruthRef().vertex.posy.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex.posy_m",&posy);
  posz    = dp.getTruthRef().vertex.posz.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex.posz_m",&posz);
  kEnergy = dp.getTruthRef().vertex.kineticenergy.numerical_value_in(eV); // quantity<eV>
  mytree->SetBranchAddress("vertex.kinenergy_eV",&kEnergy);
  pangle  = dp.getTruthRef().vertex.pitchangle.numerical_value_in(deg); // quantity<deg>
  mytree->SetBranchAddress("vertex.pitchangle_deg",&pangle);
  // experiment
  gain  = dp.getExperimentRef().gain;
  mytree->SetBranchAddress("digi.gain",&gain);
  tfrequency   = dp.getExperimentRef().target_frequency.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("digi.tfrequency_Hz",&tfrequency);
  digisamplingrate = dp.getExperimentRef().digi_sampling_rate.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("digi.samplingrate_Hz",&digisamplingrate);

  std::string brname;
  for (int i=0;i<nantenna;++i) {
    purewave.at(i) = &dp.getTruthRef().pure.at(i); // vec_t*, copy
    // store
    // std::cout << "pptr address for " << i << ", " << &purewave.at(i) << std::endl;
    brname = "pure_" + std::to_string(i) + "_V"; // unit in name
    mytree->SetBranchAddress(brname.data(), &purewave.at(i));

    scopedata.at(i) = &dp.getExperimentRef().signals.at(i); // vec_t*
    // store with right address
    // std::cout << "sptr address for " << i << ", " << &scopedata.at(i) << std::endl;
    brname = "signal_" + std::to_string(i) + "_V"; // unit in name
    mytree->SetBranchAddress(brname.data(), &scopedata.at(i));
  }
  if (! dp.hitsRef().empty()) { // there are hits to store
    // extract hit data
    for (hit_t hit : dp.hitsRef()) { // get hit struct from vector<hit_t>
      hitevID->push_back(hit.eventID);
      hittrID->push_back(hit.trackID);
      hitx->push_back(hit.locx.numerical_value_in(m)); // no unit in root file
      hity->push_back(hit.locy.numerical_value_in(m));
      hitz->push_back(hit.locz.numerical_value_in(m));
      hitedep->push_back(hit.edeposit.numerical_value_in(eV));
      hittime->push_back(hit.timestamp.numerical_value_in(ns));
      hitposttheta->push_back(hit.anglepost.numerical_value_in(deg));
    }
    mytree->SetBranchAddress("hit.eventID", &hitevID); // point to vec<int>* real address
    mytree->SetBranchAddress("hit.trackID", &hittrID); // point to vec<int>* real address
    mytree->SetBranchAddress("hit.locx_m", &hitx); // point to vec<double>* real address
    mytree->SetBranchAddress("hit.locy_m", &hity); // point to vec<double>* real address
    mytree->SetBranchAddress("hit.locz_m", &hitz); // point to vec<double>* real address
    mytree->SetBranchAddress("hit.time_ns", &hittime); // point to vec<double>* real address
    mytree->SetBranchAddress("hit.edep_eV", &hitedep); // point to vec<double>* real address
    mytree->SetBranchAddress("hit.posttheta_deg", &hitposttheta); // point to vec<double>* real address
  }
  // all output collected, write it
  mytree->Fill();
  // clear internal
  hitevID->clear();
  hittrID->clear();
  hitx->clear();
  hity->clear();
  hitz->clear();
  hittime->clear();
  hitedep->clear();
  hitposttheta->clear();
}
