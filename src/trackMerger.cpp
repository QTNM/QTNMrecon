// ------------------------------------
// trackMerger methods implementation

// std
#include <algorithm>
#include <functional>
#include <iostream>
#include <cmath>

// us
#include "trackMerger.hh"
#include <mp-units/ostream.h> // for cout stream

trackMerger::trackMerger(TTreeReader& re, TTree* tr) : 
  prevID(-1),
  reader(re),
  mytree(tr),
  nant(1),
  trackHistory(nullptr),
  purewave(nullptr),
  hitevIDOut(nullptr),
  hittrIDOut(nullptr),
  hitxOut(nullptr),
  hityOut(nullptr),
  hitzOut(nullptr),
  hitedepOut(nullptr),
  hittimeOut(nullptr),
  hitpostthetaOut(nullptr),
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
  bfield(reader, "truth_bfield_T"),
  wfmarray(reader, "sampled_V")
{

  std::cout << "in reader n entries: " << reader.GetEntries() << std::endl;

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
  mytree->Branch("vertex_trackHistory",&trackHistory); // made in Merger

  mytree->Branch("sampled_V", &purewave); // point to vec<vec>* dummy address
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
  // init prevID=-1
  int evtag;
  int mergedID = 0; // fixed trackID for merged track

  // set up DataPack structure for new merged track
  std::string brname;
  Event_map<std::any> eventmap; // data item for delivery
  Event<std::any> outdata; // to hold all the data items from file

  // make a new, empty DataPack to be filled.
  eventmap["internal"] = outdata; // with outdata an Event<std::any>
  DataPack mergedDP(eventmap); // empty but structure in place
  mergedDP.getTruthRef().vertex.eventID = prevID; // decision flag = -1
  mergedDP.getTruthRef().vertex.trackID = mergedID; // fix at 0 to signal merged wfm
  
  for (int j=0;j<reader.GetEntries();++j) {
    DataPack dp = readRow(); // data row in
    evtag = dp.getTruthRef().vertex.eventID; // decision number
    localSampling = dp.getTruthRef().sampling_time.numerical_value_in(ns); // required merge info
    //    std::cout << "in merger, entry read: " << evtag << ", local stime=" << localSampling << std::endl;

    if (prevID != evtag) { // new event ID read from file
      if (mergedDP.getTruthRef().vertex.eventID>0) { // there is a mergedDP waiting
	std::cout << "in merger, prev!=ev, mergedDP.evID>0 case." << std::endl;      
	writeRow(mergedDP);
	// Reset
	mergedDP.getTruthRef().vertex.eventID = -1; // decision flag reset
      }
      std::cout << "in merger, prev!=ev case." << std::endl;      
      // local storage, ready for next iteration
      prevID = evtag;
      trackHistory->clear();
      localWfm.clear(); // prepare for new waveform 
      prevTrackID = dp.getTruthRef().vertex.trackID;
      trackHistory->push_back(prevTrackID); // minimum single entry
      dp.getTruthRef().vertex.trackHistory = *trackHistory; // vector<int>, newly set

      for (int i=0;i<wfmarray.GetSize();++i) {
	vec_t wfm(wfmarray.At(i).begin(), wfmarray.At(i).end()); // wfmarray is ttreereaderarray
	localWfm.push_back(wfm); // local copy for potential merging
      }
      mergedDP.getTruthRef().average_omega = dp.getTruthRef().average_omega; // initial copy
      
      writeRow(dp); // write out as is, nothing else to do
    }
    else { // same event ID as previous read
      //      std::cout << "in merger, else prev==ev case." << std::endl;      
      dp.getTruthRef().vertex.trackHistory = *trackHistory; // vector<int>, newly set
      writeRow(dp); // write out as is, then merging using local data

      trackHistory->push_back(dp.getTruthRef().vertex.trackID); // the new one to be merged
      localStart = dp.getTruthRef().start_time.numerical_value_in(ns); // required merge info
      std::cout << "in merger prev==evid, local start time [ns] = " << localStart << std::endl;
      for (int i=0;i<wfmarray.GetSize();++i) {
	vec_t wfm(wfmarray.At(i).begin(), wfmarray.At(i).end()); // new wfm
	add(wfm, i); // add new wfm to previous using localStart and localWfm

	// assign values to DataPack to be written out separately.
	brname = "sampled_" + std::to_string(i) + "_V";
	outdata[brname] = std::make_any<vec_t>(localWfm.at(i)); // merged is in localWfm
      }
      // update data product, minimal info required for a merged Wfm
      mergedDP.getRef()["internal"] = outdata; // with outdata an Event<std::any>
      mergedDP.getTruthRef().vertex.eventID = evtag;
      mergedDP.getTruthRef().vertex.trackHistory = *trackHistory; // vector<int>
      mergedDP.getTruthRef().nantenna = dp.getTruthRef().nantenna; // copy
    }
  }
  std::cout << "Loop finished." << std::endl;
}


DataPack trackMerger::readRow()
{
  // protection against end of file must come from outside.
  Event_map<std::any> eventmap; // data item for delivery
  Event<std::any> outdata; // to hold all the data items from file
  
  // collect all trajectory info from file, reader holds event iterator
  // std::cout << "reader called" << std::endl;
  std::string brname;
  reader.Next();

  // test read
  // std::cout << "read Wfm 0: " << std::endl;
  // for (auto entry : wfmarray.At(0)) std::cout << entry << ", ";
  // std::cout << std::endl;
  
  for (int i=0;i<wfmarray.GetSize();++i) {
    brname = "sampled_" + std::to_string(i) + "_V";
    vec_t wfm(wfmarray.At(i).begin(), wfmarray.At(i).end());
    outdata[brname] = std::make_any<vec_t>(wfm);
  }
  eventmap["internal"] = outdata; // with outdata an Event<std::any>

  // make data product, full copy of in from file.
  // at the end, store new data product in dictionary event map.
  DataPack dp(eventmap);
  // fill truth struct with vertex info, restore units from branch names
  dp.getTruthRef().vertex.eventID = *eventID;
  dp.getTruthRef().vertex.trackID = *trackID;
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
  dp.getTruthRef().bfield = *bfield * T; // store input truth
  return dp;
}


void trackMerger::writeRow(DataPack& dp)
{
  // extract from datapack and assign to output branch variables with the correct address
  nant = dp.getTruthRef().nantenna;
  mytree->SetBranchAddress("truth_nantenna",&nant);
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
  trackHistory  = &dp.getTruthRef().vertex.trackHistory; // vector<int>*
  mytree->SetBranchAddress("vertex_trackHistory",&trackHistory);

  std::string brname;
  Event<std::any> indata = dp.getRef()["internal"];
  purewave->clear();
  for (int i=0;i<nant;++i) {
    // store
    brname = "sampled_" + std::to_string(i) + "_V"; // unit in name
    vec_t dummy = std::any_cast<vec_t>(indata[brname]); // construct first
    // std::cout << "in writerow, evid, trid: " << evID << ", " << trID << std::endl;
    // std::cout << "in writerow, write size: " << dummy.size() << std::endl;
    purewave->push_back(dummy);;
  }
  mytree->SetBranchAddress("sampled_V", &purewave);

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
  if (!hitevIDOut->empty()) {
    hitevIDOut->clear();
    hittrIDOut->clear();
    hitxOut->clear();
    hityOut->clear();
    hitzOut->clear();
    hittimeOut->clear();
    hitedepOut->clear();
    hitpostthetaOut->clear();
  }
}


void trackMerger::add(vec_t& other, int whichAntenna)
{
  // resize localWfm to fit the merger.
  int idx_start = (int)std::lround(localStart / localSampling); // find entry index for adding
  int final_idx = localWfm.at(whichAntenna).size() - 1;
  int end_other = other.size() - 1;
  int final_other = idx_start + end_other;
  int diff = final_other - final_idx;
  if (diff > 0) localWfm.at(whichAntenna).resize(final_idx+diff+1);
  // action
  std::transform(other.begin(), other.end(), localWfm.at(whichAntenna).begin()+idx_start,
		 localWfm.at(whichAntenna).begin()+idx_start, std::plus<double>()); // in-place addition
}
