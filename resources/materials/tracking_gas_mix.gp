m=1.0
m3=m**3
cm=1e-2*m
cm2=cm**2
cm3=cm**3
pascal=1.0
bar=100000 * pascal
mbar = 1e-3 * bar
kg = 1.0
g  = 1e-3*kg
mol=1.0
kelvin=1.0
joule=1.0
T=300.0*kelvin

M_He=4.0026 * g / mol
M_Ar=39.948 * g / mol
M_Et=46.068 * g / mol
R=8.314462 * joule / kelvin / mol

# Argon+Alcohol
P=880.0 * mbar
T=300.0*kelvin
propHelium=0.0
propEthanol=0.035
propArgon=0.965

# Helium+Alcohol+Argon (NEMO3)
# P=889.5 * mbar
# T=298.0*kelvin
# propHelium=94.6e-2
# propEthanol=4.44e-2
# propArgon=1e-2

# Helium+Alcohol+Argon
# P=880.0 * mbar
# T=300.0*kelvin
# propHelium=0.95
# propEthanol=0.04
# propArgon=0.01

P_He=P * propHelium 
P_Ar=P * propArgon 
P_Et=P * propEthanol 

d_He = P_He / R / T
d_Ar = P_Ar / R / T
d_Et = P_Et / R / T

print "d(He) = ", d_He / (mol/cm3) , " mol/cm3"
print "d(Ar) = ", d_Ar / (mol/cm3) , " mol/cm3"
print "d(Et) = ", d_Et / (mol/cm3) , " mol/cm3"

rho_He = d_He * M_He
rho_Ar = d_Ar * M_Ar
rho_Et = d_Et * M_Et
print "rho(He) = ", rho_He / (g/cm3) , " g/cm3"
print "rho(Ar) = ", rho_Ar / (g/cm3) , " g/cm3"
print "rho(Et) = ", rho_Et / (g/cm3) , " g/cm3"
rho = rho_He + rho_Ar + rho_Et
print "rho = ", rho / (g/cm3) , " g/cm3"
