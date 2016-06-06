# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/tracker_module/xwall.geom


#########################################################################
[name="xwall_column_left.model" type="geomtools::replicated_boxed_model"]

replicated.axis             : string = "z"
replicated.number_of_items  : integer = 16
replicated.model            : string = "xwall_module_left.rotated.model"
replicated.label            : string = "module"

material.ref                : string = "tracking_gas"

visibility.hidden_envelope   : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

#  Daughters mapping informations:
mapping.daughter_id.module : string = "[xcalo_optical_module:row+0]"


##########################################################################
[name="xwall_column_right.model" type="geomtools::replicated_boxed_model"]

replicated.axis             : string = "z"
replicated.number_of_items  : integer = 16
replicated.model            : string = "xwall_module_right.rotated.model"
replicated.label            : string = "module"

material.ref                : string = "tracking_gas"

visibility.hidden_envelope   : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

#  Daughters mapping informations:
mapping.daughter_id.module : string = "[xcalo_optical_module:row+0]"


###########################################################################
[name="xwall_left_column_left.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "z"
rotated.special_angle     : string = "180"
rotated.model             : string = "xwall_column_left.model"

material.ref              : string = "tracking_gas"

visibility.hidden_envelope : boolean = false


############################################################################
[name="xwall_left_column_right.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "z"
rotated.special_angle     : string = "180"
rotated.model             : string = "xwall_column_right.model"

material.ref              : string = "tracking_gas"

visibility.hidden_envelope : boolean = false


############################################################################
[name="xwall_right_column_left.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "z"
rotated.special_angle     : string = "0"
rotated.model             : string = "xwall_column_left.model"

material.ref              : string = "tracking_gas"

visibility.hidden_envelope : boolean = false


#############################################################################
[name="xwall_right_column_right.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "z"
rotated.special_angle     : string = "0"
rotated.model             : string = "xwall_column_right.model"

material.ref              : string = "tracking_gas"

visibility.hidden_envelope : boolean = false


#################################################################################
[name="tracker_frame_corner_plate_A.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =   6.5 mm
y                 : real as length = 480.0 mm
z                 : real as length = 325.0 mm

material.ref      : string = "basic::iron"

visibility.color  : string = "red"


#################################################################################
[name="tracker_frame_corner_plate_B.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =   6.0 mm
y                 : real as length = 325.0 mm
z                 : real as length =   7.0 mm

material.ref      : string = "basic::iron"

visibility.color  : string = "red"


#################################################################################
[name="tracker_frame_corner_plate_C.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 392.0 mm
y                 : real as length = 161.0 mm
z                 : real as length =   6.5 mm

material.ref      : string = "basic::iron"

visibility.color  : string = "red"


#################################################################################
[name="tracker_frame_corner_plate_D.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 392.0 mm
y                 : real as length =   6.0 mm
z                 : real as length =   7.0 mm

material.ref      : string = "basic::iron"

visibility.color  : string = "red"


#############################################################################
[name="tracker_frame_corner_gas.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 405.0 mm
y                 : real as length = 155.0 mm
z                 : real as length =   7.0 mm

material.ref      : string = "tracking_gas"

visibility.color  : string = "cyan"


#########################################################################
[name="tracker_frame_corner.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 405.0 mm
y                 : real as length = 480.0 mm
z                 : real as length = 332.0 mm

material.ref      : string = "lab_air"

visibility.color  : string = "grey"

internal_item.labels : string[7] = \
   "A1" \
   "A2" \
   "B1" \
   "B2" \
   "C"  \
   "D"  \
   "gas"

internal_item.placement.A1 : string = "-199.25 0 3.5 (mm)"
internal_item.model.A1     : string = "tracker_frame_corner_plate_A.model"

internal_item.placement.A2 : string = "+199.25 0 3.5 (mm)"
internal_item.model.A2     : string = "tracker_frame_corner_plate_A.model"

internal_item.placement.B1 : string = "-199.25 -77.5 -162.5 (mm)"
internal_item.model.B1     : string = "tracker_frame_corner_plate_B.model"

internal_item.placement.B2 : string = "+199.25 -77.5 -162.5 (mm)"
internal_item.model.B2     : string = "tracker_frame_corner_plate_B.model"

internal_item.placement.C  : string = "0 159.5 -155.75 (mm)"
internal_item.model.C      : string = "tracker_frame_corner_plate_C.model"

internal_item.placement.D  : string = "0 82 -162.5 (mm)"
internal_item.model.D      : string = "tracker_frame_corner_plate_D.model"

internal_item.placement.gas : string = "0 162.5 -162.5 (mm)"
internal_item.model.gas     : string = "tracker_frame_corner_gas.model"


##################################################################################
[name="tracker_frame_corner_top_left.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "z"
rotated.special_angle     : string = "0"
rotated.model             : string = "tracker_frame_corner.model"

material.ref              : string = "lab_air"

visibility.hidden_envelope : boolean = false


#####################################################################################
[name="tracker_frame_corner_bottom_left.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "y"
rotated.special_angle     : string = "180"
rotated.model             : string = "tracker_frame_corner.model"

material.ref              : string = "lab_air"

visibility.hidden_envelope : boolean = false


###################################################################################
[name="tracker_frame_corner_top_right.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "z"
rotated.special_angle     : string = "180"
rotated.model             : string = "tracker_frame_corner.model"

material.ref              : string = "lab_air"

visibility.hidden_envelope : boolean = false


######################################################################################
[name="tracker_frame_corner_bottom_right.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "x"
rotated.special_angle     : string = "180"
rotated.model             : string = "tracker_frame_corner.model"

material.ref              : string = "lab_air"

visibility.hidden_envelope : boolean = false


#############################################################################
[name="xwall_column_stock_plate.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =    1.0 mm
y                 : real as length =    6.0 mm
z                 : real as length = 3392.0 mm

material.ref      : string = "basic::iron"

visibility.color  : string = "red"


#############################################################################
[name="xwall_column_stock_outer.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =    1.0 mm
y                 : real as length =  319.0 mm
z                 : real as length = 3392.0 mm

material.ref      : string = "lab_air"

visibility.color  : string = "grey"


#############################################################################
[name="xwall_column_stock_inner.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =    1.0 mm
y                 : real as length =  155.0 mm
z                 : real as length = 3392.0 mm

material.ref      : string = "tracking_gas"

visibility.color  : string = "cyan"


######################################################################
[name="xwall_column_stock_left.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 3
stacked.model_0   : string = "xwall_column_stock_outer.model"
stacked.label_0   : string = "outer"
stacked.model_1   : string = "xwall_column_stock_plate.model"
stacked.label_1   : string = "plate"
stacked.model_2   : string = "xwall_column_stock_inner.model"
stacked.label_2   : string = "inner"

material.ref            : string = "lab_air"


#######################################################################
[name="xwall_column_stock_right.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 3
stacked.model_0   : string = "xwall_column_stock_inner.model"
stacked.label_0   : string = "outer"
stacked.model_1   : string = "xwall_column_stock_plate.model"
stacked.label_1   : string = "plate"
stacked.model_2   : string = "xwall_column_stock_outer.model"
stacked.label_2   : string = "inner"

material.ref            : string = "lab_air"


########################################################################
[name="xwall_front_right_columns.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0   : string = "xwall_right_column_left.model"
stacked.label_0   : string = "inner_column"
stacked.model_1   : string = "xwall_column_stock_right.model"
stacked.label_1   : string = "stock_column"
stacked.model_2   : string = "xwall_right_column_right.model"
stacked.label_2   : string = "outer_column"

material.ref            : string = "lab_air"

# Daughters mapping informations:
mapping.daughter_id.inner_column : string = "[xcalo_column:column=0]"
mapping.daughter_id.outer_column : string = "[xcalo_column:column=1]"


#######################################################################
[name="xwall_front_left_columns.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0   : string = "xwall_left_column_right.model"
stacked.label_0   : string = "inner_column"
stacked.model_1   : string = "xwall_column_stock_left.model"
stacked.label_1   : string = "stock_column"
stacked.model_2   : string = "xwall_left_column_left.model"
stacked.label_2   : string = "outer_column"

material.ref            : string = "lab_air"

# Daughters mapping informations:
mapping.daughter_id.inner_column : string = "[xcalo_column:column=0]"
mapping.daughter_id.outer_column : string = "[xcalo_column:column=1]"


#######################################################################
[name="xwall_back_right_columns.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0   : string = "xwall_right_column_left.model"
stacked.label_0   : string = "outer_column"
stacked.model_1   : string = "xwall_column_stock_right.model"
stacked.label_1   : string = "stock_column"
stacked.model_2   : string = "xwall_right_column_right.model"
stacked.label_2   : string = "inner_column"

material.ref            : string = "lab_air"

# Daughters mapping informations:
mapping.daughter_id.inner_column : string = "[xcalo_column:column=0]"
mapping.daughter_id.outer_column : string = "[xcalo_column:column=1]"


######################################################################
[name="xwall_back_left_columns.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0   : string = "xwall_left_column_right.model"
stacked.label_0   : string = "outer_column"
stacked.model_1   : string = "xwall_column_stock_left.model"
stacked.label_1   : string = "stock_column"
stacked.model_2   : string = "xwall_left_column_left.model"
stacked.label_2   : string = "inner_column"

material.ref            : string = "lab_air"

#  Daughters mapping informations:
mapping.daughter_id.inner_column : string = "[xcalo_column:column=0]"
mapping.daughter_id.outer_column : string = "[xcalo_column:column=1]"


################################################################
[name="xwall_front_right.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "tracker_frame_corner_bottom_right.model"
stacked.label_0   : string = "bottom_corner"
stacked.model_1   : string = "xwall_front_right_columns.model"
stacked.label_1   : string = "columns"
stacked.model_2   : string = "tracker_frame_corner_top_right.model"
stacked.label_2   : string = "top_corner"

material.ref      : string = "lab_air"

visibility.color  : string = "grey"


###############################################################
[name="xwall_front_left.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "tracker_frame_corner_bottom_left.model"
stacked.label_0   : string = "bottom_corner"
stacked.model_1   : string = "xwall_front_left_columns.model"
stacked.label_1   : string = "columns"
stacked.model_2   : string = "tracker_frame_corner_top_left.model"
stacked.label_2   : string = "top_corner"

material.ref      : string = "lab_air"

visibility.color  : string = "grey"


###############################################################
[name="xwall_back_right.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "tracker_frame_corner_bottom_right.model"
stacked.label_0   : string = "bottom_corner"
stacked.model_1   : string = "xwall_back_right_columns.model"
stacked.label_1   : string = "columns"
stacked.model_2   : string = "tracker_frame_corner_top_right.model"
stacked.label_2   : string = "top_corner"

material.ref      : string = "lab_air"

visibility.color  : string = "grey"


##############################################################
[name="xwall_back_left.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "tracker_frame_corner_bottom_left.model"
stacked.label_0   : string = "bottom_corner"
stacked.model_1   : string = "xwall_back_left_columns.model"
stacked.label_1   : string = "columns"
stacked.model_2   : string = "tracker_frame_corner_top_left.model"
stacked.label_2   : string = "top_corner"

material.ref      : string = "lab_air"

visibility.color  : string = "grey"


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/tracker_module/xwall.geom
