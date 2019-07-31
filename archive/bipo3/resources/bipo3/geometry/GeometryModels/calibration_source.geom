#@description Calibration Source for BiPo3 module

[name="calibration_source.model" type="geomtools::simple_shaped_model"]
  #@config Calibration Source for BiPo3 module
  shape_type  : string = "cylinder"
  length_unit : string = "mm"
  r : real   = 10.0  mm
  z : real   =  1.0  mm
  material.ref : string = "bipo::manganese"
  visibility.hidden : boolean = 0
  visibility.color  : string = "red"
  radioactivity.nucleide      : string = "Mn-54"
  radioactivity.activity      : real   = 37.0
  radioactivity.activity.unit : string = "kBq"

