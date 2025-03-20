// HDF5 Digitizer Writer module implementation
// writing a single value; expand for specific outputs.

// std
#include <iostream>
#include <string>

// us
#include "writeHitDigiToHDF5.hh"


WriterHitDigiToHDF5::WriterHitDigiToHDF5(HighFive::Group& gr) : 
  simgroup(gr)
{
  bnew_sim   = true; // only true once
  bnew_event = true;
  evID = -1;
  trID = -1;
  std::cout << "in HDF5 writer constructor." << std::endl;
}


void WriterHitDigiToHDF5::operator()(DataPack dp)
{
  // extract from datapack and assign to output groups and datasets
  // all ID numbers, check on previous event ID
  if (dp.getTruthRef().vertex.eventID == evID) bnew_event = false;
  else {
    evID = dp.getTruthRef().vertex.eventID;
    bnew_event = true;
  }
  std::cout << "in HDF5 writer: new event boolean " << bnew_event << " evID=" << evID << std::endl;

  trID = dp.getTruthRef().vertex.trackID; // always a new track ID
  nant = dp.getTruthRef().nantenna;

  // global attributes
  if (bnew_sim) {
    simgroup.createAttribute("truth_bfield_T", dp.getTruthRef().bfield.numerical_value_in(T));
    simgroup.createAttribute("truth_nantenna", nant);
    simgroup.createAttribute("truth_snratio", dp.getTruthRef().snratio);
    simgroup.createAttribute("truth_samplingtime_s", dp.getTruthRef().sampling_time.numerical_value_in(s));
    simgroup.createAttribute("digi_gain", dp.getExperimentRef().gain);
    simgroup.createAttribute("digi_tfrequency_Hz", dp.getExperimentRef().target_frequency.numerical_value_in(Hz));
    simgroup.createAttribute("digi_samplingrate_Hz", dp.getExperimentRef().digi_sampling_rate.numerical_value_in(Hz));
  }
  
  if (bnew_event)
    eventgr = simgroup.createGroup("event_"+std::to_string(evID));
  std::cout << "in HDF5 writer: made group with evID " << evID << std::endl;

  // enable compression
  HighFive::DataSetCreateProps props;
  props.add(HighFive::Chunking(std::vector<hsize_t>{1000})); // adjust size for IO speed
  props.add(HighFive::Deflate(6));

  HighFive::Group recordgr  = eventgr.createGroup("record_"+std::to_string(trID));
  for (int antid=0;antid<nant;++antid) {
    HighFive::Group channelgr  = recordgr.createGroup("channel_"+std::to_string(antid));
    channelgr.createDataSet("signal_V", dp.getExperimentRef().signals.at(antid), props); // compressed
    channelgr.createDataSet("truthwfm_V", dp.getTruthRef().pure.at(antid), props); // compressed
  }
  // electron-specific attributes
  recordgr.createAttribute("vertex_posx_m", dp.getTruthRef().vertex.posx.numerical_value_in(m));
  recordgr.createAttribute("vertex_posy_m", dp.getTruthRef().vertex.posy.numerical_value_in(m));
  recordgr.createAttribute("vertex_posz_m", dp.getTruthRef().vertex.posz.numerical_value_in(m));
  recordgr.createAttribute("vertex_kinenergy_eV", dp.getTruthRef().vertex.kineticenergy.numerical_value_in(eV));
  recordgr.createAttribute("vertex_pitchangle_deg", dp.getTruthRef().vertex.pitchangle.numerical_value_in(deg));
  recordgr.createAttribute("truth_avomega_Hz", dp.getTruthRef().average_omega.numerical_value_in(Hz));
  recordgr.createAttribute("truth_beatf_Hz", dp.getTruthRef().beat_frequency.numerical_value_in(Hz));
  recordgr.createAttribute("truth_chirp_Hz_s", dp.getTruthRef().chirp_rate.numerical_value_in(Hz/s));

  if (! dp.hitsRef().empty()) { // there are hits to store
    // extract hit data
    for (hit_t hit : dp.hitsRef()) { // get hit struct from vector<hit_t>
      hittrID.push_back(hit.trackID);
      hitx.push_back(hit.locx.numerical_value_in(m)); // no unit in root file
      hity.push_back(hit.locy.numerical_value_in(m));
      hitz.push_back(hit.locz.numerical_value_in(m));
      hitedep.push_back(hit.edeposit.numerical_value_in(eV));
      hittime.push_back(hit.timestamp.numerical_value_in(ns));
      hitposttheta.push_back(hit.anglepost.numerical_value_in(deg));
    }
    HighFive::Group hitgr     = eventgr.createGroup("hitdata_"+std::to_string(trID));
    hitgr.createDataSet("hit_trackID",hittrID); // vector<int>
    hitgr.createDataSet("hit_locx_m",hitx); // vector<double>
    hitgr.createDataSet("hit_locy_m",hity); // vector<double>
    hitgr.createDataSet("hit_locz_m",hitz); // vector<double>
    hitgr.createDataSet("hit_time_ns",hittime); // vector<double>
    hitgr.createDataSet("hit_edep_eV",hitedep); // vector<double>
    hitgr.createDataSet("hit_posttheta_deg",hitposttheta); // vector<double>

    // clear internal
    hittrID.clear();
    hitx.clear();
    hity.clear();
    hitz.clear();
    hittime.clear();
    hitedep.clear();
    hitposttheta.clear();
  }
  bnew_sim = false; // constant for entire rest of sim group
}
