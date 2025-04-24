// QTNMSim source antenna reader module implementation

// std
#include <iostream>

// us
#include "FullAntennaSimReader.hh"
#include "yap/pipeline.h"

// ROOT
#include "TTreeReaderValue.h"

FullAntennaSimReader::FullAntennaSimReader(TTreeReader& re1, TTreeReader& re2, std::string out) : 
    outkey(std::move(out)),
    maxEventNumber(-1), // default -1 for a all events
    evcounter(0),
    nantenna(1),
    Bfield(-1.0 * T),
    reader1(re1),
    reader2(re2),
    eventID(reader1, "EventID"), // needs reader by reference
    trackID(reader1, "TrackID"),
    hitevID(reader2, "EventID"), // interaction data
    hittrID(reader2, "TrackID"),
    edep(reader2, "Edep"), 
    tstamp(reader2, "TimeStamp"),
    prek(reader2, "PreKine"),
    postk(reader2, "PostKine"),
    preth(reader2, "PreTheta"),
    postth(reader2, "PostTheta"),
    locx(reader2, "Posx"), // interaction location
    locy(reader2, "Posy"),
    locz(reader2, "Posz"),
    posx(reader1, "Posx"), // vertex data
    posy(reader1, "Posy"),
    posz(reader1, "Posz"),
    kine(reader1, "KinEnergy"),
    pangle(reader1, "PitchAngle"),
    aID(reader1, "AntennaID"),
    omvec(reader1, "OmVec"),
    kevec(reader1, "KEVec"),
    stvec(reader1, "SourceTime"),
    tvec(reader1, "TimeVec"),
    vvec(reader1, "VoltageVec")
{
    std::cout << "in reader n entries: " << reader1.GetEntries() << std::endl;
}

DataPack FullAntennaSimReader::operator()()
{
    // catch event number limit in pipeline, -1 = all, default
    if ((evcounter > maxEventNumber &&   // reached maximum event number
        maxEventNumber > 0) ||          // max has been set
        evcounter >= reader1.GetEntries()) // reached end of file
    
        throw yap::GeneratorExit{};
    evcounter++;

    if (Bfield < 0 * T) {
        std::cout << "WARNING: Bfield is required input to pipeline. Exit" << std::endl;
        throw yap::GeneratorExit{};
    }

    Event_map<std::any> eventmap; // data item for delivery
    Event<std::any> outdata; // to hold all the data items from file

    // collect all trajectory info from file, reader holds event iterator
    // std::cout << "reader called" << std::endl;
    if (reader1.Next()) { // variables filled from file
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
    std::cout << "reader1 Next() done, evt:  " << evcounter << std::endl;

    // check on hits, separately from trajectory reader
    // the hit reader may or may not hold data.
    if (reader2.GetEntries() > 0) { // if there is any hit at all, check with trajectory
        while (reader2.Next()) { // get first entry, fill all data items
            if (*hitevID == *eventID) { // only if this trajectory has a hit
                dp.getHitRef().eventID   = *hitevID;
                dp.getHitRef().trackID   = *hittrID;
                dp.getHitRef().edeposit  = *edep * keV;
                dp.getHitRef().timestamp = *tstamp * ns;
                dp.getHitRef().kepre = *prek * keV;
                dp.getHitRef().kepost = *postk * keV;
                dp.getHitRef().anglepre = *preth * rad;
                dp.getHitRef().anglepost = *postth * rad;
                dp.getHitRef().locx = *locx * mm;
                dp.getHitRef().locy = *locy * mm;
                dp.getHitRef().locz = *locz * mm;
                // store the filled hit_t
                dp.hitsRef().push_back(dp.getHit());
                std::cout << "found hit evt/track:  " << *hitevID << ", " << *hittrID << std::endl;
            }
        }
        reader2.Restart(); // for each trajectory, have to loop over hits, then reset hits reader.
    }
    if (!stvec->empty()) // book truth from trajectory
      dp.getTruthRef().start_time = stvec->front() * ns;
    else
      dp.getTruthRef().start_time = -1.0 * ns;
    dp.getTruthRef().nantenna = nantenna; // store input truth
    dp.getTruthRef().bfield = Bfield; // store input truth
    return dp;
}
