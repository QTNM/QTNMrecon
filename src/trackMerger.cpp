// ------------------------------------
// trackMerger methods implementation

// std
#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>

// us
#include "trackMerger.hh"

trackMerger::trackMerger(TTreeReader& re, TTree* tr, int na) : 
  nant(na),
  prevID(-1),
  reader(re),
  mytree(tr)
  nantenna(reader, "truth_nantenna"),
  eventID(reader, "vertex_evID"), // needs reader by reference
  trackID(reader, "vertex_trackID"),
  hitevID(reader, "hit_eventID"), // interaction data
  hittrID(reader, "hit_trackID"),
  hitedep(reader, "hit_edep_eV"), 
  hittime(reader, "hit_time_ns"),
  hitposttheta(reader, "hit_posttheta_deg"),
  hitx(reader, "hit_locx_m"), // interaction location
  hity(reader, "hit_locy_m"),
  hitz(reader, "hit_locz_m"),
  posx(reader, "vertex_posx_m"), // vertex data
  posy(reader, "vertex_posy_m"),
  posz(reader, "vertex_posz_m"),
  kEnergy(reader, "vertex_kinenergy_eV"),
  pangle(reader, "vertex_pitchangle_deg"),
  samplingtime(reader, "truth_samplingtime_s"),
  starttime(reader, "truth_starttime_s"),
  avomega(reader, "truth_avomega_Hz"),
  beatf(reader, "truth_beatf_Hz"),
  chirprate(reader, "truth_chirp_Hz_s"),
  bfield(reader, "truth_bfield_T")
{
  // int nant for n antenna is needed at construction time
  std::string brname;
  for (int i=0;i<nant;++i) {
    brname = "sampled_" + std::to_string(i) + "_V";
    wfmarray.emplace_back(reader, brname.data());
  }
  std::cout << "in reader n entries: " << reader.GetEntries() << std::endl;

  // prepare writer structure
  // N antennae, one for each waveform; need to know at construction for writing
  // construct scopedata entries
  for (int i=0;i<nant;++i) {
    vec_t* p;
    purewave.push_back(p); // vector in purewave initialized
  }
  // can now point branch at dummy addresses; makes header only
  mytree->Branch("truth_nantenna",&nant,"truth_nantenna/I");
  mytree->Branch("truth_samplingtime_s",&samplingtimeOut,"truth_samplingtime/D");
  mytree->Branch("truth_starttime_s",&starttimeOut,"truth_starttime/D");
  mytree->Branch("truth_avomega_Hz",&avomegaOut,"truth_avomega/D");
  mytree->Branch("truth_beatf_Hz",&beatfOut,"truth_beatf/D");
  mytree->Branch("truth_chirp_Hz_s",&chirprateOut,"truth_chirp_rate/D");
  mytree->Branch("truth_bfield_T",&bfieldOut,"truth_bfield/D");
  mytree->Branch("vertex_evID",&evID,"vertex_evID/I");
  mytree->Branch("vertex_trackID",&trID,"vertex_trackID/I");
  mytree->Branch("vertex_posx_m",&posxOut,"vertex_posx/D");
  mytree->Branch("vertex_posy_m",&posyOut,"vertex_posy/D");
  mytree->Branch("vertex_posz_m",&poszOut,"vertex_posz/D");
  mytree->Branch("vertex_kinenergy_eV",&kEnergyOut,"vertex_kinenergy/D");
  mytree->Branch("vertex_pitchangle_deg",&pangleOut,"vertex_pitchangle/D");
  mytree->Branch("vertex_trackHistory",&trackHistory); // mader in Merger
  for (int i=0;i<nant;++i) {
    brname = "sampled_" + std::to_string(i) + "_V"; // unit in name
    mytree->Branch(brname.data(), &purewave.at(i)); // point to vec_t dummy address
  }
  // hit data
  mytree->Branch("hit_eventID", &hitevIDOut); // point to vec<int>* dummy address
  mytree->Branch("hit_trackID", &hittrIDOut); // point to vec<int>* dummy address
  mytree->Branch("hit_locx_m", &hitxOut); // point to vec<double>* dummy address
  mytree->Branch("hit_locy_m", &hityOut); // point to vec<double>* dummy address
  mytree->Branch("hit_locz_m", &hitzOut); // point to vec<double>* dummy address
  mytree->Branch("hit_time_ns", &hittimeOut); // point to vec<double>* dummy address
  mytree->Branch("hit_edep_eV", &hitedepOut); // point to vec<double>* dummy address
  mytree->Branch("hit_posttheta_deg", &hitpostthetaOut); // point to vec<double>* dummy address

  std::cout << "in merger, TTree set up." << std::endl;
}


void trackMerger::Loop()
{
}


DataPack trackMerger::readRow()
{
  // protection against and of file must come from outside.
  Event_map<std::any> eventmap; // data item for delivery
  Event<std::any> outdata; // to hold all the data items from file
  
  // collect all trajectory info from file, reader holds event iterator
  // std::cout << "reader called" << std::endl;
  std::string brname;
  reader.Next();
  // local storage
  for (int i=0;i<nant;++i) {
    brname = "sampled_" + std::to_string(i) + "_V";
    vec_t wfm(wfmarray.at(i).begin(), wfmarray.at(i).end());
    outdata[brname] = std::make_any<vec_t>(wfm);
    localWfm.push_back(wfm); // local copy for potential merging
  }
  eventmap["internal"] = outdata; // with outdata an Event<std::any>

  // make data product, full copy of in from file.
  // at the end, store new data product in dictionary event map.
  DataPack dp(eventmap);
  // fill truth struct with vertex info, restore units from branch names
  dp.getTruthRef().vertex.eventID = *eventID;
  prevID = *eventID; // local copy
  dp.getTruthRef().vertex.trackID = *trackID;
  prevTrackID = *trackID;
  dp.getTruthRef().vertex.posx = *posx * m;
  dp.getTruthRef().vertex.posy = *posy * m;
  dp.getTruthRef().vertex.posz = *posz * m;
  dp.getTruthRef().vertex.kineticenergy = *kEnergy * eV;
  dp.getTruthRef().vertex.pitchangle = *pangle * deg;
  
  // check on hits, separately from trajectory reader
  // the hit reader may or may not hold data.
  if (! hitevID->empty()) {
    for (unsigned int j=0;j<hitevID->size();++j) {
      dp.getHitRef().eventID   = hitevID->at(j);
      dp.getHitRef().trackID   = hittrID->at(j);
      dp.getHitRef().edeposit  = hitedep->at(j) * eV;
      dp.getHitRef().timestamp = hittime->at(j) * ns;
      dp.getHitRef().anglepost = hitposttheta->at(j) * deg;
      dp.getHitRef().locx = hitx->at(j) * m;
      dp.getHitRef().locy = hity->at(j) * m;
      dp.getHitRef().locz = hitz->at(j) * m;
      // store the filled hit_t
      dp.hitsRef().push_back(dp.getHit());
    }
  }
  dp.getTruthRef().nantenna = *nantenna; // store input truth
  dp.getTruthRef().chirp_rate = *chirprate * Hz/s; // store input truth
  dp.getTruthRef().beat_frequency = *beatf * Hz; // store input truth
  dp.getTruthRef().average_omega = *avomega * Hz; // store input truth
  dp.getTruthRef().sampling_time = *samplingtime * s; // store input truth
  dp.getTruthRef().start_time = *starttime * s; // store input truth
  localStart = *starttime; // local copy, unit [s] implicit
  dp.getTruthRef().bfield = *bfield * T; // store input truth
  return dp;
}


void trackMerger::writeRow(DataPack dp)
{
  // extract from datapack and assign to output branch variables with the correct address
  mytree->SetBranchAddress("truth_nantenna",&nantenna);
  samplingtimeOut = dp.getTruthRef().sampling_time.numerical_value_in(s); // from quantity<ns> no unit for output
  mytree->SetBranchAddress("truth_samplingtime_s",&samplingtimeOut);
  starttimeOut = dp.getTruthRef().start_time.numerical_value_in(s); // from quantity<ns> no unit for output
  mytree->SetBranchAddress("truth_starttime_s",&starttimeOut);
  avomegaOut      = dp.getTruthRef().average_omega.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("truth_avomega_Hz",&avomegaOut);
  beatfOut        = dp.getTruthRef().beat_frequency.numerical_value_in(Hz); // quantity<Hz>
  mytree->SetBranchAddress("truth_beatf_Hz",&beatfOut);
  chirprateOut    = dp.getTruthRef().chirp_rate.numerical_value_in(Hz/s); // quantity<Hz>
  mytree->SetBranchAddress("truth_chirp_Hz_s",&chirprateOut);
  bfieldOut       = dp.getTruthRef().bfield.numerical_value_in(T); // quantity<Hz>
  mytree->SetBranchAddress("truth_bfield_T",&bfieldOut);
  // vertex
  evID = dp.getTruthRef().vertex.eventID;
  mytree->SetBranchAddress("vertex_evID",&evID);
  trID = dp.getTruthRef().vertex.trackID;
  mytree->SetBranchAddress("vertex_trackID",&trID);
  posxOut    = dp.getTruthRef().vertex.posx.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex_posx_m",&posxOut);
  posyOut    = dp.getTruthRef().vertex.posy.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex_posy_m",&posyOut);
  poszOut    = dp.getTruthRef().vertex.posz.numerical_value_in(m); // quantity<m>
  mytree->SetBranchAddress("vertex_posz_m",&poszOut);
  kEnergyOut = dp.getTruthRef().vertex.kineticenergy.numerical_value_in(eV); // quantity<eV>
  mytree->SetBranchAddress("vertex_kinenergy_eV",&kEnergyOut);
  pangleOut  = dp.getTruthRef().vertex.pitchangle.numerical_value_in(deg); // quantity<deg>
  mytree->SetBranchAddress("vertex_pitchangle_deg",&pangleOut);
  trackHistory  = dp.getTruthRef().vertex.trackHistory; // vector<int>
  mytree->SetBranchAddress("vertex_trackHistory",&trackHistory);

  Event<std::any> indata = dp.getRef()["internal"];
  for (int i=0;i<nantenna;++i) {
    // store
    brname = "sampled_" + std::to_string(i) + "_V"; // unit in name
    vec_t dummy = std::any_cast<vec_t>(indata[brname]); // construct first
    purewave.at(i) = &dummy; // vec_t*, copy
    mytree->SetBranchAddress(brname.data(), &purewave.at(i));
  }
  if (! dp.hitsRef().empty()) { // there are hits to store
    // extract hit data
    for (hit_t hit : dp.hitsRef()) { // get hit struct from vector<hit_t>
      hitevIDOut->push_back(hit.eventID);
      hittrIDOut->push_back(hit.trackID);
      hitxOut->push_back(hit.locx.numerical_value_in(m)); // no unit in root file
      hityOut->push_back(hit.locy.numerical_value_in(m));
      hitzOut->push_back(hit.locz.numerical_value_in(m));
      hitedepOut->push_back(hit.edeposit.numerical_value_in(eV));
      hittimeOut->push_back(hit.timestamp.numerical_value_in(ns));
      hitpostthetaOut->push_back(hit.anglepost.numerical_value_in(deg));
    }
    mytree->SetBranchAddress("hit_eventID", &hitevIDOut); // point to vec<int>* real address
    mytree->SetBranchAddress("hit_trackID", &hittrIDOut); // point to vec<int>* real address
    mytree->SetBranchAddress("hit_locx_m", &hitxOut); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_locy_m", &hityOut); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_locz_m", &hitzOut); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_time_ns", &hittimeOut); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_edep_eV", &hitedepOut); // point to vec<double>* real address
    mytree->SetBranchAddress("hit_posttheta_deg", &hitpostthetaOut); // point to vec<double>* real address
  }
  // all output collected, write it
  mytree->Fill();
  // clear internal
  hitevIDOut->clear();
  hittrIDOut->clear();
  hitxOut->clear();
  hityOut->clear();
  hitzOut->clear();
  hittimeOut->clear();
  hitedepOut->clear();
  hitpostthetaOut->clear();
}


void trackMerger::add(vec_t& other)
{
}
