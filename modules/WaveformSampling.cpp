// Waveform interpolator module implementation

// std
#include <iostream>
#include <stdexcept>

// ROOT
#include "ROOT/RVec.hxx"

// us
#include "WaveformSampling.hh"
#include "spline.h"


WaveformSampling::WaveformSampling(std::string source, std::string in, std::string out) : 
    originkey(std::move(source)),
    inkey(std::move(in)),
    outkey(std::move(out)),
    sampletime(1.0 * ns),
    localnantenna(1)
{}


DataPack WaveformSampling::operator()(DataPack dp)
{
  if (! dp.getRef().count(originkey)) { 
    throw std::logic_error("origin key not in dictionary!");
  }
  std::cout << "interpolator called" << std::endl;

  // set before: antenna read by add chirp/ kinematic by anntenna response
  int nantenna;
  if (dp.getTruthRef().nantenna>0) // set before, normally
    nantenna = dp.getTruthRef().nantenna;
  else
    nantenna = localnantenna; // test case set separately

  Event<std::any> origindata = dp.getRef()[originkey];
  Event<std::any> outdata; // to hold all the data items
  // get hold of relevant data, transform and store in outdata
  if (origindata.count("AntennaID")) { // sample Antenna waveform from G4 sim
    try
      {
	// cast the containers directly from reader module
	auto aID = std::any_cast<std::vector<int>>(origindata["AntennaID"]);
	auto tiv = std::any_cast<std::vector<double>>(origindata["TimeVec"]);
	auto vvv = std::any_cast<std::vector<double>>(origindata["VoltageVec"]);
	// also sample the omega vector for reconstruction
	auto omvec = std::any_cast<std::vector<double>>(origindata["OmVec"]);
	ROOT::VecOps::RVec<int> tempant(aID); // temporary
	ROOT::VecOps::RVec<double> tempt(tiv); // temporary
	ROOT::VecOps::RVec<double> tempv(vvv); // temporary
	
	auto selecttvec = tempt[tempant==0]; // like NumPy selection
	vec_t tt(selecttvec.begin(),selecttvec.end()); // sample_time same
	for (int i=0;i<nantenna;++i) {
	  auto selectvvec = tempv[tempant==i];
	  vec_t tv(selectvvec.begin(),selectvvec.end());
	  
	  vec_t resampled = interpolate(tt, tv);
	  // if (dp.getTruthRef().vertex.eventID==1) {
	  //   std::cout << "interpolator check tvals " << std::endl;
	  //   for (auto val : tt) std::cout << val << ", ";
	  //   std::cout << std::endl;
	  //   for (auto val : tv) std::cout << val << ", ";
	  //   std::cout << std::endl;
	  //   for (auto val : resampled) std::cout << val << ", ";
	  //   std::cout << std::endl;
	  // }
	  tv.clear();

	  std::string tkey = "sampled_" + std::to_string(i) + "_V";
	  outdata[tkey] = std::make_any<vec_t>(resampled);
	}
	if (!omvec.empty()) {
	  vec_t omresampled = interpolate(tt, omvec);
	  outdata["omega"] = std::make_any<vec_t>(omresampled); // for the beat freq
	}
	dp.getTruthRef().average_omega = average_omega(omvec);
      }
    catch(const std::bad_any_cast& e)
      {
	std::cerr << "WaveformSampling: " << e.what() << '\n';
      }
    dp.getTruthRef().sampling_time = sampletime;
    dp.getRef()[outkey] = outdata;
    
    // clear obsolete data in Event_map
    dp.getRef()[originkey].erase("AntennaID");
    dp.getRef()[originkey].erase("VoltageVec");
    dp.getRef()[originkey].erase("TimeVec");
    dp.getRef()[originkey].erase("OmVec");
  }
  else { // sample from antenna response calculator, has inkey
    if (! dp.getRef().count(inkey)) { 
      throw std::logic_error("input key not in dictionary!");
    }
    // this case needs the inkey
    Event<std::any> indata = dp.getRef()[inkey];
    try
      {
	// cast the containers from reader and inkey from processing
	auto tiv = std::any_cast<std::vector<double>>(origindata["TimeVec"]);
	// also sample the omega vector for reconstruction
	auto omvec = std::any_cast<std::vector<double>>(origindata["OmVec"]);

	for (int i=0;i<nantenna;++i) {
	  std::string ikey = "VoltageVec_" + std::to_string(i) + "_[V]";
	  auto tv = std::any_cast<vec_t>(indata[ikey]);
	  // sample by interpolation
	  vec_t resampled = interpolate(tiv, tv);
	  // store result
	  std::string okey = "sampled_" + std::to_string(i) + "_V";
	  outdata[okey] = std::make_any<vec_t>(resampled); // for later transformation and deletion
	  dp.getRef()[inkey].erase(ikey); // used; not needed anymore
	}
	if (!omvec.empty()) {
	  vec_t omresampled = interpolate(tiv, omvec);
	  outdata["omega"] = std::make_any<vec_t>(omresampled); // for the beat freq
	}
	dp.getTruthRef().average_omega = average_omega(omvec);
      }
    catch(const std::bad_any_cast& e)
      {
	std::cerr << e.what() << '\n';
      }

    dp.getTruthRef().sampling_time = sampletime;
    dp.getRef()[outkey] = outdata;
    
    // clear obsolete data in Event_map
    dp.getRef()[originkey].erase("TimeVec");
    dp.getRef()[originkey].erase("OmVec");
  }
  std::cout << "interpolator finish." << std::endl;
  
  return dp;
}


quantity<Hz> WaveformSampling::average_omega(const vec_t& omvec)
{
  if (!omvec.empty()) {
    double omsum = 0.0;
    for (auto entry : omvec) omsum += entry;
    quantity<Hz> res = omsum/omvec.size() * Hz;
    return res;
  }
  else return 0.0*Hz;
}


vec_t WaveformSampling::interpolate(const vec_t& tvals, const vec_t& vvals)
{
    vec_t resampled;
    double interval = tvals.back() - tvals.front(); // in [ns]
    double offset = tvals.front();
    int maxpoints = (int)floor(interval / sampletime.numerical_value_in(ns));

    spline ip(tvals, vvals); // interpolator
    for (int i=1;i<=maxpoints;++i)
        resampled.push_back(ip(offset + i * sampletime.numerical_value_in(ns)));
    return resampled;
}
