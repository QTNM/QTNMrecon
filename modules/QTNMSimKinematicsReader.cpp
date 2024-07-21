// QTNMSim source kinematics reader module implementation

// std
#include <iostream>

// us
#include "QTNMSimKinematicsReader.hh"
#include "yap/pipeline.h"


QTNMSimKinematicsReader::QTNMSimKinematicsReader(TTreeReader& re, std::string out) : 
    outkey(std::move(out)),
    maxEventNumber(-1), // default -1 for a all events
    evcounter(0),
    reader(re),
    eventID(reader, "EventID"),
    trackID(reader, "TrackID"),
    posx(reader, "Posx"),
    posy(reader, "Posy"),
    posz(reader, "Posz"),
    kine(reader, "KinEnergy"),
    pangle(reader, "PitchAngle"),
    tvec(reader, "TimeVec"),
    omvec(reader, "OmVec"),
    kevec(reader, "KEVec"),
    pxvec(reader, "PosxVec"),
    pyvec(reader, "PosyVec"),
    pzvec(reader, "PoszVec"),
    bxvec(reader, "BetaxVec"),
    byvec(reader, "BetayVec"),
    bzvec(reader, "BetazVec"),
    axvec(reader, "AccxVec"),
    ayvec(reader, "AccyVec"),
    azvec(reader, "AcczVec")
{
    std::cout << "in reader n entries: " << reader.GetEntries() << std::endl;
}


DataPack QTNMSimKinematicsReader::operator()()
{
    // catch event number limit in pipeline, -1 = all, default
    if (evcounter >= maxEventNumber && 
        maxEventNumber > 0 ||             // max has been set
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
    
    if (reader.Next()) { // variables filled from file
        outdata["TimeVec"] = std::make_any<std::vector<double>>(tvec->begin(),tvec->end());
        outdata["OmVec"] = std::make_any<std::vector<double>>(omvec->begin(),omvec->end());
        outdata["KEVec"] = std::make_any<std::vector<double>>(kevec->begin(),kevec->end());
        outdata["pxVec"] = std::make_any<std::vector<double>>(pxvec->begin(),pxvec->end());
        outdata["pyVec"] = std::make_any<std::vector<double>>(pyvec->begin(),pyvec->end());
        outdata["pzVec"] = std::make_any<std::vector<double>>(pzvec->begin(),pzvec->end());
        outdata["bxVec"] = std::make_any<std::vector<double>>(bxvec->begin(),bxvec->end());
        outdata["byVec"] = std::make_any<std::vector<double>>(byvec->begin(),byvec->end());
        outdata["bzVec"] = std::make_any<std::vector<double>>(bzvec->begin(),bzvec->end());
        outdata["axVec"] = std::make_any<std::vector<double>>(axvec->begin(),axvec->end());
        outdata["ayVec"] = std::make_any<std::vector<double>>(ayvec->begin(),ayvec->end());
        outdata["azVec"] = std::make_any<std::vector<double>>(azvec->begin(),azvec->end());
        eventmap[outkey] = outdata; // with outdata an Event<std::any>
    }
    else // no more entries in TTreeReader
        throw yap::GeneratorExit{};

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

    dp.getTruthRef().bfield = Bfield; // store input truth
    return dp;
}
