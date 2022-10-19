# PMT HAMAMATSU R6594 (5")
[name="calo.PMT_HAMAMATSU_R6594.dynodes.model" type="geomtools::simple_shaped_model"]
  #@config Simplified PMT dynodes
  shape_type : string = "tube"
    outer_r : real as length = 15.0 mm
    inner_r : real as length = 14.5 mm
    z       : real as length = 80.0 mm
  material.ref : string  = "basic::copper"
  visibility.hidden : boolean = false
  visibility.color  : string  = "orange"


[name="calo.PMT_HAMAMATSU_R6594.base.model" type="geomtools::simple_shaped_model"]
  #@config Simplified PMT base
  shape_type : string = "cylinder"
    r : real as length = 25.5 mm
    z : real as length = 30.0 mm
  material.ref : string = "basic::delrin"
  visibility.hidden : boolean = false
  visibility.color  : string  = "orange"


[name="calo.PMT_HAMAMATSU_R6594.model" type="geomtools::simple_shaped_model"]
  #@config The configuration parameters for the PMT's bulb and its contents
  length_unit : string = "mm"
  shape_type  : string = "polycone"
  build_mode  : string = "datafile"
  datafile    : string = "@falaise:geometry/PMT/hamamatsu_R6594_shape.data"
  filled_mode : string = "by_envelope"
  filled_label : string = "bulb"
  material.ref : string = "glass"
  material.filled.ref : string = "vacuum"
  visibility.hidden           : boolean = true
  visibility.hidden_envelope  : boolean = true
  visibility.color            : string  = "cyan"
  visibility.daughters.hidden : boolean = true
  mapping.filled.daughter_id.bulb : string = "[calorimeter_pmt_glass]"

