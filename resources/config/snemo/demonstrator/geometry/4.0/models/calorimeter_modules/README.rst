Description of the geometry of the calorimeter modules
======================================================

This version of the SuperNEMO demonstrator geometry
implements the main calorimeter walls with bottom
and top rows of optical modules with 5" PMTs (PMT6594MOD).


 * ``calorimeter_module_11x8inch_column.model`` : replicated along the Z axis::

       ^ z
       |
       |
     +-+-+
     |   |
     +---+
     |   |
     +---+
     |   |
     +---+
     |   |
     +---+
     |   |
     +---+
     |   |
     +---+
     |   |
     +---+
     |   |
     +---+
     |   |
     +---+
     |   |
     +---+      replicated x 11
     |   | <-- "calorimeter_module_8inch_rotated.model"
     +-+-+
       |

 * ``calorimeter_column.model`` : stacked along Z axis::

       ^ z
       |
       |
     +-+-+     "top_om"
  12 |   | <-- "calorimeter_module_5inch_rotated.model"
     +===+
  11 |   |  ^
     +---+  :
  10 |   |  :
     +---+  :
  9  |   |  :
     +---+  :
  8  |   |  :
     +---+  :
  7  |   |  :
     +---+  :
  6  |   |  : "calorimeter_module_11x8inch_column.model"
     +---+  : "middle_om"
  5  |   |  :
     +---+  :
  4  |   |  :
     +---+  :
  3  |   |  :
     +---+  :
  2  |   |  :
     +---+  :
  1  |   |  v
     +===+
  0  |   | <-- "calorimeter_module_5inch_rotated.model"
     +-+-+     "bottom_om"
       |
