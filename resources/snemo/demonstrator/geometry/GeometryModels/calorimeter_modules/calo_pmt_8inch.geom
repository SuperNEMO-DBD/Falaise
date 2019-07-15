# PMT HAMAMATSU R5912 (8")
[name="calo.PMT_HAMAMATSU_R5912.dynodes.model" type="geomtools::simple_shaped_model"]
  #@config Simplified dynodes
  shape_type : string = "tube"
    outer_r : real as length = 20.0 mm
    inner_r : real as length = 19.5 mm
    z       : real as length = 100 mm
  material.ref : string = "basic::copper"
  visibility.hidden : boolean = false
  visibility.color  : string  = "orange"


[name="calo.PMT_HAMAMATSU_R5912.base.model" type="geomtools::simple_shaped_model"]
  #@config Simplified base
  shape_type : string = "cylinder"
    r : real as length = 27.5 mm
    z : real as length =  3.0 mm
  material.ref : string = "basic::delrin"
  visibility.hidden : boolean = false
  visibility.color  : string  = "orange"


[name="calo.PMT_HAMAMATSU_R5912.model" type="geomtools::simple_shaped_model"]
  #@config The configuration parameters for the PMT's bulb and its contents
  length_unit : string = "mm"
  shape_type  : string = "polycone"
  build_mode  : string = "datafile"
  datafile    : string = "@falaise:geometry/PMT/hamamatsu_R5912MOD_shape.data"
  filled_mode : string = "by_envelope"
  filled_label : string = "bulb"
  material.ref : string = "glass"
  material.filled.ref : string = "vacuum"
  visibility.hidden           : boolean = true
  visibility.hidden_envelope  : boolean = true
  visibility.color            : string  = "cyan"
  visibility.daughters.hidden : boolean = false
  mapping.filled.daughter_id.bulb : string = "[calorimeter_pmt_glass]"

