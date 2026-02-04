#@format=datatools::configuration::variant
#@format.version=1.0
#@organization=snemo
#@application=falaise

[registry="geometry"]
layout = "Basic"
layout/if_basic/magnetic_field = false
layout/if_basic/source_layout = "RealisticSNRS1"
layout/if_basic/source_calibration = true
layout/if_basic/source_calibration/is_active/type = "SDS"
layout/if_basic/source_calibration/is_active/type/if_sds/track0 = true
layout/if_basic/source_calibration/is_active/type/if_sds/track1 = true
layout/if_basic/source_calibration/is_active/type/if_sds/track2 = true
layout/if_basic/source_calibration/is_active/type/if_sds/track3 = true
layout/if_basic/source_calibration/is_active/type/if_sds/track4 = true
layout/if_basic/source_calibration/is_active/type/if_sds/track5 = true
layout/if_basic/shielding = true
layout/if_basic/shielding/is_present/layout = "RealisticShield1"
calo_film_thickness = 25 um
tracking_gas_material = "TrackingGasDefault"

