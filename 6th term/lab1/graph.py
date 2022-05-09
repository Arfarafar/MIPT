
import numpy as np 
import matplotlib.pyplot as plt

def printf(u, x, t):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    
    X, T = np.meshgrid(x, t) 
    ax.plot_surface(X, T, u, rstride=1, cstride=1, cmap='viridis')
       
  
    plt.show()


 
data = np.genfromtxt("res.mp", delimiter=';', dtype=np.double)
x = np.arrange(0.0, 0.0001*len(data[]), 0.0001)
y = np.arrange(0.0, 0.00005*len(data), 0.00005)

printf(data, x, t)
