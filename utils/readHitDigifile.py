''' reader of ROOT output file from writeHitDigiToROOT module.'''
from dataclasses import dataclass
import numpy as np
import ROOT

# set convenience data bags to collect related data like in Event.hh
@dataclass
class truth_t:
    ''' collect all truth type values from event.'''
    nantenna: int = 0
    snr: float = 0.0
    sampling_time_s: float = 0.0
    average_omega_Hz : float = 0.0
    beat_frequency_Hz: float = 0.0
    chirp_rate_Hz: float = 0.0


@dataclass
class vertex_t:
    ''' collect all vertex type values from event.'''
    event_ID: int = 0
    track_ID: int = 0
    posx_m: float = 0.0
    posy_m: float = 0.0
    posz_m: float = 0.0
    kinetic_energy_keV: float = 0.0
    pitch_angle_deg: float = 0.0


@dataclass
class hit_t:
    ''' collect all hit type values from event.'''
    event_ID: int = 0
    track_ID: int = 0
    locx_m: float = 0.0
    locy_m: float = 0.0
    locz_m: float = 0.0
    time_ns: float = 0.0
    deposited_energy_eV: float = 0.0
    post_theta_deg: float = 0.0


@dataclass
class digitizer_t:
    ''' collect all digitizer type values from event.'''
    gain: float = 0.0
    target_frequency_Hz: float = 0.0
    sampling_rate_Hz: float = 0.0


def convert_to_data_dict(event):
    ''' convert TTree event, input TTree event, return data product.'''
    datadict = {}
    truth = truth_t()
    vertex = vertex_t()
    hit = hit_t()
    measured = digitizer_t()

    # copy all data from event
    truth.nantenna = event.truth_nantenna
    truth.snr = event.truth_snratio
    truth.sampling_time_s = event.truth_samplingtime_s
    truth.average_omega_Hz = event.truth_avomega_Hz
    truth.beat_frequency_Hz = event.truth_beatf_Hz
    truth.chirp_rate_Hz = event.truth_chirp_Hz_s
    vertex.event_ID = event.vertex_evID
    vertex.track_ID = event.vertex_trackID
    vertex.posx_m = event.vertex_posx_m
    vertex.posy_m = event.vertex_posy_m
    vertex.posz_m = event.vertex_posz_m
    vertex.kinetic_energy_keV = event.vertex_kinenergy_eV
    vertex.pitch_angle_deg = event.vertex_pitchangle_deg
    measured.gain = event.digi_gain
    measured.sampling_rate_Hz = event.digi_samplingrate_Hz
    measured.target_frequency_Hz = event.digi_tfrequency_Hz
    hits = []
    nhits = event.hit_eventID.size()  # is vector.size()
    for h in range(nhits):
        hit.event_ID = event.hit_eventID[h]
        hit.track_ID = event.hit_trackID[h]
        hit.locx_m = event.hit_locx_m[h]
        hit.locy_m = event.hit_locy_m[h]
        hit.locz_m = event.hit_locz_m[h]
        hit.time_ns = event.hit_time_ns[h]
        hit.deposited_energy_eV = event.hit_edep_eV[h]
        hit.post_theta_deg = event.hit_posttheta_deg[h]
        hits.append(hit)
    
    datadict["truth"] = truth  # data class
    datadict["vertex"] = vertex  # data class
    datadict["hits"] = hits  # list of data classes
    datadict["digitizer"] = measured  # data class
    return datadict

# main reader script
# pick filename
fname = "recon.root"

file = ROOT.TFile(fname)
tree = file.Get("recon")
nantenna = 2 # needed since reading requires the vectors already be present
pvec = ROOT.std.vector(ROOT.std.vector('double'))() # explicit construction
svec = ROOT.std.vector(ROOT.std.vector('double'))()

brname1 = "pure_V"
tree.SetBranchAddress(brname1, ROOT.AddressOf(pvec))
brname2 = "signal_V"
tree.SetBranchAddress(brname2, ROOT.AddressOf(svec))

# Analysis loop for each entry/row in TTree on file
for entry in tree:
    datadict = convert_to_data_dict(entry)  # fill all the simple data
    for i in range(nantenna):
        # a single digitizer signal as np.array
        signalarray = np.asarray(svec[i])

        # ... do some analysis.

        print("got signal from antenna ",i," of size: ",signalarray.size)
    # print all simple data items
    for k,v in datadict.items():
        print("Key: ",k)
        print("Value: ",v)
