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
    tvec(reader, "TimeVec"),
    vvec(reader, "VoltageVec")
{
    std::cout << "in reader n entries: " << reader.GetEntries() << std::endl;
}

//Event_map<std::any> QTNMSimAntennaReader::operator()()
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
        outdata["AntennaID"] = std::make_any<std::vector<int>>(aID->begin(),aID->end());
        outdata["TimeVec"] = std::make_any<std::vector<double>>(tvec->begin(),tvec->end());
        outdata["VoltageVec"] = std::make_any<std::vector<double>>(vvec->begin(),vvec->end());
        eventmap[outkey] = outdata; // with outdata an Event<std::any>
    }
    else // no more entries in TTreeReader
        throw yap::GeneratorExit{};

    // make data product
    // at the end, store new data product in dictionary event map.
    DataPack dp(eventmap);
    // fill truth struct with vertex info
    dp.getTruthRef().vertex.eventID = *eventID;
    dp.getTruthRef().vertex.trackID = *trackID;
    dp.getTruthRef().vertex.posx = *posx * mm;
    dp.getTruthRef().vertex.posy = *posy * mm;
    dp.getTruthRef().vertex.posz = *posz * mm;
    dp.getTruthRef().vertex.kineticenergy = *kine * keV;
    dp.getTruthRef().vertex.pitchangle = *pangle * rad;
    std::cout << "reader Next() done, evt:  " << evcounter << std::endl;
    return dp;
}
