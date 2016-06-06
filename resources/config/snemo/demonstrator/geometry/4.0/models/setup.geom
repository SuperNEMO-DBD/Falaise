# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/setup.geom

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

# visibility.hidden_envelope  : boolean = false
# visibility.daughters.hidden : boolean = false
visibility.color            : string  = "cyan"

# # List of object inside the experimental_hall:
# internal_item.labels : string[1] = "dummy0"

# # Dummy volumes:
# internal_item.model.dummy0     : string = "source_submodule.model"
# internal_item.placement.dummy0 : string = "0 0 0 (mm)"

# # List of object inside the experimental_hall:
# internal_item.labels : string[4] = "dummy0" "dummy1" "dummy2" "dummy3"

# # Dummy volumes:
# internal_item.model.dummy0     : string = "source_yneg_vertical_beam.model"
# internal_item.placement.dummy0 : string = "0 -2685 0 (mm)"
# internal_item.model.dummy1     : string = "source_ypos_vertical_beam.model"
# internal_item.placement.dummy1 : string = "0 +2685 0 (mm)"
# internal_item.model.dummy2     : string = "source_zneg_horizontal_beam.model"
# internal_item.placement.dummy2 : string = "0 0 -1735 (mm)"
# internal_item.model.dummy3     : string = "source_zpos_horizontal_beam.model"
# internal_item.placement.dummy3 : string = "0 0 +1735 (mm)"

# List of object inside the experimental_hall:
#@description The list of daughter volumes
#@variant_only geometry:shield/if_no_shield|true
internal_item.labels : string[1] = "module_0"

# #@variant_only geometry:shield/if_shield|false
# internal_item.labels : string[7] = \
#   "module_0" \
#   "bottom_shield" \
#   "top_shield"  \
#   "back_shield"  \
#   "front_shield"  \
#   "left_shield"  \
#   "right_shield"

# Module #0 (demonstrator):
#@variant_only geometry:layout/if_basic|true
internal_item.model.module_0          : string = "module_basic.model"
#@variant_only geometry:layout/if_half_commissioning|false
internal_item.model.module_0          : string = "half_module_commissioning.model"
internal_item.placement.module_0      : string = "0 0 0 (mm)"

# # Bottom_shield:
# internal_item.model.bottom_shield     : string = "tb_iron_shield_block.model"
# internal_item.placement.bottom_shield : string = "0 0 -315 (cm)"

# # Top_shield:
# internal_item.model.top_shield        : string = "tb_iron_shield_block.model"
# internal_item.placement.top_shield    : string = "0 0 +315 (cm)"

# # Back_shield:
# internal_item.model.back_shield       : string = "bf_iron_shield_block.model"
# internal_item.placement.back_shield   : string = "-300 0 0 (cm) / y 90 (degree)"

# # Front_shield:
# internal_item.model.front_shield      : string = "bf_iron_shield_block.model"
# internal_item.placement.front_shield  : string = "+300 0 0 (cm) / y 90 (degree)"

# # Left_shield:
# internal_item.model.left_shield       : string = "lr_iron_shield_block.model"
# internal_item.placement.left_shield   : string = "0 -415 0 (cm) / x 90 (degree)"

# # Right_shield:
# internal_item.model.right_shield      : string = "lr_iron_shield_block.model"
# internal_item.placement.right_shield  : string = "0 +415 0 (cm) / x 90 (degree)"

# Daughters mapping informations:
mapping.daughter_id.module_0      : string = "[module:module=0]"
# mapping.daughter_id.back_shield   : string = "[external_shield:side=0]"
# mapping.daughter_id.front_shield  : string = "[external_shield:side=1]"
# mapping.daughter_id.left_shield   : string = "[external_shield:side=2]"
# mapping.daughter_id.right_shield  : string = "[external_shield:side=3]"
# mapping.daughter_id.bottom_shield : string = "[external_shield:side=4]"
# mapping.daughter_id.top_shield    : string = "[external_shield:side=5]"


################
# World volume #
################

###################################################
[name="world" type="geomtools::simple_world_model"]

#@config The top-level world volume

material.ref : string = "vacuum"

world.x      : real as length = 11000.0 mm
world.y      : real as length = 11000.0 mm
world.z      : real as length = 11000.0 mm

####################################
# Setup hosted in the world volume #
####################################

setup.model  : string = "experimental_hall.model"
setup.x      : real as length = 0.0 mm
setup.y      : real as length = 0.0 mm
setup.z      : real as length = 0.0 mm
setup.phi    : real as angle  = 0.0 degree
setup.theta  : real as angle  = 0.0 degree

# visibility.hidden_envelope  : boolean = false
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.setup : string = "[hall:hall=0]"


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/setup.geom
