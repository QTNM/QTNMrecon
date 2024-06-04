// template module implementation
// necessarily empty since nothing is supposed to happen
// for this example, showing boiler-plate code for any 
// processing module.

// std
#include <iostream>

// us
#include "templatemodule.hh"

ExampleModule::ExampleModule(std::string in, std::string out) : 
    inkey(std::move(in)), outkey(std::move(out)) {}

DataPack ExampleModule::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
    if (! dp.getRef().count(inkey)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return emap; // not found, return unchanged map, no processing
    }
    Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey label.

    // action of module template module.
    
    Event<std::any> outdata;
    // at the end, store new data product in dictionary event map.
    dp.getRef()[outkey] = outdata; // with outdata an Event<std::any>
    return dp;
}
