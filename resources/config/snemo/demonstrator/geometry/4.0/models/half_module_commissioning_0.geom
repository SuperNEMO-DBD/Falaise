# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/half_module_commissioning.geom


#############################################################################
[name="commissioning_source_gap.model" type="geomtools::simple_shaped_model"]
shape_type       : string = "box"
x                : real as length = @variant(geometry:layout/if_half_commissioning/gap|0.25 mm)
y                : real as length = 6000.0 mm
z                : real as length = 4000.0 mm
material.ref     : string = "lab_air"
visibility.color : string = "blue"

##############################################################################
[name="commissioning_source_spot.model" type="geomtools::simple_shaped_model"]
shape_type       : string = "cylinder"
r                : real as length =  1.0 mm
z                : real as length =  1.0 um
material.ref     : string = "lab_air"
visibility.color : string = "blue"


######################################################################################
[name="rotated_commissioning_source_spot.model" type="geomtools::rotated_boxed_model"]
rotated.model         : string = "commissioning_source_spot.model"
rotated.axis          : string  = "y"
rotated.special_angle : string  = "90"
material.ref          : string = "lab_air"
visibility.color      : string = "grey"


######################################################################
[name="commissioning_source_plane.model" type="geomtools::grid_model"]

#@description The label used to name the volumes
grid.daughter_label : string = "spot"

x                  : real as length =    0.5 mm
y                  : real as length = 6000.0 mm
z                  : real as length = 4000.0 mm

#@description The plane of the grid
grid.plane : string = "yz"

#@description The number of colums along the Y axis
grid.y.number_of_items : integer = 113

#@description The number of rows along the Z axis
grid.z.number_of_items : integer =   3

#@description The step length along the Y axis
grid.y.step : real as length = 44 mm

#@description The step length along the Z axis
grid.z.step : real as length = 80 cm

#@description The name of the geometry model to be placed on the grid
grid.model : string = "rotated_commissioning_source_spot.model"

material.ref       : string  = "lab_air"

visibility.color   : string  = "grey"

# Mapping informations for daughter volumes:
mapping.daughter_id.spot  : string  = "[commissioning_source_spot:column+0,row+0]"


##############################################################################
[name="half_module_closing_plate.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =    3.0 mm
y                  : real as length = 6000.0 mm
z                  : real as length = 4000.0 mm

material.ref       : string  = "basic::inox"

visibility.color   : string  = "blue"


######################################
# Half commissioning module geometry #
######################################

################################################################
# The SuperNEMO demonstrator half module closed by a metal plate:
#
#                         ^ y
#                         :
#          commissioning  : right
#          source plane   :
#		      +-++-+
#                     | ||:|         +----+
#                     | ||:|+-------+|    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |     stacking axis
# --------------------|-||z||-------||----|-------> x
#  back               | ||:||       ||    | front
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:|+-------+|    |
#                     | ||:| tracker +----+
#                     +-++-+  front   calo
#                      closing       front
#                       plate
#                         :
#                         : left
#                         :
#
########################################################################
[name="half_module_commissioning.model" type="geomtools::stacked_model"]

#@variant_section_only geometry:layout/if_half_commissioning|false

#@config Simplified demonstrator module

#@description Stacking axis
stacked.axis            : string = "x"

#@description Number of stacking volumes
stacked.number_of_items : integer = 5

# Stacked models:
stacked.model_0   : string = "commissioning_source_plane.model"
stacked.model_1   : string = "commissioning_source_gap.model"
stacked.model_2   : string = "half_module_closing_plate.model"
stacked.model_3   : string = "tracker_front_submodule.model"
stacked.model_4   : string = "calorimeter_front_submodule.model"

# Labels associated to each stacked model:
stacked.label_0   : string = "commissioning_source_plane"
stacked.label_1   : string = "commissioning_source_gap"
stacked.label_2   : string = "closing_plate"
stacked.label_3   : string = "tracker_front_submodule"
stacked.label_4   : string = "calorimeter_front_submodule"

# Surrounding material:
material.ref    : string = "lab_air"

# Visibility properties:
# visibility.daughters.hidden : boolean = false
# visibility.hidden_envelope  : boolean = false
visibility.color            : string  = "grey"

# Mapping informations for daughter volumes:
mapping.daughter_id.commissioning_source_plane  : string  = "[commissioning_source_plane]"
mapping.daughter_id.closing_plate               : string  = "[source_like_plate]"
mapping.daughter_id.tracker_front_submodule     : string  = "[tracker_submodule:     side=1]"
mapping.daughter_id.calorimeter_front_submodule : string  = "[calorimeter_submodule: side=1]"


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/half_module_commissioning.model.geom
