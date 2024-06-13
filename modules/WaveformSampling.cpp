// Waveform interpolator module implementation

// std
#include <iostream>
#include <stdexcept>

// ROOT
#include "ROOT/RVec.hxx"

// us
#include "WaveformSampling.hh"
#include "spline.h"


WaveformSampling::WaveformSampling(std::string in, std::string out) : 
    inkey(std::move(in)),
    outkey(std::move(out)),
    nantenna(1),
    sampletime(1.0 * ns)
{}


DataPack WaveformSampling::operator()(DataPack dp)
{
  if (! dp.getRef().count(inkey)) { 
        throw std::logic_error("input key not in dictionary!");
    }
    std::cout << "interpolator called" << std::endl;
    Event<std::any> indata = dp.getRef()[inkey];
    Event<std::any> outdata; // to hold all the data items
    // get hold of relevant data, transform and store in outdata
    if (indata.count("AntennaID")) { // sample Antenna waveform from G4 sim
        try
        {
            // cast the containers
            auto aID = std::any_cast<std::vector<int>>(indata["AntennaID"]);
            auto tiv = std::any_cast<std::vector<double>>(indata["TimeVec"]);
            auto vvv = std::any_cast<std::vector<double>>(indata["VoltageVec"]);
            ROOT::VecOps::RVec<int> tempant(aID); // temporary
            ROOT::VecOps::RVec<double> tempt(tiv); // temporary
            ROOT::VecOps::RVec<double> tempv(vvv); // temporary
            std::cout << "interpolator RVecs done" << std::endl;

            for (int i=0;i<nantenna;++i) {
                auto selecttvec = tempt[tempant==i]; // like NumPy selection
                auto selectvvec = tempv[tempant==i];
                vec_t tt(selecttvec.begin(),selecttvec.end()); // copy from RVec
                vec_t tv(selectvvec.begin(),selectvvec.end());

                vec_t resampled = interpolate(tt, tv);
                tt.clear();
                tv.clear();
                std::cout << "interpolator signal done, antenna " << i << std::endl;
                std::string tkey = "sampled_" + std::to_string(i) + "_[V]";
                outdata[tkey] = std::make_any<vec_t>(resampled);
            }
        }
        catch(const std::bad_any_cast& e)
        {
            std::cerr << e.what() << '\n';
        }
        outdata["sample_time[ns]"] = std::make_any<quantity<ns>>(sampletime);
        dp.getRef()[outkey] = outdata;

        // clear obsolete data in Event_map
        dp.getRef()[inkey].erase("AntennaID");
        dp.getRef()[inkey].erase("TimeVec");
        dp.getRef()[inkey].erase("VoltageVec");
    }
    else { // sample from antenna response calculator
        try
        {
            // cast the containers
            auto tiv = std::any_cast<std::vector<double>>(indata["TimeVec"]);

            for (int i=0;i<nantenna;++i) {
                std::string ikey = "VoltageVec_" + std::to_string(i) + "_[V]";
                auto tv = std::any_cast<vec_t>(indata[ikey]);
                // sample by interpolation
                vec_t resampled = interpolate(tiv, tv);
                // store result
                std::string okey = "sampled_" + std::to_string(i) + "_[V]";
                outdata[okey] = std::make_any<vec_t>(resampled);
                dp.getRef()[inkey].erase(ikey); // used; not needed anymore
            }
        }
        catch(const std::bad_any_cast& e)
        {
            std::cerr << e.what() << '\n';
        }
        outdata["sample_time[ns]"] = std::make_any<quantity<ns>>(sampletime);
        dp.getRef()[outkey] = outdata;

        // clear obsolete data in Event_map
        dp.getRef()[inkey].erase("TimeVec");
    }
    std::cout << "interpolator finish." << std::endl;

    return dp;
}

vec_t WaveformSampling::interpolate(const vec_t& tvals, const vec_t& vvals)
{
    vec_t resampled;
    double interval = tvals.back() - tvals.front();
    int maxpoints = (int)floor(interval / sampletime.numerical_value_in(ns));

    tk::spline ip(tvals, vvals); // interpolator
    for (int i=1;i<=maxpoints;++i)
        resampled.push_back(ip(i * sampletime.numerical_value_in(ns)));
    return resampled;
}
