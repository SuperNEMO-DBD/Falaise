============================
BiPo3 virtual geometry setup
============================

:Authors: François Mauger
:Date:    2013-10-23

.. contents::
   :depth: 3
..

Presentation
============

This directory contains  the files needed to describe  the geometry of
the full  BiPo3 module with  2x10 top  and bottom optical  modules and
shielding.

 * Setup label is : ``bipo3``
 * Setup version is : ``2.0``

Files:

 * ``README.rst`` : this file,
 * ``manager.conf``   :   the   main   configuration   file   of   the
   Bayeux/geomtools geometry manager object,
 * ``categories.lis``  : the  description of  all geometry  categories
   associated to physical  volumes of interest in  the geometry setup;
   this  allows to  build  a  dictionnary of  geometry  IDs to  enable
   automated location  of volumes  and navigation within  the geometry
   model,
 * ``materials.def`` :  the definition of specific  materials for this
   setup,
 * Files  dedicated   to  the  building  of   *geometry  models*  (ala
   Geant4/GDML logical volume factories):

   * ``bipo3.geom``:
   * ``calibration_source.geom``:
   * ``optical_module.geom``: use the ``snemo::geometry::calo_tapered_scin_box_model``
     geometry model class,
   * ``pmt_5inch.geom``:
   * ``shielding_5sce_3neg_2pos_side.geom``:
   * ``shielding_5sce_side.geom``:
   * ``shielding_5sce_side_top.geom``:
   * ``shielding_5sce_top.geom``:
   * ``shielding.geom`` :
   * ``source.geom`` :
   * ``tb_module.geom`` :
   * ``setup.geom`` :  geometric description of the  full module setup
     with the world top volume and an experimental area that hosts the
     detector.

Check the geometry:

  The BiPo3 virtual geometry relies not only on generic geometry model
  classes  implemented  in  Bayeux/geomtools  but  also  on  dedicated
  geometry model  classes from the Falaise_sngeometry  library.  It is
  thus needed to build this library in order to load and visualize the
  BiPo3 virtual geometry setup.

  1. First make sure the Bayeux software is installed and setup: ::

      $ which bxgeomtools_inspector
      ...

  2. Build Falaise from a build directory of your choice.
  3. Then from Falaise build directory, run: ::

      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::logging "warning" \
          --datatools::resource_path "falaise@$(pwd)/BuildProducts/share/Falaise-1.0.0/resources" \
          --load-dll Falaise \
          --manager-config "@falaise:config/bipo3/geometry/2.0/manager.conf"
      ...
      geomtools> help
      ...
      geomtools> display --no-labels --visu-view-3d --output bipo3-2.0.jpeg
      ...
      geomtools> display --no-labels --visu-view-xy --output bipo3-2.0_xy.jpeg
      ...
      geomtools> display --no-labels --visu-view-xz --output bipo3-2.0_xz.jpeg
      ...
      geomtools> quit

     where:

       * ``--datatools::resource_path "falaise@$(pwd)/resources"``
         registers  the   Falaise  resource  base  directory   in  the
         datatools' kernel for automated search for configuration file
         paths,
       * ``--manager-config
         "@falaise:config/bipo3/geometry/2.0/manager.conf"`` indicates
         the main configuration file of the geometry manager.
