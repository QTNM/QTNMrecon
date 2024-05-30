// template module implementation
// necessarily empty since nothing is supposed to happen
// for this example, showing boiler-plate code for any 
// processing module.

// std
#include <iostream>

// us
#include "QTNMSimAntennaReader.hh"
#include "yap/pipeline.h"

// ROOT
#include "TTreeReaderValue.h"

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
    kvec(reader, "KEVec"),
    tvec(reader, "TimeVec"),
    vvec(reader, "VoltageVec")
{
    std::cout << "in reader n entries: " << reader.GetEntries() << std::endl;
}

Event_map<std::any> QTNMSimAntennaReader::operator()()
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
    
    if (reader.Next()) {; // variables filled from file
        outdata["eventID"] = std::any(*eventID); // de-reference an int to std::any
        outdata["trackID"] = std::any(*trackID);
        outdata["VPosx"] = std::any(*posx); // vertex position
        outdata["VPosy"] = std::any(*posy); 
        outdata["VPosz"] = std::any(*posz); 
        outdata["VKinEnergy"] = std::any(*kine); // vertex energy
        outdata["VPitchAngle"] = std::any(*pangle); // vertex pitch angle to z-axis
        outdata["AntennaID"] = std::make_any<std::vector<int>>(aID->begin(),aID->end());
        outdata["KEVec"] = std::make_any<std::vector<double>>(kvec->begin(),kvec->end());
        outdata["TimeVec"] = std::make_any<std::vector<double>>(tvec->begin(),tvec->end());
        outdata["VoltageVec"] = std::make_any<std::vector<double>>(vvec->begin(),vvec->end());
    }
    else // no more entries in TTreeReader
        throw yap::GeneratorExit{};

    // at the end, store new data product in dictionary event map.
    eventmap[outkey] = outdata; // with outdata an Event<std::any>
    return eventmap;
}
