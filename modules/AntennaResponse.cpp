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
    dp.getRef()[inkey].erase("pxVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("pyVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("pzVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("bxVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("byVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("bzVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("axVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("ayVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("azVec"); // copied hence remove from source
    dp.getRef()[inkey].erase("OmVec"); // copied hence remove from source

    dp.getRef()[outkey] = outdata;

    return dp;
}
