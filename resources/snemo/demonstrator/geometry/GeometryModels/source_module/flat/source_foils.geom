# -*- mode: conf-unix; -*-
# Author : Y.LEMIERE
# Date   : June 2018
# Geometry based on Andrea source foil review



###############################################################################
[name="short_film.realistic.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =   12.0 um 
y                  : real as length =    125 mm
z                  : real as length =  425.0 mm

material.ref       : string  = "basic::mylar"

visibility.hidden  : boolean = false
visibility.color : string = "cyan"


###############################################################################
[name="source_film.realistic.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =   12.0 um 
y                  : real as length =  135.5 mm
z                  : real as length = 3350.0 mm

material.ref       : string  = "basic::mylar"

visibility.hidden  : boolean = false
visibility.color   : string  = "cyan"

##########################################################################

[name="snemo_foil_0_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 12.5  # cm
z                  : real    = 250.0  # cm
x                  : real as length = 74.0 um
material.ref       : string  = "basic::copper"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"






[name="snemo_source_0.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real    =   43.0 um
y                  : real    =   12.5 cm
z                  : real    = 3350.0 mm

material.ref       : string  = "tracking_gas"

# #@description The list of internal items
internal_item.labels : string[3] = "bottom_film" "source_0_0" "top_film"

# Describe the internal items :
internal_item.model.bottom_film       : string  = "short_film.realistic.model"
internal_item.placement.bottom_film   : string  = "0 0 -1460 (mm) "

internal_item.model.source_0_0        : string  = "snemo_foil_0_pad_0.model"
internal_item.placement.source_0_0    : string  = "0 0 0 (mm) "

internal_item.model.top_film          : string  = "short_film.realistic.model"
internal_item.placement.top_film      : string  = "0 0 +1460 (mm) "

visibility.color  : string = "green"

# # Mapping the source foil from its parent category :
mapping.daughter_id.source_0_0 : string = "[source_pad:pad=0]"
#mapping.daughter_id.bottom_film  : string = "[source_strip_film:film=0]"
#mapping.daughter_id.top_film  : string = "[source_strip_film:film=1]"	



#########################################################################
[name="snemo_strip_0.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 1

# stacked.model_0   : string = "source_film.realistic.model"
# stacked.label_0   : string = "back_film"

stacked.model_0  : string = "snemo_source_0.model"
stacked.label_0  : string = "source"

# stacked.model_2   : string = "source_film.realistic.model"
# stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
# mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
# mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


##########################################################################

[name="snemo_foil_1_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 230.0 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.811"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_1.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_1_pad_0.model"
stacked.label_0   : string  = "snemo_pad_1_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_1_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_1.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_1.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_2_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 210.0 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.951"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_2.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_2_pad_0.model"
stacked.label_0   : string  = "snemo_pad_2_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_2_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_2.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_2.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_3_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 220.0 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.800"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_3.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_3_pad_0.model"
stacked.label_0   : string  = "snemo_pad_3_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_3_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_3.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_3.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_4_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 250.0 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.258"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_4.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_4_pad_0.model"
stacked.label_0   : string  = "snemo_pad_4_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_4_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_4.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_4.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_5_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 200.0 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.823"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_5.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_5_pad_0.model"
stacked.label_0   : string  = "snemo_pad_5_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_5_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_5.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_5.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_6_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 220.0 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.960"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_6.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_6_pad_0.model"
stacked.label_0   : string  = "snemo_pad_6_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_6_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_6.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_6.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_7_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 240.0 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.592"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_7.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_7_pad_0.model"
stacked.label_0   : string  = "snemo_pad_7_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_7_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_7.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_7.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_8_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.5  # cm
x                  : real as length = 200.0 um
material.ref       : string  = "snemo::se82_enriched96.92_pva0.0787_density2.122"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_8.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_8_pad_0.model"
stacked.label_0   : string  = "snemo_pad_8_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_8_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_8.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_8.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_9_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 190.0 um
material.ref       : string  = "snemo::se82_enriched96.92_pva0.0780_density2.081"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_9.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_9_pad_0.model"
stacked.label_0   : string  = "snemo_pad_9_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_9_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_9.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_9.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_10_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 200.0 um
material.ref       : string  = "snemo::se82_enriched96.92_pva0.0790_density1.971"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_10.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_10_pad_0.model"
stacked.label_0   : string  = "snemo_pad_10_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_10_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_10.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_10.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_11_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 269.5  # cm
x                  : real as length = 200.0 um
material.ref       : string  = "snemo::se82_enriched96.92_pva0.0783_density1.949"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_11.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_11_pad_0.model"
stacked.label_0   : string  = "snemo_pad_11_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_11_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_11.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_11.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_12_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 300.0 um
material.ref       : string  = "snemo::se82_enriched98.31_pva0.0786_density2.198"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_12.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_12_pad_0.model"
stacked.label_0   : string  = "snemo_pad_12_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_12_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_12.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_12.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_13_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 280.0 um
material.ref       : string  = "snemo::se82_enriched98.31_pva0.0820_density2.104"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_13.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_13_pad_0.model"
stacked.label_0   : string  = "snemo_pad_13_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_13_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_13.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_13.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_14_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 300.0 um
material.ref       : string  = "snemo::se82_enriched99.92_pva0.0771_density2.217"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_14.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_14_pad_0.model"
stacked.label_0   : string  = "snemo_pad_14_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_14_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_14.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_14.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_15_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 280.0 um
material.ref       : string  = "snemo::se82_enriched99.6_pva0.0784_density2.388"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_15.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_15_pad_0.model"
stacked.label_0   : string  = "snemo_pad_15_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_15_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_15.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_15.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_16_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 280.0 um
material.ref       : string  = "snemo::se82_enriched97_pva0.0801_density2.276"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_16.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_16_pad_0.model"
stacked.label_0   : string  = "snemo_pad_16_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_16_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_16.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_16.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_17_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 250.0 um
material.ref       : string  = "snemo::se82_enriched97.5_pva0.0810_density2.300"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_17.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_17_pad_0.model"
stacked.label_0   : string  = "snemo_pad_17_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_17_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_17.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_17.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_18_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 240.0 um
material.ref       : string  = "snemo::se82_enriched98_pva0.0820_density2.160"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_18.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_18_pad_0.model"
stacked.label_0   : string  = "snemo_pad_18_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_18_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_18.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_18.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_19_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.2  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 390.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.227"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_19_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.2  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 397.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.299"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_19_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 283.3 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.952"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_19_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 304.17 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.886"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_19_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 304.17 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.795"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_19_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 292.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.899"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_19_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 273.3 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.959"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_19_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 285.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.919"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_19.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_19_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_19_7"

stacked.model_6   : string  = "snemo_foil_19_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_19_6"

stacked.model_5   : string  = "snemo_foil_19_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_19_5"

stacked.model_4   : string  = "snemo_foil_19_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_19_4"

stacked.model_3   : string  = "snemo_foil_19_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_19_3"

stacked.model_2   : string  = "snemo_foil_19_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_19_2"

stacked.model_1   : string  = "snemo_foil_19_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_19_1"

stacked.model_0   : string  = "snemo_foil_19_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_19_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

    
mapping.daughter_id.snemo_pad_19_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_19_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_19_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_19_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_19_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_19_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_19_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_19_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_19.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_19.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_20_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 460.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.236"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_20_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 410.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.140"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_20_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 377.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.268"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_20_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 365.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.288"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_20_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 289.17 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.972"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_20_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 263.33 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density2.096"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_20_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 279.17 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density2.093"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_20_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 266.67 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density2.068"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_20.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_20_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_20_7"

stacked.model_6   : string  = "snemo_foil_20_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_20_6"

stacked.model_5   : string  = "snemo_foil_20_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_20_5"

stacked.model_4   : string  = "snemo_foil_20_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_20_4"

stacked.model_3   : string  = "snemo_foil_20_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_20_3"

stacked.model_2   : string  = "snemo_foil_20_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_20_2"

stacked.model_1   : string  = "snemo_foil_20_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_20_1"

stacked.model_0   : string  = "snemo_foil_20_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_20_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :


mapping.daughter_id.snemo_pad_20_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_20_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_20_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_20_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_20_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_20_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_20_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_20_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_20.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_20.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_21_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 320.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.708"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_21_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 322.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.648"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_21_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 420.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.296"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_21_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 420.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.300"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_21_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 427.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.333"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_21_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 424.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.088"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_21_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 472.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.030"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_21_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 495.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.060"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_21.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_21_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_21_7"

stacked.model_6   : string  = "snemo_foil_21_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_21_6"

stacked.model_5   : string  = "snemo_foil_21_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_21_5"

stacked.model_4   : string  = "snemo_foil_21_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_21_4"

stacked.model_3   : string  = "snemo_foil_21_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_21_3"

stacked.model_2   : string  = "snemo_foil_21_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_21_2"

stacked.model_1   : string  = "snemo_foil_21_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_21_1"

stacked.model_0   : string  = "snemo_foil_21_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_21_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_21_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_21_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_21_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_21_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_21_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_21_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_21_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_21_7 : string = "[source_pad:pad=7]"
    
    
#########################################################################
[name="snemo_strip_21.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_21.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_22_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 465.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.372"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_22_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 467.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.362"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_22_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 480.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.347"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_22_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 312.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.586"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_22_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 315.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.670"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_22_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 342.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.576"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_22_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 320.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.717"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_22_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 330.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.678"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_22.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_22_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_22_7"

stacked.model_6   : string  = "snemo_foil_22_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_22_6"

stacked.model_5   : string  = "snemo_foil_22_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_22_5"

stacked.model_4   : string  = "snemo_foil_22_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_22_4"

stacked.model_3   : string  = "snemo_foil_22_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_22_3"

stacked.model_2   : string  = "snemo_foil_22_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_22_2"

stacked.model_1   : string  = "snemo_foil_22_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_22_1"

stacked.model_0   : string  = "snemo_foil_22_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_22_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

    
mapping.daughter_id.snemo_pad_22_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_22_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_22_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_22_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_22_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_22_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_22_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_22_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_22.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_22.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_23_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 345.83 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.591"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_23_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 342.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.758"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_23_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 340.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.730"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_23_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 330.83 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.721"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_23_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 326.67 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.708"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_23_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 343.33 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.654"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_23_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 334.17 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.664"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_23_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 316.17 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.749"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_23.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_23_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_23_7"

stacked.model_6   : string  = "snemo_foil_23_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_23_6"

stacked.model_5   : string  = "snemo_foil_23_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_23_5"

stacked.model_4   : string  = "snemo_foil_23_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_23_4"

stacked.model_3   : string  = "snemo_foil_23_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_23_3"

stacked.model_2   : string  = "snemo_foil_23_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_23_2"

stacked.model_1   : string  = "snemo_foil_23_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_23_1"

stacked.model_0   : string  = "snemo_foil_23_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_23_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
   
    
mapping.daughter_id.snemo_pad_23_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_23_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_23_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_23_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_23_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_23_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_23_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_23_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_23.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_23.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_24_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.2  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 472.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.363"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_24_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 370.83 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.480"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_24_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 328.33 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.714"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_24_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 365.83 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.580"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_24_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 363.33 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.524"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_24_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 365.0 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.517"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_24_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 372.53 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.552"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_24_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 360.83 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.547"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_24.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_24_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_24_7"

stacked.model_6   : string  = "snemo_foil_24_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_24_6"

stacked.model_5   : string  = "snemo_foil_24_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_24_5"

stacked.model_4   : string  = "snemo_foil_24_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_24_4"

stacked.model_3   : string  = "snemo_foil_24_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_24_3"

stacked.model_2   : string  = "snemo_foil_24_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_24_2"

stacked.model_1   : string  = "snemo_foil_24_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_24_1"

stacked.model_0   : string  = "snemo_foil_24_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_24_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
   
    
mapping.daughter_id.snemo_pad_24_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_24_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_24_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_24_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_24_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_24_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_24_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_24_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_24.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_24.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_25_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 350.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.535"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_25_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 383.33 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.300"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_25_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 379.17 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.390"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_25_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 360.83 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.448"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_25_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 362.5 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.414"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_25_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 353.33 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.545"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_25_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 355.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.526"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_25_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 366.67 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.584"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_25.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_25_pad_7.model"
stacked.limit_max_7 : real as length =16.9 cm
stacked.label_7   : string  = "snemo_pad_25_7"

stacked.model_6   : string  = "snemo_foil_25_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_25_6"

stacked.model_5   : string  = "snemo_foil_25_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_25_5"

stacked.model_4   : string  = "snemo_foil_25_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_25_4"

stacked.model_3   : string  = "snemo_foil_25_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_25_3"

stacked.model_2   : string  = "snemo_foil_25_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_25_2"

stacked.model_1   : string  = "snemo_foil_25_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_25_1"

stacked.model_0   : string  = "snemo_foil_25_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_25_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_25_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_25_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_25_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_25_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_25_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_25_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_25_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_25_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_25.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_25.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_26_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 364.17 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.577"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_26_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 337.5 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.599"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_26_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 360.83 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.566"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_26_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 386.67 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.525"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_26_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 372.5 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.537"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_26_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 360.83 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.521"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_26_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 372.5 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.530"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_26_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 370.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.543"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_26.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_26_pad_7.model"
stacked.limit_max_7 : real as length =16.9 cm
stacked.label_7   : string  = "snemo_pad_26_7"

stacked.model_6   : string  = "snemo_foil_26_pad_6.model"
stacked.limit_max_6 : real as length =16.9 cm
stacked.label_6   : string  = "snemo_pad_26_6"

stacked.model_5   : string  = "snemo_foil_26_pad_5.model"
stacked.limit_max_5 : real as length =16.9 cm
stacked.label_5   : string  = "snemo_pad_26_5"

stacked.model_4   : string  = "snemo_foil_26_pad_4.model"
stacked.limit_max_4 : real as length =16.9 cm
stacked.label_4   : string  = "snemo_pad_26_4"

stacked.model_3   : string  = "snemo_foil_26_pad_3.model"
stacked.limit_max_3 : real as length =16.9 cm
stacked.label_3   : string  = "snemo_pad_26_3"

stacked.model_2   : string  = "snemo_foil_26_pad_2.model"
stacked.limit_max_2 : real as length =16.9 cm
stacked.label_2   : string  = "snemo_pad_26_2"

stacked.model_1   : string  = "snemo_foil_26_pad_1.model"
stacked.limit_max_1 : real as length =16.9 cm
stacked.label_1   : string  = "snemo_pad_26_1"

stacked.model_0   : string  = "snemo_foil_26_pad_0.model"
stacked.limit_max_0 : real as length =16.9 cm
stacked.label_0   : string  = "snemo_pad_26_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_26_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_26_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_26_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_26_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_26_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_26_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_26_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_26_7 : string = "[source_pad:pad=7]"



    
#########################################################################
[name="snemo_strip_26.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_26.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_27_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 361.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.487"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_27_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 336.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.617"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_27_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 339.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.626"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_27_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 368.18 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.542"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_27_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 355.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.597"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_27_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 349.17 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.596"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_27_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 365.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.562"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_27_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 357.5 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.590"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_27.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_27_pad_7.model"
stacked.limit_max_7 : real as length =16.9 cm
stacked.label_7   : string  = "snemo_pad_27_7"

stacked.model_6   : string  = "snemo_foil_27_pad_6.model"
stacked.limit_max_6 : real as length =16.9 cm
stacked.label_6   : string  = "snemo_pad_27_6"

stacked.model_5   : string  = "snemo_foil_27_pad_5.model"
stacked.limit_max_5 : real as length =16.9 cm
stacked.label_5   : string  = "snemo_pad_27_5"

stacked.model_4   : string  = "snemo_foil_27_pad_4.model"
stacked.limit_max_4 : real as length =16.9 cm
stacked.label_4   : string  = "snemo_pad_27_4"

stacked.model_3   : string  = "snemo_foil_27_pad_3.model"
stacked.limit_max_3 : real as length =16.9 cm
stacked.label_3   : string  = "snemo_pad_27_3"

stacked.model_2   : string  = "snemo_foil_27_pad_2.model"
stacked.limit_max_2 : real as length =16.9 cm
stacked.label_2   : string  = "snemo_pad_27_2"

stacked.model_1   : string  = "snemo_foil_27_pad_1.model"
stacked.limit_max_1 : real as length =16.9 cm
stacked.label_1   : string  = "snemo_pad_27_1"

stacked.model_0   : string  = "snemo_foil_27_pad_0.model"
stacked.limit_max_0 : real as length =16.9 cm
stacked.label_0   : string  = "snemo_pad_27_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_27_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_27_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_27_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_27_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_27_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_27_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_27_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_27_7 : string = "[source_pad:pad=7]"


    
#########################################################################
[name="snemo_strip_27.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_27.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_28_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.5  # cm
x                  : real as length = 471.25 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.408"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_28_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.5  # cm
x                  : real as length = 400.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.306"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_28_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.5  # cm
x                  : real as length = 411.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.257"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_28_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.5  # cm
x                  : real as length = 402.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.317"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_28_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.5  # cm
x                  : real as length = 352.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.578"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_28_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.5  # cm
x                  : real as length = 351.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.531"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_28_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.5  # cm
x                  : real as length = 390.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.444"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_28_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 347.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.597"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_28.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_28_pad_7.model"
stacked.limit_max_7 : real as length =16.95 cm
stacked.label_7   : string  = "snemo_pad_28_7"

stacked.model_6   : string  = "snemo_foil_28_pad_6.model"
stacked.limit_max_6 : real as length =16.95 cm
stacked.label_6   : string  = "snemo_pad_28_6"

stacked.model_5   : string  = "snemo_foil_28_pad_5.model"
stacked.limit_max_5 : real as length =16.95 cm
stacked.label_5   : string  = "snemo_pad_28_5"

stacked.model_4   : string  = "snemo_foil_28_pad_4.model"
stacked.limit_max_4 : real as length =16.95 cm
stacked.label_4   : string  = "snemo_pad_28_4"

stacked.model_3   : string  = "snemo_foil_28_pad_3.model"
stacked.limit_max_3 : real as length =16.95 cm
stacked.label_3   : string  = "snemo_pad_28_3"

stacked.model_2   : string  = "snemo_foil_28_pad_2.model"
stacked.limit_max_2 : real as length =16.95 cm
stacked.label_2   : string  = "snemo_pad_28_2"

stacked.model_1   : string  = "snemo_foil_28_pad_1.model"
stacked.limit_max_1 : real as length =16.95 cm
stacked.label_1   : string  = "snemo_pad_28_1"

stacked.model_0   : string  = "snemo_foil_28_pad_0.model"
stacked.limit_max_0 : real as length =16.9 cm
stacked.label_0   : string  = "snemo_pad_28_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_28_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_28_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_28_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_28_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_28_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_28_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_28_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_28_7 : string = "[source_pad:pad=7]"


    
#########################################################################
[name="snemo_strip_28.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_28.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_29_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 259.17 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.593"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_29_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 275.0 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.646"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_29_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.35  # cm
x                  : real as length = 302.5 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.570"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_29_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 311.67 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.486"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_29_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 287.5 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.509"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_29_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 304.17 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.444"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_29_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.4  # cm
x                  : real as length = 290.83 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.449"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_29_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.3  # cm
x                  : real as length = 282.17 um
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.596"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_29.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_29_pad_7.model"
stacked.limit_max_7 : real as length =16.9 cm
stacked.label_7   : string  = "snemo_pad_29_7"

stacked.model_6   : string  = "snemo_foil_29_pad_6.model"
stacked.limit_max_6 : real as length =16.9 cm
stacked.label_6   : string  = "snemo_pad_29_6"

stacked.model_5   : string  = "snemo_foil_29_pad_5.model"
stacked.limit_max_5 : real as length =16.875 cm
stacked.label_5   : string  = "snemo_pad_29_5"

stacked.model_4   : string  = "snemo_foil_29_pad_4.model"
stacked.limit_max_4 : real as length =16.9 cm
stacked.label_4   : string  = "snemo_pad_29_4"

stacked.model_3   : string  = "snemo_foil_29_pad_3.model"
stacked.limit_max_3 : real as length =16.9 cm
stacked.label_3   : string  = "snemo_pad_29_3"

stacked.model_2   : string  = "snemo_foil_29_pad_2.model"
stacked.limit_max_2 : real as length =16.9 cm
stacked.label_2   : string  = "snemo_pad_29_2"

stacked.model_1   : string  = "snemo_foil_29_pad_1.model"
stacked.limit_max_1 : real as length =16.9 cm
stacked.label_1   : string  = "snemo_pad_29_1"

stacked.model_0   : string  = "snemo_foil_29_pad_0.model"
stacked.limit_max_0 : real as length =16.849999999999998 cm
stacked.label_0   : string  = "snemo_pad_29_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_29_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_29_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_29_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_29_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_29_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_29_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_29_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_29_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_29.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_29.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_30_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 348.75 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.737"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_30_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 308.3 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.148"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_30_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 280.83 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.200"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_30_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 291.67 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.176"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_30_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 309.17 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.208"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_30_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 310.0 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.107"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_30_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.25  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 277.08 um
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.321"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_30_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 346.66 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.725"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_30.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_30_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_30_7"

stacked.model_6   : string  = "snemo_foil_30_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_30_6"

stacked.model_5   : string  = "snemo_foil_30_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_30_5"

stacked.model_4   : string  = "snemo_foil_30_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_30_4"

stacked.model_3   : string  = "snemo_foil_30_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_30_3"

stacked.model_2   : string  = "snemo_foil_30_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_30_2"

stacked.model_1   : string  = "snemo_foil_30_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_30_1"

stacked.model_0   : string  = "snemo_foil_30_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_30_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_30_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_30_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_30_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_30_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_30_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_30_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_30_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_30_7 : string = "[source_pad:pad=7]"



    
#########################################################################
[name="snemo_strip_30.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_30.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_31_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 302.5 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.865"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_31_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 276.67 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.024"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_31_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 286.67 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.914"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_31_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 377.5 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.723"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_31_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 331.25 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.789"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_31_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 322.5 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.785"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_31_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 298.5 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.762"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_31_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 310.75 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.767"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_31.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_31_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_31_7"

stacked.model_6   : string  = "snemo_foil_31_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_31_6"

stacked.model_5   : string  = "snemo_foil_31_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_31_5"

stacked.model_4   : string  = "snemo_foil_31_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_31_4"

stacked.model_3   : string  = "snemo_foil_31_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_31_3"

stacked.model_2   : string  = "snemo_foil_31_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_31_2"

stacked.model_1   : string  = "snemo_foil_31_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_31_1"

stacked.model_0   : string  = "snemo_foil_31_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_31_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

    
mapping.daughter_id.snemo_pad_31_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_31_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_31_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_31_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_31_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_31_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_31_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_31_7 : string = "[source_pad:pad=7]"

#########################################################################
[name="snemo_strip_31.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_31.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_32_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 286.67 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.850"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_32_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 275.17 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.930"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_32_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 308.89 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.762"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_32_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 323.33 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.861"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_32_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 271.67 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.974"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_32_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 296.25 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.828"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_32_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 288.3 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.911"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_32_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 288.83 um
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.890"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_32.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_32_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_32_7"

stacked.model_6   : string  = "snemo_foil_32_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_32_6"

stacked.model_5   : string  = "snemo_foil_32_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_32_5"

stacked.model_4   : string  = "snemo_foil_32_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_32_4"

stacked.model_3   : string  = "snemo_foil_32_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_32_3"

stacked.model_2   : string  = "snemo_foil_32_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_32_2"

stacked.model_1   : string  = "snemo_foil_32_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_32_1"

stacked.model_0   : string  = "snemo_foil_32_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_32_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_32_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_32_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_32_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_32_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_32_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_32_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_32_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_32_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_32.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_32.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_33_pad_7.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.2  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 452.5 um
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.367"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_33_pad_6.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 285.0 um
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density2.098"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_33_pad_5.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 305.83 um
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.955"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_33_pad_4.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 309.17 um
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.991"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_33_pad_3.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 293.33 um
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.947"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_33_pad_2.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 304.17 um
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.964"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_33_pad_1.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 310.83 um
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.990"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

##########################################################################

[name="snemo_foil_33_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.3  # cm
z                  : real    = 33.55  # cm
x                  : real as length = 309.17 um
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.975"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_33.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8

stacked.model_7   : string  = "snemo_foil_33_pad_7.model"
stacked.limit_max_7 : real as length =16.974999999999998 cm
stacked.label_7   : string  = "snemo_pad_33_7"

stacked.model_6   : string  = "snemo_foil_33_pad_6.model"
stacked.limit_max_6 : real as length =16.974999999999998 cm
stacked.label_6   : string  = "snemo_pad_33_6"

stacked.model_5   : string  = "snemo_foil_33_pad_5.model"
stacked.limit_max_5 : real as length =16.974999999999998 cm
stacked.label_5   : string  = "snemo_pad_33_5"

stacked.model_4   : string  = "snemo_foil_33_pad_4.model"
stacked.limit_max_4 : real as length =16.974999999999998 cm
stacked.label_4   : string  = "snemo_pad_33_4"

stacked.model_3   : string  = "snemo_foil_33_pad_3.model"
stacked.limit_max_3 : real as length =16.974999999999998 cm
stacked.label_3   : string  = "snemo_pad_33_3"

stacked.model_2   : string  = "snemo_foil_33_pad_2.model"
stacked.limit_max_2 : real as length =16.974999999999998 cm
stacked.label_2   : string  = "snemo_pad_33_2"

stacked.model_1   : string  = "snemo_foil_33_pad_1.model"
stacked.limit_max_1 : real as length =16.974999999999998 cm
stacked.label_1   : string  = "snemo_pad_33_1"

stacked.model_0   : string  = "snemo_foil_33_pad_0.model"
stacked.limit_max_0 : real as length =16.974999999999998 cm
stacked.label_0   : string  = "snemo_pad_33_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :

mapping.daughter_id.snemo_pad_33_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.snemo_pad_33_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.snemo_pad_33_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.snemo_pad_33_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.snemo_pad_33_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.snemo_pad_33_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.snemo_pad_33_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.snemo_pad_33_7 : string = "[source_pad:pad=7]"
    
#########################################################################
[name="snemo_strip_33.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_33.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_34_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 13.5  # cm
z                  : real    = 270.0  # cm
x                  : real as length = 280.0 um
material.ref       : string  = "snemo::se82_enriched97.0_pva0.1000_density2.137"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

#########################################################################
[name="snemo_source_34.model" type="geomtools::stacked_model"]
material.ref            : string = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 1

stacked.model_0   : string  = "snemo_foil_34_pad_0.model"
stacked.label_0   : string  = "snemo_pad_34_0"



visibility.hidden  : boolean = 0

# Mapping the source foil from its parent category :
mapping.daughter_id.snemo_pad_34_0 : string = "[source_pad:pad=0]"
    
#########################################################################
[name="snemo_strip_34.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_film.realistic.model"
stacked.label_0   : string = "back_film"

stacked.model_1   : string = "snemo_source_34.model"
stacked.label_1   : string = "source"

stacked.model_2   : string = "source_film.realistic.model"
stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# Mapping the daughter volumes :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"

##########################################################################

[name="snemo_foil_35_pad_0.model" type="geomtools::simple_shaped_model"]
shape_type         : string  = "box"
length_unit        : string  = "cm"
y                  : real    = 12.5  # cm
z                  : real    = 250.0  # cm
x                  : real as length = 74.0 um
material.ref       : string  = "basic::copper"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"

[name="snemo_source_35.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real    =   43.0 um
y                  : real    =   12.5 cm
z                  : real    = 3350.0 mm

material.ref       : string  = "tracking_gas"

# #@description The list of internal items
internal_item.labels : string[3] = "bottom_film" "source_35_0" "top_film"

# Describe the internal items :
internal_item.model.bottom_film       : string  = "short_film.realistic.model"
internal_item.placement.bottom_film   : string  = "0 0 -1460 (mm) "

internal_item.model.source_35_0        : string  = "snemo_foil_35_pad_0.model"
internal_item.placement.source_35_0    : string  = "0 0 0 (mm) "

internal_item.model.top_film          : string  = "short_film.realistic.model"
internal_item.placement.top_film      : string  = "0 0 +1460 (mm) "

visibility.color  : string = "green"

# # Mapping the source foil from its parent category :
mapping.daughter_id.source_35_0 : string = "[source_pad:pad=0]"
#mapping.daughter_id.bottom_film  : string = "[source_strip_film:film=0]"
#mapping.daughter_id.top_film  : string = "[source_strip_film:film=1]"	




#########################################################################
[name="snemo_strip_35.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 1

# stacked.model_0   : string = "source_film.realistic.model"
# stacked.label_0   : string = "back_film"

stacked.model_0  : string = "snemo_source_35.model"
stacked.label_0  : string = "source"

# stacked.model_2   : string = "source_film.realistic.model"
# stacked.label_2   : string = "front_film"

material.ref      : string = "tracking_gas"

visibility.color  : string = "green"

# # Mapping the daughter volumes :
# mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
# mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"