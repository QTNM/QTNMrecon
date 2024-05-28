import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("f.csv", delimiter=',')
plt.plot(data[:,0])
plt.plot(data[:,1],'r')
plt.show()
