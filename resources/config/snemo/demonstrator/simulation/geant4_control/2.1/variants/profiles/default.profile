#@format=datatools::configuration::variant
#@format.version=1.0
#@organization=snemo
#@application=falaise

[registry="geometry"]
calo_film_thickness = 25 um
layout = "Basic"
layout/if_basic/magnetic_field = true
layout/if_basic/magnetic_field/is_active/type = "UniformVertical"
layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/direction = "+z"
layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/magnitude = 25 gauss
layout/if_basic/shielding = true
layout/if_basic/source_calibration = false
layout/if_basic/source_layout = "Basic"
layout/if_basic/source_layout/if_basic/material = "Se82"
layout/if_basic/source_layout/if_basic/thickness = 250 um

[registry="vertexes"]
generator = "source_pads_bulk"

[registry="primary_events"]
generator = "Se82.0nubb"

[registry="simulation"]
output_profile = "none"
physics_mode = "Constructors"
physics_mode/if_constructors/em_model = "standard"
production_cuts = true
