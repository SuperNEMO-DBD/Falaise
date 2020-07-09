#!/usr/bin/env python

import sys

fldata = "_work.d/run-feature-pr75-validation/calo_de.data"
if len(sys.argv) > 0 :
    fldata = sys.argv[1]

f = open(fldata, "r")
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
