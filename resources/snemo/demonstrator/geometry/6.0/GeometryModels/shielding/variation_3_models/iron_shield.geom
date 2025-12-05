#/////////////////////////////Modified Iron Shielding Model/////////////////////////////////

#///////////////Floor Sections////////////////

  [name="shielding_floor_iron_outer_block.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 1149 mm
    z : real as length =  180 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "black"

  [name="shielding_floor_poly_outer_block.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 1149 mm
    z : real as length =  200 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"


  [name="shielding_floor_iron_inner_block.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 2583 mm
    z : real as length =  180 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "black"


  [name="shielding_floor_poly_inner_block.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 2583 mm
    z : real as length =  200 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"


  [name="shielding_floor_iron_beam_vert.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 25.5 mm
    z : real as length = 570 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"

  [name="shielding_floor_iron_beam_horz.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 300 mm
    z : real as length = 30 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"
  
  
  [name="shielding_floor_iron.model" type="geomtools::simple_shaped_model"]
   #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
  shape_type : string = "box"
    x : real as length = 3100 mm
    y : real as length = 7554 mm
    z : real as length = 600 mm
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = false
  visibility.color            : string  = "transparent"
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false

        internal_item.labels : string[14] = \
        "pe_block0" \
        "pe_block1" \
        "pe_block2" \
        "pe_block3" \
        "iron_block0" \
        "iron_block1" \
        "iron_block2" \
        "iron_block3" \
        "vbeam01" \
        "vbeam12" \
        "vbeam23" \
        "hbeam01" \
        "hbeam12" \
        "hbeam23" 
 
        internal_item.model.pe_block3     : string = "shielding_floor_poly_outer_block.model"
        internal_item.placement.pe_block3 : string = "0 3202.5 -200 (mm) "
	mapping.daughter_id.pe_block3     : string = "[pe_shield_part:part=3]"  

        internal_item.model.pe_block2     : string = "shielding_floor_poly_inner_block.model"
        internal_item.placement.pe_block2 : string = "0 1306.5 -200 (mm) "
	mapping.daughter_id.pe_block2     : string = "[pe_shield_part:part=2]"  

        internal_item.model.pe_block1     : string = "shielding_floor_poly_inner_block.model"
        internal_item.placement.pe_block1 : string = "0 -1306.5 -200 (mm) "
	mapping.daughter_id.pe_block1     : string = "[pe_shield_part:part=1]"  

        internal_item.model.pe_block0     : string = "shielding_floor_poly_outer_block.model"
        internal_item.placement.pe_block0 : string = "0 -3202.5 -200 (mm) "
	mapping.daughter_id.pe_block0     : string = "[pe_shield_part:part=0]"  

        internal_item.model.vbeam23           : string = "shielding_floor_iron_beam_vert.model"
        internal_item.placement.vbeam23       : string = "0 2613 -15 (mm) "
        internal_item.model.vbeam12           : string = "shielding_floor_iron_beam_vert.model"
        internal_item.placement.vbeam12       : string = "0 0 -15 (mm) "
        internal_item.model.vbeam01           : string = "shielding_floor_iron_beam_vert.model"
        internal_item.placement.vbeam01       : string = "0 -2613 -15 (mm) "

        internal_item.model.hbeam23           : string = "shielding_floor_iron_beam_horz.model"
        internal_item.placement.hbeam23       : string = "0 2613 +285 (mm) "
        internal_item.model.hbeam12           : string = "shielding_floor_iron_beam_horz.model"
        internal_item.placement.hbeam12       : string = "0 0 +285 (mm) "
        internal_item.model.hbeam01           : string = "shielding_floor_iron_beam_horz.model"
        internal_item.placement.hbeam01       : string = "0 -2613 +285 (mm) "

        internal_item.model.iron_block3     : string = "shielding_floor_iron_outer_block.model"
        internal_item.placement.iron_block3 : string = "0 3202.5 -8 (mm) "
	mapping.daughter_id.iron_block3     : string = "[iron_shield_part:part=3]"  
	
        internal_item.model.iron_block2     : string = "shielding_floor_iron_inner_block.model"
        internal_item.placement.iron_block2 : string = "0 1306.5 -8 (mm) "
	mapping.daughter_id.iron_block2     : string = "[iron_shield_part:part=2]"  
	
        internal_item.model.iron_block1     : string = "shielding_floor_iron_inner_block.model"
        internal_item.placement.iron_block1 : string = "0 -1306.5 -8 (mm) "
	mapping.daughter_id.iron_block1     : string = "[iron_shield_part:part=1]"  
	
        internal_item.model.iron_block0     : string = "shielding_floor_iron_outer_block.model"
        internal_item.placement.iron_block0 : string = "0 -3202.5 -8 (mm) "
	mapping.daughter_id.iron_block0     : string = "[iron_shield_part:part=0]"  
 

#///////////////Fr - It Wall Componenets////////////////


  [name="shielding_iron_floor_gap.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm 
    # y : real as length = 940 mm
    # z : real as length =  400 mm
    y : real as length = 938 mm
    z : real as length =  398 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "black"
 
 [name="shielding_fr_it_block_a.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm
    y : real as length = 1668 mm
    #z : real as length =  6138 mm
    z : real as length =  6157 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "black"


 [name="shielding_fr_it_block_b.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm
    y : real as length = 1000 mm
    #z : real as length =  6138 mm
    z : real as length =  6157 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "black"


 [name="shielding_fr_it_block_c.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm
    y : real as length = 1197 mm
    z : real as length =  6157 mm
    #z : real as length =  6138 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "black"


  [name="jerry_can_iron_wall_fr_it_block_a.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm 
    y : real as length = 1608 mm
    z : real as length = 500 mm
  material.ref : string = "basic::water"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "yellow"


  [name="jerry_can_iron_wall_fr_it_block_c.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm 
    y : real as length = 1125 mm
    z : real as length = 500 mm
  material.ref : string = "basic::water"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "yellow"



# 
#    France and Italy Iron Sheilding Wall Basic Structure
#
#                      ^ y
#                      :
#                      :
#     |       |  |    |  |    |  |       |   
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     +-------+  +----+  +----+  +-------+
#         c   +--+ a  +--+ a  +--+   c
#               b       b       b
#
#
[name="shielding_wall_fr_it.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
  shape_type : string = "box"
    x : real as length = 360 mm
    y : real as length = 8550 mm
   # z : real as length = 6138.0 mm
    z : real as length = 6157.0 mm
  material.ref : string = "lab_air"
  visibility.color            : string  = "transparent"

        internal_item.labels : string[13] = \
        "shield_fr_block_c1" \
        "shield_fr_block_b1" \
        "shield_fr_block_a1" \
        "shield_fr_block_b2" \
        "shield_fr_block_a2" \
        "shield_fr_block_b3" \
        "shield_fr_block_c2" \
        "shield_floor_gap_1" \
        "shield_floor_gap_2" \
        "shield_floor_gap_3" \
        "support_top_beam_1" \
        "support_top_beam_2" \
        "support_top_beam_3" 
      

        internal_item.model.shield_fr_block_c1     : string = "shielding_fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c1 : string = "-90 -3676.5 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b1     : string = "shielding_fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b1 : string = "90 -2608 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_1     : string = "shielding_iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_1 : string = "-90 -2608 -2879.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a1     : string = "shielding_fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a1 : string = "-90 -1304 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b2     : string = "shielding_fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b2 : string = "90 0 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_2     : string = "shielding_iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_2 : string = "-90 0 -2879.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a2     : string = "shielding_fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a2 : string = "-90 1304 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b3     : string = "shielding_fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b3 : string = "90 2608 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_3     : string = "shielding_iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_3 : string = "-90 2608 -2879.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_c2     : string = "shielding_fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c2 : string = "-90 3676.5 0 (mm) / y 180 (degree)"

        internal_item.model.support_top_beam_1     : string = "support_frame_top_long_in_iron_wall_beam.model"
        internal_item.placement.support_top_beam_1 : string = "-90 -2608 2828.5 (mm) / z 90 (degree)"
        internal_item.model.support_top_beam_2     : string = "support_frame_top_long_in_iron_wall_beam.model"
        internal_item.placement.support_top_beam_2 : string = "-90 0 2828.5 (mm) / z 90 (degree)"
        internal_item.model.support_top_beam_3     : string = "support_frame_top_long_in_iron_wall_beam.model"
        internal_item.placement.support_top_beam_3 : string = "-90 2608 2828.5 (mm) / z 90 (degree)"


[name="shielding_wall_fr_it_with_jerry_can.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
  shape_type : string = "box"
    x : real as length = 360 mm
    y : real as length = 8550 mm
    z : real as length = 6138.0 mm
  material.ref : string = "lab_air"

        internal_item.labels : string[14] = \
        "shield_fr_block_c1" \
        "shield_fr_block_c1_jerry_can" \
        "shield_fr_block_b1" \
        "shield_fr_block_a1" \
        "shield_fr_block_a1_jerry_can" \
        "shield_fr_block_b2" \
        "shield_fr_block_a2" \
        "shield_fr_block_a2_jerry_can" \
        "shield_fr_block_b3" \
        "shield_fr_block_c2" \
        "shield_fr_block_c2_jerry_can" \
        "shield_floor_gap_1" \
        "shield_floor_gap_2" \
        "shield_floor_gap_3" 

        internal_item.model.shield_fr_block_c1     : string = "shielding_fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c1 : string = "-90 -3676.5 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_c1_jerry_can     : string = "jerry_can_iron_wall_fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c1_jerry_can : string = "90 -3676.5 -2819 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b1     : string = "shielding_fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b1 : string = "90 -2608 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a1     : string = "shielding_fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a1 : string = "-90 -1304 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a1_jerry_can     : string = "jerry_can_iron_wall_fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a1_jerry_can : string = "90 -1304 -2819 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_1     : string = "shielding_iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_1 : string = "-90 -2608 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b2     : string = "shielding_fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b2 : string = "90 0 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_2     : string = "shielding_iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_2 : string = "-90 0 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a2     : string = "shielding_fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a2 : string = "-90 1304 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a2_jerry_can     : string = "jerry_can_iron_wall_fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a2_jerry_can : string = "90 1304 -2819 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b3     : string = "shielding_fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b3 : string = "90 2608 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_3     : string = "shielding_iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_3 : string = "-90 2608 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_c2     : string = "shielding_fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c2 : string = "-90 3676.5 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_c2_jerry_can     : string = "jerry_can_iron_wall_fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c2_jerry_can : string = "90 3676.5 -2819 (mm) / y 180 (degree)"


#///////////////Tu - Mo Wall Componenets////////////////

[name="shielding_wall_tu_mo_block_main_piece.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 2800 mm 
    y : real as length = 180 mm
    z : real as length =  6138 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "black"


[name="shielding_wall_tu_mo_block_edge_piece.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 510 mm 
    y : real as length = 15 mm
    z : real as length =  6138 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "black"


[name="shielding_wall_jerry_can_iron_wall_mo_tu.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3320 mm 
    y : real as length = 500 mm
    z : real as length = 500 mm
  material.ref : string = "basic::water"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "yellow"


[name="shielding_wall_tu_mo_block.model" type="geomtools::simple_shaped_model"]
   #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
  shape_type : string = "box"
    x : real as length = 3820 mm
    y : real as length = 180 mm
    z : real as length = 6138.0 mm
  material.ref : string = "lab_air"
  # visibility.color : string  = "red"
  visibility.color : string  = "transparent"

        internal_item.labels : string[3] = "main" "side0" "side1" 
        internal_item.model.main      : string = "shielding_wall_tu_mo_block_main_piece.model"
        internal_item.placement.main  : string = "0 0 0 (mm) "
        internal_item.model.side0     : string = "shielding_wall_tu_mo_block_edge_piece.model"
        internal_item.placement.side0 : string = "1655 82.5 0 (mm) "
        internal_item.model.side1     : string = "shielding_wall_tu_mo_block_edge_piece.model"
        internal_item.placement.side1 : string = "-1655 82.5 0 (mm) "


#///////////////Ceiling////////////////

  [name="shielding_ceiling_iron.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3650 mm 
    y : real as length = 8800 mm
    z : real as length = 180 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.color            : string  = "black"

# end









