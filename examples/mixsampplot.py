import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("mixsamp.csv", delimiter=',')
plt.plot(data[:,0], data[:,1])
plt.scatter(data[:,0], data[:,1],c='r')
plt.show()
