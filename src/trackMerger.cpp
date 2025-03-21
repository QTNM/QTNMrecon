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


DataPack trackMerger::read()
{
}


vec_t trackMerger::add(vec_t& signal, double mtime, vec_t& other)
{
}
