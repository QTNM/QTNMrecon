// std
#include <iostream>
#include <vector>

// us
#include "Event.hh"
#include "types.hh"
#include <mp-units/ostream.h> // for cout stream
#include <mp-units/systems/hep/hep.h>

using namespace mp_units::hep::unit_symbols; // also HEP units

int main() {
    // level 2 storage of Event; takes template type argument
    Event<int> ev;
    ev["t1"] = 1;
    ev["t2"] = 2;
    ev["t3"] = 3;

    // output
    std::cout << "Event contains (range loop):" << std::endl;
    for (auto& x : ev) {
        std::cout << x.first << "; " << x.second << std::endl;
    }

    // Try an Event_map; here type for Event must be std::any wrapper
    // for general type storage in unordered_map as value. 
    // Key is enforced as std::string.
    Event_map<std::any> evmap;
    Event<std::any> ev2;
    std::any item{10};
    ev2["d1"] = item;
    ev2["d2"] = std::any(11);

    // wrap a container
    std::vector<double> vec{1.,2.,3.,4.};
    ev2["d3"] = std::make_any<std::vector<double>>(vec.begin(),vec.end()); // range construct
    ev2["d4"] = std::make_any<std::vector<double>>(std::move(vec)); // move construct

    // store all of the above in Event_map
    evmap["test"] = ev2;
    // output
    std::cout << "Event map contains (range loop):" << std::endl;
    for (auto& x : evmap) { // x is L1 unordered_map
        std::cout << "L1 inkey: " << x.first << std::endl;
        for (auto& y : x.second) { // y is L2 unordered_map<std::string, std::any>
            std::cout << "L2 key: " << y.first << std::endl;
        }
    }
    std::cout << "Event_map values:" << std::endl;

    for (auto& x : evmap) { // x is L1 unordered_map
        auto l2 = x.second;
        // for values need to know what is stored
        try // casting can go wrong; throws at run-time, catch it.
        {
            std::cout << std::any_cast<int>(l2["d1"]) << std::endl;
            std::cout << std::any_cast<int>(l2["d2"]) << std::endl;
            // can also cast the container
            auto vv1 = std::any_cast<std::vector<double>>(l2["d3"]);
            for (auto& y : vv1) std::cout << y << " ";
            std::cout << std::endl;
            auto vv2 = std::any_cast<std::vector<double>>(l2["d4"]);
            for (auto& y : vv2) std::cout << y << " ";
            std::cout << std::endl;
        }
        catch (const std::bad_any_cast& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    Event<std::any> evunits;
    quantity<eV> en = 511.0 * keV;
    quantity<s> ti = 1.1 * ns;
    evunits["kinen"] = std::make_any<quantity<isq::energy[eV]>>(en);
    evunits["time"]  = std::make_any<quantity<s>>(ti);
    try
    {
        auto energy = std::any_cast<quantity<isq::energy[eV]>>(evunits["kinen"]);
        std::cout << energy << ", converted: " << energy.in(J) << std::endl;
        auto tstamp = std::any_cast<quantity<s>>(evunits["time"]);
        std::cout << tstamp << ", converted: " << tstamp.in(ns) << std::endl;
    }
    catch(const std::bad_any_cast& e)
    {
        std::cerr << e.what() << '\n';
    }

    // try a DataPack
    DataPack dp(evmap);
    std::cout << "Event map from DataPack" << std::endl;
    auto& evdp = dp.getRef()["test"]; // evdp is L1 unordered_map
    // for values need to know what is stored
    try // casting can go wrong; throws at run-time, catch it.
    {
        std::cout << std::any_cast<int>(evdp["d1"]) << std::endl;
        std::cout << std::any_cast<int>(evdp["d2"]) << std::endl;
        // can also cast the container
        auto vv1 = std::any_cast<std::vector<double>>(evdp["d3"]);
        for (auto& y : vv1) std::cout << y << " ";
        std::cout << std::endl;
        auto vv2 = std::any_cast<std::vector<double>>(evdp["d4"]);
        for (auto& y : vv2) std::cout << y << " ";
        std::cout << std::endl;
    }
    catch (const std::bad_any_cast& e)
    {
        std::cout << e.what() << std::endl;
    }
    // test other data members in data pack
    // assign values
    dp.getTruthRef().vertex.eventID = 21;
    dp.getTruthRef().vertex.trackID = 1;
    dp.getTruthRef().nantenna = 2;
    dp.getTruthRef().sampling_time = ti;
    dp.getTruthRef().vertex.kineticenergy = en;
    std::cout << "evID: " << dp.getTruthRef().vertex.eventID << std::endl;
    std::cout << "quantity stime: " << dp.getTruthRef().sampling_time << std::endl;
    std::cout << "quantity energy: " << dp.getTruthRef().vertex.kineticenergy << std::endl;
    return 0;
}


