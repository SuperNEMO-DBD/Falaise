[name="commissioning_source_gap.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "box"
  x          : real as length = @variant(geometry:layout/if_half_commissioning/gap|0.25 mm)
  y          : real as length = 6000.0 mm
  z          : real as length = 4000.0 mm
  material.ref : string = "lab_air"
  visibility.color : string = "blue"


[name="commissioning_source_spot.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "cylinder"
  r          : real as length = 1.0 mm
  z          : real as length = 1.0 um
  material.ref : string = "lab_air"
  visibility.color : string = "blue"


[name="rotated_commissioning_source_spot.model" type="geomtools::rotated_boxed_model"]
  rotated.model         : string = "commissioning_source_spot.model"
  rotated.axis          : string = "y"
  rotated.special_angle : string = "90"
  material.ref : string = "lab_air"
  visibility.color : string = "grey"


[name="commissioning_source_plane.model" type="geomtools::grid_model"]
  x : real as length = 0.5 mm
  y : real as length = 6000.0 mm
  z : real as length = 4000.0 mm
  grid.daughter_label : string = "spot"
  grid.plane          : string = "yz"
  grid.model : string = "rotated_commissioning_source_spot.model"
    grid.y.number_of_items : integer = 113
    grid.y.step            : real as length = 44 mm
    grid.z.number_of_items : integer = 3
    grid.z.step            : real as length = 80 cm
  material.ref : string = "lab_air"
  visibility.color : string = "grey"
  mapping.daughter_id.spot : string = "[commissioning_source_spot:column+0,row+0]"


[name="half_module_closing_plate.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
  x          : real as length = 3.0 mm
  y          : real as length = 6000.0 mm
  z          : real as length = 4000.0 mm
  material.ref : string  = "basic::inox"
  visibility.color : string  = "blue"


######################################
# Half commissioning module geometry #
# The SuperNEMO demonstrator half module closed by a metal plate:
#
#                         ^ y
#                         :
#          commissioning  : right
#          source plane   :
#		      +-++-+
#                     | ||:|         +----+
#                     | ||:|+-------+|    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |     stacking axis
# --------------------|-||z||-------||----|-------> x
#  back               | ||:||       ||    | front
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:||       ||    |
#                     | ||:|+-------+|    |
#                     | ||:| tracker +----+
#                     +-++-+  front   calo
#                      closing       front
#                       plate
#                         :
#                         : left
#                         :
#
[name="half_module_commissioning.model" type="geomtools::stacked_model"]
  #@variant_section_only geometry:layout/if_half_commissioning|false
  #@config Simplified demonstrator module
  stacked.axis            : string = "x"
  stacked.number_of_items : integer = 5
    stacked.model_0 : string = "commissioning_source_plane.model"
    stacked.label_0 : string = "commissioning_source_plane"
    stacked.model_1 : string = "commissioning_source_gap.model"
    stacked.label_1 : string = "commissioning_source_gap"
    stacked.model_2 : string = "half_module_closing_plate.model"
    stacked.label_2 : string = "closing_plate"
    stacked.model_3 : string = "tracker_front_submodule.model"
    stacked.label_3 : string = "tracker_front_submodule"
    stacked.model_4 : string = "calorimeter_front_submodule.model"
    stacked.label_4 : string = "calorimeter_front_submodule"
  material.ref : string = "lab_air"
  visibility.color : string  = "grey"
  mapping.daughter_id.commissioning_source_plane  : string  = "[commissioning_source_plane]"
  mapping.daughter_id.closing_plate               : string  = "[source_like_plate]"
  mapping.daughter_id.tracker_front_submodule     : string  = "[tracker_submodule:     side=1]"
  mapping.daughter_id.calorimeter_front_submodule : string  = "[calorimeter_submodule: side=1]"

