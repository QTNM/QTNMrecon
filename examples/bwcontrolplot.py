''' Butterworth filter control plot using SciPy '''
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
duration = 2
t = np.linspace(0, duration, duration*1000, False)
sig = np.sin(2*np.pi*10*t)+np.sin(2*np.pi*20*t)
plt.plot(t, sig)
sos = signal.butter(10, 15, fs=1000, output='sos')
filtered = signal.sosfilt(sos, sig)
plt.plot(t, filtered)
plt.show()
