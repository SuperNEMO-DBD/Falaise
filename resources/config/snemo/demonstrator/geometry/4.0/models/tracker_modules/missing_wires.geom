# -*- mode: conf-unix; -*-
# missing_wires.geom

#@description The geometry models that compose the drift cell (with field wires)
#@key_label   "name"
#@meta_label  "type"


###############################################################
[name="anode_wire_missing.model" type="geomtools::simple_shaped_model"]

#@config Geometric description of a drift cell anode wire

#######################
# Geometry parameters #
#######################

#@description The name of the volume shape
shape_type : string = "cylinder"

#@description The diameter of the wire
diameter : real as length = 0.01 um

#@description The length of the wire
z : real as length = 2866.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

#visibility.hidden : boolean = true
visibility.color          : string = "blue"



##############################################################
[name="drift_cell_core_missing.model" type="geomtools::stacked_model"]

#@config Geometric description of the core volume of a drift cell
#
#   base bottom                                   base top
#      -----  ring bottom                 ring top -----
#      |   |-----                             -----|   |
# - - -|   |=====)                           (=====|   |- - - - - -> z
#      |   |-----    anode wire (missing)     -----|   |
#      -----                                       -----
#


#######################
# Geometry parameters #
#######################

#@description X dimension of the stack mother box
x : real as length = 43.950 mm

#@description Y dimension of the stack mother box
y : real as length = 43.950 mm

#@description Z dimension of the stack mother box
z : real as length = 3030   mm

#@description The stacking axis
stacked.axis : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 9

#@description The geometry model stacked at position #8
stacked.model_8     : string = "drift_cell_base.model"

#@description The label of the volume stacked at position #8
stacked.label_8     : string = "top_base"

#@description The min limit for stacking associated to the volume stacked at position #8
stacked.limit_min_8 : real as length = +37.00 mm # allowed Z-shift for stacking

stacked.model_7     : string = "anode_bus_bar.model"
stacked.label_7     : string = "top_anode_bus_bar"
stacked.limit_min_7 : real as length = -9.0 mm # allowed Z-shift for stacking

stacked.model_6     : string = "cathode_bus_bar.model"
stacked.label_6     : string = "top_cathode_bus_bar"
stacked.limit_min_6 : real as length =  -6.5 mm # allowed Z-shift for stacking

stacked.model_5     : string = "drift_cell_ring.model"
stacked.label_5     : string = "top_ring"
stacked.limit_min_5 : real as length =  +6.25 mm # allowed Z-shift for stacking

stacked.model_4     : string = "anode_wire_missing.model"
stacked.label_4     : string = "anode_wire_missing"

stacked.model_3     : string = "drift_cell_ring.model"
stacked.label_3     : string = "bottom_ring"
stacked.limit_max_3 : real as length = -6.25 mm # allowed Z-shift for stacking

stacked.model_2     : string = "cathode_bus_bar.model"
stacked.label_2     : string = "bottom_cathode_bus_bar"
stacked.limit_max_2 : real as length = +6.5 mm # allowed Z-shift for stacking

stacked.model_1     : string = "anode_bus_bar.model"
stacked.label_1     : string = "bottom_anode_bus_bar"
stacked.limit_max_1 : real as length = +9.0 mm # allowed Z-shift for stacking

stacked.model_0     : string = "drift_cell_base.model"
stacked.label_0     : string = "bottom_base"
stacked.limit_max_0 : real as length = -37.00 mm # allowed Z-shift for stacking

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden_envelope  : boolean = false
visibility.color              : string = "green"
visibility.daughters.hidden : boolean = true
# visibility.daughters.top_ring.shown    : boolean = true
# visibility.daughters.bottom_ring.shown : boolean = true

#################################
# Sensitive detector parameters #
#################################

#@description The name of the sensitive detector category
sensitive.category : string = "tracker_SD"

######################
# Mapping parameters #
######################

mapping.daughter_id.bottom_base : string = "[drift_cell_base:base=0]"
mapping.daughter_id.top_base    : string = "[drift_cell_base:base=1]"
mapping.daughter_id.bottom_ring : string = "[drift_cell_cathodic_ring:ring=0]"
mapping.daughter_id.top_ring    : string = "[drift_cell_cathodic_ring:ring=1]"





####
[name="x_field_wires_missing_gap.model" type="geomtools::simple_shaped_model"]

#@description The name of the volume shape
shape_type : string = "box"

#@description The radius of the cylinder
x            : real as length = 9.115 mm

#@description Cylinder diameter
y : real as length = 50.0 um

#@description Cylinder height
z : real as length = 2920.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string = "tracking_gas"
####
[name="x_field_wires_nonmissing_wires.model" type="geomtools::replicated_model"]
#@description The replication axis
replicated.axis : string = "x"

#@description The number of replicated volumes
replicated.number_of_items : integer = 2

#@description The model of the replicated volumes
replicated.model           : string = "field_wire.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "field_wires"

#@description The step between replicated volumes
replicated.step            : real as length = 9.115 mm

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden            : boolean = false
# visibility.hidden_envelope   : boolean = false
# visibility.daughters.hidden  : boolean = false
visibility.color             : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires : string = "[drift_cell_field_wire:wire+0]"



###################################################################
[name="x_field_wires_set_miss0.model" type="geomtools::stacked_model"]

#@config Geometric description of a set of 3 field wires on core cell X-side

# A set of field wires along the X axis:
#
#  (missing)
#  wire #0      ^ y        wire #2
#     \         :         /
#    +-\-----------------/-+      ^
#  - | --- - - -o- - - -o- |- - tiny - -> x
#    +-----------\---------+      v
#               : \
#                  wire #1
#       <- 18mm +23um ->
#




stacked.axis             : string = "x"
stacked.number_of_items  : integer = 2
stacked.model_0          : string = "x_field_wires_missing_gap.model"
stacked.label_0          : string = "missing_gap"
stacked.model_1          : string = "x_field_wires_nonmissing_wires.model"
stacked.label_1          : string = "wires"






#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden            : boolean = false
# visibility.hidden_envelope   : boolean = false
# visibility.daughters.hidden  : boolean = false
visibility.color             : string = "cyan"

###################################################################
[name="x_field_wires_set_miss1.model" type="geomtools::replicated_model"]

#@config Geometric description of a set of 3 field wires on core cell X-side

# A set of field wires along the X axis:
#
#  wire #0      ^ y        wire #2
#     \         :         /
#    +-\-----------------/-+      ^
#  - | -o- - - --- - - -o- |- - tiny - -> x
#    +-----------\---------+      v
#               : \
#                  wire #1
#       <- 18mm +23um ->
#

#######################
# Geometry parameters #
#######################

#@description The replication axis
replicated.axis : string = "x"

#@description The number of replicated volumes
replicated.number_of_items : integer = 2

#@description The model of the replicated volumes
replicated.model           : string = "field_wire.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "field_wires"

#@description The step between replicated volumes
replicated.step            : real as length = 18.23 mm

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden            : boolean = false
# visibility.hidden_envelope   : boolean = false
# visibility.daughters.hidden  : boolean = false
visibility.color             : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires : string = "[drift_cell_field_wire:wire+0]"

###################################################################
[name="x_field_wires_set_miss2.model" type="geomtools::stacked_model"]
#@config Geometric description of a set of 3 field wires on core cell X-side

# A set of field wires along the X axis:
#
#  wire #0      ^ y        wire #2
#     \         :         /
#    +-\-----------------/-+      ^
#  - | -o- - - -o- - - --- |- - tiny - -> x
#    +-----------\---------+      v
#               : \
#                  wire #1
#       <- 18mm +23um ->
#

#######################
# Geometry parameters #
#######################


stacked.axis             : string = "x"
stacked.number_of_items  : integer = 2
stacked.model_1          : string = "x_field_wires_missing_gap.model"
stacked.label_1          : string = "missing_gap"
stacked.model_0          : string = "x_field_wires_nonmissing_wires.model"
stacked.label_0          : string = "wires"



#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden            : boolean = false
# visibility.hidden_envelope   : boolean = false
# visibility.daughters.hidden  : boolean = false
visibility.color             : string = "cyan"

######################
# Mapping parameters #
######################





[name="y_field_wires_missing_gap.model" type="geomtools::simple_shaped_model"]

#@description The name of the volume shape
shape_type : string = "box"

#@description The radius of the cylinder
y            : real as length = 9.115 mm

#@description Cylinder diameter
x : real as length = 50.0 um

#@description Cylinder height
z : real as length = 2920.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string = "tracking_gas"
####
[name="y_field_wires_nonmissing_wires.model" type="geomtools::replicated_model"]
#@description The replication axis
replicated.axis : string = "y"

#@description The number of replicated volumes
replicated.number_of_items : integer = 2

#@description The model of the replicated volumes
replicated.model           : string = "field_wire.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "field_wires"

#@description The step between replicated volumes
replicated.step            : real as length = 9.115 mm

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden            : boolean = false
# visibility.hidden_envelope   : boolean = false
# visibility.daughters.hidden  : boolean = false
visibility.color             : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires : string = "[drift_cell_field_wire:wire+0]"









###################################################################
[name="y_field_wires_set_miss0.model" type="geomtools::stacked_model"]

#@config Geometric description of a set of 3 field wires on core cell Y-side

# A set of field wires along the Y axis
#
#              ^ y
#              :
#            +---+
#            | : |
#        ^   | o-|----- wire #2
#        :   | : |
#        :   | : |         x
#  18 mm :- -|-o-|- - - ->
#  +     :   | :`|----- wire #1
#  23 um :   | : |
#        v   | :-|----- wire #0
#            | : |
#            +---+
#              :
#              :
#            <tiny>
#
#

#######################
# Geometry parameters #
#######################

stacked.axis             : string = "y"
stacked.number_of_items  : integer = 2
stacked.model_0          : string = "y_field_wires_missing_gap.model"
stacked.label_0          : string = "missing_gap"
stacked.model_1          : string = "y_field_wires_nonmissing_wires.model"
stacked.label_1          : string = "wires"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "cyan"

###################################################################
[name="y_field_wires_set_miss1.model" type="geomtools::replicated_model"]

#@config Geometric description of a set of 3 field wires on core cell Y-side

# A set of field wires along the Y axis
#
#              ^ y
#              :
#            +---+
#            | : |
#        ^   | o-|----- wire #2
#        :   | : |
#        :   | : |         x
#  18 mm :- -|-:-|- - - ->
#  +     :   | :`|----- wire #1
#  23 um :   | : |
#        v   | o-|----- wire #0
#            | : |
#            +---+
#              :
#              :
#            <tiny>
#
#

#######################
# Geometry parameters #
#######################

#@description The replication axis
replicated.axis : string = "y"

#@description The number of replicated volumes
replicated.number_of_items : integer = 2

#@description The model of the replicated volumes
replicated.model : string = "field_wire.model"

#@description The label associated to the replicated volumes
replicated.label : string = "field_wires"

#@description The step between replicated volumes
replicated.step  : real as length = 18.23 mm

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires : string = "[drift_cell_field_wire:wire+0]"


###################################################################
[name="y_field_wires_set_miss2.model" type="geomtools::stacked_model"]

#@config Geometric description of a set of 3 field wires on core cell Y-side

# A set of field wires along the Y axis
#
#              ^ y
#              :
#            +---+
#            | : |
#        ^   | :-|----- wire #2
#        :   | : |
#        :   | : |         x
#  18 mm :- -|-o-|- - - ->
#  +     :   | :`|----- wire #1
#  23 um :   | : |
#        v   | o-|----- wire #0
#            | : |
#            +---+
#              :
#              :
#            <tiny>
#
#

#######################
# Geometry parameters #
#######################


stacked.axis             : string = "y"
stacked.number_of_items  : integer = 2
stacked.model_1          : string = "y_field_wires_missing_gap.model"
stacked.label_1          : string = "missing_gap"
stacked.model_0          : string = "y_field_wires_nonmissing_wires.model"
stacked.label_0          : string = "wires"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "cyan"

############################################################################
[name="drift_cell_node_0123_miss.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 4 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#          +---+------------/+---+
#          | o |      :    / | o |
#          |   |      :      |   | front (1)
# back (0)-|-o |- - - o - - -|-o-|- - -> x
#          |   |      :      |   |
#          | o |      :      | o | <--- Y aligned 'set of field wires' model
#          +---+-------------+---+
#              |  o   o   o  |
#              +-------------+
#                     : left (2)
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

surrounded.back_model  : string = "y_field_wires_set.model"
surrounded.back_label  : string = "field_wires_set_back"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.left_model  : string = "x_field_wires_set.model"
surrounded.left_label  : string = "field_wires_set_left"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = true
visibility.daughters.core_cell.shown : boolean = true
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_back  : string = "[drift_cell_field_wires_set:set=0]"
mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_left  : string = "[drift_cell_field_wires_set:set=2]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


######################################################################
[name="drift_cell_node_013_miss01.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 3 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#          +---+------------/+---+
#          | o |      :    / | o |
#          |   |      :      |   | front (1)
# back (0)-|-o |- - - o - - -|-o-|- - -> x
#          |   |      :      |   |
#          | o |      :      | o | <--- Y aligned 'set of field wires' model
#          +---+-------------+---+
#                     :
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

surrounded.back_model  : string = "y_field_wires_set_miss1.model"
surrounded.back_label  : string = "field_wires_set_back"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = true
visibility.daughters.core_cell.shown : boolean = true
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_back  : string = "[drift_cell_field_wires_set:set=0]"
mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"

######################################################################
[name="drift_cell_node_13_missing_anode.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 3 sides
#
#                    y
#                     ^       'core cell' model with missing anode
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#              +------------/+---+
#              |      :    / | o |
#              |      :      |   | front (1)
#              |- - - X - - -|-o-|- - -> x
#              |      :      |   |
#              |      :      | o | <--- Y aligned 'set of field wires' model
#              +-------------+---+
#                     :
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core_missing.model"
surrounded.label       : string = "core_cell"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = true
visibility.daughters.core_cell.shown : boolean = true
visibility.color            : string = "green"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


######################################################################
[name="drift_cell_node_13_miss32.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 3 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   -  / <-- X aligned 'set of field wires' model -- missing 0
#              +------------/+---+
#              |      :    / | O |
#              |      :      |   | front (1)
#              |- - - o - - -|-O-|- - -> x
#              |      :      |   |
#              |      :      | O | <--- Y aligned 'set of field wires' model 
#              +-------------+---+
#                     :
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.right_model : string = "x_field_wires_set_miss2.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = true
visibility.daughters.core_cell.shown : boolean = true
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


######################################################################
[name="drift_cell_node_13_miss1.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 3 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#              +------------/+---+
#              |      :    / | : |
#              |      :      |   | front (1)
#              |- - - o - - -|-:-|- - -> x
#              |      :      |   |
#              |      :      | : | <--- Y aligned 'set of field wires' model -- missing
#              +-------------+---+
#                     :
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

#surrounded.front_model : string = "y_field_wires_set.model"
#surrounded.front_label : string = "field_wires_set_front"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = true
visibility.daughters.core_cell.shown : boolean = true
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

#mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


#####################################################################
[name="drift_cell_node_123_miss.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 3 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#              +------------/+---+
#              |      :    / | o |
#              |      :      |   | front (1)
#              |- - - o - - -|-o-|- - -> x
#              |      :      |   |
#              |      :      | o | <--- Y aligned 'set of field wires' model
#              +-------------+---+
#              |  o   o   o  |
#              +-------------+
#                     : left (2)
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.left_model  : string = "x_field_wires_set.model"
surrounded.left_label  : string = "field_wires_set_left"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

# visibility.hidden           : boolean = false
# visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = true
visibility.daughters.core_cell.shown : boolean = true
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_left  : string = "[drift_cell_field_wires_set:set=2]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


# end of missing_wires.geom
