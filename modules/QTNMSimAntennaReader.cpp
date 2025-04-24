// QTNM Antenna sim output reader
// must set the constant B-field from sim as meta-data.

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
    nantenna(1),
    Bfield(-1.0 * T),
    reader(re),
    eventID(reader, "EventID"), // needs reader by reference
    trackID(reader, "TrackID"),
    posx(reader, "Posx"), // vertex data
    posy(reader, "Posy"),
    posz(reader, "Posz"),
    kine(reader, "KinEnergy"),
    pangle(reader, "PitchAngle"),
    aID(reader, "AntennaID"),
    kevec(reader, "KEVec"),
    omvec(reader, "OmVec"),
    tvec(reader, "TimeVec"),
    stvec(reader, "SourceTime"),
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
    if (Bfield < 0 * T) {
        std::cout << "WARNING: Bfield is required input to pipeline. Exit" << std::endl;
        throw yap::GeneratorExit{};
    }

  Event_map<std::any> eventmap; // data item for delivery
    Event<std::any> outdata; // to hold all the data items from file

    // collect all Signal info from file, reader holds event iterator
    std::cout << "reader called" << std::endl;
    if (reader.Next()) { // variables filled from file
        outdata["AntennaID"] = std::make_any<std::vector<int>>(aID->begin(),aID->end());
        outdata["TimeVec"] = std::make_any<std::vector<double>>(tvec->begin(),tvec->end());
        outdata["VoltageVec"] = std::make_any<std::vector<double>>(vvec->begin(),vvec->end());
        outdata["SourceTime"] = std::make_any<std::vector<double>>(stvec->begin(),stvec->end());
        outdata["OmVec"] = std::make_any<std::vector<double>>(omvec->begin(),omvec->end());
        outdata["KEVec"] = std::make_any<std::vector<double>>(kevec->begin(),kevec->end());
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
    //    std::cout << "content check, antenna id size:  " << aID->size() << std::endl;
    // if(dp.getTruthRef().vertex.eventID==1) {
    //   std::cout << "event 1 check:" << std::endl;
    //   for (int j=0;j<vvec->size();++j)
    // 	std::cout << vvec->at(j) << ", ";
    //   std::cout << std::endl;
    // }

    if (!stvec->empty()) // book truth from trajectory
      dp.getTruthRef().start_time = stvec->front() * ns;
    else
      dp.getTruthRef().start_time = -1.0 * ns;
    dp.getTruthRef().nantenna = nantenna; // store input truth
    dp.getTruthRef().bfield = Bfield; // store input truth
    return dp;
}
