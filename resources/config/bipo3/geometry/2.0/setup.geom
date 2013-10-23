# -*- mode: conf-unix; -*-

#############################################################
[name="detector.model" type="geomtools::simple_shaped_model"]
#@config Detector unit

#######################
# Geometry parameters #
#######################

shape_type        : string = "box"
length_unit       : string = "mm"
x                 : real   = 5000.0 mm
y                 : real   = 2500.0 mm
z                 : real   = 3000.0 mm

#######################
# Material parameters #
#######################

material.ref      : string = "snemo::lab_air"

#########################
# Visibility parameters #
#########################

visibility.hidden : boolean   = 0
visibility.color  : string    = "blue"

##################
# Internal items #
##################

internal_item.labels                    : string[1] = "shielded_module"
internal_item.model.shielded_module     : string = "shielded_bipo3_module.model"
internal_item.placement.shielded_module : string = "0 0 0 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.shielded_module     : string = "[shielded_bipo3_module.category]"


#########################################################
[name="hall.model" type="geomtools::simple_shaped_model"]

#@config Hall volume

#######################
# Geometry parameters #
#######################

shape_type        : string = "box"
length_unit       : string = "mm"
x                 : real   = 5200.0 mm
y                 : real   = 2600.0 mm
z                 : real   = 3200.0 mm

#######################
# Material parameters #
#######################

material.ref      : string = "snemo::lab_air"

#########################
# Visibility parameters #
#########################

visibility.hidden : boolean   = 0
visibility.color  : string    = "cyan"

##################
# Internal items #
##################

internal_item.labels                   : string[1] = "bipo3_detector"
internal_item.model.bipo3_detector     : string = "detector.model"
internal_item.placement.bipo3_detector : string = "0 0 0 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.bipo3_detector     : string = "[detector.category:detector=0]"


###################################################
[name="world" type="geomtools::simple_world_model"]
#@config World volume

#######################
# Geometry parameters #
#######################

setup.model     : string = "hall.model"

angle_unit      : string = "degree"
length_unit     : string = "mm"

world.x         : real   = 5400.0 mm
world.y         : real   = 2800.0 mm
world.z         : real   = 3400.0 mm

setup.x         : real   = 0.0 mm
setup.y         : real   = 0.0 mm
setup.z         : real   = 0.0 mm
setup.phi       : real   = 0.0 degree
setup.theta     : real   = 0.0 degree

#######################
# Material parameters #
#######################

material.ref    : string = "vacuum"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = 0
visibility.daughters.hidden : boolean = 0
visibility.envelop_hidden   : boolean = 0
visibility.color            : string = "grey"

# end of file
