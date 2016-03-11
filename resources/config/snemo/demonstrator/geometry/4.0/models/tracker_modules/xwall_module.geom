# -*- mode: conf-unix; -*-
# xwall_module.geom

########################
# In tracking gas part #
########################


#####################################################################
[name="xwall_scin_block.model" type="geomtools::simple_shaped_model"]
# DocDB 1599
# SuperNEMO A2/5359/303-2
#

shape_type        : string = "box"
x                 : real as length = 200.0 mm
y                 : real as length = 208.5 mm
z                 : real as length = 150.0 mm

material.ref      : string = "ENVINET_PS_scintillator"

visibility.hidden : boolean = false
visibility.color  : string = "blue"

# Sensitive detector category:
sensitive.category : string = "xcalorimeter_SD"


#############################################################################
[name="xwall_scin_front_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 200.0 mm
y                 : real as length = 208.5 mm
z                 : real as length = 12.0 um

material.ref      : string = "basic::mylar"

visibility.hidden : boolean = false
visibility.color  : string = "green"


#######################################################################################
[name="xwall_scin_side_x_external_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 12.0 um
y                 : real as length = 208.5 mm
z                 : real as length = 150.0 mm

material.ref      : string = "basic::mylar"

visibility.hidden : boolean = false
visibility.color  : string = "green"


#######################################################################################
[name="xwall_scin_side_x_internal_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =  0.6 mm
y                 : real as length = 208.5 mm
z                 : real as length = 150.0 mm

material.ref      : string = "basic::ptfe"

visibility.hidden : boolean = false
visibility.color  : string = "green"


#############################################################################
[name="xwall_scin_side_x_left_wrapper.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 2
stacked.model_0         : string = "xwall_scin_side_x_external_wrapper.model"
stacked.label_0         : string = "external_wrapping"
stacked.model_1         : string = "xwall_scin_side_x_internal_wrapper.model"
stacked.label_1         : string = "internal_wrapping"

material.ref            : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string = "grey"


##############################################################################
[name="xwall_scin_side_x_right_wrapper.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 2
stacked.model_1         : string = "xwall_scin_side_x_external_wrapper.model"
stacked.label_1         : string = "external_wrapping"
stacked.model_0         : string = "xwall_scin_side_x_internal_wrapper.model"
stacked.label_0         : string = "internal_wrapping"

material.ref            : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string = "grey"


#######################################################################################
[name="xwall_scin_side_y_internal_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 200.0 mm
y                 : real as length =   0.6 mm
z                 : real as length = 150.0 mm

material.ref      : string = "basic::ptfe"

visibility.hidden : boolean = false
visibility.color  : string = "green"


#######################################################################################
[name="xwall_scin_side_y_external_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 200.0 mm
y                 : real as length = 12.0 um
z                 : real as length = 150.0 mm

material.ref      : string = "basic::mylar"

visibility.hidden : boolean = false
visibility.color  : string = "green"


###############################################################################
[name="xwall_scin_side_y_bottom_wrapper.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2
stacked.model_0         : string = "xwall_scin_side_y_external_wrapper.model"
stacked.label_0         : string = "external_wrapping"
stacked.model_1         : string = "xwall_scin_side_y_internal_wrapper.model"
stacked.label_1         : string = "internal_wrapping"

material.ref            : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string = "grey"


############################################################################
[name="xwall_scin_side_y_top_wrapper.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2
stacked.model_1         : string = "xwall_scin_side_y_external_wrapper.model"
stacked.label_1         : string = "external_wrapping"
stacked.model_0         : string = "xwall_scin_side_y_internal_wrapper.model"
stacked.label_0         : string = "internal_wrapping"

material.ref            : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string = "grey"


################################################################################
[name="xwall_wrapped_scin_block.model" type="geomtools::surrounded_boxed_model"]

surrounded.model       : string = "xwall_scin_block.model"
surrounded.label       : string = "scin_block"

surrounded.top_model   : string = "xwall_scin_front_wrapper.model"
surrounded.top_label   : string = "top_wrapper"

surrounded.left_model  : string = "xwall_scin_side_y_bottom_wrapper.model"
surrounded.left_label  : string = "left_wrapper"

surrounded.right_model : string = "xwall_scin_side_y_top_wrapper.model"
surrounded.right_label : string = "right_wrapper"

surrounded.front_model : string = "xwall_scin_side_x_right_wrapper.model"
surrounded.front_label : string = "front_wrapper"

surrounded.back_model  : string = "xwall_scin_side_x_left_wrapper.model"
surrounded.back_label  : string = "back_wrapper"

material.ref           : string = "tracking_gas"

visibility.hidden      : boolean = false
visibility.color       : string = "grey"

mapping.daughter_id.scin_block  : string = "[xcalo_block]"
mapping.daughter_id.top_wrapper : string = "[xcalo_wrapper]"


##############################################################################
[name="xwall_scin_back_wrapper.model" type="geomtools::plate_with_hole_model"]

x                 : real as length = 200.0 mm
y                 : real as length = 208.5 mm
z                 : real as length = 12.0 um
r_hole            : real as length = 70.0 mm

material.ref      : string = "basic::mylar"

visibility.hidden : boolean = false
visibility.hidden_envelop : boolean = false
visibility.color  : string = "green"


############################################################################
[name="xwall_light_guide_inner.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "cylinder"
r                 : real as length =  64.0 mm
z                 : real as length =   3.5 mm

material.ref      : string = "basic::plexiglass"

visibility.hidden : boolean = false
visibility.color  : string = "blue"


###################################################################################
[name="xwall_module_inner_front_chock.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 202.0 mm
y                 : real as length = 212.0 mm
z                 : real as length =  1.488 mm

material.ref      : string = "tracking_gas"

visibility.hidden : boolean = false
visibility.color  : string = "cyan"


#################################################################
[name="xwall_module_inner.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 4

stacked.model_0   : string = "xwall_light_guide_inner.model"
stacked.label_0   : string = "light_guide_inner"

stacked.limit_min_1 : real as length = +0.006 mm
stacked.model_1   : string = "xwall_scin_back_wrapper.model"
stacked.label_1   : string = "back_wrapping"

stacked.model_2   : string = "xwall_wrapped_scin_block.model"
stacked.label_2   : string = "wrapped_scintillator_block"

stacked.model_3   : string = "xwall_module_inner_front_chock.model"
stacked.label_3   : string = "front_chock"

material.ref            : string = "lab_air"

visibility.hidden : boolean = false
visibility.color  : string = "cyan"


###############
# In air part #
###############


###############################################################################
[name="xwall_beam_support_plate.model" type="geomtools::plate_with_hole_model"]

x                 : real as length = 202.0 mm
y                 : real as length = 212.0 mm
z                 : real as length =  6.0 mm
r_hole            : real as length = 77.5 mm

material.ref      : string = "basic::iron"

visibility.hidden : boolean = false
visibility.color  : string = "red"

##########################################################################
[name="xwall_beam_side_plate.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 319.0 mm
y                 : real as length = 212.0 mm
z                 : real as length =  6.0 mm

material.ref      : string = "basic::iron"

visibility.hidden : boolean = false
visibility.color  : string = "red"


##########################################################################
[name="xwall_magnetic_shield.model" type="geomtools::simple_shaped_model"]

shape_type   : string = "tube"
inner_r      : real as length = 73.3 mm
outer_r      : real as length = 74.8 mm
z            : real as length = 250.0 mm

material.ref : string = "snemo::mu_metal"

visibility.hidden : boolean = false
visibility.color  : string = "magenta"


###########################################################################################
[name="xwall_light_guide_outer.model" type="geomtools::spherical_extrusion_cylinder_model"]

r                 : real as length = 64.0 mm
z                 : real as length = 71.5 mm
r_sphere          : real as length = 82.0 mm
r_extrusion       : real as length = 62.5 mm
bottom            : boolean = true

material.ref      : string = "basic::plexiglass"

visibility.hidden : boolean = false
visibility.color  : string = "blue"

#############################################################################
[name="xwall_module_outer_right.model" type="geomtools::simple_shaped_model"]

shape_type              : string = "box"
x                       : real as length = 202.0 mm
y                       : real as length = 212.0 mm
z                       : real as length = 325.0 mm

material.ref            : string = "lab_air"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = false
visibility.color          : string = "grey"

internal_item.labels  : string[6] = \
   "magnetic_shield" \
   "beam_side" \
   "beam_support" \
   "light_guide" \
   "pmt"  \
   "pmt_base"

internal_item.placement.magnetic_shield : string = "0 0 31.5 (mm)"
internal_item.model.magnetic_shield     : string = "xwall_magnetic_shield.model"

internal_item.placement.beam_side       : string = "98 0 -3 (mm) / y 90 (deg)"
internal_item.model.beam_side           : string = "xwall_beam_side_plate.model"

internal_item.placement.beam_support    : string = "0 0 159.5 (mm)"
internal_item.model.beam_support        : string = "xwall_beam_support_plate.model"

internal_item.placement.light_guide     : string = "0 0 126.75 (mm)"
internal_item.model.light_guide         : string = "xwall_light_guide_outer.model"

internal_item.placement.pmt             : string = "0 0 29.75 (mm)"
internal_item.model.pmt                 : string = "PMT_HAMAMATSU_R6594"

internal_item.placement.pmt_base        : string = "0 0 -74.25 (mm)"
internal_item.model.pmt_base            : string = "PMT_HAMAMATSU_R6594.base"


############################################################################
[name="xwall_module_outer_left.model" type="geomtools::simple_shaped_model"]

shape_type              : string = "box"
x                       : real as length = 202.0 mm
y                       : real as length = 212.0 mm
z                       : real as length = 325.0 mm

material.ref            : string = "lab_air"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = false
visibility.color          : string = "grey"

internal_item.labels  : string[6] = \
   "magnetic_shield" \
   "beam_side" \
   "beam_support" \
   "light_guide" \
   "pmt"  \
   "pmt_base"

internal_item.placement.magnetic_shield : string = "0 0 31.5 (mm)"
internal_item.model.magnetic_shield     : string = "xwall_magnetic_shield.model"

internal_item.placement.beam_side       : string = "-98 0 -3 (mm) / y 90 (deg)"
internal_item.model.beam_side           : string = "xwall_beam_side_plate.model"

internal_item.placement.beam_support    : string = "0 0 159.5 (mm)"
internal_item.model.beam_support        : string = "xwall_beam_support_plate.model"

internal_item.placement.light_guide     : string = "0 0 126.75 (mm)"
internal_item.model.light_guide         : string = "xwall_light_guide_outer.model"

internal_item.placement.pmt             : string = "0 0 29.75 (mm)"
internal_item.model.pmt                 : string = "PMT_HAMAMATSU_R6594"

internal_item.placement.pmt_base        : string = "0 0 -74.25 (mm)"
internal_item.model.pmt_base            : string = "PMT_HAMAMATSU_R6594.base"


#################################################################
[name="xwall_module_right.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 2

stacked.model_0   : string = "xwall_module_outer_right.model"
stacked.label_0   : string = "xwall_module_outer"

stacked.model_1   : string = "xwall_module_inner.model"
stacked.label_1   : string = "xwall_module_inner"

material.ref            : string = "tracking_gas"


################################################################
[name="xwall_module_left.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 2

stacked.model_0   : string = "xwall_module_outer_left.model"
stacked.label_0   : string = "xwall_module_outer"

stacked.model_1   : string = "xwall_module_inner.model"
stacked.label_1   : string = "xwall_module_inner"

material.ref            : string = "tracking_gas"


##############################################################################
[name="xwall_module_left.rotated.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "x"
rotated.special_angle     : string = "90"
rotated.model             : string = "xwall_module_left.model"

material.ref              : string = "tracking_gas"

visibility.hidden_envelop : boolean = false


###############################################################################
[name="xwall_module_right.rotated.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string = "x"
rotated.special_angle     : string = "90"
rotated.model             : string = "xwall_module_right.model"

material.ref              : string = "tracking_gas"

visibility.hidden_envelop : boolean = false


# end of xwall_module.geom
