# -*- mode: conf-unix; -*-
# calorimeter_submodules.geom


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
