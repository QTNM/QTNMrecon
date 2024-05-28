// reader module implementation

// std
#include <iostream>

// us
#include "QTNMSimHitReader.hh"
#include "yap/pipeline.h"

// ROOT
#include "TTreeReaderValue.h"

QTNMSimHitReader::QTNMSimHitReader(TTreeReader& re, std::string out) : 
    outkey(std::move(out)),
    maxEventNumber(-1), // default -1 for a all events
    evcounter(0),
    reader(re)
{
}

Event_map<std::any> QTNMSimHitReader::operator()()
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
    TTreeReaderValue<int> eventID(reader, "EventID"); // needs reader by reference
    TTreeReaderValue<int> trackID(reader, "TrackID");
    TTreeReaderValue<double> edep(reader, "Edep"); // interaction data
    TTreeReaderValue<double> tstamp(reader, "TimeStamp");
    TTreeReaderValue<double> prek(reader, "PreKine");
    TTreeReaderValue<double> postk(reader, "PostKine");
    TTreeReaderValue<double> preth(reader, "PreTheta");
    TTreeReaderValue<double> postth(reader, "PostTheta");
    TTreeReaderValue<double> posx(reader, "Posx"); // interaction location
    TTreeReaderValue<double> posy(reader, "Posy");
    TTreeReaderValue<double> posz(reader, "Posz");
    
    if (reader.Next()) {; // variables filled from file
        outdata["eventID"] = std::any(*eventID); // de-reference an int to std::any
        outdata["trackID"] = std::any(*trackID);
        outdata["Edep"] = std::any(*edep);
        outdata["TimeStamp"] = std::any(*tstamp);
        outdata["PreKine"] = std::any(*prek);
        outdata["PostKine"] = std::any(*postk);
        outdata["PreTheta"] = std::any(*preth);
        outdata["PostTheta"] = std::any(*postth);
        outdata["Posx"] = std::any(*posx);
        outdata["Posy"] = std::any(*posy); 
        outdata["Posz"] = std::any(*posz); 
    }
    else // no more entries in TTreeReader
        throw yap::GeneratorExit{};

    // at the end, store new data product in dictionary event map.
    eventmap[outkey] = outdata; // with outdata an Event<std::any>
    return eventmap;
}
