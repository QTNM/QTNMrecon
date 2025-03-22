// ------------------------------------
// trackMerger methods implementation

// std
#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>

// us
#include "trackMerger.hh"

trackMerger::trackMerger(TTreeReader& re, int na) : 
  nant(na),
  prevID(-1),
  reader(re)
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
}


DataPack trackMerger::readRow()
{
  // protection against and of file must come from outside.
  Event_map<std::any> eventmap; // data item for delivery
  Event<std::any> outdata; // to hold all the data items from file
  
  // collect all trajectory info from file, reader holds event iterator
  // std::cout << "reader called" << std::endl;
  std::string brname;
  reader.Next()
    for (int i=0;i<nant;++i) {
      brname = "sampled_" + std::to_string(i) + "_V";
      vec_t wfm(wfmarray.at(i).begin(), wfmarray.at(i).end());
      outdata[brname] = std::make_any<vec_t>(wfm);
      localWfm.push_back(wfm); // local copy for potential merging
    }
  eventmap[outkey] = outdata; // with outdata an Event<std::any>

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


vec_t trackMerger::add(vec_t& signal, double mtime, vec_t& other)
{
}
