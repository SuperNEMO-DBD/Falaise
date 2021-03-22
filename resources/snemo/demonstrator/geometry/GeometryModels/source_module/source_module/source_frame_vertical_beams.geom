#    Source frame vertical (left=y<0/right=y>0) beams   #
[name="source_vertical_beam_rod.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "cylinder"
    z : real as length = 39.0 mm
    r : real as length =  6.0 mm
  material.ref : string = "basic::copper"
  visibility.color : string = "red"


[name="source_vertical_beam_boxed_rod.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "box"
    x : real as length = 39.0 mm
    y : real as length = 20.0 mm
    z : real as length = 20.0 mm
  material.ref : string = "lab_air"
  internal_item.labels : string[1] = "rod"
    internal_item.placement.rod : string = "0 0 0 (mm) / y 90.0 (deg)"
    internal_item.model.rod     : string = "source_vertical_beam_rod.model"
  visibility.color : string = "cyan"


[name="source_vertical_beam_multirod.model" type="geomtools::replicated_model"]
  replicated.axis            : string = "z"
  replicated.number_of_items : integer = 28
  replicated.model           : string = "source_vertical_beam_boxed_rod.model"
  replicated.label           : string = "pads"
  replicated.step            : real as length = 121.78 mm
  z : real as length = 3410.0 mm
  material.ref : string = "lab_air"
  visibility.color : string = "grey"


[name="source_vertical_beam_main_air.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "box"
    x : real as length =   39.0 mm
    y : real as length =   30.5 mm
    z : real as length = 3410.0 mm
  material.ref : string = "lab_air"
  visibility.hidden : boolean = true


[name="source_vertical_beam_main_plate.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "box"
    x : real as length =   39.0 mm
    y : real as length =    9.5 mm
    z : real as length = 3410.0 mm
  material.ref : string = "basic::copper"
  visibility.color : string = "red"


[name="source_yneg_vertical_beam_main.model" type="geomtools::stacked_model"]
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "source_vertical_beam_multirod.model"
    stacked.label_0 : string = "rods"
    stacked.model_1 : string = "source_vertical_beam_main_air.model"
    stacked.label_1 : string = "air"
    stacked.model_2 : string = "source_vertical_beam_main_plate.model"
    stacked.label_2 : string = "plate"
  material.ref : string = "lab_air"
  visibility.color : string = "grey"


[name="source_ypos_vertical_beam_main.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "source_vertical_beam_main_plate.model"
    stacked.label_0 : string = "plate"
    stacked.model_1 : string = "source_vertical_beam_main_air.model"
    stacked.label_1 : string = "air"
    stacked.model_2 : string = "source_vertical_beam_multirod.model"
    stacked.label_2 : string = "rods"
  material.ref : string = "lab_air"
  visibility.color : string = "grey"


[name="source_vertical_beam_side_plate.model" type="geomtools::simple_shaped_model"]
  shape_type : string = "box"
    x : real as length =    6.5 mm
    y : real as length =   60.0 mm
    z : real as length = 3410.0 mm
  material.ref : string = "basic::copper"
  visibility.color : string = "red"


[name="source_vertical_beam_tracker_interface_a.model" type="geomtools::simple_shaped_model"]
  #@config Interface with the tracker frame
  shape_type : string = "box"
    x : real as length =    3.0 mm
    y : real as length =   30.0 mm
    z : real as length = 3410.0 mm
  material.ref : string = "basic::copper"
  visibility.color : string = "red"


[name="source_vertical_beam_tracker_interface_b.model" type="geomtools::simple_shaped_model"]
  #@config Interface with the tracker frame
  shape_type : string = "box"
    x : real as length =    3.0 mm
    y : real as length =   20.0 mm
    z : real as length = 3410.0 mm
  material.ref : string = "RTV"
  visibility.color : string = "magenta"


[name="source_vertical_beam_tracker_interface_c.model" type="geomtools::simple_shaped_model"]
  #@config Interface with the tracker frame
  shape_type : string = "box"
    x : real as length =    3.0 mm
    y : real as length =    5.0 mm
    z : real as length = 3410.0 mm
  material.ref : string = "basic::copper"
  visibility.color : string = "red"


[name="source_vertical_beam_tracker_interface_d.model" type="geomtools::simple_shaped_model"]
  #@config Interface with the tracker frame
  shape_type : string = "box"
    x : real as length =    3.0 mm
    y : real as length =    5.0 mm
    z : real as length = 3410.0 mm
  material.ref : string = "SBR"
  visibility.color : string = "orange"


[name="source_yneg_vertical_beam_tracker_interface.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 4
    stacked.model_0 : string = "source_vertical_beam_tracker_interface_d.model"
    stacked.label_0 : string = "interface_3"
    stacked.model_1 : string = "source_vertical_beam_tracker_interface_c.model"
    stacked.label_1 : string = "interface_2"
    stacked.model_2 : string = "source_vertical_beam_tracker_interface_b.model"
    stacked.label_2 : string = "interface_1"
    stacked.model_3 : string = "source_vertical_beam_tracker_interface_a.model"
    stacked.label_3 : string = "interface_0"
  material.ref : string = "lab_air"
  visibility.color : string = "grey"


[name="source_ypos_vertical_beam_tracker_interface.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 4
    stacked.model_0 : string = "source_vertical_beam_tracker_interface_a.model"
    stacked.label_0 : string = "interface_0"
    stacked.model_1 : string = "source_vertical_beam_tracker_interface_b.model"
    stacked.label_1 : string = "interface_1"
    stacked.model_2 : string = "source_vertical_beam_tracker_interface_c.model"
    stacked.label_2 : string = "interface_2"
    stacked.model_3 : string = "source_vertical_beam_tracker_interface_d.model"
    stacked.label_3 : string = "interface_3"
  material.ref : string = "lab_air"
  visibility.color : string = "grey"


[name="source_yneg_vertical_beam.model" type="geomtools::stacked_model"]
  #@config Source vertical beam (y<0)
  stacked.axis            : string = "x"
  stacked.number_of_items : integer = 5
    stacked.model_0 : string = "source_yneg_vertical_beam_tracker_interface.model"
    stacked.label_0 : string = "interface0"
    stacked.model_1 : string = "source_vertical_beam_side_plate.model"
    stacked.label_1 : string = "side0"
    stacked.model_2 : string = "source_yneg_vertical_beam_main.model"
    stacked.label_2 : string = "main"
    stacked.model_3 : string = "source_vertical_beam_side_plate.model"
    stacked.label_3 : string = "side1"
    stacked.model_4 : string = "source_yneg_vertical_beam_tracker_interface.model"
    stacked.label_4 : string = "interface1"
  material.ref : string = "lab_air"
  visibility.color : string = "grey"


[name="source_ypos_vertical_beam.model" type="geomtools::stacked_model"]
  #@config Source vertical beam (y>0)
  stacked.axis            : string = "x"
  stacked.number_of_items : integer = 5
    stacked.model_0 : string = "source_ypos_vertical_beam_tracker_interface.model"
    stacked.label_0 : string = "interface0"
    stacked.model_1 : string = "source_vertical_beam_side_plate.model"
    stacked.label_1 : string = "side0"
    stacked.model_2 : string = "source_ypos_vertical_beam_main.model"
    stacked.label_2 : string = "main"
    stacked.model_3 : string = "source_vertical_beam_side_plate.model"
    stacked.label_3 : string = "side1"
    stacked.model_4 : string = "source_ypos_vertical_beam_tracker_interface.model"
    stacked.label_4 : string = "interface1"
  material.ref : string = "lab_air"
  visibility.color : string = "grey"

