# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/module_0.geom

#########################
# Badic module geometry #
#########################

################################################################
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
###########################################################
[name="module_basic.model" type="geomtools::stacked_model"]

#@variant_section_only geometry:layout/if_basic|true

#@config Simplified demonstrator module

# stacked.number_of_items : integer = 1
# stacked.model_0   : string = "source_submodule.model"
# stacked.label_0   : string = "source_submodule"

#@description Stacking axis
stacked.axis            : string = "x"

#@description Number of stacking volumes
stacked.number_of_items : integer = 5

# Stacked models:
stacked.model_0   : string = "calorimeter_back_submodule.model"
stacked.model_1   : string = "tracker_back_submodule.model"
stacked.model_2   : string = "source_submodule.model"
stacked.model_3   : string = "tracker_front_submodule.model"
stacked.model_4   : string = "calorimeter_front_submodule.model"

# Labels associated to each stacked model:
stacked.label_0   : string = "calorimeter_back_submodule"
stacked.label_1   : string = "tracker_back_submodule"
stacked.label_2   : string = "source_submodule"
stacked.label_3   : string = "tracker_front_submodule"
stacked.label_4   : string = "calorimeter_front_submodule"

# Surrounding material:
material.ref    : string = "lab_air"

# Visibility properties:
# visibility.daughters.hidden : boolean = false
# visibility.hidden_envelope  : boolean = false
visibility.color            : string  = "grey"

# Mapping informations for daughter volumes:
mapping.daughter_id.calorimeter_back_submodule  : string  = "[calorimeter_submodule: side=0]"
mapping.daughter_id.tracker_back_submodule      : string  = "[tracker_submodule:     side=0]"
mapping.daughter_id.source_submodule            : string  = "[source_submodule]"
mapping.daughter_id.tracker_front_submodule     : string  = "[tracker_submodule:     side=1]"
mapping.daughter_id.calorimeter_front_submodule : string  = "[calorimeter_submodule: side=1]"


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/module_0.geom
