# -*- mode: conf-unix; -*-
# shielding.geom


#################
# Shield blocks #
#################

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
# visibility.envelop_hidden   : boolean = false
# visibility.daughters.hidden : boolean = false
# visibility.color            : string  = "blue"


#########################################################################
[name="bf_iron_shield_block.model" type="geomtools::simple_shaped_model"]

#@config Back-Front shielding wall

shape_type : string = "box"
x : real as length = 600.0 cm
y : real as length = 800.0 cm
z : real as length =  20.0 cm

material.ref : string = "basic::iron"

visibility.hidden           : boolean = false
visibility.envelop_hidden   : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "black"


########################################################################
[name="lr_iron_shield_block.model" type="geomtools::simple_boxed_model"]

#@config Left-Right shielding wall

shape_type : string = "box"
x : real as length = 600.0 cm
y : real as length = 600.0 cm
z : real as length =  20.0 cm

material.ref : string = "basic::iron"

visibility.hidden           : boolean = false
visibility.envelop_hidden   : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "black"


########################################################################
[name="tb_iron_shield_block.model" type="geomtools::simple_boxed_model"]

#@config Top-Bottom shielding wall

shape_type : string = "box"
x : real as length = 650.0 cm
y : real as length = 800.0 cm
z : real as length =  20.0 cm

material.ref : string = "basic::iron"

visibility.hidden           : boolean = false
visibility.envelop_hidden   : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "black"


# end of shielding.geom
