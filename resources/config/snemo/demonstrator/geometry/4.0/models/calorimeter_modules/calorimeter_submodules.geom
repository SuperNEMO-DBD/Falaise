# -*- mode: conf-unix; -*-
# calorimeter_submodules.geom

# ###############################################################################
# [name="calorimeter_tightness_film.model" type="geomtools::simple_shaped_model"]
# shape_type : string  = "box"
# x          : real as length = @variant(geometry:calo_film_thickness|25.0 um)
# y          : real as length = 5180.0 mm
# z          : real as length = 3370.0 mm
# material.ref : string  = "basic::nylon"
# # Visibility properties:
# visibility.color            : string  = "green"


# ##########################################################################
# [name="calorimeter_front_submodule.model" type="geomtools::stacked_model"]
# #@description Stacking axis
# stacked.axis            : string = "x"
# #@description Number of stacked volumes
# stacked.number_of_items : integer = 2
# # Stacked models:
# stacked.model_0   : string = "calorimeter_tightness_film.model"
# stacked.model_1   : string = "calorimeter_front_wall.model"
# # Labels associated to each stacked model:
# stacked.label_0   : string = "tightness_film"
# stacked.label_1   : string = "wall"
# # Surrounding material:
# material.ref    : string = "lab_air"
# # Visibility properties:
# # visibility.daughters.hidden : boolean = false
# visibility.hidden_envelope  : boolean = true
# visibility.color            : string  = "grey"
# mapping.daughter_id.tightness_film : string = "[calorimeter_tightness_film]"
# mapping.daughter_id.wall           : string = "[calorimeter_wall]"


# #########################################################################
# [name="calorimeter_back_submodule.model" type="geomtools::stacked_model"]
# #@description Stacking axis
# stacked.axis            : string = "x"
# #@description Number of stacked volumes
# stacked.number_of_items : integer = 2
# # Stacked models:
# stacked.model_0   : string = "calorimeter_back_wall.model"
# stacked.model_1   : string = "calorimeter_tightness_film.model"
# # Labels associated to each stacked model:
# stacked.label_0   : string = "wall"
# stacked.label_1   : string = "tightness_film"
# # Surrounding material:
# material.ref    : string = "lab_air"
# # Visibility properties:
# # visibility.daughters.hidden : boolean = false
# visibility.hidden_envelope  : boolean = true
# visibility.color            : string  = "grey"
# mapping.daughter_id.tightness_film : string = "[calorimeter_tightness_film]"
# mapping.daughter_id.wall           : string = "[calorimeter_wall]"


################################################################################
[name="calorimeter_front_submodule.model" type="geomtools::simple_shaped_model"]
shape_type : string  = "box"
x          : real as length = 500.0 mm
y          : real as length = 6000.0 mm
z          : real as length = 4000.0 mm
material.ref : string  = "lab_air"
internal_item.labels  : string[1] = "wall"
internal_item.placement.wall : string  = "0 0 0 (mm)"
internal_item.model.wall     : string  = "calorimeter_front_wall.model"

################################################################################
[name="calorimeter_back_submodule.model" type="geomtools::simple_shaped_model"]
shape_type : string  = "box"
x          : real as length = 500.0 mm
y          : real as length = 6000.0 mm
z          : real as length = 4000.0 mm
material.ref : string  = "lab_air"
internal_item.labels  : string[1] = "wall"
internal_item.placement.wall : string  = "0 0 0 (mm)"
internal_item.model.wall     : string  = "calorimeter_back_wall.model"


# end of calorimeter_submodules.geom
