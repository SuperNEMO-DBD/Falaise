#@format=datatools::configuration::variant
#@format.version=1.0
#@organization=snemo
#@application=falaise

[registry="geometry"]
layout = "Basic"
layout/if_basic/magnetic_field = false
layout/if_basic/source_layout = "RealisticFlat"
layout/if_basic/source_calibration = false
layout/if_basic/shielding = true
layout/if_basic/shielding/is_present/layout = "XXXXX"
calo_film_thickness = 25 um
tracking_gas_material = "YYYYY"
