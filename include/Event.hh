// Event data model header.
// Specialize the general std template unordered_map
// by requiring a const std::string as key while leaving the
// value argument as free template.
// Setting up a two-level dictionary with
// level 2 as Event template storing a std::any
// template and the Event_map storing L2
// events at level 1 in an unordered_map.
// The Event_map then holds general data items
// at L2, each in a category labelled by string
// at L1. It can be handed over as single data bag
// between pipeline modules.

#ifndef tryEvent_HH
#define tryEvent_HH 1

#include <unordered_map>
#include <vector>
#include <string>
#include <any>
// us
#include "types.hh"

template <typename T> using Event = std::unordered_map<std::string, T>;
template <typename T> using Event_map = std::unordered_map<std::string, Event<T>>;

// ---------------------
// Data model structures
// consider as final data output to define writer modules
// intermediate outputs could also have EDM structs defined.
// Otherwise hard-wire std:::any types for modules
// considered to run in a pipeline without separate
// output to file or prints other than for tests.
struct truth_t {
  int nantenna; // from sampling or antenna response
  double snratio; // from adding noise
  quantity<T> bfield;
  quantity<ns> sampling_time; // from sampling
  quantity<ns> start_time;    // from time vector for each trackID
  quantity<Hz> average_omega; // from antenna
  quantity<Hz> beat_frequency; // from omega vector
  quantity<Hz/s> chirp_rate;
  std::vector<vec_t> pure; // sampled signal, no noise, one per antenna
  
  struct vertex_t {
    int eventID;
    int trackID;
    std::vector<int> trackHistory;
    quantity<m> posx, posy,posz; // turn no unit numbers from file
    quantity<eV> kineticenergy;  // into quantities with unit.
    quantity<deg> pitchangle;
  } vertex;
};

struct hit_t {
  int eventID;
  int trackID;
  quantity<m> locx, locy,locz; // turn no unit numbers from file
  quantity<ns> timestamp;
  quantity<eV> edeposit;
  quantity<eV> kepre;  // into quantities with unit.
  quantity<eV> kepost;  // into quantities with unit.
  quantity<deg> anglepre, anglepost;
};

struct experiment_t {
  double gain; // set by digitizer or earlier
  quantity<Hz> target_frequency; // from mixer
  quantity<Hz> digi_sampling_rate; // set-up with digitizer
  std::vector<vec_t> signals; // digitized endproduct
};

// pipeline data structure
class DataPack
{
private:
  Event_map<std::any> mymap; // defined at construction
  truth_t truthPack; // undefined at construction; bag with structure
  experiment_t expPack; // undefined at construction; fill structure
  hit_t hitPack; // undefined at construction; fill structure
  std::vector<hit_t> hits; // for multiple hits per event
  
public:
  DataPack(Event_map<std::any> emap) noexcept : mymap(std::move(emap))
  {
    init();
  }
  
  DataPack(const DataPack&) = delete; // no copy constructor
  DataPack& operator=(const DataPack&) = delete; // no copy assignment
  
  DataPack(DataPack &&) = default; // move constructor
  
  inline Event_map<std::any>& getRef() {return mymap;} // access
  inline truth_t& getTruthRef() {return truthPack;} // access
  inline experiment_t& getExperimentRef() {return expPack;} // access
  inline hit_t& getHitRef() {return hitPack;} // access
  inline hit_t getHit() {return hitPack;} // access
  inline std::vector<hit_t>& hitsRef() {return hits;} // access

private:
  inline void init() { // define quantity<> at construction
    truthPack.bfield = 0.0 * T;
    truthPack.sampling_time = 0.0 * ns;
    truthPack.start_time = 0.0 * ns;
    truthPack.average_omega = 0.0 * Hz;
    truthPack.beat_frequency = 0.0 * Hz;
    truthPack.chirp_rate = 0.0 * Hz/s;
    truthPack.vertex.posx = 0.0 * m;
    truthPack.vertex.posy = 0.0 * m;
    truthPack.vertex.posz = 0.0 * m;
    truthPack.vertex.kineticenergy = 0.0 * eV;
    truthPack.vertex.pitchangle = 0.0 * deg;
    hitPack.locx = 0.0 * m;
    hitPack.locy = 0.0 * m;
    hitPack.locz = 0.0 * m;
    hitPack.timestamp = 0.0 * ns;
    hitPack.edeposit = 0.0 * eV;
    hitPack.kepre = 0.0 * eV;
    hitPack.kepost = 0.0 * eV;
    hitPack.anglepre = 0.0 * deg;
    expPack.target_frequency = 0.0 * Hz;
    expPack.digi_sampling_rate = 0.0 * Hz;
  }
};

#endif
