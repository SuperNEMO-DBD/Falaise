# -*- mode: conf-unix; -*-
# tracker_submodules.geom


###############################################################################
[name="tracker_front_submodule.model" type="geomtools::surrounded_boxed_model"]
#@config Front tracker submodule

material.ref            : string  = "tracking_gas"
surrounded.centered_x   : boolean = 1
surrounded.centered_y   : boolean = 1
surrounded.centered_z   : boolean = 1

surrounded.model        : string = "tracker_front_chamber.model"
surrounded.label        : string = "tracking_chamber"

surrounded.left_model   : string = "xwall_front_left.model"
surrounded.right_model  : string = "xwall_front_right.model"
surrounded.top_model    : string = "tracker_horizontal_beam_front_top.model"
surrounded.bottom_model : string = "tracker_horizontal_beam_front_bottom.model"

surrounded.left_label   : string = "frame_left"
surrounded.right_label  : string = "frame_right"
surrounded.top_label    : string = "frame_top"
surrounded.bottom_label : string = "frame_bottom"

# --> Daughters mapping informations:
mapping.daughter_id.tracking_chamber : string = "[tracker_volume]"
mapping.daughter_id.frame_left       : string = "[xcalo_wall:wall=0]"
mapping.daughter_id.frame_right      : string = "[xcalo_wall:wall=1]"
mapping.daughter_id.frame_bottom     : string = "[gveto_row:wall=0]"
mapping.daughter_id.frame_top        : string = "[gveto_row:wall=1]"

visibility.hidden           : boolean = 0
visibility.color            : string  = "grey"
visibility.hidden_envelop   : boolean = 0
visibility.daughters.hidden : boolean = 0


##############################################################################
[name="tracker_back_submodule.model" type="geomtools::surrounded_boxed_model"]
#@config Back tracker submodule

material.ref            : string  = "tracking_gas"
surrounded.centered_x   : boolean = 1
surrounded.centered_y   : boolean = 1
surrounded.centered_z   : boolean = 1

surrounded.model        : string = "tracker_back_chamber.model"
surrounded.label        : string = "tracking_chamber"

surrounded.left_model   : string = "xwall_back_left.model"
surrounded.right_model  : string = "xwall_back_right.model"
surrounded.top_model    : string = "tracker_horizontal_beam_back_top.model"
surrounded.bottom_model : string = "tracker_horizontal_beam_back_bottom.model"

surrounded.left_label   : string = "frame_left"
surrounded.right_label  : string = "frame_right"
surrounded.top_label    : string = "frame_top"
surrounded.bottom_label : string = "frame_bottom"

# --> Daughters mapping informations:
mapping.daughter_id.tracking_chamber : string = "[tracker_volume]"
mapping.daughter_id.frame_left       : string = "[xcalo_wall:wall=0]"
mapping.daughter_id.frame_right      : string = "[xcalo_wall:wall=1]"
mapping.daughter_id.frame_bottom     : string = "[gveto_row:wall=0]"
mapping.daughter_id.frame_top        : string = "[gveto_row:wall=1]"

visibility.hidden           : boolean = 0
visibility.color            : string  = "grey"
visibility.hidden_envelop   : boolean = 0
visibility.daughters.hidden : boolean = 0

# end of tracker_submodules.geom
