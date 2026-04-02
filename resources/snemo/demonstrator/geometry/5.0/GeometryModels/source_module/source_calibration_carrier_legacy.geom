# @falaise:snemo/demonstrator/geometry/5.0/GeometryModels/source_module/source_calibration.geom
# References:
# - DocDB-2875 (old)
# - DocDB-4816

############################################################################
#
#                           ^ y
#                           :
#                          _:_ spot
#                         / : \
#                   ------|-o-|----> x
#                         \_:_/
#                           :
#                           :
#
[name="source_calibration_spot.model" type="geomtools::simple_shaped_model"]
  #@config The description of the calibration source spot (Bi207)
  shape_type : string  = "cylinder"
    r : real as length = 1.0 mm
    z : real as length = 1.0 um
  material.ref : string = "basic::mylar"
  visibility.color : string  = "magenta"
  sensitive.category : string ="bi207_spot_SD"



############################################################################
#
#                         ^ y
#                         :
#                  +-------------+
#                  |      :      |
#                  |      :      |
#                  |     _:_ spot|
#                  |    / : \    |
#               ---|----|-o-|----|----> x
#                  |    \_:_/    |
#                  |      :      |
#                  |      :      |
#                  |      :      |
#                  +-------------+
#                         :
#                         :
#
[name="source_calibration_envelope.model" type="geomtools::simple_shaped_model"]
  #@config The description of the calibration source envelope (Bi207)
  shape_type : string  = "box"
    x : real as length = 13.0 mm
    y : real as length =  8.0 mm
    z : real as length = 10.0 um
  material.ref : string  = "basic::mylar"
  internal_item.labels         : string[1] = "spot"
    internal_item.model.spot     : string = "source_calibration_spot.model"
    internal_item.placement.spot : string = "0 0 0 (mm)"
  mapping.daughter_id.spot : string = "[source_calibration_spot]"
  visibility.color : string  = "cyan"
  sensitive.category : string ="bi207_envelope_SD"


#######################################################################################
#
#                         ^ y
#                         :
#                         :
#               +-------------------+
#               |         :         |
#               |  +-------------+  |
#               |  |      :      |  |
#               |  |      :      |  |
#               |  |      :      |  |
#               |  |      :      |  |
#            ---|--|------o------|--|----> x
#               |  |      :      |  |
#               |  |      :      |  |
#               |  |      :      |  |
#               |  |      :      |  |
#               |  +-------------+  |
#               |         :         |
#               +-------------------+
#                         :
#
[name="source_calibration_carrier_frame.model" type="geomtools::plate_with_hole_model"]
  x      : real as length = 35.0 mm
  y      : real as length = 15.0 mm
  z      : real as length =  4.1 mm
  x_hole : real as length = 13.0 mm
  y_hole : real as length =  8.0 mm
  material.ref : string  = "basic::copper"
  visibility.color : string  = "red"
  sensitive.category : string ="bi207_carrier_frame_SD"

############################################################################
#
#                         ^ y
#                         :
#                         :
#               +-------------------+
#               |         :         |
#               |  +-------------+  |
#               |  |      :      |  |
#               |  |      :      |  |
#               |  |     _:_ spot|  |
#               |  |    / : \    |  |
#            ---|--|----|-o-|----|--|----> x
#               |  |    \_:_/    |  |
#               |  |      :      |  |
#               |  |      :      |  |
#               |  |      :      |  |
#               |  +-------------+  |
#               |         :         |
#               +-------------------+
#                         :
#
[name="source_calibration_carrier.model" type="geomtools::stacked_model"]
  stacked.axis            : string  = "z"
  stacked.number_of_items : integer =  2
    stacked.model_0     : string  = "source_calibration_carrier_frame.model"
    stacked.label_0     : string  = "frame"
    stacked.limit_max_0 : real as length =  -5.0 um
    stacked.model_1 : string  = "source_calibration_envelope.model"
    stacked.label_1 : string  = "envelope"
  z : real as length = 4.1 mm
  material.ref : string = "tracking_gas"
  visibility.color : string  = "grey"
  mapping.daughter_id.envelope : string = "[source_calibration_envelope]"
  mapping.daughter_id.frame    : string = "[source_calibration_frame]"

# end