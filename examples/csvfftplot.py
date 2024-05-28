import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("t.csv", delimiter=',')
f = np.linspace(0,data.shape[0],data.shape[0])
plt.plot(f, data[:, 0])
plt.plot(f, data[:, 1], 'r')
plt.show()
