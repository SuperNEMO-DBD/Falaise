#@format=datatools::configuration::variant
#@format.version=1.0
#@organization=snemo
#@application=falaise

[registry="geometry"]
layout = "Basic"
layout/if_basic/magnetic_field = true
layout/if_basic/magnetic_field/is_active/type = "UniformVertical"
layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/magnitude = 25 gauss
layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/direction = "+z"
layout/if_basic/source_layout = "Basic"
layout/if_basic/source_layout/if_basic/thickness = 250 um
layout/if_basic/source_layout/if_basic/material = "Se82"
layout/if_basic/source_calibration = true
layout/if_basic/source_calibration/is_active/type = "Bi207"
layout/if_basic/shielding = true
calo_film_thickness = 25 um
tracking_gas_material = "Nemo3"

[registry="vertexes"]
generator = "pmt_xcalo_gveto_glass_bulk"

[registry="primary_events"]
generator = "electron.1MeV"

[registry="simulation"]
physics_mode = "Constructors"
physics_mode/if_constructors/em_model = "standard"
production_cuts = true
output_profile = "none"

