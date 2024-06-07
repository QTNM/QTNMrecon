// QTNMSim Antenna signal reader module implementation

// std
#include <iostream>

// us
#include "QTNMSimAntennaReader.hh"
#include "yap/pipeline.h"


QTNMSimAntennaReader::QTNMSimAntennaReader(TTreeReader& re, std::string out) : 
    outkey(std::move(out)),
    maxEventNumber(-1), // default -1 for a all events
    evcounter(0),
    reader(re),
    eventID(reader, "EventID"), // needs reader by reference
    trackID(reader, "TrackID"),
    posx(reader, "Posx"), // vertex data
    posy(reader, "Posy"),
    posz(reader, "Posz"),
    kine(reader, "KinEnergy"),
    pangle(reader, "PitchAngle"),
    aID(reader, "AntennaID"),
    tvec(reader, "TimeVec"),
    vvec(reader, "VoltageVec")
{
    std::cout << "in reader n entries: " << reader.GetEntries() << std::endl;
}


DataPack QTNMSimAntennaReader::operator()()
{
    // catch event number limit in pipeline, -1 = all, default
  if ((evcounter > maxEventNumber &&   // reached maximum event number
       maxEventNumber > 0) ||          // max has been set
      evcounter >= reader.GetEntries()) // reached end of file
    
    throw yap::GeneratorExit{};
  evcounter++;

  Event_map<std::any> eventmap; // data item for delivery
    Event<std::any> outdata; // to hold all the data items from file

    // collect all Signal info from file, reader holds event iterator
    std::cout << "reader called" << std::endl;
    if (reader.Next()) { // variables filled from file
        outdata["eventID"] = std::any(*eventID); // de-reference an int to std::any
        outdata["trackID"] = std::any(*trackID);
        outdata["VPosx"] = std::any(*posx); // vertex position
        outdata["VPosy"] = std::any(*posy); 
        outdata["VPosz"] = std::any(*posz); 
        outdata["VKinEnergy"] = std::any(*kine); // vertex energy
        outdata["VPitchAngle"] = std::any(*pangle); // vertex pitch angle to z-axis
        outdata["AntennaID"] = std::make_any<std::vector<int>>(aID->begin(),aID->end());
        outdata["TimeVec"] = std::make_any<std::vector<double>>(tvec->begin(),tvec->end());
        outdata["VoltageVec"] = std::make_any<std::vector<double>>(vvec->begin(),vvec->end());
    }
    else // no more entries in TTreeReader
        throw yap::GeneratorExit{};

    std::cout << "reader Next() done, evt:  " << evcounter << std::endl;
    // at the end, store new data product in dictionary event map.
    eventmap[outkey] = outdata; // with outdata an Event<std::any>
    DataPack dp(eventmap);
    return dp;
}
