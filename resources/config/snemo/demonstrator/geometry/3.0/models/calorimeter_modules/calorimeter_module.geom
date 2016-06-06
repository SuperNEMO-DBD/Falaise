# -*- mode: conf-unix; -*-
# calorimeter_module.geom

#################################################################################
[name="calorimeter_scin_block_front.model" type="geomtools::simple_shaped_model"]
shape_type        : string  = "box"
length_unit       : string  = "mm"
x                 : real    = 256.0 # mm
y                 : real    = 256.0 # mm
z                 : real    =  31.0 # mm
material.ref      : string  = "ENVINET_PS_scintillator"
visibility.hidden : boolean = 0
visibility.color  : string  = "blue"

# Sensitive detector category:
sensitive.category : string = "calorimeter_SD"


#####################################################################################################
[name="calorimeter_scin_block_front_gas_curtain.model" type="geomtools::simple_shaped_model"]
shape_type        : string  = "box"
x                 : real    = 259.0   # mm
y                 : real    = 259.0   # mm
z                 : real    =   0.020 # mm
### FM+MB 2016-01-12: TO BE CHANGED in next release:
### z             : real    =   25 um
length_unit       : string  = "mm"
material.ref      : string  = "std::nylon"
visibility.hidden : boolean = 0
visibility.color  : string  = "magenta"

[name="calorimeter_scin_block_front_wrapper_front_stock.model" type="geomtools::simple_shaped_model"]
shape_type        : string  = "box"
x                 : real    = 259.0 # mm
y                 : real    = 259.0 # mm
z                 : real    =   0.968 # mm
### FM+MB 2016-01-12: TO BE CHANGED in next release:
### z             : real    =   0.96XXX mm
length_unit       : string  = "mm"
material.ref      : string  = "tracking_gas"
visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"


########################################################################################################
[name="calorimeter_scin_block_front_wrapper_front_wrapping.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 258.0 # mm
y                 : real   = 258.0 # mm
z                 : real   =   0.012 # mm
length_unit       : string = "mm"
material.ref      : string = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


#########################################################################################
[name="calorimeter_scin_block_front_wrapper_front.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "z"
stacked.number_of_items : integer = 3
stacked.model_0         : string  = "calorimeter_scin_block_front_wrapper_front_wrapping.model"
stacked.label_0         : string  = "wrapping"
stacked.model_1         : string  = "calorimeter_scin_block_front_wrapper_front_stock.model"
stacked.label_1         : string  = "stock"
stacked.model_2         : string  = "calorimeter_scin_block_front_gas_curtain.model"
stacked.label_2         : string  = "curtain"

visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"

mapping.daughter_id.wrapping : string = "[calorimeter_wrapper]"
mapping.daughter_id.curtain  : string = "[calorimeter_curtain]"

#########################################################################################################
[name="calorimeter_scin_block_front_wrapper_side_x_external.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   =   0.012 # mm
y                 : real   = 256.0   # mm
z                 : real   =  31.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"

#########################################################################################################
[name="calorimeter_scin_block_front_wrapper_side_x_internal.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   =   1.0   # mm
y                 : real   = 256.0   # mm
z                 : real   =  31.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::ptfe"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


###############################################################################################
[name="calorimeter_scin_block_front_wrapper_side_x_left.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "x"
stacked.number_of_items : integer = 2
stacked.model_0         : string  = "calorimeter_scin_block_front_wrapper_side_x_external.model"
stacked.label_0         : string  = "external_wrapping"
stacked.model_1         : string  = "calorimeter_scin_block_front_wrapper_side_x_internal.model"
stacked.label_1         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


################################################################################################
[name="calorimeter_scin_block_front_wrapper_side_x_right.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "x"
stacked.number_of_items : integer = 2
stacked.model_1         : string  = "calorimeter_scin_block_front_wrapper_side_x_external.model"
stacked.label_1         : string  = "external_wrapping"
stacked.model_0         : string  = "calorimeter_scin_block_front_wrapper_side_x_internal.model"
stacked.label_0         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


#########################################################################################################
[name="calorimeter_scin_block_front_wrapper_side_y_external.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 256.0   # mm
y                 : real   =   0.012 # mm
z                 : real   =  31.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


#########################################################################################################
[name="calorimeter_scin_block_front_wrapper_side_y_internal.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 256.0   # mm
y                 : real   =   1.0   # mm
z                 : real   =  31.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::ptfe"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


#################################################################################################
[name="calorimeter_scin_block_front_wrapper_side_y_bottom.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "y"
stacked.number_of_items : integer = 2
stacked.model_0         : string  = "calorimeter_scin_block_front_wrapper_side_y_external.model"
stacked.label_0         : string  = "external_wrapping"
stacked.model_1         : string  = "calorimeter_scin_block_front_wrapper_side_y_internal.model"
stacked.label_1         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


##############################################################################################
[name="calorimeter_scin_block_front_wrapper_side_y_top.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "y"
stacked.number_of_items : integer = 2
stacked.model_1         : string  = "calorimeter_scin_block_front_wrapper_side_y_external.model"
stacked.label_1         : string  = "external_wrapping"
stacked.model_0         : string  = "calorimeter_scin_block_front_wrapper_side_y_internal.model"
stacked.label_0         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


############################################################################################
[name="calorimeter_wrapped_scin_block_front.model" type="geomtools::surrounded_boxed_model"]
material.ref           : string  = "tracking_gas"

surrounded.model       : string  = "calorimeter_scin_block_front.model"
surrounded.label       : string  = "scin_block"

surrounded.top_model   : string  = "calorimeter_scin_block_front_wrapper_front.model"
surrounded.top_label   : string  = "top_wrapper"

surrounded.left_model  : string  = "calorimeter_scin_block_front_wrapper_side_y_bottom.model"
surrounded.left_label  : string  = "left_wrapper"

surrounded.right_model : string  = "calorimeter_scin_block_front_wrapper_side_y_top.model"
surrounded.right_label : string  = "right_wrapper"

surrounded.front_model : string  = "calorimeter_scin_block_front_wrapper_side_x_right.model"
surrounded.front_label : string  = "front_wrapper"

surrounded.back_model  : string  = "calorimeter_scin_block_front_wrapper_side_x_left.model"
surrounded.back_label  : string  = "back_wrapper"

visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"

mapping.daughter_id.scin_block : string = "[calorimeter_block:part=1]"


##########################################################################################
[name="calorimeter_scin_block_back.model" type="geomtools::spherical_extrusion_box_model"]
length_unit       : string  = "mm"
x                 : real    = 240.0 # mm
y                 : real    = 240.0 # mm
z                 : real    = 163.0 # mm
r_sphere          : real    = 112.0 # mm
r_extrusion       : real    = 105.356 # mm
bottom            : boolean = 1
material.ref      : string  = "ENVINET_PS_scintillator"
visibility.hidden : boolean = 0
visibility.color  : string  = "blue"

# Sensitive detector category:
sensitive.category : string = "calorimeter_SD"


########################################################################################################
[name="calorimeter_scin_block_back_wrapper_side_x_external.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   =   0.012 # mm
y                 : real   = 240.0   # mm
z                 : real   = 163.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"

########################################################################################################
[name="calorimeter_scin_block_back_wrapper_side_x_internal.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   =   1.0   # mm
y                 : real   = 240.0   # mm
z                 : real   = 163.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::ptfe"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


##############################################################################################
[name="calorimeter_scin_block_back_wrapper_side_x_left.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "x"
stacked.number_of_items : integer = 2
stacked.model_0         : string  = "calorimeter_scin_block_back_wrapper_side_x_external.model"
stacked.label_0         : string  = "external_wrapping"
stacked.model_1         : string  = "calorimeter_scin_block_back_wrapper_side_x_internal.model"
stacked.label_1         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


###############################################################################################
[name="calorimeter_scin_block_back_wrapper_side_x_right.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "x"
stacked.number_of_items : integer = 2
stacked.model_1         : string  = "calorimeter_scin_block_back_wrapper_side_x_external.model"
stacked.label_1         : string  = "external_wrapping"
stacked.model_0         : string  = "calorimeter_scin_block_back_wrapper_side_x_internal.model"
stacked.label_0         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


########################################################################################################
[name="calorimeter_scin_block_back_wrapper_side_y_external.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 240.0   # mm
y                 : real   =   0.012 # mm
z                 : real   = 163.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::mylar"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


########################################################################################################
[name="calorimeter_scin_block_back_wrapper_side_y_internal.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 240.0   # mm
y                 : real   =   1.0   # mm
z                 : real   = 163.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::ptfe"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


################################################################################################
[name="calorimeter_scin_block_back_wrapper_side_y_bottom.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "y"
stacked.number_of_items : integer = 2
stacked.model_0         : string  = "calorimeter_scin_block_back_wrapper_side_y_external.model"
stacked.label_0         : string  = "external_wrapping"
stacked.model_1         : string  = "calorimeter_scin_block_back_wrapper_side_y_internal.model"
stacked.label_1         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


#############################################################################################
[name="calorimeter_scin_block_back_wrapper_side_y_top.model" type="geomtools::stacked_model"]
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "y"
stacked.number_of_items : integer = 2
stacked.model_1         : string  = "calorimeter_scin_block_back_wrapper_side_y_external.model"
stacked.label_1         : string  = "external_wrapping"
stacked.model_0         : string  = "calorimeter_scin_block_back_wrapper_side_y_internal.model"
stacked.label_0         : string  = "internal_wrapping"
visibility.hidden         : boolean = 0
visibility.hidden_envelop : boolean = 1
visibility.color          : string  = "grey"


###############################################################################################
[name="calorimeter_scin_block_back_wrapper_back.model" type="geomtools::plate_with_hole_model"]
x                 : real    = 240.0   # mm
y                 : real    = 240.0   # mm
z                 : real    =   1.0   # mm
r_hole            : real    = 110.0   # mm
length_unit       : string  = "mm"
material.ref      : string  = "std::ptfe"
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


################################################################################################
[name="calorimeter_module_internal_support_plate.model" type="geomtools::plate_with_hole_model"]
x                 : real    = 240.0   # mm
y                 : real    = 240.0   # mm
z                 : real    =   3.0   # mm
r_hole            : real    = 111.0   # mm
length_unit       : string  = "mm"
material.ref      : string  = "std::iron"
visibility.hidden : boolean = 0
visibility.color  : string  = "red"


################################################################################################
[name="calorimeter_module_vertical_shielding_plate.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   =   3.0   # mm
y                 : real   = 251   # mm
z                 : real   = 466.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::iron"
visibility.hidden : boolean = 0
visibility.color  : string  = "red"


##################################################################################################
[name="calorimeter_module_horizontal_shielding_plate.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 245   # mm
y                 : real   =   3.0   # mm
z                 : real   = 466.0   # mm
length_unit       : string = "mm"
material.ref      : string = "std::iron"
visibility.hidden : boolean = 0
visibility.color  : string  = "red"


####################################################################################
[name="calorimeter_module_spacer_plate.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   =   3.0  # mm
y                 : real   = 230.0  # mm
z                 : real   = 420.0  # mm
length_unit       : string = "mm"
material.ref      : string = "std::plexiglass"
visibility.hidden : boolean = 0
visibility.color  : string  = "blue"


##########################################################################################
[name="calorimeter_module_closing_back_plate.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 245  # mm
y                 : real   = 245  # mm
z                 : real   =   6.0  # mm
length_unit       : string = "mm"
material.ref      : string = "std::plexiglass"
visibility.hidden : boolean = 0
visibility.color  : string  = "blue"


#######################################################################
[name="calorimeter_module.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 259.0   # mm
y                 : real   = 259.0   # mm
z                 : real   = 500.0   # mm
length_unit       : string = "mm"
material.ref      : string = "lab_air"
visibility.hidden           : boolean = 0
visibility.hidden_envelop   : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "grey"

internal_item.labels  : string[19] = \
   "front_block" \
   "back_block"  \
   "pmt"         \
   "pmt_base"    \
   "back_block_wrapping_back"   \
   "back_block_wrapping_left"   \
   "back_block_wrapping_right"  \
   "back_block_wrapping_top"    \
   "back_block_wrapping_bottom" \
   "support_plate_left"   \
   "support_plate_right"  \
   "support_plate_top"    \
   "support_plate_bottom" \
   "spacer_plate_left"    \
   "spacer_plate_right"   \
   "spacer_plate_top"     \
   "spacer_plate_bottom"  \
   "closing_back_plate"   \
   "internal_support_plate"

internal_item.placement.front_block : string  = "0 0 234 (mm)"
internal_item.model.front_block     : string  = "calorimeter_wrapped_scin_block_front.model"

internal_item.placement.back_block  : string  = "0 0 136.5 (mm)"
internal_item.model.back_block      : string  = "calorimeter_scin_block_back.model"

internal_item.placement.pmt         : string  = "0 0 13 (mm)"
internal_item.model.pmt             : string  = "PMT_HAMAMATSU_R5912"

internal_item.placement.pmt_base    : string  = "0 0 -127 (mm)"
internal_item.model.pmt_base        : string  = "PMT_HAMAMATSU_R5912.base"

internal_item.placement.back_block_wrapping_back   : string  = "0    0  54 (mm)"
internal_item.model.back_block_wrapping_back       : string  = "calorimeter_scin_block_back_wrapper_back.model"

internal_item.placement.back_block_wrapping_left   : string  = "-121 0 136.5 (mm)"
internal_item.model.back_block_wrapping_left       : string  = "calorimeter_scin_block_back_wrapper_side_x_left.model"

internal_item.placement.back_block_wrapping_right  : string  = "+121 0 136.5 (mm)"
internal_item.model.back_block_wrapping_right      : string  = "calorimeter_scin_block_back_wrapper_side_x_right.model"

internal_item.placement.back_block_wrapping_top    : string  = "0 +121 136.5 (mm)"
internal_item.model.back_block_wrapping_top        : string  = "calorimeter_scin_block_back_wrapper_side_y_top.model"

internal_item.placement.back_block_wrapping_bottom : string  = "0 -121 136.5 (mm)"
internal_item.model.back_block_wrapping_bottom     : string  = "calorimeter_scin_block_back_wrapper_side_y_bottom.model"

internal_item.placement.support_plate_left         : string  = "-124.0 0 -17 (mm)"
internal_item.model.support_plate_left             : string  = "calorimeter_module_vertical_shielding_plate.model"

internal_item.placement.support_plate_right        : string  = "+124.0 0 -17 (mm)"
internal_item.model.support_plate_right            : string  = "calorimeter_module_vertical_shielding_plate.model"

internal_item.placement.support_plate_top          : string  = "0 -124.0 -17 (mm)"
internal_item.model.support_plate_top              : string  = "calorimeter_module_horizontal_shielding_plate.model"

internal_item.placement.support_plate_bottom       : string  = "0 +124.0 -17 (mm)"
internal_item.model.support_plate_bottom           : string  = "calorimeter_module_horizontal_shielding_plate.model"

internal_item.placement.spacer_plate_left         : string  = "-128.0 0 0 (mm)"
internal_item.model.spacer_plate_left             : string  = "calorimeter_module_spacer_plate.model"

internal_item.placement.spacer_plate_right        : string  = "+128.0 0 0 (mm)"
internal_item.model.spacer_plate_right            : string  = "calorimeter_module_spacer_plate.model"

internal_item.placement.spacer_plate_top          : string  = "0 -128.0 0 (mm) / z 90 (degree)"
internal_item.model.spacer_plate_top              : string  = "calorimeter_module_spacer_plate.model"

internal_item.placement.spacer_plate_bottom       : string  = "0 +128.0 0 (mm) / z 90 (degree)"
internal_item.model.spacer_plate_bottom           : string  = "calorimeter_module_spacer_plate.model"

internal_item.placement.closing_back_plate        : string  = "0 0 -247 (mm)"
internal_item.model.closing_back_plate            : string  = "calorimeter_module_closing_back_plate.model"

internal_item.placement.internal_support_plate    : string  = "0 0 +48 (mm)"
internal_item.model.internal_support_plate        : string  = "calorimeter_module_internal_support_plate.model"

mapping.daughter_id.back_block : string = "[calorimeter_block:part=0]"
# mapping.daughter_id.pmt        : string = "[calorimeter_pmt]"


################################################################################
[name="calorimeter_module_rotated.model" type="geomtools::rotated_boxed_model"]
material.ref              : string = "lab_air"
rotated.axis              : string  = "y"
rotated.special_angle     : string  = "90"
rotated.model             : string  = "calorimeter_module.model"


# end of calorimeter_module.geom
