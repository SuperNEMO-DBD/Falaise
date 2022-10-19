################################################################################################
# References:
# SuperNEMO Calorimeter demonstrator - Scintillator
# 5" Hamamatsu R6594 [A-233-4562]
#
#               z
#       +---------------+
#       |            .--|
#       |           /   |re
#       |    h     |  c |    a
#  - - -+- - - - - +- - + - - - - + - -
#       |          |    |      .
#       |           \   |   .
#       |            `--|.    rs
#       +---------------+
#
# z = 194 - 31 = 163
# h = ?
# c = z - h = ?
# rs = 82.5
# a = rs - c = ?
# re = sqrt(rs^2-a^2) = 70.1
#
[name="calorimeter_scin_block_5inch_back.model" type="geomtools::spherical_extrusion_box_model"]
  #@config Back part of the 5" PMT scintillator block
  x           : real as length = 240.0 mm
  y           : real as length = 240.0 mm
  z           : real as length = 163.0 mm
  r_sphere    : real as length =  82.5 mm
  r_extrusion : real as length =  70.1 mm
  bottom      : boolean = true
  material.ref : string  = "ENVINET_PS_scintillator"
  visibility.hidden : boolean = false
  visibility.color  : string  = "blue"
  sensitive.category : string = "calorimeter_SD"


[name="calorimeter_scin_block_5inch_back_wrapper_back.model" type="geomtools::plate_with_hole_model"]
  x      : real as length = 240.0 mm
  y      : real as length = 240.0 mm
  z      : real as length =   1.0 mm
  r_hole : real as length =  65.0 mm
  material.ref : string  = "basic::PTFE"
  visibility.hidden : boolean =  false
  visibility.color  : string  = "green"


[name="calorimeter_module_internal_support_plate_5inch.model" type="geomtools::plate_with_hole_model"]
  x      : real as length = 240.0 mm
  y      : real as length = 240.0 mm
  z      : real as length =   3.0 mm
  r_hole : real as length =  70.0 mm
  material.ref : string  = "basic::iron"
  visibility.hidden : boolean = false
  visibility.color  : string  = "red"


[name="calorimeter_module_5inch.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "box"
    x : real as length = 259.0 mm
    y : real as length = 259.0 mm
    z : real as length = 500.0 mm
  material.ref : string = "lab_air"
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
    internal_item.placement.front_block : string  = "0 0 234 (mm)"
    internal_item.model.front_block     : string  = "calorimeter_wrapped_scin_block_front.model"
    internal_item.placement.back_block : string  = "0 0 136.5 (mm)"
    internal_item.model.back_block     : string  = "calorimeter_scin_block_5inch_back.model"
    internal_item.placement.pmt : string  = "0 0 3 (mm)"
    internal_item.model.pmt     : string  = "calo.PMT_HAMAMATSU_R6594.model"
    internal_item.placement.back_block_wrapping_back : string  = "0 0 54 (mm)"
    internal_item.model.back_block_wrapping_back     : string  = "calorimeter_scin_block_5inch_back_wrapper_back.model"
    internal_item.placement.back_block_wrapping_left : string  = "-121 0 136.5 (mm)"
    internal_item.model.back_block_wrapping_left     : string  = "calorimeter_scin_block_back_wrapper_side_x_left.model"
    internal_item.placement.back_block_wrapping_right : string  = "+121 0 136.5 (mm)"
    internal_item.model.back_block_wrapping_right     : string  = "calorimeter_scin_block_back_wrapper_side_x_right.model"
    internal_item.placement.back_block_wrapping_top : string  = "0 +121 136.5 (mm)"
    internal_item.model.back_block_wrapping_top     : string  = "calorimeter_scin_block_back_wrapper_side_y_top.model"
    internal_item.placement.back_block_wrapping_bottom : string  = "0 -121 136.5 (mm)"
    internal_item.model.back_block_wrapping_bottom     : string  = "calorimeter_scin_block_back_wrapper_side_y_bottom.model"
    internal_item.placement.support_plate_left : string  = "-123.0 0 3 (mm)"
    internal_item.model.support_plate_left     : string  = "calorimeter_module_vertical_shielding_plate.model"
    internal_item.placement.support_plate_right : string  = "+123.0 0 3 (mm)"
    internal_item.model.support_plate_right     : string  = "calorimeter_module_vertical_shielding_plate.model"
    internal_item.placement.support_plate_top : string  = "0 -123.0 3 (mm)"
    internal_item.model.support_plate_top     : string  = "calorimeter_module_horizontal_shielding_plate.model"
    internal_item.placement.support_plate_bottom : string  = "0 +123.0 3 (mm)"
    internal_item.model.support_plate_bottom     : string  = "calorimeter_module_horizontal_shielding_plate.model"
    internal_item.placement.spacer_plate_left : string  = "-128.0 0 3 (mm)"
    internal_item.model.spacer_plate_left     : string  = "calorimeter_module_spacer_plate.model"
    internal_item.placement.spacer_plate_right : string  = "+128.0 0 3 (mm)"
    internal_item.model.spacer_plate_right     : string  = "calorimeter_module_spacer_plate.model"
    internal_item.placement.spacer_plate_top : string  = "0 -128.0 3 (mm) / z 90 (degree)"
    internal_item.model.spacer_plate_top     : string  = "calorimeter_module_spacer_plate.model"
    internal_item.placement.spacer_plate_bottom : string  = "0 +128.0 3 (mm) / z 90 (degree)"
    internal_item.model.spacer_plate_bottom     : string  = "calorimeter_module_spacer_plate.model"
    internal_item.placement.closing_back_plate : string  = "0 0 -204 (mm)"
    internal_item.model.closing_back_plate     : string  = "calorimeter_module_closing_back_plate.model"
    internal_item.placement.closing_back_door : string  = "0 0 -210 (mm)"
    internal_item.model.closing_back_door     : string  = "calorimeter_module_closing_back_door.model"
    internal_item.placement.internal_support_plate : string  = "0 0 +48 (mm)"
    internal_item.model.internal_support_plate     : string  = "calorimeter_module_internal_support_plate_5inch.model"
  mapping.daughter_id.back_block : string = "[calorimeter_block:part=0]"
  mapping.daughter_id.pmt        : string = "[calorimeter_pmt]"
  mapping.daughter_id.internal_support_plate : string = "[calorimeter_pmt_support_plate]"
  mapping.daughter_id.support_plate_left     : string = "[calorimeter_shieding_plate:side=0]"
  mapping.daughter_id.support_plate_right    : string = "[calorimeter_shieding_plate:side=1]"
  mapping.daughter_id.support_plate_bottom   : string = "[calorimeter_shieding_plate:side=2]"
  mapping.daughter_id.support_plate_top      : string = "[calorimeter_shieding_plate:side=3]"
  mapping.daughter_id.spacer_plate_left      : string = "[calorimeter_spacer:side=0]"
  mapping.daughter_id.spacer_plate_right     : string = "[calorimeter_spacer:side=1]"
  mapping.daughter_id.spacer_plate_bottom    : string = "[calorimeter_spacer:side=2]"
  mapping.daughter_id.spacer_plate_top       : string = "[calorimeter_spacer:side=3]"
  visibility.hidden               : boolean = false
  visibility.hidden_envelope      : boolean = true
  visibility.color                : string  = "grey"
  visibility.daughters.hidden     : boolean = false
  visibility.daughters.pmt.hidden : boolean = true


[name="calorimeter_module_5inch_rotated.model" type="geomtools::rotated_boxed_model"]
  material.ref : string = "lab_air"
  rotated.axis          : string = "y"
  rotated.special_angle : string = "90"
  rotated.model         : string = "calorimeter_module_5inch.model"

