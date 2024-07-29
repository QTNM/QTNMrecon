// QTNMSim source kinematics reader module implementation

// std
#include <iostream>

// us
#include "FullKinematicsSimReader.hh"
#include "yap/pipeline.h"


FullKinematicsSimReader::FullKinematicsSimReader(TTreeReader& re1, TTreeReader& re2, std::string out) : 
    outkey(std::move(out)),
    maxEventNumber(-1), // default -1 for a all events
    evcounter(0),
    Bfield(-1.0 * T),
    reader1(re1),
    reader2(re2),
    eventID(reader1, "EventID"),
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
    tvec(reader1, "TimeVec"),
    omvec(reader1, "OmVec"),
    pxvec(reader1, "PosxVec"),
    pyvec(reader1, "PosyVec"),
    pzvec(reader1, "PoszVec"),
    bxvec(reader1, "BetaxVec"),
    byvec(reader1, "BetayVec"),
    bzvec(reader1, "BetazVec"),
    axvec(reader1, "AccxVec"),
    ayvec(reader1, "AccyVec"),
    azvec(reader1, "AcczVec")
{
    std::cout << "in reader n entries: " << reader1.GetEntries() << std::endl;
}


DataPack FullKinematicsSimReader::operator()()
{
    // catch event number limit in pipeline, -1 = all, default
    if (evcounter >= maxEventNumber && 
        maxEventNumber > 0 ||             // max has been set
        evcounter >= reader1.GetEntries()) // reached end of file
        throw yap::GeneratorExit{};
    evcounter++;

    if (Bfield < 0 * T) {
        std::cout << "WARNING: Bfield is required input to pipeline. Exit" << std::endl;
        throw yap::GeneratorExit{};
    }

    Event_map<std::any> eventmap; // data item for delivery
    Event<std::any> outdata; // to hold all the data items from file

    // collect all Signal info from file, reader holds event iterator
    
    if (reader1.Next()) { // variables filled from file
        outdata["TimeVec"] = std::make_any<std::vector<double>>(tvec->begin(),tvec->end());
        outdata["OmVec"] = std::make_any<std::vector<double>>(omvec->begin(),omvec->end());
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

    dp.getTruthRef().bfield = Bfield; // store input truth
    return dp;
}
