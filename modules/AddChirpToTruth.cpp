// add chirp rate to truth implementation

// std
#include <iostream>

// us
#include "AddChirpToTruth.hh"
#include <mp-units/ostream.h> // for cout stream


AddChirpToTruth::AddChirpToTruth(std::string in) : 
    inkey(std::move(in))
{}

DataPack AddChirpToTruth::operator()(DataPack dp)
{
    // example getting hold of requested input data for processing
    if (! dp.getRef().count(inkey)) { 
        std::cout << "input key not in dictionary!" << std::endl;
        return dp; // not found, return unchanged map, no processing
    }
    Event<std::any> indata = dp.getRef()[inkey]; // access L1 dictionary
    // yields a L2 unordered map called Event<std::any> with the 
    // help of the inkey label.
    if (! indata.count("KEVec")) { 
        std::cout << "KE vector not in dictionary!" << std::endl;
        return dp; // not found, return unchanged map, no processing
    }

    // use KEvec data vector for fitting
    nantenna = dp.getTruthRef().nantenna; // got that from reader
    lft = new TLinearFitter(1,"pol1",""); // line fit, intend to use robust version
    lft->StoreData(false);
    try
    {
        auto temptiv = std::any_cast<std::vector<double>>(indata["TimeVec"]); // [ns] from file
        // get hold of truth data from sim
        vec_t timev; // deconstruct timevec for both input cases with nantenna
        // antenna input stream: interleaved values per antenna / kinematic: contiguous values(i.e. +1)
        for (int i=0;i<temptiv.size();i+=nantenna) timev.push_back(temptiv[i]); // select
        auto ov = std::any_cast<vec_t>(indata["KEVec"]); // KE vector in keV

        lft->AssignData(ov.size(), 1, timev.data(), ov.data());
        lft->EvalRobust(0.8); // allow 20% outlier data
        double tslope = lft->GetParameter(1); // fit result for chirp rate [keV/ns]
        double tinter = lft->GetParameter(0); // fit result for intercept [keV]
	quantity<keV> i0 = tinter * keV; // manual unit
	quantity<keV> i1 = i0 + tslope * 1.e3 * keV; // KE after 1 mus
        std::cout << "loss/mus: " << (i1-i0).in(eV) << " from " << i0 << std::endl;

	quantity<Hz> fslope = e2f(i1, dp.getTruthRef().bfield) - e2f(i0, dp.getTruthRef().bfield);
        std::cout << "frequency chirp per mus: " << fslope << std::endl;
        dp.getTruthRef().chirp_rate = fslope*1.e6 / s; // store truth Hz/s
    }
    catch(const std::bad_any_cast& e)
    {
      std::cerr << "AddChirpTo Truth: " << e.what() << '\n';
    }
        
    dp.getRef()[inkey].erase("KEVec"); // used, obsolete
    return dp;
}

quantity<Hz> AddChirpToTruth::e2f(quantity<keV> en, quantity<T> bf)
{
    double enmass = en.numerical_value_in(J) / (c_SI*c_SI); // [kg]
    quantity<Hz> f0 = bf.numerical_value_in(T) * qe_SI / (2.0*myPi) / (me_SI + enmass) * Hz; // dim analysis is checked
    return f0;
}
