import numpy as np
import matplotlib.pyplot as plt

plt.close("all")

from matplotlib import rc
rc('text', usetex=True)


data = np.loadtxt("N.dat")


cumu = np.zeros(20)

h1,e1 = np.histogram(data,bins=20,range=[0,20],normed=True)

for i in range(len(h1)):
    cumu[i] = 0
    for j in range(i):
        cumu[i] += h1[j]

x = np.array([i for i in range(20)])

fig,ax1 = plt.subplots(figsize=(5,4))

ax1.hist(data,bins=20,range=[0,20],color="royalblue",histtype="step",normed=True)

ax2 = ax1.twinx()
ax2.plot(x,cumu,ls="--",marker="^",color="crimson",ms=3,lw=0.5)
ax1.tick_params(axis='both', which='major', labelsize=15)
ax1.tick_params(axis='both', which='minor', labelsize=10) 
ax2.tick_params(axis='both', which='major', labelsize=15)
ax2.tick_params(axis='both', which='minor', labelsize=10) 

ax1.set_xlabel("$N$",fontsize=17)
ax1.set_ylabel("Prob. Density",fontsize = 17,color="royalblue")
ax1.set_ylim([0,0.22])
ax2.set_ylim([0,1.1])
ax2.set_ylabel("CDF",fontsize=17,color="crimson")
plt.savefig("plotN.pdf",bbox_inches="tight")
