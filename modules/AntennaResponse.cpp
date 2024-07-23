// Antenna Response calculation module implementation

// std
#include <iostream>
#include <stdexcept>

// us
#include "AntennaResponse.hh"


AntennaResponse::AntennaResponse(std::string in, std::string out) : 
    inkey(std::move(in)),
    outkey(std::move(out))
{}


DataPack AntennaResponse::operator()(DataPack dp)
{
    if (! dp.getRef().count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    Event<std::any> indata = dp.getRef()[inkey];
    Event<std::any> outdata; // to hold all the data items
    int counter = 0;
    for (auto* antenna : receiver) {
        std::string tkey = "VoltageVec_" + std::to_string(counter) + "_[V]";
	    std::cout << "in antenna resp, made key: " << tkey << std::endl;
        outdata[tkey] = std::make_any<vec_t>(antenna->voltage_response(indata));
        counter++;
    }
    // clear obsolete data
    dp.getRef()[inkey].erase("pxVec"); // used hence remove from source
    dp.getRef()[inkey].erase("pyVec"); // used hence remove from source
    dp.getRef()[inkey].erase("pzVec"); // used hence remove from source
    dp.getRef()[inkey].erase("bxVec"); // used hence remove from source
    dp.getRef()[inkey].erase("byVec"); // used hence remove from source
    dp.getRef()[inkey].erase("bzVec"); // used hence remove from source
    dp.getRef()[inkey].erase("axVec"); // used hence remove from source
    dp.getRef()[inkey].erase("ayVec"); // used hence remove from source
    dp.getRef()[inkey].erase("azVec"); // used hence remove from source
    dp.getRef()[inkey].erase("KEVec"); // used hence remove from source

    dp.getRef()[outkey] = outdata;
    dp.getTruthRef().average_omega = std::any_cast<quantity<Hz>>(indata["avomega_[Hz]"]); // stored from antenna
    dp.getTruthRef().nantenna = receiver.size(); // overwrite for kinematic input stream
    return dp;
}
