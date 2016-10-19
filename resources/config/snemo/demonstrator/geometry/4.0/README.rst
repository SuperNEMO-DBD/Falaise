================================
SuperNEMO virtual geometry setup
================================

:Authors: François Mauger, Xavier Garrido, Yves Lemière, Guillaume Oliviero
:Date:    2016-10-19

.. contents::
   :depth: 3
..

Presentation
============

This directory contains  the files needed to describe  the geometry of
the full SuperNEMO demonstrator module.

 * Setup label is : ``snemo::demonstrator``
 * Setup version is : ``4.0``

This version supports variant parameters.

The geometry now takes into account  the use of 5"
PMTs for bottom/top  rows of the calorimeter main walls.   Most of the
geometry models have been defined from the original version 3.0 of the
``snemo::demonstrator`` setup.

The beta-beta source layout has been reviewed. Source strips are now made
of pads with new source material defined. The source module frame has been
made more realistic. Calibration source (Bi207) can now be inserted
at standard position from the 6 calibration tracks.

Magnetic field can be enabled/disabled. Also when enable, we can choose
between uniform vertical B-field of mapped field (using Steve Snow's work).

This  setup  uses  the  new  ``basic/1.0``  materials  resource  files
published in Bayeux >= 2.1. The ``std::XXXX`` materials are considered
obsolete and must not be used anymore.

We also have added a simple model for the external shielding
with pure iron walls surrounding the demonstrator module.

A preliminary model of the ground (rock material is approximated with
basic concrete) has been added too.

It uses the common definition of materials 2.0 from Falaise.

Files
========

 * ``README.rst`` : this file,
 * ``manager.conf``   :   the   main   configuration   file   of   the
   Bayeux/geomtools geometry manager object,
 * ``mapping/`` : the  directory that contains files  dedicated to the
   building  of  *geometry  models* (ala  Geant4/GDML  logical  volume
   factories)

   * ``*_categories.lis`` :  the description of all  geometry categories
     associated to physical volumes of interest in the geometry setup;
     this  allows to  build a  dictionnary of  geometry IDs  to enable
     automated location of volumes  and navigation within the geometry
     model

 * ``models/`` :  the directory that  contains files dedicated  to the
   building  of  *geometry  models* (ala  Geant4/GDML  logical  volume
   factories)

 * ``plugins/``  : the  directory  that contains  files that  describe
   geometry plugins.

   * ``materials/plugin.conf`` : definitions of specific materials, aliases,
   * ``locators/plugin.conf`` : locators,
   * ``magnetic_field/`` : support for magnetic field,

     * ``plugin.conf`` : plugin configuration,
     * ``manager.conf`` : field manager,
     * ``magnetic_fields.def`` : definitions of fields,

   * ``mapping/plugins.conf`` : mapping.

 * ``variants/``  : the  directory  that contains  files that  describe
   geometry variants.


Display the geometry
=======================

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

      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --load-dll Falaise \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
          --variant-gui \
	  --variant-store  "myprofile.conf" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"

     where:

       * ``--datatools::resource-path "falaise@$(pwd)/resources"``
         registers  the   Falaise  resource  base  directory   in  the
         datatools kernel for automated search for configuration file
         paths,
       * ``--manager-config
         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"``
         indicates the main configuration file of the geometry manager.
       * ``--variant-config`` loads the main configuration file for variant support.
       * ``--variant-gui`` launched the GUI at start to select/change variant
	 configuration parameters (only available if Bayeux was compiled with Qt GUI support).
       * ``--variant-store`` store the selected variant profile in a file

     ::
      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::logging "warning" \
          --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --load-dll Falaise \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
 	  --variant-load "myprofile.conf" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"

     ::
      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::logging "warning" \
          --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --load-dll Falaise \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
          --variant-set "geometry:layout=HalfCommissioning" \
          --variant-set "magnetic_field:active=0" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"


Materials used in the geometry
============================================

a. Print the list of materials:

   From Falaise ``resources`` directory (source or installation):

.. raw:: sh

   $ find ./config/snemo/demonstrator/geometry/4.0/models/ -name "*.geom" -exec grep "material.ref" \{\} \; | cut -d= -f2 | tr -d " \t\"" | sort | uniq
   ...

b. Search the file where a given material is used:

   From Falaise ``resources`` directory (source or installation):

.. raw:: sh


   $ find ./config/snemo/demonstrator/geometry/4.0/models/ -name "*.geom" -exec grep -l "basic::PTFE" \{\} \;
   ...
