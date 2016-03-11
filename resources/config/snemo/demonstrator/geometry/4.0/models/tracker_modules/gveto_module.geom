# -*- mode: conf-unix; -*-
# gveto_module.geom
#
# DocDB 2079
#

########################
# In tracking gas part #
########################


#####################################################################
[name="gveto_scin_block.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length   = 290.0 mm
y                 : real as length   = 308.0 mm
z                 : real as length   = 150.0 mm

material.ref : string = "Dubna_PS_scintillator"

visibility.hidden : boolean = false
visibility.color  : string = "blue"

# Sensitive detector category:
sensitive.category : string ="gveto_SD"


#############################################################################
[name="gveto_scin_front_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length   = 290.0 mm
y                 : real as length   = 308.0 mm
z                 : real as length   =   0.012 mm

material.ref : string = "basic::mylar"

visibility.hidden : boolean = false
visibility.color  : string = "green"


#######################################################################################
[name="gveto_scin_side_x_external_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length   =   0.012 mm
y                 : real as length   = 308.0   mm
z                 : real as length   = 150.0   mm

material.ref : string = "basic::mylar"

visibility.hidden : boolean = false
visibility.color  : string = "green"


#######################################################################################
[name="gveto_scin_side_x_internal_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length   =   0.6 mm
y                 : real as length   = 308.0 mm
z                 : real as length   = 150.0 mm

material.ref : string = "basic::ptfe"

visibility.hidden : boolean = false
visibility.color  : string = "green"


#############################################################################
[name="gveto_scin_side_x_left_wrapper.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 2
stacked.model_0         : string = "gveto_scin_side_x_external_wrapper.model"
stacked.label_0         : string = "external_wrapping"
stacked.model_1         : string = "gveto_scin_side_x_internal_wrapper.model"
stacked.label_1         : string = "internal_wrapping"

material.ref : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string = "grey"


##############################################################################
[name="gveto_scin_side_x_right_wrapper.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 2
stacked.model_1         : string = "gveto_scin_side_x_external_wrapper.model"
stacked.label_1         : string = "external_wrapping"
stacked.model_0         : string = "gveto_scin_side_x_internal_wrapper.model"
stacked.label_0         : string = "internal_wrapping"

material.ref : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string = "grey"


# ##############################################################################
# [name="gveto_scin_side_x_wrapper.model" type="geomtools::simple_shaped_model"]
# shape_type        : string = "box"
# x                 : real as length   =   0.6 mm
# y                 : real as length   = 308.0 mm
# z                 : real as length   = 150.0 mm
# length_unit       : string = "mm"
# material.ref : string = "basic::ptfe"
# visibility.hidden : boolean = false
# visibility.color  : string = "green"


#######################################################################################
[name="gveto_scin_side_y_internal_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length   = 290.0 mm
y                 : real as length   =   0.6 mm
z                 : real as length   = 150.0 mm

material.ref : string = "basic::ptfe"

visibility.hidden : boolean = false
visibility.color  : string = "green"


#######################################################################################
[name="gveto_scin_side_y_external_wrapper.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length   = 290.0   mm
y                 : real as length   =   0.012 mm
z                 : real as length   = 150.0   mm

material.ref : string = "basic::mylar"

visibility.hidden : boolean = false
visibility.color  : string = "green"


###############################################################################
[name="gveto_scin_side_y_bottom_wrapper.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2
stacked.model_0         : string = "gveto_scin_side_y_external_wrapper.model"
stacked.label_0         : string = "external_wrapping"
stacked.model_1         : string = "gveto_scin_side_y_internal_wrapper.model"
stacked.label_1         : string = "internal_wrapping"

material.ref : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string = "grey"


############################################################################
[name="gveto_scin_side_y_top_wrapper.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2
stacked.model_1         : string = "gveto_scin_side_y_external_wrapper.model"
stacked.label_1         : string = "external_wrapping"
stacked.model_0         : string = "gveto_scin_side_y_internal_wrapper.model"
stacked.label_0         : string = "internal_wrapping"

material.ref : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string = "grey"


# ##############################################################################
# [name="gveto_scin_side_y_wrapper.model" type="geomtools::simple_shaped_model"]
# shape_type        : string = "box"
# x                 : real as length   = 290.0 mm
# y                 : real as length   =   0.6 mm
# z                 : real as length   = 150.0 mm
# length_unit       : string = "mm"
# material.ref : string = "basic::ptfe"
# visibility.hidden : boolean = false
# visibility.color  : string = "green"


################################################################################
[name="gveto_wrapped_scin_block.model" type="geomtools::surrounded_boxed_model"]

surrounded.model       : string = "gveto_scin_block.model"
surrounded.label       : string = "scin_block"

surrounded.top_model   : string = "gveto_scin_front_wrapper.model"
surrounded.top_label   : string = "top_wrapper"

surrounded.left_model  : string = "gveto_scin_side_y_bottom_wrapper.model"
surrounded.left_label  : string = "left_wrapper"

surrounded.right_model : string = "gveto_scin_side_y_top_wrapper.model"
surrounded.right_label : string = "right_wrapper"

surrounded.front_model : string = "gveto_scin_side_x_right_wrapper.model"
surrounded.front_label : string = "front_wrapper"

surrounded.back_model  : string = "gveto_scin_side_x_left_wrapper.model"
surrounded.back_label  : string = "back_wrapper"

material.ref : string = "tracking_gas"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = false
visibility.color          : string = "grey"

mapping.daughter_id.scin_block  : string = "[gveto_block]"
mapping.daughter_id.top_wrapper : string = "[gveto_wrapper]"


##############################################################################
[name="gveto_scin_back_wrapper.model" type="geomtools::plate_with_hole_model"]

x                 : real as length    = 290.0   mm
y                 : real as length    = 308.0   mm
z                 : real as length    =   0.012 mm
r_hole            : real as length    =  70.0   mm

material.ref : string = "basic::mylar"

visibility.hidden : boolean = false
visibility.color  : string = "green"


############################################################################
[name="gveto_light_guide_inner.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "cylinder"
r                 : real as length    =   64.0 mm
z                 : real as length    =    3.0 mm

material.ref : string = "basic::plexiglass"

visibility.hidden : boolean = false
visibility.color  : string = "blue"


###################################################################################
[name="gveto_module_inner_front_chock.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length    = 292.0 mm
y                 : real as length    = 311.5 mm
z                 : real as length    =   0.988 mm

material.ref : string = "tracking_gas"

visibility.hidden : boolean = false
visibility.color  : string = "cyan"


#################################################################
[name="gveto_module_inner.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 4

stacked.model_0   : string = "gveto_light_guide_inner.model"
stacked.label_0   : string = "light_guide_inner"

stacked.limit_min_1 : real as length  = +0.006 mm
stacked.model_1   : string = "gveto_scin_back_wrapper.model"
stacked.label_1   : string = "back_wrapping"

stacked.model_2   : string = "gveto_wrapped_scin_block.model"
stacked.label_2   : string = "wrapped_scintillator_block"

stacked.model_3   : string = "gveto_module_inner_front_chock.model"
stacked.label_3   : string = "front_chock"

material.ref : string = "tracking_gas"

visibility.hidden : boolean = false
visibility.color  : string = "cyan"


###############
# In air part #
###############


###############################################################################
[name="gveto_beam_support_plate.model" type="geomtools::plate_with_hole_model"]

x                 : real as length   = 292.0 mm
y                 : real as length   = 311.5 mm
z                 : real as length   =   6.5 mm
r_hole            : real as length   =  75.0 mm

material.ref : string = "basic::iron"

visibility.hidden : boolean = false
visibility.color  : string = "red"


##########################################################################
[name="gveto_magnetic_shield.model" type="geomtools::simple_shaped_model"]

shape_type   : string = "tube"
inner_r      : real as length   =   73.3 mm
outer_r      : real as length   =   74.8 mm
z            : real as length   =  250.0 mm

material.ref : string = "snemo::mu_metal"

visibility.hidden          : boolean = false
visibility.color           : string = "magenta"


###########################################################################################
[name="gveto_light_guide_outer.model" type="geomtools::spherical_extrusion_cylinder_model"]

r                 : real as length    =  64.0 mm
z                 : real as length    =  72.0 mm
r_sphere          : real as length    =  82.0 mm
r_extrusion       : real as length    =  62.5 mm
bottom            : boolean = true

material.ref : string = "basic::plexiglass"

visibility.hidden : boolean = false
visibility.color  : string = "blue"


#######################################################################
[name="gveto_module_outer.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length   = 292.0 mm
y          : real as length   = 311.5 mm
z          : real as length   = 325.0 mm

material.ref : string = "lab_air"

visibility.hidden : boolean = false
visibility.color  : string = "grey"

internal_item.labels  : string[5] = \
   "magnetic_shield" \
   "beam_support" \
   "light_guide" \
   "pmt"  \
   "pmt_base"

internal_item.placement.magnetic_shield : string = "0 0 31 (mm)"
internal_item.model.magnetic_shield     : string = "gveto_magnetic_shield.model"

internal_item.placement.beam_support    : string = "0 0 159.25 (mm)"
internal_item.model.beam_support        : string = "gveto_beam_support_plate.model"

internal_item.placement.light_guide     : string = "0 0 126 (mm)"
internal_item.model.light_guide         : string = "gveto_light_guide_outer.model"

internal_item.placement.pmt             : string = "0 0 29 (mm)"
internal_item.model.pmt                 : string = "PMT_HAMAMATSU_R6594"

internal_item.placement.pmt_base        : string = "0 0 -75 (mm)"
internal_item.model.pmt_base            : string = "PMT_HAMAMATSU_R6594.base"


###########################################################
[name="gveto_module.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 2

stacked.model_0   : string = "gveto_module_outer.model"
stacked.label_0   : string = "gveto_module_outer"

stacked.model_1   : string = "gveto_module_inner.model"
stacked.label_1   : string = "gveto_module_inner"

material.ref : string = "tracking_gas"


# end of gveto_module.geom
