// print module implementation

// std
#include <iostream>
#include <vector>

// us
#include "printSimReader.hh"

printSimReader::printSimReader(std::string in) : 
    inkey(std::move(in)) {}

void printSimReader::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
  if (! dp.getRef().count(inkey)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return; // not found, return unchanged map, no processing
    }
  Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey label.

    std::cout << "Event_map keys:" << std::endl;
    for (auto& entry : indata) std::cout << entry.first << std::endl;

    std::cout << "Event_map values:" << std::endl;
    // for values need to know what is stored
    try // casting can go wrong; throws at run-time, catch it.
    {
        std::cout << "evID " << std::any_cast<int>(indata["eventID"]) << std::endl;
        std::cout << "trID " << std::any_cast<int>(indata["trackID"]) << std::endl;
        std::cout << "posx " << std::any_cast<double>(indata["VPosx"]) << std::endl;
        std::cout << "posy " << std::any_cast<double>(indata["VPosy"]) << std::endl;
        std::cout << "posz " << std::any_cast<double>(indata["VPosz"]) << std::endl;
        std::cout << "KE " << std::any_cast<double>(indata["VKinEnergy"]) << std::endl;
        std::cout << "Angle " << std::any_cast<double>(indata["VPitchAngle"]) << std::endl;
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
        std::cout << e.what() << std::endl;
    }

    // action of module template module.
    
    return;
}
