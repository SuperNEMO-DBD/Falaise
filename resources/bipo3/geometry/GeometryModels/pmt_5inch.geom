#@description PMT HAMAMATSU R6594

[name="PMT_HAMAMATSU_R6594.dynodes" type="geomtools::simple_shaped_model"]
  #@config Simplified PMT dynodes
  shape_type  : string = "tube"
  length_unit : string = "mm"
  outer_r : real   = 15.0
  inner_r : real   = 14.5
  z       : real   = 80
  material.ref      : string = "std::copper"
  visibility.hidden : boolean = 0
  visibility.color  : string  = "orange"


[name="PMT_HAMAMATSU_R6594.base" type="geomtools::simple_shaped_model"]
  #@config Simplified PMT base
  shape_type  : string = "cylinder"
  length_unit : string = "mm"
  r : real   = 25.5
  z : real   = 30.0
  material.ref : string = "std::delrin"
  visibility.hidden : boolean = 0
  visibility.color  : string  = "orange"


[name="PMT_HAMAMATSU_R6594" type="geomtools::simple_shaped_model"]
  #@config The configuration parameters for the PMT's bulb and its contents
  length_unit : string = "mm"
  shape_type  : string = "polycone"
  build_mode  : string = "datafile"
  datafile    : string = "@falaise:geometry/PMT/hamamatsu_R6594_shape.data"
  filled_mode : string = "by_envelope"
  filled_label : string = "bulb"
  material.ref        : string = "glass"
  material.filled.ref : string = "vacuum"
  visibility.hidden           : boolean = 1
  visibility.hidden_envelop   : boolean = 1
  visibility.color            : string  = "cyan"
  visibility.daughters.hidden : boolean = 1
  internal_item.filled.labels            : string[1] = "dynodes"
  internal_item.filled.placement.dynodes : string  = "0 0 -30 (mm)"
  internal_item.filled.model.dynodes     : string  = "PMT_HAMAMATSU_R6594.dynodes"

