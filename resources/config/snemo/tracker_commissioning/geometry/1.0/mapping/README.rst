SuperNEMO Tracker Commissioning mapping
=======================================

Introduction
------------

This is  the description of  the geometry categories used  to classify
volumes in the SuperNEMO Tracker Commissioning setup and build the
geometry identifiers associated to volumes.


Geometry categories
-------------------

 * Category for the top-level volume:

   * Category: ``world``
   * Type: ``0`` (``Bayeux/geomtools`` convention)
   * Address depth: 1

     * Depth 0 : the ``world`` number (value: 0)

   * Example of geometry identifier (GID):

     * ``[0:0]`` : the default unique *World* volume

 * Category for an experimental hall:

   * Category: ``hall``
   * Type: ``100``
   * Address depth: 1

     * Depth 0 : the ``hall`` number (value: 0)

   * Example of geometry identifier (GID):

     * ``[100:0]`` : the default unique hall where the C-section is installed

 * Category for a SuperNEMO module:

   * Category: ``module``
   * Type: ``1000``
   * Address depth: 1

     * Depth 0 : the ``module`` number (value: 0)

   * Example of geometry identifier (GID):

     * ``[1000:0]`` : the demonstrator module (here it consists of an unique C-section)

 * Category for a muon trigger plate:

   * Category: ``mu_trigger``
   * Type: ``1300``
   * Address depth: 1

     * Depth 0 : the ``position`` number (value: [0,1])

   * Examples of geometry identifier (GID):

     *  ``[1300:0]`` : the muon trigger plate on the calorimeter side
     *  ``[1300:1]`` : the muon trigger plate on the source side

 * Category for a SuperNEMO tracker submodule:

   * Category: ``tracker_submodule``
   * Type: ``1200``
   * Address depth: 2 (extends the ``module`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)

   * Example of geometry identifier (GID):

     *  ``[1200:0.0]`` : side number 0 in module number 0

 * Category for a SuperNEMO tracker volume:

   * Category: ``tracker_volume``
   * Type: ``1201``
   * Address depth: 2 (inherited from the ``tracker_submodule`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)

   * Examples of geometry identifier (GID):

     * ``[1201:0.0]`` : the tracker volume of tracker submodule side
       number 0 in module number 0

 * Category for a SuperNEMO tracker layer:

   * Category: ``tracker_layer``
   * Type: ``1202``
   * Address depth: 3 (extends the ``tracker_volume`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)
     * Depth 2 : the ``layer`` number (value: 0..8)

   * Examples of geometry identifier (GID):

     * ``[1202:0.0.0]``  : the  Geiger layer  near  the  source foil in  tracker submodule side  number 0  in module
        number 0
     * ``[1202:0.0.8]`` :  the Geiger  layer near the  main calorimeter wall in  tracker submodule side number  0 in module number 0

 * Category for a SuperNEMO drift cell:

   * Category: ``drift_cell``
   * Type: ``1203``
   * Address depth: 4  (extends the ``tracker_layer`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)
     * Depth 2 : the ``layer`` number (value: 0..8)
     * Depth 3 : the ``row`` number (value: 0..55)

   * Examples of geometry identifier (GID):

     * ``[1203:0.0.0.0]`` : the first Geiger cell in the layer near the
       source foil in tracker submodule  side number 0 in module number
       0
     * ``[1203:0.0.8.55]`` : the last Geiger cell in the layer near the
        main calorimeter  wall in  tracker submodule  side number  0 in
        module number  0 (last cell  is numbered  55 because it  is the
        C0-section)

 * Category  for a  SuperNEMO  drift cell  sensitive volume  (excluding
   surrounding field wires):

   * Category: ``drift_cell_core``
   * Type: ``1204``
   * Address depth: 4  (inherits the ``drift_cell`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)
     * Depth 2 : the ``layer`` number (value: 0..8)
     * Depth 3 : the ``row`` number (value: 0..55)

   * Example of geometry identifier (GID):

     * ``[1204:0.0.0.0]`` :  the sensitive  volume of the  first Geiger
       cell in the layer near the source foil in tracker submodule side
       number 0 in module number 0

 * Category  for a  SuperNEMO  drift cell anode wire:

   * Category: ``drift_cell_anodic_wire``
   * Type: ``1210``
   * Address depth: 4  (inherits the ``drift_cell_core`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)
     * Depth 2 : the ``layer`` number (value: 0..8)
     * Depth 3 : the ``row`` number (value: 0..55)

   * Example of geometry identifier (GID):

     * ``[1210:0.0.0.0]`` :  the anode wire of the  first Geiger
       cell in the layer near the source foil in tracker submodule side
       number 0 in module number 0

 * Category  for a  SuperNEMO  drift cell cathode ring:

   * Category: ``drift_cell_cathodic_ring``
   * Type: ``1211``
   * Address depth: 5  (extends the ``drift_cell_core`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)
     * Depth 2 : the ``layer`` number (value: 0..8)
     * Depth 3 : the ``row`` number (value: 0..55)
     * Depth 4 : the ``ring`` number (value: 0..1)

   * Example of geometry identifier (GID):

     * ``[1211:0.0.0.0.1]`` :  the top cathode ring of the  first Geiger
       cell in the layer near the source foil in tracker submodule side
       number 0 in module number 0

 * Category  for a  SuperNEMO  drift cell base:

    * Category: ``drift_cell_base``
    * Type: ``1208``
    * Address depth: 5  (extends the ``drift_cell_core`` category)

      * Depth 0 : the ``module`` number (value: 0)
      * Depth 1 : the ``side`` number (value: 0)
      * Depth 2 : the ``layer`` number (value: 0..8)
      * Depth 3 : the ``row`` number (value: 0..55)
      * Depth 4 : the ``base`` number (value: 0..1)

    * Example of geometry identifier (GID):

      * ``[1208:0.0.0.0.1]`` :  the top base of the  first Geiger
        cell in the layer near the source foil in tracker submodule side
        number 0 in module number 0

 * Category  for a  set of SuperNEMO  drift cell field wires:

  * Category: ``drift_cell_field_wires_set``
  * Type: ``1212``
  * Address depth: 5  (extends the ``drift_cell`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)
     * Depth 2 : the ``layer`` number (value: 0..8)
     * Depth 3 : the ``row`` number (value: 0..55)
     * Depth 4 : the ``set`` number (value: 0..3)

  * Example of geometry identifier (GID):

     * ``[1212:0.0.0.0.1]`` :  the right set of field wires in the  first Geiger
       cell in the layer near the source foil in tracker submodule side
       number 0 in module number 0.

 * Category  for a SuperNEMO  drift cell field wire:

   * Category: ``drift_cell_field_wire``
   * Type: ``1207``
   * Address depth: 6  (extends the ``drift_cell_field_wires_set`` category)

     * Depth 0 : the ``module`` number (value: 0)
     * Depth 1 : the ``side`` number (value: 0)
     * Depth 2 : the ``layer`` number (value: 0..8)
     * Depth 3 : the ``row`` number (value: 0..55)
     * Depth 4 : the ``set`` number (value: 0..3)
     * Depth 5 : the ``wire`` number (value: 0..2)

   * Example of geometry identifier (GID):

     * ``[1212:0.0.0.0.1.2]`` : the up wire in the right  set of field
       wires of  the first Geiger  cell in  the layer near  the source
       foil in tracker submodule side number 0 in module number 0.
