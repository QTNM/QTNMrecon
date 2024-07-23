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
  quantity<Hz> average_omega; // from antenna
  quantity<Hz> beat_frequency;
  quantity<Hz/s> chirp_rate;
  std::vector<waveform_t> pure; // sampled signal, no noise, one per antenna
  
  struct vertex_t {
    int eventID;
    int trackID;
    quantity<m> posx, posy,posz; // turn no unit numbers from file
    quantity<eV> kineticenergy;  // into quantities with unit.
    quantity<deg> pitchangle;
  } vertex;
};

struct experiment_t {
  double gain; // set by digitizer or earlier
  quantity<Hz> target_frequency; // from mixer
  quantity<Hz> digi_sampling_rate; // set-up with digitizer
  std::vector<waveform_t> signals; // digitized endproduct
};

// pipeline data structure
class DataPack
{
  private:
    Event_map<std::any> mymap; // defined at construction
    truth_t truthPack; // undefined at construction; bag with structure
    experiment_t expPack; // undefined at construction; fill structure

  public:
    DataPack(Event_map<std::any> emap) noexcept : mymap(std::move(emap))
    {}

    DataPack(const DataPack&) = delete; // no copy constructor
    DataPack& operator=(const DataPack&) = delete; // no copy assignment

    DataPack(DataPack &&) = default; // move constructor

    inline Event_map<std::any>& getRef() {return mymap;} // access
    inline truth_t& getTruthRef() {return truthPack;} // access
    inline experiment_t& getExperimentRef() {return expPack;} // access

};

#endif
