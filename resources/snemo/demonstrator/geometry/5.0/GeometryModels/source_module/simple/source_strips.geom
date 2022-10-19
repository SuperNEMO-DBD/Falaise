# source_strips.geom
# Basic geometry:
#  - Based on :
#  -  DocDB #3998 - 2016-04-09 (Marek Proga)
#  -  DocDB #2282 - 2012-06-19 (Marek Proga)
#  -  DocDB #???? - 2012-04-18 (Marek Proga)
#  -  DocDB #2049 - 2012-02-08 (Marek Proga)
#  -  DocDB #2097 - 2012-02-27 (Marek Proga)
#  -  DocDB #1918 - 2011-10-24 (Marek Proga)

# LAPP source pads' geometry
[name="source_external_film.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real as length =   12.0 um
    y : real as length =  125.0 mm
    z : real as length = 3350.0 mm
  material.ref : string  = "basic::mylar"
  visibility.hidden : boolean = false
  visibility.color  : string  = "cyan"


[name="source_internal_film.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real as length =   12.0 um
    y : real as length =  135.5 mm
    z : real as length = 3350.0 mm
  material.ref : string  = "basic::mylar"
  visibility.hidden : boolean = false
  visibility.color  : string  = "cyan"


##########################################################################
#
#
#    +-------+
#    |       |
#    | +---+ |
#    | |   | |
#    | |   | pad #7
#    | |   | |
#    | +---+ |
#    | :   : |
#    | +---+ |
#    | |   | |
#    | |   | pad #1
#    | |   | |
#    | +---+ |
#    | +---+ |
#    | |   | |
#    | |   | pad #0
#    | |   | |
#    | +---+ |
#    |       |
#    +-------+
#
[name="source_external_multipad.model" type="geomtools::replicated_model"]
  stacked.axis            : string = "z"
  stacked.number_of_items : integer = 8
  replicated.model        : string = "source_external_pad_lapp.model"
  replicated.label        : string = "pads"
  replicated.step         : real as length = 337.8 mm
  z                       : real as length = 3350.0 mm
  material.ref : string = "basic::mylar"
  visibility.hidden           : boolean = false
  visibility.envelop_hidden   : boolean = true
  visibility.daughters.hidden : boolean = false
  visibility.color            : string = "grey"
  mapping.daughter_id.pads : string = "[source_pad:pad+0]"


##########################################################################
#
#    +-------+
#    |       |
#    | +---+ |
#    | |   | |
#    | |   | pad #7
#    | |   | |
#    | +---+ |
#    | :   : |
#    | +---+ |
#    | |   | |
#    | |   | pad #1
#    | |   | |
#    | +---+ |
#    | +---+ |
#    | |   | |
#    | |   | pad #0
#    | |   | |
#    | +---+ |
#    |       |
#    +-------+
#
[name="source_internal_multipad.model" type="geomtools::replicated_model"]
  stacked.axis            : string = "z"
  stacked.number_of_items : integer = 8
  replicated.model        : string = "source_internal_pad_lapp.model"
  replicated.label        : string = "pads"
  replicated.step         : real as length = 337.8 mm
  z                       : real as length = 3350.0 mm
  material.ref : string = "basic::mylar"
  visibility.hidden           : boolean = false
  visibility.envelop_hidden   : boolean = true
  visibility.daughters.hidden : boolean = false
  visibility.color            : string = "grey"
  mapping.daughter_id.pads : string = "[source_pad:pad+0]"


#########################################################################
#              z
#               ^
#               :
#               :
#           full strip
#           +-+---+-+
#           | |   | |
#           | |   | |
#           | | s | |
#         f | | o | | f
#         i | | u | | i
# - - - - l | | r | | l - - - -> x
#         m | | c | | m
#         0 | | e | | 1
#           | |   | |
#           | |   | |
#           | |   | |
#           +-+---+-+
#               :
#               :
#
[name="source_internal_full_strip.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "x"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "source_internal_film.model"
    stacked.label_0 : string = "film_0"
    stacked.model_1 : string = "source_internal_multipad.model"
    stacked.label_1 : string = "source"
    stacked.model_2 : string = "source_internal_film.model"
    stacked.label_2 : string = "film_1"
  material.ref : string = "tracking_gas"
  visibility.hidden : boolean = false
  mapping.daughter_id.film_0 : string = "[source_strip_film:film=0]"
  mapping.daughter_id.film_1 : string = "[source_strip_film:film=1]"


#########################################################################
#              z
#               ^
#               :
#               :
#           full strip
#           +-+---+-+
#           | |   | |
#           | |   | |
#           | | s | |
#         f | | o | | f
#         i | | u | | i
# - - - - l | | r | | l - - - -> x
#         m | | c | | m
#         0 | | e | | 1
#           | |   | |
#           | |   | |
#           | |   | |
#           +-+---+-+
#               :
#               :
#
[name="source_external_full_strip.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "x"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "source_external_film.model"
    stacked.label_0 : string = "film_0"
    stacked.model_1 : string = "source_external_multipad.model"
    stacked.label_1 : string = "source"
    stacked.model_2 : string = "source_external_film.model"
    stacked.label_2 : string = "film_1"
  material.ref : string = "tracking_gas"
  visibility.hidden          : boolean = false
  visibility.hidden_envelope : boolean = true
  visibility.color           : string  = "grey"
  mapping.daughter_id.film_0 : string = "[source_strip_film:film=0]"
  mapping.daughter_id.film_1 : string = "[source_strip_film:film=1]"

