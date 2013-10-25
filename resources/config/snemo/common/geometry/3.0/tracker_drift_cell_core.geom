# -*- mode: conf-unix; -*-
# tracker_drift_cell_core.geom

#@description The geometry models that compose the core volume of the drift cells (without field wires)
#@key_label   "name"
#@meta_label  "type"

# Reference: Document DocDB 1273-v1 Y.Shitov

##################################################################
[name="anode_bus_bar.model" type="geomtools::simple_shaped_model"]

#@config Geometric description of a drift cell anode bus bar

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit        : string = "mm"

#@description The name of the volume shape
shape_type         : string = "cylinder"

#@description The radius of the cylinder
r                  : real as length =  1.32 mm

#@description The length of the cylinder
z                  : real as length = 82.0  mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref       : string = "std::copper"

#########################
# Visibility parameters #
#########################

visibility.hidden  : boolean = 0
visibility.color   : string  = "red"


###############################################################
[name="anode_wire.model" type="geomtools::simple_shaped_model"]

#@config Geometric description of a drift cell anode wire

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit               : string = "mm"

#@description The name of the volume shape
shape_type                : string = "cylinder"

#@description The diameter of the wire
diameter                  : real as length =   40.0 um

#@description The length of the wire
z                         : real as length = 2866.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref              : string = "wire_metal"

#########################
# Visibility parameters #
#########################

visibility.hidden         : boolean = 1
visibility.wired_cylinder : boolean = 1
visibility.color          : string  = "blue"


####################################################################
[name="drift_cell_base.model" type="geomtools::simple_shaped_model"]

#@config Geometric description of a drift cell base

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit               : string = "mm"

#@description The name of the volume shape
shape_type                : string = "tube"

#@description The inner radius of the tube
inner_r                   : real as length = 10.0 mm

#@description The outer radius of the tube
outer_r                   : real as length = 14.5 mm

#@description The length of the tube
z                         : real as length = 74.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref              : string = "std::delrin"

#########################
# Visibility parameters #
#########################

visibility.hidden         : boolean = 0
visibility.wired_cylinder : boolean = 0
visibility.color          : string  = "orange"


####################################################################
[name="cathode_bus_bar.model" type="geomtools::simple_shaped_model"]

# @config Geometric description of drift cell cathode bar:

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit               : string = "mm"

#@description The name of the volume shape
shape_type                : string = "tube"

#@description The outer radius of the tube
outer_r                   : real as length = 21.0 mm

#@description The inner radius of the tube
inner_r                   : real as length = 19.5 mm

#@description The length of the tube
z                         : real as length = 10.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref              : string = "std::copper"

#########################
# Visibility parameters #
#########################

visibility.hidden         : boolean = 0
visibility.wired_cylinder : boolean = 0
visibility.color          : string  = "red"


####################################################################
[name="drift_cell_ring.model" type="geomtools::simple_shaped_model"]

#@config Drift cell cathode ring:

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit         : string = "mm"

#@description The name of the volume shape
shape_type          : string  = "tube"

#@description The outer radius of the tube
outer_r             : real as length = 21.0 mm

#@description The inner radius of the tube
inner_r             : real as length = 19.8 mm

#@description The length of the tube
z                   : real as length = 53.5 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref        : string  = "std::copper"

#########################
# Visibility parameters #
#########################

visibility.hidden   : boolean = 0
visibility.color    : string  = "red"


##############################################################
[name="drift_cell_core.model" type="geomtools::stacked_model"]

#@config Geometric description of the core volume of a drift cell
#
#   base bottom                                   base top
#      -----  ring bottom                 ring top -----
#      |   |-----                             -----|   |
# - - -|   |=====)===========================(=====|   |- - - - - -> z
#      |   |-----          anode wire         -----|   |
#      -----                                       -----
#


#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit             : string  = "mm"

#@description X dimension of the stack mother box
x                       : real as length = 43.950 mm

#@description Y dimension of the stack mother box
y                       : real as length = 43.950 mm

#@description Z dimension of the stack mother box
z                       : real as length = 3030   mm

#@description The stacking axis
stacked.axis            : string  = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 9

#@description The geometry model stacked at position #8
stacked.model_8     : string  = "drift_cell_base.model"

#@description The label of the volume stacked at position #8
stacked.label_8     : string  = "top_base"

#@description The min limit for stacking associated to the volume stacked at position #8
stacked.limit_min_8 : real as length = +37.00 mm # allowed Z-shift for stacking

stacked.model_7     : string  = "anode_bus_bar.model"
stacked.label_7     : string  = "top_anode_bus_bar"
stacked.limit_min_7 : real as length = -9.0 mm # allowed Z-shift for stacking

stacked.model_6     : string  = "cathode_bus_bar.model"
stacked.label_6     : string  = "top_cathode_bus_bar"
stacked.limit_min_6 : real as length =  -6.5 mm # allowed Z-shift for stacking

stacked.model_5     : string  = "drift_cell_ring.model"
stacked.label_5     : string  = "top_ring"
stacked.limit_min_5 : real as length =  +6.25 mm # allowed Z-shift for stacking

stacked.model_4     : string  = "anode_wire.model"
stacked.label_4     : string  = "anode_wire"

stacked.model_3     : string  = "drift_cell_ring.model"
stacked.label_3     : string  = "bottom_ring"
stacked.limit_max_3 : real as length = -6.25 mm # allowed Z-shift for stacking

stacked.model_2     : string  = "cathode_bus_bar.model"
stacked.label_2     : string  = "bottom_cathode_bus_bar"
stacked.limit_max_2 : real as length = +6.5 mm # allowed Z-shift for stacking

stacked.model_1     : string  = "anode_bus_bar.model"
stacked.label_1     : string  = "bottom_anode_bus_bar"
stacked.limit_max_1 : real as length = +9.0 mm # allowed Z-shift for stacking

stacked.model_0     : string  = "drift_cell_base.model"
stacked.label_0     : string  = "bottom_base"
stacked.limit_max_0 : real as length = -37.00 mm # allowed Z-shift for stacking

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string  = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = 0
visibility.hidden_envelope  : boolean = 1
visibility.daughters.hidden : boolean = 1
visibility.color            : string  = "cyan"

#################################
# Sensitive detector parameters #
#################################

#@description The name of the sensitive detector category
sensitive.category          : string = "tracker_SD"

######################
# Mapping parameters #
######################

mapping.daughter_id.anode_wire  : string = "[drift_cell_anodic_wire]"
mapping.daughter_id.bottom_base : string = "[drift_cell_base:base=0]"
mapping.daughter_id.top_base    : string = "[drift_cell_base:base=1]"
mapping.daughter_id.bottom_ring : string = "[drift_cell_cathodic_ring:ring=0]"
mapping.daughter_id.top_ring    : string = "[drift_cell_cathodic_ring:ring=1]"


# end of tracker_drift_cell_core.geom
