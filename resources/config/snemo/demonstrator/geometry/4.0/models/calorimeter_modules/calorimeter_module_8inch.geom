# -*- mode: conf-unix; -*-
# calorimeter_module_8inch.geom

################################################################################################
# References:
# SuperNEMO Calorimeter demonstrator - Scintillator
# 8" Hamamatsu R5912 [A-233-4563]
#
#               z
#       +---------------+
#       |            .--|
#       |           /   |re
#       |    h     |  c |    a
#  - - -+- - - - - +- - + - - - - + - - - -
#       |          |    |      .
#       |           \   |   .
#       |            `--|.    rs
#       +---------------+
#
# z = 194 - 31 = 163
# h = 95
# c = z - h = 68
# rs = 120
# a = rs - c = 52
# re = sqrt(rs^2-a^2) = 108.148 = 108.15
#
[name="calorimeter_scin_block_8inch_back.model" type="geomtools::spherical_extrusion_box_model"]

#@config Back part of the 8" PMT scintillator block

x           : real as length = 236.0 mm
y           : real as length = 236.0 mm
z           : real as length = 163.0 mm
r_sphere    : real as length = 120.0 mm
r_extrusion : real as length = 108.15 mm
bottom      : boolean = true

material.ref : string  = "ENVINET_PS_scintillator"

visibility.hidden : boolean = false
visibility.color  : string  = "blue"

# Sensitive detector category:
sensitive.category : string = "calorimeter_SD"


#####################################################################################################
[name="calorimeter_scin_block_8inch_back_wrapper_back.model" type="geomtools::plate_with_hole_model"]

x      : real as length = 236.0 mm
y      : real as length = 236.0 mm
z      : real as length =   1.0 mm
r_hole : real as length = 102.0 mm

material.ref : string  = "basic::ptfe"

visibility.hidden : boolean = false
visibility.color  : string  = "green"


######################################################################################################
[name="calorimeter_module_internal_support_plate_8inch.model" type="geomtools::plate_with_hole_model"]

x       : real as length = 242.0 mm
y       : real as length = 242.0 mm
z       : real as length =   3.0 mm
r_hole  : real as length = 113.0 mm

material.ref : string  = "basic::iron"

visibility.hidden : boolean = false
visibility.color  : string  = "red"


#############################################################################
[name="calorimeter_module_8inch.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length = 259.0 mm
y          : real as length = 259.0 mm
z          : real as length = 500.0 mm

material.ref : string = "lab_air"

visibility.hidden           : boolean = false
visibility.hidden_envelop   : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "grey"

internal_item.labels : string[19] = \
   "front_block" \
   "back_block"  \
   "pmt"         \
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
   "closing_back_door"   \
   "internal_support_plate"

# PMT base is not set for now:
#  "pmt_base"

internal_item.placement.front_block : string  = "0 0 234 (mm)"
internal_item.model.front_block     : string  = "calorimeter_wrapped_scin_block_front.model"

internal_item.placement.back_block  : string  = "0 0 136.5 (mm)"
internal_item.model.back_block      : string  = "calorimeter_scin_block_8inch_back.model"

internal_item.placement.pmt         : string  = "0 0 11 (mm)"
internal_item.model.pmt             : string  = "calo.PMT_HAMAMATSU_R5912.model"

# internal_item.placement.pmt_base    : string  = "0 0 -129 (mm)"
# internal_item.model.pmt_base        : string  = "calo.PMT_HAMAMATSU_R5912.base.model"

internal_item.placement.back_block_wrapping_back   : string  = "0 0 54.5 (mm)"
internal_item.model.back_block_wrapping_back       : string  = "calorimeter_scin_block_8inch_back_wrapper_back.model"

internal_item.placement.back_block_wrapping_left   : string  = "-121 0 136.5 (mm)"
internal_item.model.back_block_wrapping_left       : string  = "calorimeter_scin_block_back_wrapper_side_x_left.model"

internal_item.placement.back_block_wrapping_right  : string  = "+121 0 136.5 (mm)"
internal_item.model.back_block_wrapping_right      : string  = "calorimeter_scin_block_back_wrapper_side_x_right.model"

internal_item.placement.back_block_wrapping_top    : string  = "0 +121 136.5 (mm)"
internal_item.model.back_block_wrapping_top        : string  = "calorimeter_scin_block_back_wrapper_side_y_top.model"

internal_item.placement.back_block_wrapping_bottom : string  = "0 -121 136.5 (mm)"
internal_item.model.back_block_wrapping_bottom     : string  = "calorimeter_scin_block_back_wrapper_side_y_bottom.model"

internal_item.placement.support_plate_left         : string  = "-123.0 0 3 (mm)"
internal_item.model.support_plate_left             : string  = "calorimeter_module_vertical_shielding_plate.model"

internal_item.placement.support_plate_right        : string  = "+123.0 0 3 (mm)"
internal_item.model.support_plate_right            : string  = "calorimeter_module_vertical_shielding_plate.model"

internal_item.placement.support_plate_top          : string  = "0 -123.0 3 (mm)"
internal_item.model.support_plate_top              : string  = "calorimeter_module_horizontal_shielding_plate.model"

internal_item.placement.support_plate_bottom       : string  = "0 +123.0 3 (mm)"
internal_item.model.support_plate_bottom           : string  = "calorimeter_module_horizontal_shielding_plate.model"

internal_item.placement.spacer_plate_left          : string  = "-128.0 0 3 (mm)"
internal_item.model.spacer_plate_left              : string  = "calorimeter_module_spacer_plate.model"

internal_item.placement.spacer_plate_right         : string  = "+128.0 0 3 (mm)"
internal_item.model.spacer_plate_right             : string  = "calorimeter_module_spacer_plate.model"

internal_item.placement.spacer_plate_top           : string  = "0 -128.0 3 (mm) / z 90 (degree)"
internal_item.model.spacer_plate_top               : string  = "calorimeter_module_spacer_plate.model"

internal_item.placement.spacer_plate_bottom        : string  = "0 +128.0 3 (mm) / z 90 (degree)"
internal_item.model.spacer_plate_bottom            : string  = "calorimeter_module_spacer_plate.model"

internal_item.placement.closing_back_plate         : string  = "0 0 -204 (mm)"
internal_item.model.closing_back_plate             : string  = "calorimeter_module_closing_back_plate.model"

internal_item.placement.closing_back_door          : string  = "0 0 -210 (mm)"
internal_item.model.closing_back_door              : string  = "calorimeter_module_closing_back_door.model"

internal_item.placement.internal_support_plate     : string  = "0 0 +51.5 (mm)"
internal_item.model.internal_support_plate         : string  = "calorimeter_module_internal_support_plate_8inch.model"

mapping.daughter_id.back_block : string = "[calorimeter_block:part=0]"
mapping.daughter_id.pmt        : string = "[calorimeter_pmt]"


#####################################################################################
[name="calorimeter_module_8inch_rotated.model" type="geomtools::rotated_boxed_model"]

rotated.axis          : string = "y"
rotated.special_angle : string = "90"
rotated.model         : string = "calorimeter_module_8inch.model"

material.ref : string = "lab_air"


# end of calorimeter_module_8inch.geom
