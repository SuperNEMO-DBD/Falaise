# -*- mode: conf-unix; -*-
# shielding.geom


######################
# Water Shield Walls #
######################

# ######################################################################
# [name="water_shield_block.model" type="geomtools::simple_boxed_model"]
#
# shape_type : string = "box"
# x : real as length =  50.0 cm
# y : real as length = 700.0 cm
# z : real as length = 400.0 cm
#
# material.ref : string = "snemo::water_with_borax"
#
# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
# visibility.daughters.hidden : boolean = false
# visibility.color            : string  = "blue"


######################
# Iron Shield Walls  #
######################

#########################################################################
[name="bf_iron_shield_block.model" type="geomtools::simple_shaped_model"]

#@config Back-Front shielding wall

shape_type : string = "box"
x : real as length = 6460.0 mm
y : real as length = 8870.0 mm
z : real as length =  200.0 mm

material.ref : string = "basic::iron"

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "black"


########################################################################
[name="lr_iron_shield_block.model" type="geomtools::simple_boxed_model"]

#@config Left-Right shielding wall

shape_type : string = "box"
x : real as length = 3070.0 mm
y : real as length = 6460.0 mm
z : real as length =  200.0 mm

material.ref : string = "basic::iron"

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "black"


############################################################################
[name="bottom_iron_shield_block.model" type="geomtools::simple_boxed_model"]

#@config Bottom shielding wall

shape_type : string = "box"
x : real as length = 3070.0 mm
y : real as length = 8470.0 mm
z : real as length =  200.0 mm

material.ref : string = "basic::iron"

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "black"


#########################################################################
[name="top_iron_shield_block.model" type="geomtools::simple_boxed_model"]

#@config Top-Bottom shielding wall

shape_type : string = "box"
x : real as length = 3470.0 mm
y : real as length = 8870.0 mm
z : real as length =  200.0 mm

material.ref : string = "basic::iron"

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "black"


# end of shielding.geom
