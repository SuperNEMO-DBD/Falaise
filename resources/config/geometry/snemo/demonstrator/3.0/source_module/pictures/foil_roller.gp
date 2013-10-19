
# Top source foil roller data :

mm=1.0
cm=10.0*mm
cm2=cm*cm
cm3=cm2*cm
g=1.

mass=(73. + 3.25 + 3 * 0.08) *g
mass=77 *g
h = 125.00 * mm
d=1.42 * g / cm**3
rext=12.5*mm
v = mass/d
print "Top source foil roller :"
print "|-- Mass    = " , mass / g , " g"
print "|-- Volume  = " , v / cm**3 , " cm3"
s=v/h
print "|-- Section = " , s / cm**2 , " cm2"
print "|-- External radius = " , rext / cm , " cm"
rint=(sqrt(rext**2 - s/pi))
print "|-- Internal radius = " , rint / cm , " cm"
rint=3.5* mm
print "\`-- Effective internal radius = " , rint / cm , " cm"

# Bottom source foil holder data :
mass=53 *g
x= 58*mm
y= 18*mm
d=1.42 * g / cm**3
v = mass/d
print "Bottom source foil holder :"
print "|-- Mass    = " , mass / g , " g"
print "|-- Volume  = " , v / cm**3 , " cm3"
s=x*y
z=v/s
print "|-- Z  = " , z / cm , " cm"
z=36* mm
print "\`-- Effective Z = " , z / cm , " cm"

# Top source foil holder data :
mass=48 *g
x= 58*mm
y= 18*mm
d=1.42 * g / cm**3
v = mass/d
print "Top source foil holder :"
print "|-- Mass    = " , mass / g , " g"
print "|-- Volume  = " , v / cm**3 , " cm3"
s=x*y
z=v/s
print "|-- Z  = " , z / cm , " cm"
z=33* mm
print "\`-- Effective Z = " , z / cm , " cm"

# Approximation : a long plate between the beam and the foil
# that have the proper Delrin mass and reasonnable similar
# effective surface.

# Top quantity of Delrin :

Mdelrin_top=3900*g
ddelrin_top=1.42*g/cm3
Vdelrin_top=Mdelrin_top/ddelrin_top
print "Total Delrin (top) :"
print "|-- Mass    = " , Mdelrin_top / g , " g"
print "|-- Volume    = " , Vdelrin_top / cm3 , " cm3"
Ydelrin_top=5000*mm
Sdelrin_top=Vdelrin_top/Ydelrin_top
print "|-- Section    = " , Sdelrin_top / cm2 , " cm2"
Xdelrin_top=58*mm
Zdelrin_top=Sdelrin_top/Xdelrin_top
print "|-- Z          = " , Zdelrin_top /cm , " cm"
Surfdelrin_top=Xdelrin_top*Ydelrin_top
print "\`-- Surface    = " , Surfdelrin_top /cm2 , " cm2"

# Bottom quantity of Delrin :

Mdelrin_bottom=4755*g
ddelrin_bottom=1.42*g/cm3
Vdelrin_bottom=Mdelrin_bottom/ddelrin_bottom
print "Total Delrin (bottom) :"
print "|-- Mass    = " , Mdelrin_bottom / g , " g"
print "|-- Volume    = " , Vdelrin_bottom / cm3 , " cm3"
Ydelrin_bottom=5000*mm
Sdelrin_bottom=Vdelrin_bottom/Ydelrin_bottom
print "|-- Section    = " , Sdelrin_bottom / cm2 , " cm2"
Xdelrin_bottom=58*mm
Zdelrin_bottom=Sdelrin_bottom/Xdelrin_bottom
print "|-- Z          = " , Zdelrin_bottom /cm , " cm"
Surfdelrin_bottom=Xdelrin_bottom*Ydelrin_bottom
print "\`-- Surface    = " , Surfdelrin_bottom /cm2 , " cm2"

# end
