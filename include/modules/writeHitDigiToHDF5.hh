// HDF5 Hit and Digitizer writer for a pipeline module

#ifndef writerHdf5HitDigi_HH
#define writerHdf5HitDigi_HH 1

// std
#include <vector>

// HighFive
#include <highfive/highfive.hpp>

// must have include for pipeline
#include <Event.hh>

class WriterHitDigiToHDF5
{
    public:
        WriterHitDigiToHDF5(HighFive::Group& gr); // constructor; required

        void operator()(DataPack dp); // this is called by the pipeline
        // Writers/Sinks only receive a DataPack and return void as signature.

        // getter/setter methods for configuration could live here.

    private:
      // include any configuration data members for internal use here.
      HighFive::Group& simgroup; // from outside
      HighFive::Group  eventgr; // used in operator
  
      int nant, evID, trID; // ID numbers for grouping
      bool bnew_event, bnew_sim;

      std::vector<int> trackHistory;
      // hit data
      std::vector<int> hittrID;
      std::vector<double> hitx;
      std::vector<double> hity;
      std::vector<double> hitz;
      std::vector<double> hitedep;
      std::vector<double> hittime;
      std::vector<double> hitposttheta;

};
#endif
