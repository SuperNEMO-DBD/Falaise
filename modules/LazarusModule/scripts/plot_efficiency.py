
import numpy as np
import matplotlib.pyplot as plt

with open('efficiency_BETWEEN.txt') as f:
    lines = f.readlines()
    dead_cells = [line.split()[0] for line in lines]
    efficiency = [line.split()[1] for line in lines]
    zero = [line.split()[2] for line in lines]
    single = [line.split()[3] for line in lines]
    more = [line.split()[4] for line in lines]



fig = plt.figure()

ax1 = fig.add_subplot(111)

ax1.set_ylim([0,100])


ax1.set_title("Killing cells in the Tracker using a Falaise module")    
ax1.set_xlabel('Number of dead cells')
ax1.set_ylabel('[%]')

ax1.plot(dead_cells,efficiency, color='green', label='Efficiency', marker='o')
ax1.plot(dead_cells,zero, color='red', label='Zero reconstructed tracks', marker='o')
ax1.plot(dead_cells,single, color='yellow', label='Single track', marker='o')
ax1.plot(dead_cells,more, color='blue', label='More than two reconstructed tracks', marker='o')

leg = ax1.legend(loc='upper left', fontsize='small')

plt.savefig('efficiency.png')
plt.show()
plt.draw()

