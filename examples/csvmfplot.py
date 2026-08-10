import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("mf.csv",delimiter=',')
plt.plot(data[:,1])
plt.xlabel('sample points')
plt.ylabel('filter output')
plt.show()
plt.plot(data[:,0],data[:,1])
plt.xlabel('time lag [s]')
plt.ylabel('filter output')
plt.show()
data2 = np.loadtxt("mf2.csv",delimiter=',')
plt.plot(data2[:,1])
plt.xlabel('sample points')
plt.ylabel('filter output')
plt.show()
plt.plot(data2[:,0],data2[:,1])
plt.xlabel('time lag [s]')
plt.ylabel('filter output')
plt.show()

