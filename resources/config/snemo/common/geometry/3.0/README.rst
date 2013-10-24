======================================================
SuperNEMO virtual geometry setup (common resources)
======================================================

:Authors: François Mauger
:Date:    2013-10-18

.. contents::
   :depth: 3
..

Presentation
============

This  directory contains  the  files needed  to  describe some  common
elements of the SuperNEMO virtual geometry setup.

 * Setup label is : ``snemo::common``
 * Setup version is : ``3.0``

Files:

 * ``README.rst`` : This file.
 * Files  dedicated   to  the  building  of   *geometry  models*  (ala
   Geant4/GDML logical volume factories):
   * ``tracker_drift_cell_core.geom`` :  geometric description  of the
     drift cell core volume without the surrounding field wires,
   * ``tracker_drift_cell.geom`` : geometric  description of the drift
     cells equipped with surrounding field wires.

Usage:

 * List the geometry models used in the *geom* files: ::

    $ grep "^\[name=" *.geom | cut -d' ' -f1 | cut -d= -f2

 * List the materials used in the *geom* files: ::

    $ grep "^material\.ref" *.geom | cut -d= -f2 | awk '!x[$0]++'

 * Check the geometry models defined in the *geom* files: ::

    $ bxgeomtools_inspector \
     --with-visu  \
     --geometry-file tracker_drift_cell_core.geom \
     --geometry-file tracker_drift_cell.geom
    geomtools> help
    ...
    geomtools> M
    List of available geometry models :
      anode_bus_bar.model                 anode_wire.model
      cathode_bus_bar.model               drift_cell_base.model
      drift_cell_core.model               drift_cell_node_0123.model
      drift_cell_node_013.model           drift_cell_node_123.model
      drift_cell_node_13.model            drift_cell_ring.model
      field_wire.model                    x_field_wires_set.model
      y_field_wires_set.model
    geomtools> display -xy drift_cell_node_0123.model
    ...
    geomtools> quit
