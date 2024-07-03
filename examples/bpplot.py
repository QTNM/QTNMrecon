import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("bp.csv", delimiter=',')
plt.plot(data[:,0], data[:,1])
plt.show()
