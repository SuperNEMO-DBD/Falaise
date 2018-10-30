# Source pad #
# 2016-05-12 FM+GO: NOT READY YET !!! waiting for inputs from LAPP...
# LAPP source pads' geometry

[name="source_external_pad_lapp.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real as length = @variant(geometry:layout/if_basic/source_layout/if_basic/thickness|276 um)
    y : real as length = 123.0 mm
    z : real as length = 335.3 mm
  material.ref : string  = @variant(geometry:layout/if_basic/source_layout/if_basic/material|"bb_source_material")
  visibility.hidden : boolean = false
  visibility.color  : string  = "magenta"


[name="source_internal_pad_lapp.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real as length = @variant(geometry:layout/if_basic/source_layout/if_basic/thickness|276 um)
    y : real as length = 133.5 mm
    z : real as length = 335.3 mm
  material.ref : string  = @variant(geometry:layout/if_basic/source_layout/if_basic/material|"bb_source_material")
  visibility.hidden : boolean = false
  visibility.color  : string  = "magenta"

