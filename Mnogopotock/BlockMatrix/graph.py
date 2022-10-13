
import numpy as np
import matplotlib.pyplot as plt

x, y = np.loadtxt("naive_many.txt", delimiter=',', unpack=True)


fig, ax = plt.subplots()
ax.scatter(x, y, color = "orange",  label = "naive")

x, y = np.loadtxt("block_many.txt", delimiter=',', unpack=True)
ax.scatter(x, y, color = "green",  label = "block")
    
ax.set_xlabel("Treads")
ax.set_ylabel("Time")
ax.grid()
ax.legend()
plt.show()

x, y = np.loadtxt("naive_one.txt", delimiter=',', unpack=True)


fig, bx = plt.subplots()
bx.scatter(x, y, color = "cyan", label = "naive")

x, y = np.loadtxt("block_one.txt", delimiter=',', unpack=True)
bx.scatter(x, y, color = "pink", label = "block")
    
bx.set_xlabel("Matrix size")
bx.set_ylabel("Time")
bx.grid()
bx.legend()
plt.show()
