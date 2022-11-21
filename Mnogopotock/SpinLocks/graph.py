
import numpy as np
import matplotlib.pyplot as plt

xtas, ytas = np.loadtxt("tas", delimiter=' ', unpack=True)
xttas, yttas = np.loadtxt("ttas", delimiter=' ', unpack=True)
xticket, yticket = np.loadtxt("ticket", delimiter=' ', unpack=True)


fig, ax = plt.subplots()

ax.scatter(xtas, ytas, color = "orange",  label = "tas")
ax.scatter(xttas, yttas, color = "green",  label = "ttas")
ax.scatter(xticket, yticket, color = "pink",  label = "ticket")

    
ax.set_xlabel("Treads")
ax.set_ylabel("Time")
ax.grid()
ax.legend()
fig.set_figwidth(8)
fig.set_figheight(6)
plt.show()
