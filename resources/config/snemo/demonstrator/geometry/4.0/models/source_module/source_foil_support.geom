# -*- mode: conf-unix; -*-
# source_foil_support.geom

#############################################
# Approximation of the total mass of Delrin #
# for top/bottom source support (roller...) #
#############################################


############################################################################
[name="source_foil_top_gas_gap.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =   58.0 mm
y                  : real as length = 5000.0 mm
z                  : real as length =   20.5 mm

material.ref       : string  = "tracking_gas"

visibility.hidden  : boolean = false


############################################################################
[name="source_foil_top_support.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =   58.0 mm
y                  : real as length = 5000.0 mm
z                  : real as length =    9.5 mm

material.ref       : string  = "basic::delrin"

visibility.hidden  : boolean = false
visibility.color   : string  = "green"


###############################################################################
[name="source_foil_bottom_support.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =   58.0 mm
y                  : real as length = 5000.0 mm
z                  : real as length =   12.0 mm

material.ref       : string  = "basic::delrin"

visibility.hidden  : boolean = false
visibility.color   : string  = "green"


###############################################################################
[name="source_foil_bottom_gas_gap.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =   58.0 mm
y                  : real as length = 5000.0 mm
z                  : real as length =   18.0 mm

material.ref       : string  = "tracking_gas"

visibility.hidden  : boolean = false

# end of source_foil_support.geom
