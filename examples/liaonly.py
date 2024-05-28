""" Lock-in amplifier test """
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal


def sine_wave(amplitude, freq, phase=0, nsamples=1000):
    """ make a sine wave. """
    phi = np.deg2rad(phase)
    time_array = np.linspace(0, 1, nsamples)
    sig = amplitude * np.sin(freq*2*np.pi*time_array + phi)
    return sig


def apply_lp(data, lp_freq, nsamp):
    """ butterworth low pass filter. """
    sos = signal.butter(10, lp_freq, output='sos', fs=nsamp)
    return signal.sosfilt(sos, data)


def lia(sig, reference, crossref, filter_freq, srate):
    """ lock-in amplifier operation: return magnitude response."""
    sample_freq = srate  # [Hz]
    sine_noisy_mixed = np.multiply(sig, reference)
    sine_filt = apply_lp(sine_noisy_mixed, filter_freq, sample_freq)
    cosine_noisy_mixed = np.multiply(sig, crossref)
    cosine_filt = apply_lp(cosine_noisy_mixed, filter_freq, sample_freq)

    noisy_quad_mag = np.sqrt(np.add(np.square(sine_filt),
                                    np.square(cosine_filt)))
    return noisy_quad_mag


def plotsig(sig, fromto=()):
    """quick plottting with index range."""
    if fromto == ():
        plt.plot(sig)
    else:
        plt.plot(sig[fromto[0]:fromto[1]])
    plt.xlabel('time [ns]')
    plt.ylabel('Amplitude')
    plt.show()

# make a signal and analyse
NSR    = 1.0      # noise-to-signal ratio
FREQ   = 20.0     # signal frequency
NS     = 1000     # sampling rate
AMP    = 1.0      # signal amplitude

# signals
sine = sine_wave(AMP, FREQ, nsamples=NS)
cosine = sine_wave(AMP, FREQ, phase=90.0, nsamples=NS)
sine_noisy = sine + NSR * np.random.normal(size=len(sine))
#sine_noisy /= np.max(sine_noisy)  # normalize to voltage range

# lock-in operation in quadrature
FFREQ = 5.0  # low-pass filter frequency cut-off [kHz]
# result = lia(sine_noisy, sine, cosine, FFREQ, NS)
result = lia(sine, sine, cosine, FFREQ, NS)
plotsig(result)

# plot
#plt.plot(result)
#plt.xlabel('time')
#plt.ylabel('quadrature')
#plt.show()
