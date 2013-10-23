# -*- mode: gnuplot; -*-

mm=1.0
cm=10*mm
g=1
cm2=cm*cm
cm3=cm*cm2

# Cathode Ring :
ro=21.0*mm
ri=20.0*mm
h=53.5*mm
s=pi*(ro**2-ri**2)
v=s*h
print "Cathode ring volume=", v / cm3 , " cm3"
d=8.96*g/cm3
mass=v*d
print "Cathode ring mass=", mass / g , " g"

# Anode bus bar :
mass=4*g
z=82.0
d=8.96*g/cm3
v=mass/d
s=v/z
r=sqrt(s/pi)
print "Anode bus bar radius=", r / mm , " mm"

# Cathode bus bar :
mass=15.3*g
z=10.0*mm
d=8.96*g/cm3
v=mass/d
print "Cathode bus bar volume=", v / cm3 , " cm3"
s=v/z
print "Cathode bus bar section=", s / cm2 , " cm2"
ro=21*mm
ri=sqrt(ro**2-(s/pi))
print "Cathode bus internal radius=", ri / mm , " mm"

# CarrierEndCell
z=10*mm
ro=21*mm
m=18.93*g+17.65*g
print "Carrier+end cell outer mass=", m / g , " g"
d=1.42*g/cm3
v=m/d
print "Carrier+end cell outer volume=", v / cm3 , " cm3"
s=v/z
ro=sqrt((v/(pi*z))+ri**2)
print "Carrier+end cell outer radius=", ro / mm , " mm"
