#@format=datatools::configuration::variant
#@format.version=1.0
#@organization=snemo
#@application=falaise

[registry="geometry"]
layout = "HalfCommissioning"
layout/if_half_commissioning/gap = 5 mm
calo_film_thickness = 25 um

[registry="vertexes"]
generator = "commissioning_all_spots"

[registry="primary_events"]
generator = "Co60"

[registry="simulation"]
physics_mode = "Constructors"
physics_mode/if_constructors/em_model = "standard"
production_cuts = true
output_profile = "none"

