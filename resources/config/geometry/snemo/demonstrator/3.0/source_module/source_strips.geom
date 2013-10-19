# -*- mode: conf-unix; -*-

#
# Basic geometry:
#  - Based on :
#  -  DocDB #???? - 2012-04-18 (Marek Proga)
#  -  DocDB #2049 - 2012-02-08 (Marek Proga)
#  -  DocDB #2097 - 2012-02-27 (Marek Proga)
#  -  DocDB #1918 - 2011-10-24 (Marek Proga)
# Authors: F. Mauger

##############
# Source pad #
##############

[name="source_external_support_strip.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
x                  : real    = 0.1  # mm
y                  : real    = 125.0  # mm
z                  : real    = 325.0 # mm
length_unit        : string  = "mm"
material.ref       : string  = "std::mylar"
visibility.hidden  : boolean = 0
visibility.color   : string  = "cyan"

[name="source_internal_support_strip.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
x                  : real    = 0.1  # mm
y                  : real    = 135.5  # mm
z                  : real    = 325.0 # mm
length_unit        : string  = "mm"
material.ref       : string  = "std::mylar"
visibility.hidden  : boolean = 0
visibility.color   : string  = "cyan"

[name="source_external_strip.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
y                  : real    = 125.0  # mm
z                  : real    = 2700.0 # mm
x                  : real    = 0.167  # mm
length_unit        : string  = "mm"
material.ref       : string  = "bb_source_material"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

[name="source_internal_strip.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
y                  : real    = 135.5  # mm
z                  : real    = 2700.0 # mm
x                  : real    = 0.167  # mm
length_unit        : string  = "mm"
material.ref       : string  = "bb_source_material"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

[name="source_internal_full_strip.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 3

stacked.model_2   : string  = "source_internal_support_strip.model"
stacked.label_2   : string  = "top_source_support"

stacked.model_1   : string  = "source_internal_strip.model"
stacked.label_1   : string  = "source_strip"

stacked.model_0   : string  = "source_internal_support_strip.model"
stacked.label_0   : string  = "bottom_source_support"

visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.source_strip : string = "[source_strip]"

[name="source_external_full_strip.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 3

stacked.model_2   : string  = "source_external_support_strip.model"
stacked.label_2   : string  = "top_source_support"

stacked.model_1   : string  = "source_external_strip.model"
stacked.label_1   : string  = "source_strip"

stacked.model_0   : string  = "source_external_support_strip.model"
stacked.label_0   : string  = "bottom_source_support"

visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.source_strip : string = "[source_strip]"

[name="source_strip_spacing.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
y                  : real    =    1.0  # mm
z                  : real    = 3350.0  # mm
x                  : real    =   58.0  # mm
length_unit        : string  = "mm"
material.ref       : string  = "tracking_gas"
visibility.hidden  : boolean = 0
visibility.color   : string  = "grey"

[name="source_strip_ender_spacing.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
y                  : real    =    3.0  # mm
z                  : real    = 3350.0  # mm
x                  : real    =   58.0  # mm
length_unit        : string  = "mm"
material.ref       : string  = "tracking_gas"
visibility.hidden  : boolean = 0
visibility.color   : string  = "grey"

# end of file





