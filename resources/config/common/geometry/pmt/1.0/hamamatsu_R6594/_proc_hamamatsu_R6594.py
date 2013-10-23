#!/usr/bin/python

import os
import sys

file = open ("hamamatsu_R6594_capture_1.data");

lines = file.readlines ()
mm = 1.0

r0=0.0
zmin=10000000.0
zmax=-10000000.0
rmin=10000000.0
rmax=-10000000.0
for line in lines:
    l = line.strip ()
    if len (l) == 0:
        continue
    if l[0:1] == '#' :
        # if l.startswith ("#@r0") :
        #     dum = l.split (" ")
        #     sys.stderr.write ("DEVEL: dum[0]=%s\n" % dum[0])
        #     sys.stderr.write ("DEVEL: dum[1]=%s\n" % dum[1])
        #     r0 = float (dum[1])
        #     sys.stderr.write ("DEVEL: r0=%g\n" % r0)
        continue
    token = l.split (' ');
    z = float (token[0])
    r = float (token[1])
    #sys.stderr.write ("DEVEL: z=%f r=%f\n" % (z, r))
    if z < zmin: 
        zmin = z
    if z > zmax: 
        zmax = z
    if r < rmin: 
        rmin = r
    if r > rmax: 
        rmax = r
zmed = 0.5 * (zmin + zmax)
#sys.stderr.write ("DEVEL: zmin=%f\n" % zmin)
#sys.stderr.write ("DEVEL: zmed=%f\n" % zmed)
#sys.stderr.write ("DEVEL: zmax=%f\n" % zmax)
#sys.stderr.write ("DEVEL: rmin=%f\n" % rmin)
#sys.stderr.write ("DEVEL: rmax=%f\n" % rmax)

r0 = rmax
#sys.stderr.write ("DEVEL: r0=%f\n" % r0)
dr = rmax - rmin
dz = zmax - zmin
#sys.stderr.write ("DEVEL: dz=%g\n" % dz)
#sys.stderr.write ("DEVEL: dr=%g\n" % dr)

# mm
DR = 0.5 * 128.0 * mm
DZ = 178.0 * mm
Zmin = -0.5 * DZ
Zmax = +0.5 * DZ
ar = -DR / dr
br = - ar * rmax
#sys.stderr.write ("DEVEL: ar=%g\n" % ar)
#sys.stderr.write ("DEVEL: br=%g\n" % br)
az = DZ / dz
bz = - az * zmed
#sys.stderr.write ("DEVEL: az=%g\n" % az)
#sys.stderr.write ("DEVEL: bz=%g\n" % bz)

print "#@length_unit mm"
print """# This polycone building mode use an interpolation scheme
# to generate inner and outer surfaces from the following list
# of points [z ; r(outer envelope)]:
#@skin_thickness 1.0
#@skin_step      5"""

for line in lines:
    l = line.strip ()
    if len (l) == 0:
        continue
    if l[0:1] == '#' :
        continue
    token = l.split (' ');
    z = float (token[0])
    r = float (token[1])
    R = r * ar + br
    Z = z * az + bz
    print Z , " ", R

# end
