// print module implementation

// std
#include <iostream>
#include <vector>

// us
#include "printSimReader.hh"
#include <mp-units/ostream.h> // for cout stream

printSimReader::printSimReader(std::string in) : 
    inkey(std::move(in)) {}

//void printSimReader::operator()(Event_map<std::any> emap)
void printSimReader::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
  if (! dp.getRef().count(inkey)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return; // not found, return unchanged map, no processing
    }
  //    Event_map<std::any> mymap = std::move(emap); // move from buffer copy
  Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey label.

    std::cout << "Event_map keys:" << std::endl;
    for (auto& entry : indata) std::cout << entry.first << std::endl;

    std::cout << "Event_map values:" << std::endl;
    // for values need to know what is stored
    try // casting can go wrong; throws at run-time, catch it.
    {
        std::cout << "evID " << dp.getTruthRef().vertex.eventID << std::endl;
        std::cout << "trID " << dp.getTruthRef().vertex.trackID << std::endl;
        std::cout << "posx " << dp.getTruthRef().vertex.posx << std::endl;
        std::cout << "posy " << dp.getTruthRef().vertex.posy << std::endl;
        std::cout << "posz " << dp.getTruthRef().vertex.posz << std::endl;
        std::cout << "KE " << dp.getTruthRef().vertex.kineticenergy << std::endl;
        std::cout << "Angle " << dp.getTruthRef().vertex.pitchangle << std::endl;
        std::cout << "Chirp rate " << dp.getTruthRef().chirp_rate << std::endl;

        // can also cast the container
        auto aID = std::any_cast<std::vector<int>>(indata["AntennaID"]);
        std::cout << "AntennaID size = " << aID.size() << std::endl;
        auto tiv = std::any_cast<std::vector<double>>(indata["TimeVec"]);
        std::cout << "TimeVec size = " << tiv.size() << std::endl;
        auto vvv = std::any_cast<std::vector<double>>(indata["VoltageVec"]);
        std::cout << "VoltageVec size = " << vvv.size() << std::endl;
    }
    catch (const std::bad_any_cast& e)
    {
      std::cout << "printSimReader: " << e.what() << std::endl;
    }

    // action of module template module.
    
    return;
}
