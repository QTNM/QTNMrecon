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

template <typename T> using Event = std::unordered_map<std::string, T>;
template <typename T> using Event_map = std::unordered_map<std::string, Event<T>>;

class DataPack
{
    Event_map<std::any> mymap;

public:
    DataPack(Event_map<std::any> emap) noexcept : mymap(std::move(emap))
    {}

    DataPack(const DataPack&) = delete;
    DataPack& operator=(const DataPack&) = delete;

    DataPack(DataPack &&) = default;

  inline Event_map<std::any>& getRef() {return mymap;}
};

#endif
