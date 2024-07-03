import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("mix.csv", delimiter=',')
plt.plot(data[:,0], data[:,1])
plt.show()
