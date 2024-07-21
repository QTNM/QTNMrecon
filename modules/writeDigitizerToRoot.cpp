// Root Digitizer Writer module implementation
// writing a single value; expand for specific outputs.

// std
#include <iostream>
#include <algorithm>

// us
#include "writeDigitizerToRoot.hh"
#include "types.hh"

WriterDigiToRoot::WriterDigiToRoot(TTree* tr, int na) : 
  mytree(tr),
  nantenna(na)
{
  // N antennae, one for each waveform; need to know at construction for writing
  // construct scopedata entries
  for (int i=0;i<nantenna;++i) {
    std::vector<double> vv;
    scopedata.push_back(vv); // vector in scopedata initialized
    std::vector<double> p;
    purewave.push_back(p); // vector in purewave initialized
  }
  // can now point branch at dummy addresses; makes header only
  mytree->Branch("truth.nantenna",&nant,"truth.nantenna/I");
  mytree->Branch("truth.snratio",&snratio,"truth.snratio/D");
  mytree->Branch("truth.samplingtime_s",&samplingtime,"truth.samplingtime/D");
  mytree->Branch("truth.avomega_Hz",&avomega,"truth.avomega/D");
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
    std::string brname = "signal_" + std::to_string(i) + "_V"; // unit in name
    mytree->Branch(brname.data(), &scopedata.at(i)); // point to std::vector<double>
    brname = "pure_" + std::to_string(i) + "_V"; // unit in name
    mytree->Branch(brname.data(), &purewave.at(i)); // point to std::vector<double>
  }
}


void WriterDigiToRoot::operator()(DataPack dp)
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

  for (int i=0;i<nantenna;++i) {
    waveform_t sig = dp.getExperimentRef().signals.at(i);
    std::cout << "got wave size " << sig.size() << std::endl;
    // strip units from vector entries
    scopedata.at(i).resize(sig.size());
    std::transform(sig.begin(), sig.end(), scopedata.at(i).begin(),[](waveform_value x){return x.numerical_value_in(V);});
    // store with right address
    std::vector<double>* sptr = &scopedata.at(i);
    std::string brname = "signal_" + std::to_string(i) + "_V"; // unit in name
    mytree->SetBranchAddress(brname.data(), &sptr);

    waveform_t pure = dp.getTruthRef().pure.at(i);
    std::cout << "got truth wave size " << pure.size() << std::endl;
    // strip units from vector entries
    purewave.at(i).resize(pure.size());
    std::transform(pure.begin(), pure.end(), purewave.at(i).begin(),[](waveform_value x){return x.numerical_value_in(V);});
    // store
    std::vector<double>* pptr = &purewave.at(i);
    brname = "pure_" + std::to_string(i) + "_V"; // unit in name
    mytree->SetBranchAddress(brname.data(), &pptr);
  }
  // all output collected, write it
  mytree->Fill();

  // clear memory
  for (int i=0;i<nantenna;++i) {
    scopedata.at(i).clear();
    purewave.at(i).clear();
  }

}
