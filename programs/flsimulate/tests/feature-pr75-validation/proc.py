#!/usr/bin/env python


f = open("_work.d/calo_de.data")
lines = f.readlines()

n=0
s=0.0
for l in lines :
    line = l[:-1]
    tokens = line.split()
    if len(tokens) > 0 :
        delta_e = float(tokens[0])
        s += delta_e
        n += 1

mean_energy = s/n
print(int(mean_energy * 1000))
