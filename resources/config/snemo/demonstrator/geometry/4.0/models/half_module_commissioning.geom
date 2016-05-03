# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/half_module_commissioning.geom


##############################################################################
[name="half_module_closing_plate.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =    3.0 mm
y                  : real as length = 6000.0 mm
z                  : real as length = 4000.0 mm

material.ref       : string  = "basic::inox"

visibility.hidden  : boolean = false
visibility.color   : string  = "blue"


######################################
# Half commissioning module geometry #
######################################

################################################################
# The SuperNEMO demonstrator half module closed by a metal plate:
#
#                         ^ y
#                         :
#                         : right
#                         :
#                        +-+
#                        |:|         +----+
#                        |:|+-------+|    |
#                        |:||       ||    |
#                        |:||       ||    |
#                        |:||       ||    |
#                        |:||       ||    |
#                        |:||       ||    |
#                        |:||       ||    |     stacking axis
# -----------------------|z||-------||----|-------> x
#  back                  |:||       ||    | front
#                        |:||       ||    |
#                        |:||       ||    |
#                        |:||       ||    |
#                        |:||       ||    |
#                        |:||       ||    |
#                        |:|+-------+|    |
#                        |:| tracker +----+
#                        +-+          calo
#                       plate
#                         :
#                         : left
#                         :
#
########################################################################
[name="half_module_commissioning.model" type="geomtools::stacked_model"]

#@config Simplified demonstrator module

#@description Stacking axis
stacked.axis            : string = "x"

#@description Number of stacking volumes
stacked.number_of_items : integer = 3

# Stacked models:
stacked.model_0   : string = "half_module_closing_plate.model"
stacked.model_1   : string = "tracker_front_submodule.model"
stacked.model_2   : string = "calorimeter_front_submodule.model"

# Labels associated to each stacked model:
stacked.label_0   : string = "closing_plate"
stacked.label_1   : string = "tracker_front_submodule"
stacked.label_2   : string = "calorimeter_front_submodule"

# Surrounding material:
material.ref    : string = "lab_air"

# Visibility properties:
visibility.hidden           : boolean = false
visibility.daughters.hidden : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.color            : string  = "grey"

# Mapping informations for daughter volumes:
mapping.daughter_id.closing_plate               : string  = "[source_like_plate]"
mapping.daughter_id.tracker_front_submodule     : string  = "[tracker_submodule:     side=1]"
mapping.daughter_id.calorimeter_front_submodule : string  = "[calorimeter_submodule: side=1]"


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/half_module_commissioning.model.geom
