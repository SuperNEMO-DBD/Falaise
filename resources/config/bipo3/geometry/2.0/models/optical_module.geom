# -*- mode: conf-unix; -*-

##################################
#@description BiPo3 Optical module
##################################
#
# DocDB #1392
# http://nile.hep.utexas.edu/DocDB/ut-nemo/docs/0013/001392/002/bongrand-bipo3-10-2010.pdf
#

##############################################################
[name="reflector.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real as length = 300.0 mm
y : real as length = 300.0 mm
z : real as length =   0.2 um

#material.ref     : string  = "snemo::aluminium"
material.ref      : string  = "snemo::lab_air"

# --> Visibility:
visibility.hidden : boolean = 0
visibility.color  : string  = "green"


###############################################################
[name="scin_block.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real as length = 300.0 mm
y : real as length = 300.0 mm
z : real as length =   2.0 mm

material.ref       : string = "std::polystyrene"

# --> Visibility
visibility.hidden  : boolean = 0
visibility.color   : string = "blue"

# --> Sensitive detector category
sensitive.category : string = "scin_SD"


################################################################
[name="light_guide.model" type="geomtools::simple_shaped_model"]
shape_type : string = "polyhedra"
build_mode : string = "points"
sides        : integer = 4
list_of_z    : real [3] =  -50.      +45.     +50.
list_of_rmin : real [3] =    0.        0.       0.
list_of_rmax : real [3] =   70.      150.     150.
length_unit  : string = "mm"

material.ref : string  = "std::plexiglass"

# --> Visibility:
visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"


########################################################################
[name="rotated_light_guide.model" type="geomtools::rotated_boxed_model"]
rotated.axis  : string = "z"
rotated.angle : real as angle = 45.0 degree

x             : real as length = 300.0 mm
y             : real as length = 300.0 mm

rotated.model : string = "light_guide.model"
rotated.label : string = "light_guide"

material.ref  : string  = "snemo::nitrogen"

visibility.hidden_envelope  : boolean = 0
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "grey"

mapping.daughter_id.light_guide : string = "[light_guide.category]"


######################################################################################
[name="tapered_light_guide.model" type="snemo::geometry::calo_tapered_scin_box_model"]
logging.priority : string = "warning"
width                  : real as length = 300.0 mm
z                      : real as length = 140.0 mm
h                      : real as length = 100.0 mm
r                      : real as length =  85.0 mm
tapered_r              : real as length =  80.0 mm
tapered_angle          : real as angle  =  50.0 degree
optical_glue_thickness : real as length =   1.0 mm
material.ref  : string = "std::plexiglass"

# --> Visibility:
visibility.hidden          : boolean = 0
visibility.color           : string  = "cyan"


##############################################################################
[name="support_plate.model" type="geomtools::cylindric_extrusion_boxed_model"]
extrusion_radius : real as length =  75.0 mm
x                : real as length = 250.0 mm
y                : real as length = 250.0 mm
z                : real as length =  10.0 mm

material.ref : string = "std::delrin"

visibility.hidden : boolean = 0
visibility.color  : string  = "blue"


####################################################################
[name="protection_tube.model" type="geomtools::simple_shaped_model"]
shape_type : string = "tube"
outer_r : real as length =  78.0 mm
inner_r : real as length =  75.0 mm
z       : real as length = 190.0 mm
material.ref : string = "std::delrin"

# --> Visibility:
visibility.hidden : boolean = 0
visibility.color  : string = "blue"


######################################################################
[name="protection_endcap.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x : real as length = 160.0 mm
y : real as length = 160.0 mm
z : real as length =  10.0 mm
material.ref : string = "std::delrin"

# --> Visibility:
visibility.hidden : boolean = 0
visibility.color  : string = "blue"


#############################################################
[name="optical_module.model" type="geomtools::stacked_model"]
material.ref : string = "snemo::nitrogen"
#z            : real   = 400.0 # mm
length_unit  : string = "mm"

stacked.axis             : string  = "z"
stacked.number_of_items  : integer = 8

stacked.model_0     : string  = "protection_endcap.model"
stacked.label_0     : string  = "protection_endcap"

stacked.model_1     : string  = "protection_tube.model"
stacked.label_1     : string  = "protection_tube"
stacked.limit_max_1 : real    = 95

stacked.model_2     : string  = "support_plate.model"
stacked.label_2     : string  = "support_plate"
stacked.limit_max_2 : real    = -180

stacked.model_3     : string  = "PMT_HAMAMATSU_R6594.base"
stacked.label_3     : string  = "PMT_base"

stacked.model_4     : string  = "PMT_HAMAMATSU_R6594"
stacked.label_4     : string  = "PMT"

stacked.model_5     : string  = "tapered_light_guide.model"
stacked.label_5     : string  = "light_guide"

stacked.model_6     : string  = "scin_block.model"
stacked.label_6     : string  = "scin_block"

stacked.model_7     : string  = "reflector.model"
stacked.label_7     : string  = "reflector"


# --> Visibility:
visibility.hidden           : boolean = 0
visibility.hidden_envelop   : boolean = 0
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "grey"

# --> Daughters mapping informations:
mapping.daughter_id.reflector         : string = "[reflector.category]"
mapping.daughter_id.scin_block        : string = "[scin_block.category]"
mapping.daughter_id.PMT_base          : string = "[pmt_base.category]"
mapping.daughter_id.PMT               : string = "[pmt.category]"
mapping.daughter_id.light_guide       : string = "[light_guide.category]"
mapping.daughter_id.protection_endcap : string = "[protection_endcap.category]"
mapping.daughter_id.protection_tube   : string = "[protection_tube.category]"
mapping.daughter_id.support_plate     : string = "[support_plate.category]"


# end of file
