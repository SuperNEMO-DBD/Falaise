# @falaise:snemo/demonstrator/geometry/GeometryModels/source_module/source_calibration.geom
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
  visibility.color : string  = "red"


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
  visibility.color : string  = "blue"


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

# Total of 6 source_calibration_track_[0..5].model with this geometry:
####################################################################################
#
#                        ^ z
#                        :
#                      +---+
#                      | : |
#                      | o | source 6
#                      | : |
#                      | : |
#                      | : |
#                      | o | source 5
#                      | : |
#                      | : |
#                      | : |
#                      | o | source 4
#                      | : |
#                      | : |
#                      | : | source 3
#            ----------|-o-|---------> y
#                      | : |
#                      | : |
#                      | : |
#                      | o | source 2
#                      | : |
#                      | : |
#                      | : |
#                      | o | source 1
#                      | : |
#                      | : |
#                      | : |
#                      | o | source 0
#                      | : |
#                      +---+
#                        :
#                        :
#
#############################################################################

[name="source_calibration_track_0.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real =   58.0 mm
    y : real =   18.0 mm
    z : real = 3350.0 mm
  material.ref : string  = "tracking_gas"
  visibility.color : string  = "grey"
  #@variant_if geometry:layout/if_basic/source_calibration/is_active|false
    # Seven Bi207 calibration source carriers are set in the track only if
    # the "geometry:layout/if_basic/source_calibration/is_active" variant
    # is active.
    internal_item.labels : string[7] = "source_0" "source_1" "source_2" "source_3"  "source_4" "source_5" "source_6"
      internal_item.model.source_0     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_0 : string  = "0 0 -1281 (mm) / y +90 (degree) "
      internal_item.model.source_1     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_1 : string  = "0 0  -861 (mm) / y +90 (degree) "
      internal_item.model.source_2     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_2 : string  = "0 0  -434 (mm) / y +90 (degree) "
      internal_item.model.source_3     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_3 : string  = "0 0     0 (mm) / y +90 (degree) "
      internal_item.model.source_4     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_4 : string  = "0 0  +427 (mm) / y +90 (degree) "
      internal_item.model.source_5     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_5 : string  = "0 0  +848 (mm) / y +90 (degree) "
      internal_item.model.source_6     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_6 : string  = "0 0 +1272 (mm) / y +90 (degree) "
    mapping.daughter_id.source_0 : string = "[source_calibration_carrier:position=0]"
    mapping.daughter_id.source_1 : string = "[source_calibration_carrier:position=1]"
    mapping.daughter_id.source_2 : string = "[source_calibration_carrier:position=2]"
    mapping.daughter_id.source_3 : string = "[source_calibration_carrier:position=3]"
    mapping.daughter_id.source_4 : string = "[source_calibration_carrier:position=4]"
    mapping.daughter_id.source_5 : string = "[source_calibration_carrier:position=5]"
    mapping.daughter_id.source_6 : string = "[source_calibration_carrier:position=6]"
  #@variant_endif

[name="source_calibration_track_1.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real =   58.0 mm
    y : real =   18.0 mm
    z : real = 3350.0 mm
  material.ref : string  = "tracking_gas"
  visibility.color : string  = "grey"
  #@variant_if geometry:layout/if_basic/source_calibration/is_active|false
    # Seven Bi207 calibration source carriers are set in the track only if
    # the "geometry:layout/if_basic/source_calibration/is_active" variant
    # is active.
    internal_item.labels : string[7] = "source_0" "source_1" "source_2" "source_3"  "source_4" "source_5" "source_6"
      internal_item.model.source_0     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_0 : string  = "0 0 -1276 (mm) / y +90 (degree) "
      internal_item.model.source_1     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_1 : string  = "0 0  -842 (mm) / y +90 (degree) "
      internal_item.model.source_2     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_2 : string  = "0 0  -424 (mm) / y +90 (degree) "
      internal_item.model.source_3     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_3 : string  = "0 0     0 (mm) / y +90 (degree) "
      internal_item.model.source_4     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_4 : string  = "0 0  +433 (mm) / y +90 (degree) "
      internal_item.model.source_5     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_5 : string  = "0 0  +853 (mm) / y +90 (degree) "
      internal_item.model.source_6     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_6 : string  = "0 0 +1280 (mm) / y +90 (degree) "
    mapping.daughter_id.source_0 : string = "[source_calibration_carrier:position=0]"
    mapping.daughter_id.source_1 : string = "[source_calibration_carrier:position=1]"
    mapping.daughter_id.source_2 : string = "[source_calibration_carrier:position=2]"
    mapping.daughter_id.source_3 : string = "[source_calibration_carrier:position=3]"
    mapping.daughter_id.source_4 : string = "[source_calibration_carrier:position=4]"
    mapping.daughter_id.source_5 : string = "[source_calibration_carrier:position=5]"
    mapping.daughter_id.source_6 : string = "[source_calibration_carrier:position=6]"
  #@variant_endif

[name="source_calibration_track_2.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real =   58.0 mm
    y : real =   18.0 mm
    z : real = 3350.0 mm
  material.ref : string  = "tracking_gas"
  visibility.color : string  = "grey"
  #@variant_if geometry:layout/if_basic/source_calibration/is_active|false
    # Seven Bi207 calibration source carriers are set in the track only if
    # the "geometry:layout/if_basic/source_calibration/is_active" variant
    # is active.
    internal_item.labels : string[7] = "source_0" "source_1" "source_2" "source_3"  "source_4" "source_5" "source_6"
      internal_item.model.source_0     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_0 : string  = "0 0 -1279 (mm) / y +90 (degree) "
      internal_item.model.source_1     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_1 : string  = "0 0  -841 (mm) / y +90 (degree) "
      internal_item.model.source_2     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_2 : string  = "0 0  -422 (mm) / y +90 (degree) "
      internal_item.model.source_3     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_3 : string  = "0 0     0 (mm) / y +90 (degree) "
      internal_item.model.source_4     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_4 : string  = "0 0  +436 (mm) / y +90 (degree) "
      internal_item.model.source_5     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_5 : string  = "0 0  +857 (mm) / y +90 (degree) "
      internal_item.model.source_6     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_6 : string  = "0 0 +1282 (mm) / y +90 (degree) "
    mapping.daughter_id.source_0 : string = "[source_calibration_carrier:position=0]"
    mapping.daughter_id.source_1 : string = "[source_calibration_carrier:position=1]"
    mapping.daughter_id.source_2 : string = "[source_calibration_carrier:position=2]"
    mapping.daughter_id.source_3 : string = "[source_calibration_carrier:position=3]"
    mapping.daughter_id.source_4 : string = "[source_calibration_carrier:position=4]"
    mapping.daughter_id.source_5 : string = "[source_calibration_carrier:position=5]"
    mapping.daughter_id.source_6 : string = "[source_calibration_carrier:position=6]"
  #@variant_endif

[name="source_calibration_track_3.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real =   58.0 mm
    y : real =   18.0 mm
    z : real = 3350.0 mm
  material.ref : string  = "tracking_gas"
  visibility.color : string  = "grey"
  #@variant_if geometry:layout/if_basic/source_calibration/is_active|false
    # Seven Bi207 calibration source carriers are set in the track only if
    # the "geometry:layout/if_basic/source_calibration/is_active" variant
    # is active.
    internal_item.labels : string[7] = "source_0" "source_1" "source_2" "source_3"  "source_4" "source_5" "source_6"
      internal_item.model.source_0     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_0 : string  = "0 0 -1270 (mm) / y +90 (degree) "
      internal_item.model.source_1     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_1 : string  = "0 0  -848 (mm) / y +90 (degree) "
      internal_item.model.source_2     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_2 : string  = "0 0  -418 (mm) / y +90 (degree) "
      internal_item.model.source_3     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_3 : string  = "0 0     0 (mm) / y +90 (degree) "
      internal_item.model.source_4     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_4 : string  = "0 0  +431 (mm) / y +90 (degree) "
      internal_item.model.source_5     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_5 : string  = "0 0  +856 (mm) / y +90 (degree) "
      internal_item.model.source_6     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_6 : string  = "0 0 +1279 (mm) / y +90 (degree) "
    mapping.daughter_id.source_0 : string = "[source_calibration_carrier:position=0]"
    mapping.daughter_id.source_1 : string = "[source_calibration_carrier:position=1]"
    mapping.daughter_id.source_2 : string = "[source_calibration_carrier:position=2]"
    mapping.daughter_id.source_3 : string = "[source_calibration_carrier:position=3]"
    mapping.daughter_id.source_4 : string = "[source_calibration_carrier:position=4]"
    mapping.daughter_id.source_5 : string = "[source_calibration_carrier:position=5]"
    mapping.daughter_id.source_6 : string = "[source_calibration_carrier:position=6]"
  #@variant_endif

[name="source_calibration_track_4.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real =   58.0 mm
    y : real =   18.0 mm
    z : real = 3350.0 mm
  material.ref : string  = "tracking_gas"
  visibility.color : string  = "grey"
  #@variant_if geometry:layout/if_basic/source_calibration/is_active|false
    # Seven Bi207 calibration source carriers are set in the track only if
    # the "geometry:layout/if_basic/source_calibration/is_active" variant
    # is active.
    internal_item.labels : string[7] = "source_0" "source_1" "source_2" "source_3"  "source_4" "source_5" "source_6"
      internal_item.model.source_0     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_0 : string  = "0 0 -1275 (mm) / y +90 (degree) "
      internal_item.model.source_1     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_1 : string  = "0 0  -848 (mm) / y +90 (degree) "
      internal_item.model.source_2     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_2 : string  = "0 0  -424 (mm) / y +90 (degree) "
      internal_item.model.source_3     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_3 : string  = "0 0     0 (mm) / y +90 (degree) "
      internal_item.model.source_4     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_4 : string  = "0 0  +428 (mm) / y +90 (degree) "
      internal_item.model.source_5     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_5 : string  = "0 0  +854 (mm) / y +90 (degree) "
      internal_item.model.source_6     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_6 : string  = "0 0 +1284 (mm) / y +90 (degree) "
    mapping.daughter_id.source_0 : string = "[source_calibration_carrier:position=0]"
    mapping.daughter_id.source_1 : string = "[source_calibration_carrier:position=1]"
    mapping.daughter_id.source_2 : string = "[source_calibration_carrier:position=2]"
    mapping.daughter_id.source_3 : string = "[source_calibration_carrier:position=3]"
    mapping.daughter_id.source_4 : string = "[source_calibration_carrier:position=4]"
    mapping.daughter_id.source_5 : string = "[source_calibration_carrier:position=5]"
    mapping.daughter_id.source_6 : string = "[source_calibration_carrier:position=6]"
  #@variant_endif

[name="source_calibration_track_5.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real =   58.0 mm
    y : real =   18.0 mm
    z : real = 3350.0 mm
  material.ref : string  = "tracking_gas"
  visibility.color : string  = "grey"
  #@variant_if geometry:layout/if_basic/source_calibration/is_active|false
    # Seven Bi207 calibration source carriers are set in the track only if
    # the "geometry:layout/if_basic/source_calibration/is_active" variant
    # is active.
    internal_item.labels : string[7] = "source_0" "source_1" "source_2" "source_3"  "source_4" "source_5" "source_6"
      internal_item.model.source_0     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_0 : string  = "0 0 -1277 (mm) / y +90 (degree) "
      internal_item.model.source_1     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_1 : string  = "0 0  -852 (mm) / y +90 (degree) "
      internal_item.model.source_2     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_2 : string  = "0 0  -426 (mm) / y +90 (degree) "
      internal_item.model.source_3     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_3 : string  = "0 0     0 (mm) / y +90 (degree) "
      internal_item.model.source_4     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_4 : string  = "0 0  +422 (mm) / y +90 (degree) "
      internal_item.model.source_5     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_5 : string  = "0 0  +850 (mm) / y +90 (degree) "
      internal_item.model.source_6     : string  = "source_calibration_carrier.model"
      internal_item.placement.source_6 : string  = "0 0 +1278 (mm) / y +90 (degree) "
    mapping.daughter_id.source_0 : string = "[source_calibration_carrier:position=0]"
    mapping.daughter_id.source_1 : string = "[source_calibration_carrier:position=1]"
    mapping.daughter_id.source_2 : string = "[source_calibration_carrier:position=2]"
    mapping.daughter_id.source_3 : string = "[source_calibration_carrier:position=3]"
    mapping.daughter_id.source_4 : string = "[source_calibration_carrier:position=4]"
    mapping.daughter_id.source_5 : string = "[source_calibration_carrier:position=5]"
    mapping.daughter_id.source_6 : string = "[source_calibration_carrier:position=6]"
  #@variant_endif


