================================
SuperNEMO virtual geometry setup
================================

:Authors: François Mauger, Xavier Garrido
:Date:    2016-02-12

.. contents::
   :depth: 3
..

Presentation
============

This directory contains  the files needed to describe  the geometry of
the full SuperNEMO demonstrator module.

 * Setup label is : ``snemo::demonstrator``
 * Setup version is : ``4.0``

This version updates  the geometry to take into account  the use of 5"
PMTs for bottom/top  rows of the calorimeter main walls.   Most of the
geometry models have been defined from the original version 3.0 of the
``snemo::demonstrator`` setup..

This  setup  uses  the  new  ``basic/1.0``  materials  resource  files
published in Bayeux >= 2.1

Files:

 * ``README.rst`` : this file,
 * ``manager.conf``   :   the   main   configuration   file   of   the
   Bayeux/geomtools geometry manager object,
 * ``mapping/`` : the  directory that contains files  dedicated to the
   building  of  *geometry  models* (ala  Geant4/GDML  logical  volume
   factories)

   * ``categories.lis`` :  the description of all  geometry categories
     associated to physical volumes of interest in the geometry setup;
     this  allows to  build a  dictionnary of  geometry IDs  to enable
     automated location of volumes  and navigation within the geometry
     model

 * ``models/`` :  the directory that  contains files dedicated  to the
   building  of  *geometry  models* (ala  Geant4/GDML  logical  volume
   factories)

   * ``geom_models.lis`` : the list of geometry model definition file
   * ``calorimeter_modules/calo_pmt_5inch.geom``:
   * ``calorimeter_modules/calo_pmt_8inch.geom``:
   * ``calorimeter_modules/calorimeter_module_common.geom`` :
   * ``calorimeter_modules/calorimeter_module_5inch.geom`` :
   * ``calorimeter_modules/calorimeter_module_8inch.geom`` :
   * ``calorimeter_modules/calorimeter_submodules.geom``:
   * ``calorimeter_modules/calorimeter_walls.geom``:

 * ``plugins/``  : the  directory  that contains  files that  describe
   geometry plugins.

   * ``materials_plugin.conf`` : definitions of materials,
   * ``locators_plugin.conf`` : locators,
   * ``magnetic_field_plugin.conf`` : support for magnetic field,

     * ``magnetic_field_manager.conf`` : field manager,
     * ``magnetic_fields.def`` : definitions of fields,

   * ``mapping_plugins.conf`` : mapping.

 * ``variants/``  : the  directory  that contains  files that  describe
   geometry variants.


Check the geometry:

  The  SuperNEMO  demonstrator virtual  geometry  relies  not only  on
  generic geometry  model classes implemented in  Bayeux/geomtools but
  also on dedicated  geometry model classes from  the Falaise library.
  It  is thus  needed  to build  this  library in  order  to load  and
  visualize the SuperNEMO virtual geometry setup.

  1. First make sure the Bayeux software is installed and setup: ::

      $ which bxquery
      $ bxquery --version
      ...
      $ which bxgeomtools_inspector
      ...

  2. Build Falaise from a build directory of your choice.
  3. Then from Falaise build directory, run: ::

      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::logging "warning" \
          --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
          --load-dll Falaise \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"

     where:

       * ``--datatools::resource-path "falaise@$(pwd)/resources"``
         registers  the   Falaise  resource  base  directory   in  the
         datatools kernel for automated search for configuration file
         paths,
       * ``--manager-config
         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"``
         indicates the main configuration file of the geometry manager.
