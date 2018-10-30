#
# See DocDB-3212

# Units:
mm=1.0
cm=10*mm
cm2=cm**2
cm3=cm**3
m=100*cm
um=1e-3*mm
um3=um**3
kg=1.0
g=1e-3*kg
ug=1e-6*g
g_cm3=g/cm3
g_cm2=g/cm2
mg_cm2=1e-3*g/cm2

pad_height   = 335.3 * mm
pad_width    = 133.5 * mm
pad_thickness = 276.0 * um
# pad_thickness = 280.0 * um
edge_pad_width = 123 * mm
nb_strips = 34
nb_edge_strips = 0

strip_height = 2700 * mm - 7 * 2.5 * mm
foil_width  = nb_strips * pad_width + nb_edge_strips * edge_pad_width
foil_surface = strip_height * foil_width
foil_volume = foil_surface * pad_thickness
print "Strip height (mm)  = " , strip_height / mm
print "Foil width  (mm)  = " , foil_width / mm
print "Foil surface (cm2)  = " , foil_surface / cm2
print "Foil volume (cm3) = " , foil_volume / cm3

se_mass_fraction = 0.9
pva_mass_fraction = 1 - se_mass_fraction
se_mass = 7 * kg
pva_mass = se_mass * pva_mass_fraction / se_mass_fraction
print "Se mass (g)  = " , se_mass / g
print "PVA mass (g) = " , pva_mass / g

foil_mass = se_mass + pva_mass
print "Foil mass (g) = " , foil_mass / g
foil_density = foil_mass / foil_volume
print "Foil density (g/cm3) = " , foil_density / g_cm3
se_density = foil_density * se_mass_fraction
print "Se density (g/cm3) = " , se_density / g_cm3
se_surface_density = se_density * pad_thickness
print "Se surface density (g/cm2) = " , se_surface_density / g_cm2


#foil_rho_dx =
exit

foil_surface = strip_height * (34 * pad_width ) #+ 2 * pad_width2)
# foil_thickness=276*um
# print "Foil thickness (um) = " , foil_thickness /um

pad_thickness = 276.0 * um
foil_surface_density =
#pad_density   = 2.13 * g_cm3
pad_density   = 2.2 * g_cm3
foil_density  = pad_density * pad_thickness
print "Foil density (mg/cm2) = " , foil_density / mg_cm2

foil_volume = foil_surface * pad_thickness
print "Pad thickness (um)  = " , pad_thickness / um
print "Foil surface  (cm2) = " , foil_surface / cm2
print "Foil volume   (cm3) = " , foil_volume / cm3
foil_mass = pad_density * foil_volume
print "Foil mass     (g)   = " , foil_mass / g
se_mass = foil_mass * se_mass_fraction
print "Se mass       (g)   = " , se_mass / g
print ""
print ""
pad_cell_volume = pad_thickness**3

se_density=4.26 * g / cm3
pva_density=1.25 * g / cm3
se_radius=0.5*pad_thickness
se_volume=4*pi*se_radius**3/3
pva_volume = pad_cell_volume - se_volume
se_volume_fraction=se_volume/pad_cell_volume
pva_volume_fraction=pva_volume/pad_cell_volume

print "Cubic:"
print "  Se volume fraction (%)  = " , se_volume_fraction*100
print "  PVA volume fraction (%) = " , pva_volume_fraction*100


print "Packing:"
# https://en.wikipedia.org/wiki/Close-packing_of_equal_spheres
# https://en.wikipedia.org/wiki/Sphere_packing
# https://en.wikipedia.org/wiki/Random_close_pack
f=se_volume_fraction
print "  f (%) = " , f*100
fp=f*(1+(1-f))
print "  fp (%) = " , fp*100
#fp=0.70137155
fp=0.725345
print "  fp (%) = " , fp*100

se_volume=pad_cell_volume*fp
pva_volume = pad_cell_volume - se_volume

se_mass=se_density*se_volume
pva_mass=pva_density*pva_volume
pad_mass=se_mass+pva_mass
se_mass_fraction=se_mass/pad_mass
pva_mass_fraction=pva_mass/pad_mass

print "Se mass fraction (%) = " , se_mass_fraction*100
print "PVA mass fraction (%) = " , pva_mass_fraction*100

pad_cell_total_mass=pva_mass+se_mass
# pad_density=pad_cell_total_mass/pad_cell_volume
# print "Pad density (g/cm3) = " , pad_density / g_cm3

print ""
print "############################################################"
print "Se grain with PVA crust model: "
se_grain_radius = 5 * um
se_grain_volume = 4 * pi * se_grain_radius**3 / 3
print "  Se grain volume (um3) = " , se_grain_volume / um3
se_grain_mass = se_density * se_grain_volume
print "  Se grain mass (ug) = " , se_grain_mass / ug

pva_crust_mass=se_grain_mass*(1 - se_mass_fraction) / se_mass_fraction
print "  PVA crust mass (ug) = " , pva_crust_mass / ug
pva_crust_volume = pva_crust_mass / pva_density
print "  PVA crust volume (um3) = " , pva_crust_volume / um3

crusted_grain_volume=se_grain_volume+pva_crust_volume
print "  Crusted grain volume (um3) = " , crusted_grain_volume / um3
crusted_grain_radius = (3 * crusted_grain_volume/(4*pi))**(1./3)
print "  Crusted grain radius (um) = " , crusted_grain_radius / um
pva_crust_thickness=crusted_grain_radius - se_grain_radius
print "  PVA crust thickness (um) = " , pva_crust_thickness / um

pva_volume_fraction=pva_crust_volume/crusted_grain_volume
se_volume_fraction=se_grain_volume/crusted_grain_volume
print "  Se volume fraction (%)  = " , se_volume_fraction * 100
print "  PVA volume fraction (%) = " , pva_volume_fraction * 100

mass_Se=7.0*kg
foil_surface=strip_height*(34*pad_width+2*pad_width2)
se_thickness_equiv=mass_Se/(se_density*foil_surface)
pva_thickness_equiv=se_thickness_equiv*(pva_volume_fraction/se_volume_fraction)
print "Se thickness equiv (um)  = " , se_thickness_equiv / um
print "PVA thickness equiv (um) = " , pva_thickness_equiv / um
foil_thickness_equiv=se_thickness_equiv+pva_thickness_equiv
print "Foil thickness equiv (um) = ", foil_thickness_equiv / um

# Maximum random pack density (spherical objects):
pack_density=0.635
#pack_density=0.60
pack_density=0.5236

interstitial_thickness_equiv=foil_thickness_equiv * (1 - pack_density) / pack_density
print "Interstitial thickness equiv (um) = ", interstitial_thickness_equiv / um
film_thickness_equiv=foil_thickness_equiv+interstitial_thickness_equiv
print "Film thickness equiv (um) = ", film_thickness_equiv / um

foil_se_mass=foil_surface*se_thickness_equiv*se_density
print "  Foil Se mass (g) = " , foil_se_mass / g
foil_pva_mass=foil_surface*pva_thickness_equiv*pva_density
print "  Foil PVA mass (g) = " , foil_pva_mass / g
foil_mass=foil_se_mass+foil_pva_mass
print "  Foil mass (g) = " , foil_mass / g

# print "######"
# B = se_grain_mass * ((1.0 - se_mass_fraction) / (se_mass_fraction)) * (3 / (pva_density * 4 * pi))
# Rc3 = B + se_grain_radius**3
# Rc = Rc3**(1./3)
# pva_crust_thickness = Rc - se_grain_radius
# print "  PVA crust thickness (um) = " , pva_crust_thickness / um
# pva_crust_mass = pva_density * 4 * pi * ( (se_grain_radius + pva_crust_thickness)**3 - se_grain_radius**3)/ 3
# print "  PVA crust mass (ug) = " , pva_crust_mass / ug
# print "Se grain mass fraction: " , se_grain_mass / (se_grain_mass + pva_crust_mass)

# end
