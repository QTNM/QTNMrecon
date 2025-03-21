// Root Digitizer Writer module implementation
// writing a single value; expand for specific outputs.

// std
#include <iostream>

// us
#include "writeDigitizerToRoot.hh"

WriterDigiToRoot::WriterDigiToRoot(TTree* tr, int na) : 
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
  mytree->Branch("truth_nantenna",&nant,"truth_nantenna/I");
  mytree->Branch("truth_snratio",&snratio,"truth_snratio/D");
  mytree->Branch("truth_samplingtime_s",&samplingtime,"truth_samplingtime/D");
  mytree->Branch("truth_starttime_s",&starttime,"truth_starttime/D");
  mytree->Branch("truth_avomega_Hz",&avomega,"truth_avomega/D");
  mytree->Branch("truth_beatf_Hz",&beatf,"truth_beatf/D");
  mytree->Branch("truth_chirp_Hz_s",&chirprate,"truth_chirp_rate/D");
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
  for (int i=0;i<nantenna;++i) {
    // std::cout << "in write: sptr address for " << i << ", " << &scopedata.at(i) << std::endl;
    // std::cout << "in write: pptr address for " << i << ", " << &purewave.at(i) << std::endl;
    std::string brname = "signal_" + std::to_string(i) + "_V"; // unit in name
    mytree->Branch(brname.data(), &scopedata.at(i)); // point to vec_t dummy address
    brname = "pure_" + std::to_string(i) + "_V"; // unit in name
    mytree->Branch(brname.data(), &purewave.at(i)); // point to vec_t dummy address
  }
}


void WriterDigiToRoot::operator()(DataPack dp)
{
  // extract from datapack and assign to output branch variables with the correct address
  nant    = dp.getTruthRef().nantenna;
  mytree->SetBranchAddress("truth_nantenna",&nant);
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
  trackHistory  = dp.getTruthRef().vertex.trackHistory; // vector<int>
  mytree->SetBranchAddress("vertex_trackHistory",&trackHistory);
  // experiment
  gain  = dp.getExperimentRef().gain;
  mytree->SetBranchAddress("digi_gain",&gain);
  tfrequency   = dp.getExperimentRef().target_frequency.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("digi_tfrequency_Hz",&tfrequency);
  digisamplingrate = dp.getExperimentRef().digi_sampling_rate.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("digi_samplingrate_Hz",&digisamplingrate);

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
  // all output collected, write it
  mytree->Fill();
}
