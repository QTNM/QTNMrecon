// Root Digitizer Writer module implementation
// writing a single value; expand for specific outputs.

// std
#include <iostream>

// us
#include "writeHitDigiToRoot.hh"
#include "types.hh"

WriterHitDigiToRoot::WriterHitDigiToRoot(TTree* tr) : 
  mytree(tr),
  nantenna(1),
  trackHistory(nullptr),
  hitevID(nullptr),
  hittrID(nullptr),
  hitx(nullptr),
  hity(nullptr),
  hitz(nullptr),
  hitedep(nullptr),
  hittime(nullptr),
  hitposttheta(nullptr),
  purewave(nullptr),
  scopedata(nullptr)
{
  // can now point branch at dummy addresses; makes header only
  mytree->Branch("truth_nantenna",&nantenna,"truth_nantenna/I");
  mytree->Branch("truth_snratio",&snratio,"truth_snratio/D");
  mytree->Branch("truth_samplingtime_s",&samplingtime,"truth_samplingtime/D");
  mytree->Branch("truth_starttime_s",&starttime,"truth_starttime/D");
  mytree->Branch("truth_avomega_Hz",&avomega,"truth_avomega/D");
  mytree->Branch("truth_beatf_Hz",&beatf,"truth_beatf/D");
  mytree->Branch("truth_chirp_Hz_s",&chirprate,"truth_chirp_rate/D");
  mytree->Branch("truth_bfield_T",&bfield,"truth_bfield/D");
  mytree->Branch("vertex_evID",&evID,"vertex_evID/I");
  mytree->Branch("vertex_trackID",&trID,"vertex_trackID/I");
  mytree->Branch("vertex_posx_m",&posx,"vertex_posx/D");
  mytree->Branch("vertex_posy_m",&posy,"vertex_posy/D");
  mytree->Branch("vertex_posz_m",&posz,"vertex_posz/D");
  mytree->Branch("vertex_kinenergy_eV",&kEnergy,"vertex_kinenergy/D");
  mytree->Branch("vertex_pitchangle_deg",&pangle,"vertex_pitchangle/D");
  mytree->Branch("vertex_trackHistory",&trackHistory);
  mytree->Branch("digi_gain",&gain,"digi_gain/D");
  mytree->Branch("digi_tfrequency_Hz",&tfrequency,"digi_tfrequency/D");
  mytree->Branch("digi_samplingrate_Hz",&digisamplingrate,"digi_samplingrate/D");

  mytree->Branch("pure_V", &purewave); // vec<vec>* dummy address
  mytree->Branch("signal_V", &scopedata); // vec<vec>* dummy address

  // hit data
  mytree->Branch("hit_eventID", &hitevID); // point to vec<int>* dummy address
  mytree->Branch("hit_trackID", &hittrID); // point to vec<int>* dummy address
  mytree->Branch("hit_locx_m", &hitx); // point to vec<double>* dummy address
  mytree->Branch("hit_locy_m", &hity); // point to vec<double>* dummy address
  mytree->Branch("hit_locz_m", &hitz); // point to vec<double>* dummy address
  mytree->Branch("hit_time_ns", &hittime); // point to vec<double>* dummy address
  mytree->Branch("hit_edep_eV", &hitedep); // point to vec<double>* dummy address
  mytree->Branch("hit_posttheta_deg", &hitposttheta); // point to vec<double>* dummy address

}


void WriterHitDigiToRoot::operator()(DataPack dp)
{
  // extract from datapack and assign to output branch variables with the correct address
  nantenna    = dp.getTruthRef().nantenna;
  mytree->SetBranchAddress("truth_nantenna",&nantenna);
  snratio     = dp.getTruthRef().snratio;
  mytree->SetBranchAddress("truth_snratio",&snratio);
  samplingtime = dp.getTruthRef().sampling_time.numerical_value_in(s); // from quantity<ns> no unit for output
  mytree->SetBranchAddress("truth_samplingtime_s",&samplingtime);
  starttime = dp.getTruthRef().start_time.numerical_value_in(s); // from quantity<ns> no unit for output
  mytree->SetBranchAddress("truth_starttime_s",&starttime);
  avomega      = dp.getTruthRef().average_omega.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("truth_avomega_Hz",&avomega);
  beatf        = dp.getTruthRef().beat_frequency.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("truth_beatf_Hz",&beatf);
  chirprate    = dp.getTruthRef().chirp_rate.numerical_value_in(Hz/s); // quantity<Hz>
  mytree->SetBranchAddress("truth_chirp_Hz_s",&chirprate);
  bfield       = dp.getTruthRef().bfield.numerical_value_in(T); // quantity<Hz>
  mytree->SetBranchAddress("truth_bfield_T",&bfield);
  // vertex
  evID = dp.getTruthRef().vertex.eventID;
  mytree->SetBranchAddress("vertex_evID",&evID);
  trID = dp.getTruthRef().vertex.trackID;
  mytree->SetBranchAddress("vertex_trackID",&trID);
  posx    = dp.getTruthRef().vertex.posx.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex_posx_m",&posx);
  posy    = dp.getTruthRef().vertex.posy.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex_posy_m",&posy);
  posz    = dp.getTruthRef().vertex.posz.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex_posz_m",&posz);
  kEnergy = dp.getTruthRef().vertex.kineticenergy.numerical_value_in(eV); // quantity<eV>
  mytree->SetBranchAddress("vertex_kinenergy_eV",&kEnergy);
  pangle  = dp.getTruthRef().vertex.pitchangle.numerical_value_in(deg); // quantity<deg>
  mytree->SetBranchAddress("vertex_pitchangle_deg",&pangle);
  trackHistory  = &dp.getTruthRef().vertex.trackHistory; // vector<int>*
  mytree->SetBranchAddress("vertex_trackHistory",&trackHistory);
  // experiment
  gain  = dp.getExperimentRef().gain;
  mytree->SetBranchAddress("digi_gain",&gain);
  tfrequency   = dp.getExperimentRef().target_frequency.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("digi_tfrequency_Hz",&tfrequency);
  digisamplingrate = dp.getExperimentRef().digi_sampling_rate.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("digi_samplingrate_Hz",&digisamplingrate);

  purewave->clear();
  scopedata->clear();
  for (int i=0;i<nantenna;++i) {
    // store
    scopedata->push_back(dp.getExperimentRef().signals.at(i)); // vec_t
    purewave->push_back(dp.getTruthRef().pure.at(i)); // vec_t, copy
  }
  mytree->SetBranchAddress("pure_V", &purewave); // point to vec<vec>* real address
  mytree->SetBranchAddress("signal_V", &scopedata); // point to vec<vec>* real address

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
    mytree->SetBranchAddress("hit_eventID", &hitevID); // point to vec<int>* real address
    mytree->SetBranchAddress("hit_trackID", &hittrID); // point to vec<int>* real address
    mytree->SetBranchAddress("hit_locx_m", &hitx); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_locy_m", &hity); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_locz_m", &hitz); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_time_ns", &hittime); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_edep_eV", &hitedep); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_posttheta_deg", &hitposttheta); // point to vec<double>* real address
  }
  // all output collected, write it
  mytree->Fill();
  // clear internal
  if (!hitevID->empty()) {
    hitevID->clear();
    hittrID->clear();
    hitx->clear();
    hity->clear();
    hitz->clear();
    hittime->clear();
    hitedep->clear();
    hitposttheta->clear();
  }
}
