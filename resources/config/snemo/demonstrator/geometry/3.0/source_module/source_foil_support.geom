# -*- mode: conf-unix; -*-

#############################################
# Approximation of the total mass of Delrin #
# for top/bottom source support (roller...) #
#############################################


############################################################################
[name="source_foil_top_gas_gap.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
x                  : real    =   58.0  # mm
y                  : real    = 5000.0  # mm
z                  : real    =   20.5 # mm
length_unit        : string  = "mm"
material.ref       : string  = "tracking_gas"
visibility.hidden  : boolean = 0


############################################################################
[name="source_foil_top_support.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
x                  : real    =   58.0  # mm
y                  : real    = 5000.0  # mm
z                  : real    =    9.5 # mm
length_unit        : string  = "mm"
material.ref       : string  = "std::delrin"
visibility.hidden  : boolean = 0
visibility.color   : string  = "green"


###############################################################################
[name="source_foil_bottom_support.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
x                  : real    =   58.0  # mm
y                  : real    = 5000.0  # mm
z                  : real    =   12.0 # mm
length_unit        : string  = "mm"
material.ref       : string  = "std::delrin"
visibility.hidden  : boolean = 0
visibility.color   : string  = "green"


###############################################################################
[name="source_foil_bottom_gas_gap.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
x                  : real    =   58.0  # mm
y                  : real    = 5000.0  # mm
z                  : real    =   18.0 # mm
length_unit        : string  = "mm"
material.ref       : string  = "tracking_gas"
visibility.hidden  : boolean = 0

# end
