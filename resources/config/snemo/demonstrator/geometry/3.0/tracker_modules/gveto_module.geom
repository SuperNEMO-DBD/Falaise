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
x                 : real   = 290.0 # mm
y                 : real   = 308.0 # mm
z                 : real   = 150.0 # mm
length_unit       : string = "mm"
material.ref      : string = "Dubna_PS_scintillator"
visibility.hidden : boolean = 0
visibility.color  : string  = "blue"

# Sensitive detector category:
sensitive.category                : string = "gveto.SD"
sensitive.record_alpha_quenching  : boolean = 1
sensitive.record_track_id         : boolean = 1
sensitive.record_primary_particle : boolean = 1


#############################################################################
[name="gveto_scin_front_wrapper.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 290.0 # mm
y                 : real   = 308.0 # mm
z                 : real   =   0.012 # mm
length_unit       : string = "mm"
material.ref      : string = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


#######################################################################################
[name="gveto_scin_side_x_external_wrapper.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   =   0.012 # mm
y                 : real   = 308.0   # mm
z                 : real   = 150.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


#######################################################################################
[name="gveto_scin_side_x_internal_wrapper.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   =   0.6 # mm
y                 : real   = 308.0 # mm
z                 : real   = 150.0 # mm
length_unit       : string = "mm"
material.ref      : string = "std::pfte"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


#############################################################################
[name="gveto_scin_side_x_left_wrapper.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "x"
stacked.number_of_items : integer = 2
stacked.model_0         : string  = "gveto_scin_side_x_external_wrapper.model"
stacked.label_0         : string  = "external_wrapping"
stacked.model_1         : string  = "gveto_scin_side_x_internal_wrapper.model"
stacked.label_1         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


##############################################################################
[name="gveto_scin_side_x_right_wrapper.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "x"
stacked.number_of_items : integer = 2
stacked.model_1         : string  = "gveto_scin_side_x_external_wrapper.model"
stacked.label_1         : string  = "external_wrapping"
stacked.model_0         : string  = "gveto_scin_side_x_internal_wrapper.model"
stacked.label_0         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


# ##############################################################################
# [name="gveto_scin_side_x_wrapper.model" type="geomtools::simple_shaped_model"]
# shape_type        : string = "box"
# x                 : real   =   0.6 # mm
# y                 : real   = 308.0 # mm
# z                 : real   = 150.0 # mm
# length_unit       : string = "mm"
# material.ref      : string = "std::pfte"
# visibility.hidden : boolean = 0
# visibility.color  : string  = "green"


#######################################################################################
[name="gveto_scin_side_y_internal_wrapper.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 290.0 # mm
y                 : real   =   0.6 # mm
z                 : real   = 150.0 # mm
length_unit       : string = "mm"
material.ref      : string = "std::pfte"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


#######################################################################################
[name="gveto_scin_side_y_external_wrapper.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 290.0   # mm
y                 : real   =   0.012 # mm
z                 : real   = 150.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


###############################################################################
[name="gveto_scin_side_y_bottom_wrapper.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "y"
stacked.number_of_items : integer = 2
stacked.model_0         : string  = "gveto_scin_side_y_external_wrapper.model"
stacked.label_0         : string  = "external_wrapping"
stacked.model_1         : string  = "gveto_scin_side_y_internal_wrapper.model"
stacked.label_1         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


############################################################################
[name="gveto_scin_side_y_top_wrapper.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "y"
stacked.number_of_items : integer = 2
stacked.model_1         : string  = "gveto_scin_side_y_external_wrapper.model"
stacked.label_1         : string  = "external_wrapping"
stacked.model_0         : string  = "gveto_scin_side_y_internal_wrapper.model"
stacked.label_0         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"

# ##############################################################################
# [name="gveto_scin_side_y_wrapper.model" type="geomtools::simple_shaped_model"]
# shape_type        : string = "box"
# x                 : real   = 290.0 # mm
# y                 : real   =   0.6 # mm
# z                 : real   = 150.0 # mm
# length_unit       : string = "mm"
# material.ref      : string = "std::pfte"
# visibility.hidden : boolean = 0
# visibility.color  : string  = "green"


################################################################################
[name="gveto_wrapped_scin_block.model" type="geomtools::surrounded_boxed_model"]
material.ref           : string  = "tracking_gas"

surrounded.model       : string  = "gveto_scin_block.model"
surrounded.label       : string  = "scin_block"

surrounded.top_model   : string  = "gveto_scin_front_wrapper.model"
surrounded.top_label   : string  = "top_wrapper"

surrounded.left_model  : string  = "gveto_scin_side_y_bottom_wrapper.model"
surrounded.left_label  : string  = "left_wrapper"

surrounded.right_model : string  = "gveto_scin_side_y_top_wrapper.model"
surrounded.right_label : string  = "right_wrapper"

surrounded.front_model : string  = "gveto_scin_side_x_right_wrapper.model"
surrounded.front_label : string  = "front_wrapper"

surrounded.back_model  : string  = "gveto_scin_side_x_left_wrapper.model"
surrounded.back_label  : string  = "back_wrapper"

visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 0
visibility.color          : string  = "grey"

mapping.daughter_id.scin_block  : string = "[gveto_block]"
mapping.daughter_id.top_wrapper : string = "[gveto_wrapper]"


##############################################################################
[name="gveto_scin_back_wrapper.model" type="geomtools::plate_with_hole_model"]
x                 : real    = 290.0   # mm
y                 : real    = 308.0   # mm
z                 : real    =   0.012 # mm
r_hole            : real    =  70.0   # mm
length_unit       : string  = "mm"
material.ref      : string  = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


############################################################################
[name="gveto_light_guide_inner.model" type="geomtools::simple_shaped_model"]
shape_type        : string  = "cylinder"
r                 : real    =   64.0 # mm
z                 : real    =    3.0 # mm
length_unit       : string  = "mm"
material.ref      : string  = "std::plexiglass"
visibility.hidden : boolean = 0
visibility.color  : string  = "blue"


###################################################################################
[name="gveto_module_inner_front_chock.model" type="geomtools::simple_shaped_model"]
shape_type        : string  = "box"
x                 : real    = 292.0 # mm
y                 : real    = 311.5 # mm
z                 : real    =   0.988 # mm
length_unit       : string  = "mm"
material.ref      : string  = "tracking_gas"
visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"


#################################################################
[name="gveto_module_inner.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
length_unit             : string = "mm"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 4

stacked.model_0   : string  = "gveto_light_guide_inner.model"
stacked.label_0   : string  = "light_guide_inner"

stacked.limit_min_1 : real  = +0.006
stacked.model_1   : string  = "gveto_scin_back_wrapper.model"
stacked.label_1   : string  = "back_wrapping"

stacked.model_2   : string  = "gveto_wrapped_scin_block.model"
stacked.label_2   : string  = "wrapped_scintillator_block"

stacked.model_3   : string  = "gveto_module_inner_front_chock.model"
stacked.label_3   : string  = "front_chock"

visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"


###############
# In air part #
###############


###############################################################################
[name="gveto_beam_support_plate.model" type="geomtools::plate_with_hole_model"]
length_unit       : string = "mm"
x                 : real   = 292.0 # mm
y                 : real   = 311.5 # mm
z                 : real   =   6.5 # mm
r_hole            : real   =  75.0 # mm
material.ref      : string  = "std::iron"
visibility.hidden : boolean = 0
visibility.color  : string  = "red"


##########################################################################
[name="gveto_magnetic_shield.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "tube"
inner_r      : real   =   73.3 # mm
outer_r      : real   =   74.8 # mm
z            : real   =  250.0 # mm
length_unit  : string = "mm"
material.ref : string = "snemo::mu_metal"
visibility.hidden          : boolean = 0
visibility.color           : string  = "magenta"


###########################################################################################
[name="gveto_light_guide_outer.model" type="geomtools::spherical_extrusion_cylinder_model"]
length_unit       : string  = "mm"
r                 : real    =  64.0
z                 : real    =  72.0
r_sphere          : real    =  82.0
r_extrusion       : real    =  62.5
bottom            : boolean = 1
material.ref      : string  = "std::plexiglass"
visibility.hidden : boolean = 0
visibility.color  : string  = "blue"


#######################################################################
[name="gveto_module_outer.model" type="geomtools::simple_shaped_model"]
shape_type              : string = "box"
material.ref            : string = "lab_air"
length_unit             : string = "mm"
x                       : real   = 292.0
y                       : real   = 311.5
z                       : real   = 325.
visibility.hidden       : boolean = 0
visibility.color        : string  = "grey"

internal_item.labels  : string[5] = \
   "magnetic_shield" \
   "beam_support" \
   "light_guide" \
   "pmt"  \
   "pmt_base"

internal_item.placement.magnetic_shield : string  = "0 0 31 (mm)"
internal_item.model.magnetic_shield     : string  = "gveto_magnetic_shield.model"

internal_item.placement.beam_support    : string  = "0 0 159.25 (mm)"
internal_item.model.beam_support        : string  = "gveto_beam_support_plate.model"

internal_item.placement.light_guide     : string  = "0 0 126 (mm)"
internal_item.model.light_guide         : string  = "gveto_light_guide_outer.model"

internal_item.placement.pmt             : string  = "0 0 29 (mm)"
internal_item.model.pmt                 : string  = "PMT_HAMAMATSU_R6594"

internal_item.placement.pmt_base        : string  = "0 0 -75 (mm)"
internal_item.model.pmt_base            : string  = "PMT_HAMAMATSU_R6594.base"


###########################################################
[name="gveto_module.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
length_unit             : string  = "mm"
stacked.axis            : string  = "z"
stacked.number_of_items : integer = 2

stacked.model_0   : string  = "gveto_module_outer.model"
stacked.label_0   : string  = "gveto_module_outer"

stacked.model_1   : string  = "gveto_module_inner.model"
stacked.label_1   : string  = "gveto_module_inner"


# end of gveto_module.geom
