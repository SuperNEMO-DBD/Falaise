[name="bipo3_column.model" type="geomtools::replicated_boxed_model"]
  #@config BiPo3 column of capsules
  replicated.axis            : string  = "x"
  replicated.number_of_items : integer = 10
  replicated.model           : string  = "top_bottom_module.model"
  replicated.label           : string  = "rows"
  material.ref               : string  = "snemo::nitrogen"
  mapping.daughter_id.rows : string  = "[tb_module.category:row+0]"


[name="bipo3_module.model" type="geomtools::replicated_boxed_model"]
  #@config BiPo3 module
  replicated.axis            : string  = "y"
  replicated.number_of_items : integer = 2
  replicated.model           : string  = "bipo3_column.model"
  replicated.label           : string  = "columns"
  material.ref               : string  = "snemo::nitrogen"
  mapping.daughter_id.columns : string  = "[bipo3_column.category:column+0]"

