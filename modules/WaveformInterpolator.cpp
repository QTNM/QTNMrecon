// Waveform interpolator module implementation

// std
#include <iostream>
#include <stdexcept>

// ROOT
#include "ROOT/RVec.hxx"

// us
#include "WaveformInterpolator.hh"
#include "spline.h"


WaveformInterpolator::WaveformInterpolator(std::string in, std::string out) : 
    inkey(std::move(in)),
    outkey(std::move(out)),
    nantenna(1),
    sampletime(1.0 * ns)
{}


Event_map<std::any> WaveformInterpolator::operator()(Event_map<std::any> emap)
{
    if (! emap.count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    Event<std::any> indata = emap[inkey];
    Event<std::any> outdata; // to hold all the data items
    // get hold of relevant data, transform and store in outdata
    try
    {
        // cast the containers
        auto aID = std::any_cast<std::vector<int>>(indata["AntennaID"]);
        auto tiv = std::any_cast<std::vector<double>>(indata["TimeVec"]);
        auto vvv = std::any_cast<std::vector<double>>(indata["VoltageVec"]);
        ROOT::VecOps::RVec<int> tempant(aID); // temporary
        ROOT::VecOps::RVec<double> tempt(tiv); // temporary
        ROOT::VecOps::RVec<double> tempv(vvv); // temporary
        for (int i=0;i<nantenna;++i) {
            auto selecttvec = tempt[tempant==i]; // like NumPy sepection
            auto selectvvec = tempv[tempant==i];
            const vec_t tt(selecttvec.begin(),selecttvec.end()); // copy from RVec
            const vec_t tv(selectvvec.begin(),selectvvec.end());

            vec_t resampled = interpolate(tt, tv);
            std::string tkey = "sampled_" + std::to_string(i) + "_[V]";
            outdata[tkey] = std::make_any<vec_t>(resampled);
        }
    }
    catch(const std::bad_any_cast& e)
    {
        std::cerr << e.what() << '\n';
    }
    outdata["sample_time[ns]"] = std::make_any<quantity<ns>>(sampletime);
    emap[outkey] = outdata;

    // clear obsolete data in Event_map
    emap[inkey]["AntennaID"]  = std::any(0); // replaces vector<int>
    emap[inkey]["TimeVec"]    = std::any(0); // replaces vector<double>
    emap[inkey]["VoltageVec"] = std::any(0); // replaces vector<double>

    return emap;
}

vec_t WaveformInterpolator::interpolate(const vec_t& tvals, const vec_t& vvals)
{
    vec_t resampled;
    double interval = tvals.back() - tvals.front();
    int maxpoints = (int)floor(interval / sampletime.numerical_value_in(ns));

    tk::spline ip(tvals, vvals); // interpolator
    for (int i=1;i<=maxpoints;++i)
        resampled.push_back(ip(i * sampletime.numerical_value_in(ns)));
    return resampled;
}
