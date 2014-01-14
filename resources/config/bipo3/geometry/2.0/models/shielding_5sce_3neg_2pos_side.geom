# -*- mode: conf-unix; -*-

### BiPo3 module in its shielding ###

# +-------------------------------------------------------+
# |    shielded_bipo3_module = surrounding air volume     |
# | +---------------------------------------------------+ |
# | |                     water_tank                    | |
# | | +-----------------------------------------------+ | |
# | | |                   inox_tank                   | | |
# | | | +-------------------------------------------+ | | |
# | | | |                lead_shield                | | | |
# | | | | +---------------------------------------+ | | | |
# | | | | |   nitrogen_gap + calibration_sources  | | | | |
# | | | | |  +---------------------------------+  | | | | |
# | | | | |  |            iron_tank            |  | | | | |
# | | | | |  |     +---------------------+     |  | | | | |
# | | | | |  |     | nitrogen_inner_gap  |     |  | | | | |
# | | | | |  |     |  +--------------+   |     |  | | | | |
# | | | | |  |     |  | bipo3_module |   |     |  | | | | |
# | | | | |  |     |  +--------------+   |     |  | | | | |
# +-+-+-+-+--+-----+---------------------+-----+--+-+-+-+-+

[name="nitrogen_inner_gap.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real = 3120.0 # mm
y : real = 660.0 # mm
z : real = 918.0 # mm

logging.priority : string = "warning"
material.ref               : string  = "snemo::nitrogen"

visibility.hidden          : boolean = 0
visibility.color           : string  = "green"

internal_item.labels : string[1]  = "bipo3_module"
internal_item.model.bipo3_module     : string    = "bipo3_module.model"
internal_item.placement.bipo3_module : string    = "0 0 0 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.bipo3_module     : string    = "[bipo3_module.category]"

#############################################################################

[name="iron_tank.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real = 3124.0 # mm
y : real = 664.0 # mm
z : real = 922.0 # mm

logging.priority : string = "warning"
material.ref               : string  = "std::iron"

visibility.hidden          : boolean = 0
visibility.color           : string  = "cyan"

internal_item.labels : string[1]  = "nitrogen_inner_gap"
internal_item.model.nitrogen_inner_gap     : string    = "nitrogen_inner_gap.model"
internal_item.placement.nitrogen_inner_gap : string    = "0 0 0 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.nitrogen_inner_gap     : string    = "[nitrogen_inner_gap.category]"

#############################################################################

[name="nitrogen_gap.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real = 3440.0 # mm
y : real = 800.0 # mm
z : real = 998.0 # mm

logging.priority : string = "warning"
material.ref               : string  = "snemo::nitrogen"

visibility.hidden          : boolean = 0
visibility.color           : string  = "red"

internal_item.labels : string[6]  = "iron_tank" \
		       		    "calibration_source0"\
		       		    "calibration_source1"\
		       		    "calibration_source2"\
		       		    "calibration_source3"\
		       		    "calibration_source4"

internal_item.model.iron_tank     : string    = "iron_tank.model"
internal_item.placement.iron_tank : string    = "0 0 -38 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.iron_tank     : string    = "[iron_tank.category]"

#####################
#Calibration Sources#
#####################

# The (x,y) plan at z = -38 is the scintillators plan
# which is used as a reference position for the calibration sources

##################
#Reference source#
##################

# This source is in the center of the detector along z and x
# It is placed along the lead shielding in y
internal_item.model.calibration_source0     : string    = "calibration_source.model"
internal_item.placement.calibration_source0 : string    = "0 -399 -38 (mm) / x 90 (deg)"
mapping.daughter_id.calibration_source0     : string    = "[calibration_source.category:source=0]"

###############
#Other sources#
###############

internal_item.model.calibration_source1     : string    = "calibration_source.model"
internal_item.placement.calibration_source1 : string    = "602 399 -38 (mm) / x 90 (deg)"
mapping.daughter_id.calibration_source1     : string    = "[calibration_source.category:source=1]"

internal_item.model.calibration_source2     : string    = "calibration_source.model"
internal_item.placement.calibration_source2 : string    = "1204 -399 -38 (mm) / x 90 (deg)"
mapping.daughter_id.calibration_source2     : string    = "[calibration_source.category:source=2]"

internal_item.model.calibration_source3     : string    = "calibration_source.model"
internal_item.placement.calibration_source3 : string    = "-602 399 -38 (mm) / x 90 (deg)"
mapping.daughter_id.calibration_source3     : string    = "[calibration_source.category:source=3]"

internal_item.model.calibration_source4     : string    = "calibration_source.model"
internal_item.placement.calibration_source4 : string    = "-1204 -399 -38 (mm) / x 90 (deg)"
mapping.daughter_id.calibration_source4     : string    = "[calibration_source.category:source=4]"

#############################################################################

# Iron plate on top of the shielding
[name="iron_plate.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real = 3640.0 # mm
y : real = 1000.0 # mm
z : real = 20.0 # mm

logging.priority : string = "warning"
material.ref               : string  = "std::iron"

visibility.hidden          : boolean = 0
visibility.color           : string  = "cyan"

#############################################################################

[name="lead_shield.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real = 3640.0 # mm
y : real = 1000.0 # mm
z : real = 1318.0 # mm

logging.priority : string = "warning"
material.ref               : string  = "std::lead"

visibility.hidden          : boolean = 0
visibility.color           : string  = "cyan"

internal_item.labels : string[2]  = "nitrogen_gap" \
		       		     "iron_plate"

internal_item.model.nitrogen_gap     : string    = "nitrogen_gap.model"
internal_item.placement.nitrogen_gap : string    = "0 0 -10 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.nitrogen_gap     : string    = "[nitrogen_gap.category]"

internal_item.model.iron_plate     : string    = "iron_plate.model"
internal_item.placement.iron_plate : string    = "0 0 499 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.iron_plate     : string    = "[iron_plate.category]"


#############################################################################

[name="inox_tank.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real = 3644.0 # mm
y : real = 1004.0 # mm
z : real = 1150.0 # mm

logging.priority : string = "warning"
material.ref               : string  = "std::inox"

visibility.hidden          : boolean = 0
visibility.color           : string  = "black"

internal_item.labels : string[1]  = "lead_shield"
internal_item.model.lead_shield     : string    = "lead_shield.model"
internal_item.placement.lead_shield : string    = "0 0 86 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.lead_shield     : string    = "[lead_shield.category]"

#############################################################################

[name="water_tank.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real = 4640.0 # mm
y : real = 2000.0 # mm
z : real = 1320.0 # mm

logging.priority : string = "warning"
material.ref : string  = "std::water"
visibility.hidden          : boolean = 0
visibility.color           : string  = "blue"

internal_item.labels : string[1]  = "inox_tank"
internal_item.model.inox_tank     : string    = "inox_tank.model"
internal_item.placement.inox_tank : string    = "0 0 -85 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.inox_tank     : string    = "[inox_tank.category]"

#############################################################################

# Last Air Volume around the detector
[name="shielded_bipo3_module.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real = 4700.0 # mm
y : real = 2100.0 # mm
z : real = 1800.0 # mm

logging.priority : string = "warning"
material.ref : string  = "snemo::lab_air"


internal_item.labels : string[1]  =  "water_tank"

internal_item.model.water_tank     : string    = "water_tank.model"
internal_item.placement.water_tank : string    = "0 0 0 (mm) @ 0 0 0 (deg)"
mapping.daughter_id.water_tank     : string    = "[water_tank.category]"

visibility.hidden          : boolean = 0
visibility.color           : string  = "gray"

# end
