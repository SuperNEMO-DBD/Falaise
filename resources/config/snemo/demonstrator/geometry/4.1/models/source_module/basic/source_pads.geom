# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.1/models/source_module/basic/source_pads.geom

###############
# Source pads #
###############

# Description of source pads geometry for 'basic' source layout

##############################################################################
[name="source_external_pad.basic.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length = @variant(geometry:layout/if_basic/source_layout/if_basic/thickness|250 um)
y                  : real as length = 125.0 mm
z                  : real as length = 337.5 mm

material.ref       : string  = "bb_source_material.basic"

visibility.color   : string  = "magenta"


##############################################################################
[name="source_internal_pad.basic.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length = @variant(geometry:layout/if_basic/source_layout/if_basic/thickness|250 um)
y                  : real as length = 135.5 mm
z                  : real as length = 337.5 mm

material.ref       : string  = "bb_source_material.basic"

visibility.color   : string  = "magenta"


# end of @falaise:config/snemo/demonstrator/geometry/4.1/models/source_module/basic/source_pads.geom