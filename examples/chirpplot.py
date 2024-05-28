import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("chirp.csv")
plt.plot(data)
plt.show()
