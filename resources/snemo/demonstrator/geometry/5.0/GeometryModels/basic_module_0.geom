# Basic module geometry
# The SuperNEMO demonstrator module is built from an assembly
# of 5 submodules:
#
#                         ^ y
#                         :
#                         : right
#                         :
#                        +-+
#         +----+         |:|         +----+
#         |    |+-------+|:|+-------+|    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |     stacking axis
# --------|----||-------||z||-------||----|-------> x
#  back   |    ||       ||:||       ||    | front
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    |+-------+|:|+-------+|    |
#         +----+ tracker |:| tracker +----+
#          calo          +-+          calo
#                       source
#                         :
#                         : left
#                         :
#
[name="module_basic.model" type="geomtools::stacked_model"]
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
  mapping.daughter_id.calorimeter_back_submodule  : string  = "[calorimeter_submodule: side=0]"
  mapping.daughter_id.tracker_back_submodule      : string  = "[tracker_submodule:     side=0]"
  mapping.daughter_id.source_submodule            : string  = "[source_submodule]"
  mapping.daughter_id.tracker_front_submodule     : string  = "[tracker_submodule:     side=1]"
  mapping.daughter_id.calorimeter_front_submodule : string  = "[calorimeter_submodule: side=1]"

