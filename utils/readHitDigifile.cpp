// reader of ROOT output file from writeHitDigiYoRoot module
// std includes
#include <iostream>
#include <vector>
#include <string>

// ROOT include
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// us
#include "CLI11.hpp"

struct truth_t {
    int nantenna;
    double snr;
    double sampling_time_s;
    double average_omega_Hz;
    double beat_frequency_Hz;
    double chirp_rate_Hz;
};

struct vertex_t {
    int event_ID;
    int track_ID;
    double posx_m;
    double posy_m;
    double posz_m;
    double kinetic_energy_keV;
    double pitch_angle_deg;
};

struct hit_t {
    int event_ID;
    int track_ID;
    double locx_m;
    double locy_m;
    double locz_m;
    double time_ns;
    double deposited_energy_eV;
    double post_theta_deg;
};

struct digitizer_t {
    double gain;
    double target_frequency_Hz;
    double sampling_rate_Hz;
};

int main(int argc, char** argv)
{
    // command line interface
    CLI::App app{"Example Recon Pipeline"};
    int nantenna = 2; // must be known to set up the reader before reading
    std::string fname = "recon.root";

    app.add_option("-i,--input", fname, "<input file name> Default: recon.root");
    app.add_option("-n,--nantenna", nantenna, "<int number of antenna on file> Default: 2");

    CLI11_PARSE(app, argc, argv);

    // bring some order to values
    std::vector<hit_t> hits;
    hit_t hit;
    truth_t truth;
    vertex_t vertex;
    digitizer_t measured;

    TFile* ff = new TFile(fname.data(),"READ");
    TTreeReader reader("recon", ff);

    // truth
    TTreeReaderValue<int> tnan(reader, "truth_nantenna");
    TTreeReaderValue<double> tsnr(reader, "truth_snratio");
    TTreeReaderValue<double> tsam(reader, "truth_samplingtime_s");
    TTreeReaderValue<double> tom(reader, "truth_avomega_Hz");
    TTreeReaderValue<double> tb(reader, "truth_beatf_Hz");
    TTreeReaderValue<double> tch(reader, "truth_chirp_Hz_s");
    // vertex
    TTreeReaderValue<int> vev(reader, "vertex_evID");
    TTreeReaderValue<int> vtr(reader, "vertex_trackID");
    TTreeReaderValue<double> vx(reader, "vertex_posx_m");
    TTreeReaderValue<double> vy(reader, "vertex_posy_m");
    TTreeReaderValue<double> vz(reader, "vertex_posz_m");
    TTreeReaderValue<double> vk(reader, "vertex_kinenergy_eV");
    TTreeReaderValue<double> vp(reader, "vertex_pitchangle_deg");
    // digi data
    TTreeReaderValue<double> dg(reader, "digi_gain");
    TTreeReaderValue<double> ds(reader, "digi_samplingrate_Hz");
    TTreeReaderValue<double> dt(reader, "digi_tfrequency_Hz");

    // hit data
    TTreeReaderValue<std::vector<int>> hitevID(reader, "hit_eventID");
    TTreeReaderValue<std::vector<int>> hittrID(reader, "hit_trackID");
    TTreeReaderValue<std::vector<double>> hitx(reader, "hit_locx_m");
    TTreeReaderValue<std::vector<double>> hity(reader, "hit_locy_m");
    TTreeReaderValue<std::vector<double>> hitz(reader, "hit_locz_m");
    TTreeReaderValue<std::vector<double>> hitedep(reader, "hit_edep_eV");
    TTreeReaderValue<std::vector<double>> hittime(reader, "hit_time_ns");
    TTreeReaderValue<std::vector<double>> hitposttheta(reader, "hit_posttheta_deg");

    // signals
    TTreeReaderArray<std::vector<double>> purewave(reader, "pure_V");
    TTreeReaderArray<std::vector<double>> scopedata(reader, "signal_V");;

    // event loop and organize hits to vec<hit_t>
    while (reader.Next()) {
      // assignments, only once values are present
      truth.nantenna = *tnan;
      truth.snr = *tsnr;
      truth.sampling_time_s = *tsam;
      truth.average_omega_Hz = *tom;
      truth.beat_frequency_Hz = *tb;
      truth.chirp_rate_Hz = *tch;
      vertex.event_ID = *vev;
      vertex.track_ID = *vtr;
      vertex.posx_m = *vx;
      vertex.posy_m = *vy;
      vertex.posz_m = *vz;
      vertex.kinetic_energy_keV = *vk;
      vertex.pitch_angle_deg = *vp;
      measured.gain = *dg;
      measured.sampling_rate_Hz = *ds;
      measured.target_frequency_Hz = *dt;
      // hits
      hits.clear();
      for (int j=0;j<hitevID->size();++j) {
	hit.event_ID = hitevID->at(j);
	hit.track_ID = hittrID->at(j);
	hit.locx_m  = hitx->at(j);
	hit.locy_m  = hity->at(j);
	hit.locz_m  = hitz->at(j);
	hit.time_ns = hittime->at(j);
	hit.deposited_energy_eV = hitedep->at(j);
	hit.post_theta_deg = hitposttheta->at(j);
	hits.push_back(hit);
      }
        // got all data, can analyse
        // trial prints to check reading
	// NOTE: scopedata is a TTreeReaderArray
	std::cout << "got TTreeReaderArray from antenna 0 of length " << scopedata.At(0).size() << std::endl;
	std::cout << "got TTreeReaderArray from antenna 1 of length " << scopedata.At(1).size() << std::endl;
	// signal in vector<double> instead, convert:
	std::vector<double> signal(scopedata.At(0).begin(),scopedata.At(0).end());
	std::cout << "got signal from antenna 0 in vector " << signal.size() << std::endl;
	
        std::cout << "also n hits " << hits.size() << std::endl;
        // truth data read in
        std::cout << "truth struct:" << std::endl;
        std::cout << "nantenna: " << truth.nantenna << std::endl;
        std::cout << "snr: " << truth.snr << std::endl;
        std::cout << "sampling time [s]: " << truth.sampling_time_s << std::endl;
        std::cout << "av omega [Hz]: " << truth.average_omega_Hz << std::endl;
        std::cout << "beat freq [Hz]: " << truth.beat_frequency_Hz << std::endl;
        std::cout << "chirp rate [Hz]: " << truth.chirp_rate_Hz << std::endl;
    }
    ff->Close();
    return 0;
}
