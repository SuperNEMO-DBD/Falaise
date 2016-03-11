# -*- mode: conf-unix; -*-
# setup.geom


#####################
# Experimental hall #
#####################

######################################################################
[name="experimental_hall.model" type="geomtools::simple_shaped_model"]

#@config A fake experimental hall

shape_type : string = "box"
x : real as length = 10000.0 mm
y : real as length = 10000.0 mm
z : real as length = 10000.0 mm

material.ref : string = "lab_air"

visibility.hidden           : boolean = false
visibility.envelop_hidden   : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "cyan"

# List of object inside the experimental_hall:
internal_item.labels : string[7] = \
  "module_0" \
  "bottom_shield" \
  "top_shield"  \
  "back_shield"  \
  "front_shield"  \
  "left_shield"  \
  "right_shield"

# Module #0 (demonstrator):
internal_item.model.module_0          : string = "module_basic.model"
internal_item.placement.module_0      : string = "0 0 0 (mm)"

# Bottom_shield:
internal_item.model.bottom_shield     : string = "tb_iron_shield_block.model"
internal_item.placement.bottom_shield : string = "0 0 -315 (cm)"

# Top_shield:
internal_item.model.top_shield        : string = "tb_iron_shield_block.model"
internal_item.placement.top_shield    : string = "0 0 +315 (cm)"

# Back_shield:
internal_item.model.back_shield       : string = "bf_iron_shield_block.model"
internal_item.placement.back_shield   : string = "-300 0 0 (cm) / y 90 (degree)"

# Front_shield:
internal_item.model.front_shield      : string = "bf_iron_shield_block.model"
internal_item.placement.front_shield  : string = "+300 0 0 (cm) / y 90 (degree)"

# Left_shield:
internal_item.model.left_shield       : string = "lr_iron_shield_block.model"
internal_item.placement.left_shield   : string = "0 -415 0 (cm) / x 90 (degree)"

# Right_shield:
internal_item.model.right_shield      : string = "lr_iron_shield_block.model"
internal_item.placement.right_shield  : string = "0 +415 0 (cm) / x 90 (degree)"

# Daughters mapping informations:
mapping.daughter_id.module_0      : string = "[module:module=0]"
mapping.daughter_id.back_shield   : string = "[external_shield:side=0]"
mapping.daughter_id.front_shield  : string = "[external_shield:side=1]"
mapping.daughter_id.left_shield   : string = "[external_shield:side=2]"
mapping.daughter_id.right_shield  : string = "[external_shield:side=3]"
mapping.daughter_id.bottom_shield : string = "[external_shield:side=4]"
mapping.daughter_id.top_shield    : string = "[external_shield:side=5]"


################
# World volume #
################

###################################################
[name="world" type="geomtools::simple_world_model"]

#@config The top-level world volume

material.ref : string = "vacuum"

world.x      : real as length = 11000. mm
world.y      : real as length = 11000. mm
world.z      : real as length = 11000. mm

####################################
# Setup hosted in the world volume #
####################################

setup.model  : string = "experimental_hall.model"
setup.x      : real as length = 0.0 mm
setup.y      : real as length = 0.0 mm
setup.z      : real as length = 0.0 mm
setup.phi    : real as angle  = 0.0 degree
setup.theta  : real as angle  = 0.0 degree

visibility.hidden           : boolean = false
visibility.envelop_hidden   : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.setup : string = "[hall:hall=0]"


# end of setup.geom
