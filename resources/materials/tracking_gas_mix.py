#!/usr/bin/python3

# Units
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
percent=1e-2

# Constants:
R=8.314462 * joule / kelvin / mol
M_He=4.0026 * g / mol
M_Ar=39.948 * g / mol
M_Et=46.068 * g / mol
M_H=1.00794 * g / mol
M_C=12.0107 * g / mol
M_O=15.9994 * g / mol
M_N=14.0067 * g / mol
M_Air=28.965  * g / mol
M_CO2=44.0095  * g / mol


print("Air composition:")
pAirN2 = 78.08e-2
pAirO2 = 20.95e-2
pAirAr = 0.93e-2
pAirCO2 = 0.04e-2
print(f"- N2 = {pAirN2 / percent:.7} %")
print(f"- O2 = {pAirO2 / percent:.7} %")
print(f"- Ar = {pAirAr / percent:.7} %")
print(f"- CO2 = {pAirCO2 / percent:.7} %")

print(f"M_Air = {M_Air / (g/mol):.5} g/mol")
print(f"M_CO2 = {M_CO2 / (g/mol):.5} g/mol")
# M_Air_2 = pAirN2 * M_N * 2 + pAirO2 * M_O * 2 + pAirAr * M_Ar + pAirCO2 * M_CO2
# print(f"M_Air = {M_Air_2 / (g/mol):.5} g/mol (check)")
# M_CO2_2 = M_C + M_O * 2
# print(f"M_CO2 = {M_CO2_2 / (g/mol):.5} g/mol (check)")

# Various tests:
# Argon+Alcohol
# P=880.0 * mbar
# T=300.0*kelvin
# propHelium=0.0
# propEthanol=0.035
# propArgon=0.965

# Helium+Alcohol+Argon
# P=880.0 * mbar
# T=300.0*kelvin
# propHelium=0.95
# propEthanol=0.04
# propArgon=0.01

# # Helium+Alcohol+Argon (NEMO3)
# P=889.5 * mbar
# T=298.0*kelvin
# propHelium=94.6e-2
# propEthanol=4.44e-2
# propArgon=1e-2

# # Helium+Alcohol+Argon (helium_mix SuperNEMO tracking gas)
# P=880.0 * mbar
# T=300.0*kelvin
# propAir=0.0
# propNotAir=1.0-propAir
# propHelium=95e-2*propNotAir
# propEthanol=4e-2*propNotAir
# propArgon=1e-2*propNotAir

# Pure helium:
# P=1013.25 * mbar
# T=273.15*kelvin
# propAir=0.0
# propNotAir=1.0-propAir
# propHelium=1*propNotAir
# propEthanol=0*propNotAir
# propArgon=0*propNotAir

# Helium+Alcohol+Argon+Air (SuperNEMO tracking gas, FM+MG+EC, 2025-12-15)
# [name="snemo::tracking_gas::default" type="material"]
P=880.0 * mbar
T=293.0*kelvin
propAir=0.01
propNotAir=1.0-propAir
propHelium=94.6e-2*propNotAir
propEthanol=4.44e-2*propNotAir
propArgon=0.96e-2*propNotAir

# Helium+Alcohol+Argon+Air (SuperNEMO tracking gas, FM+MG+EC, 2025-12-16)
# [name="snemo::tracking_gas::default" type="material"]
# P=880.0 * mbar
# T=293.0*kelvin
# propAir=0.01
# propNotAir=1.0-propAir
# propHelium=95e-2*propNotAir
# propEthanol=4e-2*propNotAir
# propArgon=1e-2*propNotAir

print("Temperature T     = ", T / kelvin , " K")
print("Pressure P        = ", P / mbar , " mbar")
print("Proportion of gas components:")
print(f"- Proportion He     = {propHelium / percent:.7f} %")
print(f"- Proportion Ar     = {propArgon / percent:.7f} %")
print(f"- Proportion C2H5OH = {propEthanol / percent:.7f} %")
print(f"- Proportion Air    = {propAir / percent:.7f} %")

print("Partial pressure of gas components:")
P_He = P * propHelium
P_Ar = P * propArgon
P_Et = P * propEthanol
P_Air = P * propAir
print(f"- P(He)  = {P_He / mbar:.7f} mbar")
print(f"- P(Ar)  = {P_Ar / mbar:.7f} mbar")
print(f"- P(Et)  = {P_Et / mbar:.7f} mbar")
print(f"- P(Air) = {P_Air / mbar:.7f} mbar")

P_tot = P_He + P_Ar + P_Et + P_Air
print(f"Check P_tot = {P_tot / mbar:.5f} mbar")

fmHe = (propHelium * 1)* M_He
fmAr = (propArgon * 1 + propAir * pAirAr * 1) * M_Ar 
fmH  = (propEthanol * 6) * M_H
fmC  = (propEthanol * 2 + propAir * pAirCO2 *1) * M_C
fmO  = (propEthanol * 1 + propAir *(pAirO2 * 2 + pAirCO2 * 2)) * M_O
fmN  = (propAir * pAirN2 * 2) * M_N
fmTot = fmHe+fmAr+fmH+fmC+fmO+fmN

fmHe=fmHe/fmTot
fmAr=fmAr/fmTot
fmH=fmH/fmTot
fmC=fmC/fmTot
fmO=fmO/fmTot
fmN=fmN/fmTot
print("Mass fractions per element:")
print(f"- fmH  = {fmH:.7}")
print(f"- fmHe = {fmHe:.7}")
print(f"- fmC  = {fmC:.7}")
print(f"- fmO  = {fmO:.7}")
print(f"- fmAr = {fmAr:.7}")
print(f"- fmN  = {fmN:.7}")
fmTot = fmHe+fmAr+fmH+fmC+fmO+fmN
print(f"Check sum = {fmTot}")
# print("check Mathis= ", 0.578887250000 + 0.041054250000 + 0.163080800000 +  0.120217300000 + 0.059060400000 + 0.037700000000)

# density par gas component:
d_He = P_He / R / T
d_Ar = P_Ar / R / T
d_Et = P_Et / R / T
d_Air = P_Air / R / T

print("Molar density per gas component:")
print(f"- d(He)  = {d_He / (mol/cm3):.7} mol/cm3")
print(f"- d(Ar)  = {d_Ar / (mol/cm3):.7} mol/cm3")
print(f"- d(Et)  = {d_Et / (mol/cm3):.7} mol/cm3")
print(f"- d(Air) = {d_Air / (mol/cm3):.7} mol/cm3")

print("Mass density per gas component:")
rho_He = d_He * M_He
rho_Ar = d_Ar * M_Ar
rho_Et = d_Et * M_Et
rho_Air = d_Air * M_Air
print(f"-rho(He)  = {rho_He / (g/cm3):.7} g/cm3")
print(f"-rho(Ar)  = {rho_Ar / (g/cm3):.7} g/cm3")
print(f"-rho(Et)  = {rho_Et / (g/cm3):.7} g/cm3")
print(f"-rho(Air) = {rho_Air / (g/cm3):.7} g/cm3")
rho = rho_He + rho_Ar + rho_Et + rho_Air
print("Mass density of the tracking gas:")
print(f"rho = {rho / (g/cm3):.7} g/cm3")
