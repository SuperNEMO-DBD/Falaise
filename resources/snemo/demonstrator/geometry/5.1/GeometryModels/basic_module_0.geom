# Basic module geometry
# The SuperNEMO demonstrator module is built from an assembly
# of 5 submodules + 8 magnetic coil panels :
#
#                         ^ y
#                         :
#                         : right
#                         :                 
#(corner4).------------------------------.  coil panel (corner 1)
#       .'               +-+ source       `.
#      |  +----+         |:|         +----+ |
#      |  |    |+-------+|:|+-------+|    | |
#      |  |    ||       ||:||       ||    | |
#      |  |    ||       ||:||       ||    | |coil panel
#      |  |    ||       ||:||       ||    | |
#      |  |    ||       ||:||       ||    | |
#      |  |    ||       ||:||       ||    | |
#      |  |    ||       ||:||       ||    | |   stacking axis
# -----|--|----||-------||z||-------||----|-|-----> x
# back |  |    ||       ||:||       ||    | |   front
#      |  |    ||       ||:||       ||    | |
#      |  |    ||       ||:||       ||    | |
#      |  |    ||tracker||:||tracker||    | |
#      |  |    ||       ||:||       ||calo| |
#      |  |calo||       ||:||       ||    | |
#      |  |    |+-------+|:|+-------+|    | |
#      |  +----+         |:|         +----+ |
#       .                +-+              .'
#(corner2).------------------------------' (corner 3)
#                         :
#                         : left
#                         :
#
[name="module_basic_old.model" type="geomtools::stacked_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
  stacked.axis            : string = "x"
  stacked.number_of_items : integer = 5
    stacked.model_0   : string = "calorimeter_back_submodule.model"
    stacked.label_0   : string = "calorimeter_back_submodule"
    stacked.model_1   : string = "tracker_back_submodule.model"
    stacked.label_1   : string = "tracker_back_submodule"
    stacked.model_2   : string = "source_submodule.model"
    stacked.label_2   : string = "source_submodule"
    stacked.model_3   : string = "tracker_front_submodule.model"
    stacked.label_3   : string = "tracker_front_submodule"
    stacked.model_4   : string = "calorimeter_front_submodule.model"
    stacked.label_4   : string = "calorimeter_front_submodule"
  material.ref    : string = "lab_air"
  visibility.color            : string  = "grey"
  visibility.hidden           : boolean = true
  visibility.envelop_hidden   : boolean = true
  visibility.daughters.hidden : boolean = true

  mapping.daughter_id.calorimeter_back_submodule  : string  = "[calorimeter_submodule: side=0]"
  mapping.daughter_id.tracker_back_submodule      : string  = "[tracker_submodule:     side=0]"
  mapping.daughter_id.source_submodule            : string  = "[source_submodule]"
  mapping.daughter_id.tracker_front_submodule     : string  = "[tracker_submodule:     side=1]"
  mapping.daughter_id.calorimeter_front_submodule : string  = "[calorimeter_submodule: side=1]"


[name="module_basic.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
  shape_type : string = "box"
    x : real as length = 2500 mm
    y : real as length = 6500 mm
    z : real as length = 4200 mm
  material.ref     : string = "lab_air"
  visibility.color : string  = "grey"
  visibility.hidden           : boolean = true
  visibility.envelop_hidden   : boolean = true
  visibility.daughters.hidden : boolean = true

  internal_item.labels : string[13] = \
        "calorimeter_back_submodule" \
        "calorimeter_front_submodule" \
        "tracker_front_submodule" \
        "tracker_back_submodule" \
        "source_submodule" \
        "magnetic_coil_mo_wall" \
        "magnetic_coil_tu_wall" \
        "magnetic_coil_fr_wall" \
        "magnetic_coil_it_wall" \
        "magnetic_coil_corner1" \
        "magnetic_coil_corner2" \
        "magnetic_coil_corner3" \
        "magnetic_coil_corner4" 

        internal_item.model.calorimeter_back_submodule      : string = "calorimeter_back_submodule.model"
        internal_item.placement.calorimeter_back_submodule  : string = "-684 0 0 (mm)"

        internal_item.model.calorimeter_front_submodule     : string = "calorimeter_front_submodule.model"
        internal_item.placement.calorimeter_front_submodule : string = "684 0 0 (mm)"

        internal_item.model.tracker_front_submodule     : string = "tracker_front_submodule.model"
        internal_item.placement.tracker_front_submodule : string = "+231.5 0 0 (mm)"

        internal_item.model.tracker_back_submodule      : string = "tracker_back_submodule.model"
        internal_item.placement.tracker_back_submodule  : string = "-231.5 0 0 (mm)"

        internal_item.model.source_submodule     : string = "source_submodule.model"
        internal_item.placement.source_submodule : string = "0 0 0 (mm)"


        internal_item.model.magnetic_coil_mo_wall     : string = "magnetic_coil_mo_tu_wall.model"
        internal_item.placement.magnetic_coil_mo_wall : string = "0 3025 0 (mm) / z 180 (degree)"

	internal_item.model.magnetic_coil_tu_wall     : string = "magnetic_coil_mo_tu_wall.model"
        internal_item.placement.magnetic_coil_tu_wall : string = "0 -3025 0 (mm)"

        internal_item.model.magnetic_coil_fr_wall     : string = "magnetic_coil_fr_it_wall.model"
        internal_item.placement.magnetic_coil_fr_wall : string = "-960 0 0 (mm)"

        internal_item.model.magnetic_coil_it_wall     : string = "magnetic_coil_fr_it_wall.model"
        internal_item.placement.magnetic_coil_it_wall : string = "960 0 0 (mm) / z 180 (degree)"

        internal_item.model.magnetic_coil_corner1     : string = "magnetic_coil_tilted_wall.model"
        internal_item.placement.magnetic_coil_corner1 : string = "680 2830 0 (mm) / z 140 (degree)"

	internal_item.model.magnetic_coil_corner2     : string = "magnetic_coil_tilted_wall.model"
        internal_item.placement.magnetic_coil_corner2 : string = "-680 -2830 0 (mm) / z 320 (degree)"

	internal_item.model.magnetic_coil_corner3     : string = "magnetic_coil_tilted_wall.model"
        internal_item.placement.magnetic_coil_corner3 : string = "680 -2830 0 (mm) / z 40 (degree)"

	internal_item.model.magnetic_coil_corner4     : string = "magnetic_coil_tilted_wall.model"
        internal_item.placement.magnetic_coil_corner4 : string = "-680 2830 0 (mm) / z 220 (degree)"

        mapping.daughter_id.magnetic_coil_it_wall     : string = "[coil_wall:wall=0]"
        mapping.daughter_id.magnetic_coil_fr_wall     : string = "[coil_wall:wall=1]"
        mapping.daughter_id.magnetic_coil_mo_wall     : string = "[coil_wall:wall=2]"
        mapping.daughter_id.magnetic_coil_tu_wall     : string = "[coil_wall:wall=3]"
        mapping.daughter_id.magnetic_coil_corner1     : string = "[coil_wall:wall=6]"
        mapping.daughter_id.magnetic_coil_corner2     : string = "[coil_wall:wall=8]"
        mapping.daughter_id.magnetic_coil_corner3     : string = "[coil_wall:wall=9]"
        mapping.daughter_id.magnetic_coil_corner4     : string = "[coil_wall:wall=7]"

        mapping.daughter_id.calorimeter_back_submodule  : string  = "[calorimeter_submodule: side=0]"
        mapping.daughter_id.tracker_back_submodule      : string  = "[tracker_submodule:     side=0]"
        mapping.daughter_id.source_submodule            : string  = "[source_submodule]"
        mapping.daughter_id.tracker_front_submodule     : string  = "[tracker_submodule:     side=1]"
        mapping.daughter_id.calorimeter_front_submodule : string  = "[calorimeter_submodule: side=1]"

# end
