### Source for BiPo3 test capsule ###

[name="source_sample.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "box"
  x : real = 300.0  mm
  y : real = 300.0  mm
  z : real = 0.177  mm
  length_unit : string = "mm"
  material.ref : string  = "snemo::aluminium"
  visibility.hidden : boolean = 0
  visibility.color  : string  = "magenta"


[name="source_gas_gap.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "box"
  x : real = 300.0  mm
  y : real = 300.0  mm
  z : real =  0.025  mm
  length_unit : string = "mm"
  material.ref : string  = "snemo::nitrogen"
  visibility.hidden : boolean = 0
  visibility.color  : string  = "cyan"

