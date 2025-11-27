#///////////////////////Support Frame Beam Top Short///////////////////////

[name="support_frame_top_short_beam_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 18 mm 
  y : real as length = 2417 mm
  z : real as length = 460 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_top_short_beam_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 185 mm 
  y : real as length = 2417 mm
  z : real as length = 20 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_top_short_beam.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "support_frame_top_short_beam_section_b.model"
    stacked.label_0 : string = "t_beam_1"
    stacked.model_1 : string = "support_frame_top_short_beam_section_a.model"
    stacked.label_1 : string = "central_beam"
    stacked.model_2 : string = "support_frame_top_short_beam_section_b.model"
    stacked.label_2 : string = "t_beam_2"
    
  material.ref : string = "lab_air"


#///////////////////////Support Frame Beam Top Long///////////////////////

[name="support_frame_top_long_beam_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 18 mm 
  y : real as length = 4800 mm
  z : real as length = 460 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_top_long_beam_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 185 mm 
  y : real as length = 4800 mm
  z : real as length = 20 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_top_long_beam.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "support_frame_top_long_beam_section_b.model"
    stacked.label_0 : string = "t_beam_1"
    stacked.model_1 : string = "support_frame_top_long_beam_section_a.model"
    stacked.label_1 : string = "central_beam"
    stacked.model_2 : string = "support_frame_top_long_beam_section_b.model"
    stacked.label_2 : string = "t_beam_2"
    
  material.ref : string = "lab_air"


#///////////////////////Support Frame Beam Top Iron Shield Segment 1///////////////////////

[name="support_frame_top_long_beam_segment_1_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 18 mm 
  y : real as length = 930 mm
  z : real as length = 460 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_top_long_beam_segment_1_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 185 mm 
  y : real as length = 930 mm
  z : real as length = 20 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_top_long_beam_segment_1.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "support_frame_top_long_beam_segment_1_section_b.model"
    stacked.label_0 : string = "t_beam_1"
    stacked.model_1 : string = "support_frame_top_long_beam_segment_1_section_a.model"
    stacked.label_1 : string = "central_beam"
    stacked.model_2 : string = "support_frame_top_long_beam_segment_1_section_b.model"
    stacked.label_2 : string = "t_beam_2"
    
  material.ref : string = "lab_air"

#///////////////////////Support Frame Beam Top Iron Shield Segment 2///////////////////////


[name="support_frame_top_long_beam_segment_2_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 18 mm 
  y : real as length = 3000 mm
  z : real as length = 460 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"

[name="support_frame_top_long_beam_segment_2_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 185 mm 
  y : real as length = 3000 mm
  z : real as length = 20 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_top_long_beam_segment_2.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "support_frame_top_long_beam_segment_2_section_b.model"
    stacked.label_0 : string = "t_beam_1"
    stacked.model_1 : string = "support_frame_top_long_beam_segment_2_section_a.model"
    stacked.label_1 : string = "central_beam"
    stacked.model_2 : string = "support_frame_top_long_beam_segment_2_section_b.model"
    stacked.label_2 : string = "t_beam_2"
    
  material.ref : string = "lab_air"


#///////////////////////Support Frame Beam Top Iron Shield Segment in Wall///////////////////////

[name="support_frame_top_long_in_iron_wall_beam_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 18 mm 
  y : real as length = 180 mm
  z : real as length = 460 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"

[name="support_frame_top_long_in_iron_wall_beam_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 185 mm 
  y : real as length = 180 mm
  z : real as length = 20 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"

[name="support_frame_top_long_in_iron_wall_beam.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "support_frame_top_long_in_iron_wall_beam_section_b.model"
    stacked.label_0 : string = "t_beam_1"
    stacked.model_1 : string = "support_frame_top_long_in_iron_wall_beam_section_a.model"
    stacked.label_1 : string = "central_beam"
    stacked.model_2 : string = "support_frame_top_long_in_iron_wall_beam_section_b.model"
    stacked.label_2 : string = "t_beam_2"
    
  material.ref : string = "lab_air"


#///////////////////////Support Frame Bot///////////////////////

[name="support_frame_bot_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 25.5 mm 
  y : real as length = 4800 mm
  z : real as length = 540 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_bot_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 300 mm 
  y : real as length = 4800 mm
  z : real as length = 30 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_bot.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "z"
stacked.number_of_items : integer = 3
stacked.model_0 : string = "support_frame_bot_section_b.model"
stacked.label_0 : string = "t_beam_1"
stacked.model_1 : string = "support_frame_bot_section_a.model"
stacked.label_1 : string = "central_beam"
stacked.model_2 : string = "support_frame_bot_section_b.model"
stacked.label_2 : string = "t_beam_2"
    
material.ref : string = "lab_air"


#///////////////////////Support Frame Bot Iron Shield Segment 1///////////////////////

[name="support_frame_bot_beam_segment_1_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 25.5 mm 
  y : real as length = 930 mm
  z : real as length = 540 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_bot_beam_segment_1_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 300 mm 
  y : real as length = 930 mm
  z : real as length = 30 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_bot_beam_segment_1.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "z"
stacked.number_of_items : integer = 3
stacked.model_0 : string = "support_frame_bot_beam_segment_1_section_b.model"
stacked.label_0 : string = "t_beam_1"
stacked.model_1 : string = "support_frame_bot_beam_segment_1_section_a.model"
stacked.label_1 : string = "central_beam"
stacked.model_2 : string = "support_frame_bot_beam_segment_1_section_b.model"
stacked.label_2 : string = "t_beam_2"
    
material.ref : string = "lab_air"


#///////////////////////Support Frame Bot Iron Shield Segment 2///////////////////////

[name="support_frame_bot_beam_segment_2_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 25.5 mm 
  y : real as length = 3000 mm
  z : real as length = 540 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_bot_beam_segment_2_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 300 mm 
  y : real as length = 3000 mm
  z : real as length = 30 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_bot_beam_segment_2.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "z"
stacked.number_of_items : integer = 3
stacked.model_0 : string = "support_frame_bot_beam_segment_2_section_b.model"
stacked.label_0 : string = "t_beam_1"
stacked.model_1 : string = "support_frame_bot_beam_segment_2_section_a.model"
stacked.label_1 : string = "central_beam"
stacked.model_2 : string = "support_frame_bot_beam_segment_2_section_b.model"
stacked.label_2 : string = "t_beam_2"
    
material.ref : string = "lab_air"


#///////////////////////Support Frame Bot Iron Shield Segment in Wall///////////////////////

[name="support_frame_bot_in_iron_wall_beam_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 25.5 mm 
  y : real as length = 180 mm
  z : real as length = 540 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_bot_in_iron_wall_beam_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 300 mm 
  y : real as length = 180 mm
  z : real as length = 30 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_bot_in_iron_wall_beam.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "z"
stacked.number_of_items : integer = 3
stacked.model_0 : string = "support_frame_bot_in_iron_wall_beam_section_b.model"
stacked.label_0 : string = "t_beam_1"
stacked.model_1 : string = "support_frame_bot_in_iron_wall_beam_section_a.model"
stacked.label_1 : string = "central_beam"
stacked.model_2 : string = "support_frame_bot_in_iron_wall_beam_section_b.model"
stacked.label_2 : string = "t_beam_2"
    
material.ref : string = "lab_air"


#///////////////////////Support Frame Beam Vertical///////////////////////

[name="support_frame_beam_vertical_section_a.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 460 mm 
  y : real as length = 18 mm
  z : real as length = 6157 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_beam_vertical_section_b.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 20 mm 
  y : real as length = 185 mm
  z : real as length = 6157 mm
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


[name="support_frame_beam_vertical.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "x"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "support_frame_beam_vertical_section_b.model"
    stacked.label_0 : string = "t_beam_1"
    stacked.model_1 : string = "support_frame_beam_vertical_section_a.model"
    stacked.label_1 : string = "central_beam"
    stacked.model_2 : string = "support_frame_beam_vertical_section_b.model"
    stacked.label_2 : string = "t_beam_2"
    
  material.ref : string = "lab_air"


#///////////////////////Support Frame Beam Slanted Section///////////////////////

[name="support_frame_slanted_beam.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
  x : real as length = 250 mm 
  y : real as length = 270 mm
  z : real as length = 3700 mm  #adjusted so no overlap from 3832#
material.ref : string = "basic::iron"
visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "green"


#///////////////////////Support Frame Walls//////////////////////

   [name="support_frame_fr_it.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 500 mm
    y : real as length = 9200.0 mm
    z : real as length = 6157.0 mm
   material.ref : string = "lab_air"

        internal_item.labels : string[7] = \
        "vertical_column_1" \
        "vertical_column_2" \
        "vertical_column_3" \
        "slanted_column_1" \
        "slanted_column_2" \
        "horizontal_column_1" \
        "horizontal_column_2" 

        internal_item.model.vertical_column_1     : string = "support_frame_beam_vertical.model"
        internal_item.placement.vertical_column_1 : string = "0 -2602 0 (mm) / y 180 (degree)"
        internal_item.model.vertical_column_2     : string = "support_frame_beam_vertical.model"
        internal_item.placement.vertical_column_2 : string = "0 0 0 (mm) / y 180 (degree)"
        internal_item.model.vertical_column_3     : string = "support_frame_beam_vertical.model"
        internal_item.placement.vertical_column_3 : string = "0 2602 0 (mm) / y 180 (degree)"
        internal_item.model.slanted_column_1     : string = "support_frame_slanted_beam.model"
        internal_item.placement.slanted_column_1 : string = "0 3600 -1350 (mm) / x -155 (degree)" 
        internal_item.model.slanted_column_2     : string = "support_frame_slanted_beam.model"
        internal_item.placement.slanted_column_2 : string = "0 -3600 -1350 (mm) / x 155 (degree)"  
        internal_item.model.horizontal_column_1     : string = "support_frame_top_short_beam.model"
        internal_item.placement.horizontal_column_1 : string = "-157.5 1301 2828.5 (mm) "
        internal_item.model.horizontal_column_2     : string = "support_frame_top_short_beam.model"
        internal_item.placement.horizontal_column_2 : string = "-157.5 -1301 2828.5 (mm) "

  
[name="support_frame_it_on_iron_shield.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 930 mm
    y : real as length = 8550 mm
    z : real as length = 6138.0 mm
  material.ref : string = "lab_air"

        internal_item.labels : string[6] = \
        "support_top_beam_1" \
        "support_top_beam_2" \
        "support_top_beam_3" \
        "support_bot_beam_1" \
        "support_bot_beam_2" \
        "support_bot_beam_3" 

        internal_item.model.support_top_beam_1     : string = "support_frame_top_long_beam_segment_1.model"
        internal_item.placement.support_top_beam_1 : string = "0 -2608 2769 (mm) / z 90 (degree)"
        internal_item.model.support_top_beam_2     : string = "support_frame_top_long_beam_segment_1.model"
        internal_item.placement.support_top_beam_2 : string = "0 0 2769 (mm) / z 90 (degree)"
        internal_item.model.support_top_beam_3     : string = "support_frame_top_long_beam_segment_1.model"
        internal_item.placement.support_top_beam_3 : string = "0 2608 2769 (mm) / z 90 (degree)"
        internal_item.model.support_bot_beam_1     : string = "support_frame_bot_beam_segment_1.model"
        internal_item.placement.support_bot_beam_1 : string = "0 -2608 -2769 (mm) / z 90 (degree)"
        internal_item.model.support_bot_beam_2     : string = "support_frame_bot_beam_segment_1.model"
        internal_item.placement.support_bot_beam_2 : string = "0 0 -2769 (mm) / z 90 (degree)"
        internal_item.model.support_bot_beam_3     : string = "support_frame_bot_beam_segment_1.model"
        internal_item.placement.support_bot_beam_3 : string = "0 2608 -2769 (mm) / z 90 (degree)"


   [name="support_frame_ceiling.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 6716 mm
    y : real as length = 4800 mm
    z : real as length = 500 mm
   material.ref : string = "lab_air"

        internal_item.labels : string[3] = \
        "horizontal_column_long1" \
        "horizontal_column_long2" \
        "horizontal_column_long3" 

        internal_item.model.horizontal_column_long1     : string = "support_frame_top_long_beam.model"
        internal_item.placement.horizontal_column_long1 : string = "-2608 0 0 (mm) / y 180 (degree)"
        internal_item.model.horizontal_column_long2     : string = "support_frame_top_long_beam.model"
        internal_item.placement.horizontal_column_long2 : string = "0 0 0 (mm) / y 180 (degree)"
        internal_item.model.horizontal_column_long3     : string = "support_frame_top_long_beam.model"
        internal_item.placement.horizontal_column_long3 : string = "+2608 0 0 (mm) / y 180 (degree)"
     

   [name="support_frame_ceiling_inside_shield.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 3000 mm
    y : real as length = 6000 mm
    z : real as length = 500 mm
   material.ref : string = "lab_air"

        internal_item.labels : string[3] = \
        "horizontal_column_long1" \
        "horizontal_column_long2" \
        "horizontal_column_long3" 

        internal_item.model.horizontal_column_long1     : string = "support_frame_top_long_beam_segment_2.model"
        internal_item.placement.horizontal_column_long1 : string = "0 -2608 0 (mm) / z 90 (degree)"
        internal_item.model.horizontal_column_long2     : string = "support_frame_top_long_beam_segment_2.model"
        internal_item.placement.horizontal_column_long2 : string = "0 0 0 (mm) / z 90 (degree)"
        internal_item.model.horizontal_column_long3     : string = "support_frame_top_long_beam_segment_2.model"
        internal_item.placement.horizontal_column_long3 : string = "0 2608 0 (mm) / z 90 (degree)"


  [name="support_frame_floor.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 6716 mm
    y : real as length = 4800 mm
    z : real as length = 600 mm
   material.ref : string = "lab_air"

        internal_item.labels : string[3] = \
        "horizontal_column_long1" \
        "horizontal_column_long2" \
        "horizontal_column_long3" 

        internal_item.model.horizontal_column_long1     : string = "support_frame_bot.model"
        internal_item.placement.horizontal_column_long1 : string = "-2608 0 0 (mm) / y 180 (degree)"
        internal_item.model.horizontal_column_long2     : string = "support_frame_bot.model"
        internal_item.placement.horizontal_column_long2 : string = "0 0 0 (mm) / y 180 (degree)"
        internal_item.model.horizontal_column_long3     : string = "support_frame_bot.model"
        internal_item.placement.horizontal_column_long3 : string = "+2608 0 0 (mm) / y 180 (degree)"
     

# end
