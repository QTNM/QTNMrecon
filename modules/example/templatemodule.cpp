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

Event_map<std::any> ExampleModule::operator()(Event_map<std::any> emap)
{
    // example getting hold of requested input data for processing
    if (! emap.count(inkey)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return emap; // not found, return unchanged map, no processing
    }
    Event<std::any> indata = emap[inkey]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey label.

    // action of module template module.
    
    Event<std::any> outdata;
    // at the end, store new data product in dictionary event map.
    emap[outkey] = outdata; // with outdata an Event<std::any>
    return emap;
}
