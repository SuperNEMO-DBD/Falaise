# -*- mode: conf-unix; -*-

#####################################
### Top/bottom BiPo3 test capsule ###
#####################################

#####

[name="bottom_optical_module.model" type="geomtools::rotated_boxed_model"]
rotated.axis          : string = "y"
rotated.special_angle : string = "0"
rotated.model         : string = "optical_module.model"
rotated.label         : string = "bottom_optical_module"
material.ref          : string  = "snemo::nitrogen"

#####

[name="top_optical_module.model" type="geomtools::rotated_boxed_model"]
rotated.axis          : string = "y"
rotated.special_angle : string = "180"
rotated.model         : string = "optical_module.model"
rotated.label         : string = "top_optical_module"
material.ref          : string  = "snemo::nitrogen"

#####

[name="top_bottom_module.model" type="geomtools::stacked_model"]
material.ref            : string  = "snemo::nitrogen"
x                       : real as length = 301.0 mm
y                       : real as length = 301.0 mm
z                       : real as length = 780.0 mm
stacked.axis            : string  = "z"

stacked.number_of_items : integer = 5

stacked.model_0         : string  = "bottom_optical_module.model"
stacked.label_0         : string  = "bottom_module"

stacked.model_1         : string  = "source_gas_gap.model"
stacked.label_1         : string  = "bottom_gap"

stacked.model_2         : string  = "source_sample.model"
stacked.label_2         : string  = "source"

stacked.model_3         : string  = "source_gas_gap.model"
stacked.label_3         : string  = "top_gap"

stacked.model_4         : string  = "top_optical_module.model"
stacked.label_4         : string  = "top_module"

# --> Daughters mapping informations:
mapping.daughter_id.bottom_module : string = "[optical_module.category: position=0]"
mapping.daughter_id.bottom_gap    : string = "[gap.category           : position=0]"
mapping.daughter_id.source        : string = "[source.category]"
mapping.daughter_id.top_gap       : string = "[gap.category           : position=1]"
mapping.daughter_id.top_module    : string = "[optical_module.category: position=1]"

#visibility.color           : string = "red"

# end
