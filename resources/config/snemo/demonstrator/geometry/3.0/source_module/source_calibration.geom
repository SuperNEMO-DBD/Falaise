# -*- mode: conf-unix; -*-

[name="source_calibration_spot.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "cylinder"
r                  : real    =   1.0     # mm
z                  : real    =   1.0e-6  # mm
length_unit        : string  = "mm"
material.ref       : string  = ""
visibility.hidden  : boolean = 0
visibility.color   : string  = "red"


[name="source_calibration_film.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
x                  : real    =   17.0     # mm
y                  : real    =   11.0     # mm
z                  : real    =   10.0e-6  # mm
length_unit        : string  = "mm"
material.ref       : string  = "std::mylar"
visibility.hidden  : boolean = 0
visibility.color   : string  = "cyan"

internal_item.labels         : string[1] = "spot"
internal_item.model.spot     : string    = "source_calibration_spot.model"
internal_item.placement.spot : string    = "0 0 0 (mm)"


[name="source_calibration_carrier_frame.model" type="geomtools::plate_with_hole_model"]
x                  : real    =   26.0  # mm
y                  : real    =   16.0  # mm
z                  : real    =    2.0  # mm
x_hole             : real    =   17.0  # mm
y_hole             : real    =   11.0  # mm
length_unit        : string  = "mm"
material.ref       : string  = "std::polyethylene"
visibility.hidden  : boolean = 0
visibility.color   : string  = "blue"


[name="source_calibration_carrier.model" type="geomtools::stacked_model"]
z                       : real    = 2.0
length_unit             : string  = "mm"
material.ref            : string  = "tracking_gas"
stacked.axis            : string  = "z"
stacked.number_of_items : integer = 2

stacked.model_1   : string  = "source_calibration_film.model"
stacked.label_1   : string  = "film"

stacked.limit_max_0 : real  =  -5.0e-6
stacked.limit_min_0 : real  =  +5.0e-6

stacked.model_0   : string  = "source_calibration_carrier_frame.model"
stacked.label_0   : string  = "frame"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"


[name="source_calibration_suspender.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
x                  : real    =  824.0  # mm
y                  : real    =    5.0  # mm
z                  : real    =    0.5 # mm
material.ref       : string  = "std::copper"


[name="source_calibration_track.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
y                  : real    =   18.0  # mm
z                  : real    = 3350.0  # mm
x                  : real    =   58.0  # mm
length_unit        : string  = "mm"
material.ref       : string  = "tracking_gas"
visibility.hidden  : boolean = 0
visibility.color   : string  = "grey"


[name="source_calibration_track_active.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
y                  : real    =   18.0  # mm
z                  : real    = 3350.0  # mm
x                  : real    =   58.0  # mm
length_unit        : string  = "mm"
material.ref       : string  = "tracking_gas"
visibility.hidden  : boolean = 0
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
