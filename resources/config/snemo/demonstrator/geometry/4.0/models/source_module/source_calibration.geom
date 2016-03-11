# -*- mode: conf-unix; -*-
# source_calibration.geom

############################################################################
[name="source_calibration_spot.model" type="geomtools::simple_shaped_model"]

shape_type        : string  = "cylinder"
r                 : real as length = 1.0 mm
z                 : real as length = 1.0 um

material.ref      : string = ""

visibility.hidden : boolean = false
visibility.color  : string  = "red"


############################################################################
#
#                           ^ y
#                           :
#                  +-----------------+
#                  |        :        |
#                  |        :        |
#                  |       _:_ spot  |
#                  |      / : \      |
#               ---|------|-o-|------|----> x
#                  |      \_:_/      |
#                  |        :        |
#                  |        :        |
#                  |        :        |
#                  +-----------------+
#                           :
#                           :
#
############################################################################
[name="source_calibration_film.model" type="geomtools::simple_shaped_model"]

shape_type        : string  = "box"
x                 : real as length =   17.0 mm
y                 : real as length =   11.0 mm
z                 : real as length =   10.0 um

material.ref      : string  = "basic::mylar"

visibility.hidden : boolean = false
visibility.color  : string  = "cyan"

internal_item.labels         : string[1] = "spot"
internal_item.model.spot     : string    = "source_calibration_spot.model"
internal_item.placement.spot : string    = "0 0 0 (mm)"


#######################################################################################
[name="source_calibration_carrier_frame.model" type="geomtools::plate_with_hole_model"]

x                 : real as length =   26.0 mm
y                 : real as length =   16.0 mm
z                 : real as length =    2.0 mm
x_hole            : real as length =   17.0 mm
y_hole            : real as length =   11.0 mm

material.ref      : string  = "basic::polyethylene"

visibility.hidden : boolean = false
visibility.color  : string  = "blue"


#########################################################################
[name="source_calibration_carrier.model" type="geomtools::stacked_model"]

z  : real as length = 2.0 mm

material.ref : string = "tracking_gas"

stacked.axis            : string  = "z"
stacked.number_of_items : integer = 2

stacked.model_1   : string  = "source_calibration_film.model"
stacked.label_1   : string  = "film"

stacked.limit_max_0 : real as length =  -5.0 um
stacked.limit_min_0 : real as length =  +5.0 um

stacked.model_0   : string  = "source_calibration_carrier_frame.model"
stacked.label_0   : string  = "frame"

visibility.hidden : boolean = false
visibility.color  : string  = "grey"


#################################################################################
[name="source_calibration_suspender.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real    =  824.0 mm
y                  : real    =    5.0 mm
z                  : real    =    0.5 mm

material.ref       : string  = "basic::copper"


#############################################################################
#
#                        ^ z
#                        :
#                      +---+
#                      | : |
#                      | : |
#                      | : |
#                      | : |
#                      | : |
#                      | : |
#                      | : |
#            ----------|-x-|---------> y
#                      | : |
#                      | : |
#                      | : |
#                      | : |
#                      | : |
#                      | : |
#                      | : |
#                      +---+
#                        :
#                        :
#
#############################################################################
[name="source_calibration_track.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
y                  : real    =   18.0 mm
z                  : real    = 3350.0 mm
x                  : real    =   58.0 mm

material.ref       : string  = "tracking_gas"

visibility.hidden  : boolean = false
visibility.color   : string  = "grey"


####################################################################################
#
#                        ^ z
#                        :
#                      +---+
#                      | : |
#                      | o | source 3
#                      | : |
#                      | : |
#                      | : |
#                      | o | source 2
#                      | : |
#            ----------|-x-|---------> y
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
####################################################################################
[name="source_calibration_track_active.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =   18.0 mm
z                  : real    = 3350.0 mm

material.ref       : string  = "tracking_gas"

visibility.hidden  : boolean = false
visibility.color   : string  = "grey"

internal_item.labels : string[4] = \
 "source_3" \
 "source_2" \
 "source_1" \
 "source_0"

internal_item.model.source_3     : string  = "source_calibration_carrier.model"
internal_item.placement.source_3 : string  = "0 0 +1275 (mm) / y +90 (degree) "

internal_item.model.source_2     : string  = "source_calibration_carrier.model"
internal_item.placement.source_2 : string  = "0 0  +425 (mm) / y +90 (degree) "

internal_item.model.source_1     : string  = "source_calibration_carrier.model"
internal_item.placement.source_1 : string  = "0 0  -425 (mm) / y +90 (degree) "

internal_item.model.source_0     : string  = "source_calibration_carrier.model"
internal_item.placement.source_0 : string  = "0 0 -1275 (mm) / y +90 (degree) "


# end of source_calibration.geom
