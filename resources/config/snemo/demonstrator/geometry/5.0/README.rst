================================
SuperNEMO virtual geometry setup
================================

:Authors: François Mauger, Xavier Garrido, Yves Lemière, Guillaume Oliviero
:Date:    2017-03-24

.. contents::
   :depth: 3
..

Presentation
============

This directory contains  the files needed to describe  the geometry of
the full SuperNEMO demonstrator module.

 * Setup label is : ``snemo::demonstrator``
 * Setup version is : ``5.0``

This version supports variant parameters.

The geometry now takes into account  the realistic DBD sources geometry
(based on Andrea Jeremy work)



Files
========

 * ``README.rst`` : this file,
 * ``manager.conf``   :   the   main   configuration   file   of   the
   Bayeux/geomtools geometry manager object,
 

 * ``models/`` :  the directory that  contains files dedicated  to the
   building  of  *geometry  models* (ala  Geant4/GDML  logical  volume
   factories)

* ``mapping/`` : the directory contains files dedicated to the uniq object
  identification defined into ``models/`` directory 


   * ``XXX_categories.lis``   :  the   description  of   all  geometry
     categories  associated to  physical  volumes of  interest in  the
     geometry setup; this allows to build a dictionary of geometry IDs
     to enable automated location of volumes and navigation within the
     geometry model



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

   * ``variants/profiles/``  : the  directory  that contains official variant profiles.


Check the configuration
=======================

First make sure the Bayeux and Falaise software is installed and setup: ::

.. code:: sh

   $ which bxvariant_inspector
   ...
   $ which bxgeomtools_inspector
   ...
..


Browse and edit geometry variant parameters and options
=====================================================================

1. From the build directory, generate documentation about variants:

.. code:: sh

   $ bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.X.X/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/5.0/variants/repository.conf" \
	  --action doc > flgeometry.rst
   $ rst2html flgeometry.rst > flgeometry.html
   $ xdg-open flgeometry.html &
   $ rst2pdf flgeometry.rst
   $ xdg-open flgeometry.pdf &
..

   or:
.. code:: sh

   $ pandoc -w html flgeometry.rst > flgeometry.html
   $ pandoc -w latex flgeometry.rst -o flgeometry.pdf
..

2. From the build directory, browse/edit the geometry variant:

.. code:: sh

   $ bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.X.X/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/5.0/variants/repository.conf" \
          --variant-gui \
	  --variant-store "myprofile.conf"
..

   This allows you to prepare a variant profile before to *inject* it
   in some app of your choice.


Display the geometry
=======================

  The  SuperNEMO  demonstrator virtual  geometry  relies  not only  on
  generic geometry  model classes implemented in  Bayeux/geomtools but
  also on dedicated  geometry model classes from  the Falaise library.
  It  is thus  needed  to build  this  library in  order  to load  and
  visualize the SuperNEMO virtual geometry setup.

  1. First make sure the Bayeux software is installed and setup:

.. code:: sh

      $ which bxquery
      $ bxquery --version
      ...
      $ which bxgeomtools_inspector
      ...
..

  2. Build Falaise from a build directory of your choice.
  3. Then from Falaise build directory, run:

.. code:: sh

      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.X.X/resources" \
          --load-dll Falaise \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/5.0/variants/repository.conf" \
          --variant-gui \
	  --variant-store  "myprofile.conf" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/5.0/manager.conf"
..

     where:

       * ``--datatools::resource-path "falaise@$(pwd)/resources"``
         registers  the   Falaise  resource  base  directory   in  the
         datatools kernel for automated search for configuration file
         paths,
       * ``--manager-config
         "@falaise:config/snemo/demonstrator/geometry/5.0/manager.conf"``
         indicates the main configuration file of the geometry manager.
       * ``--variant-config`` loads the main configuration file for variant support.
       * ``--variant-gui`` launched the GUI at start to select/change variant
	 configuration parameters (only available if Bayeux was compiled with Qt GUI support).
       * ``--variant-store`` store the selected variant profile in a file

.. code:: sh

      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::logging "warning" \
          --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.x.x/resources" \
          --load-dll Falaise \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/5.0/variants/repository.conf" \
 	  --variant-load "myprofile.conf" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/5.0/manager.conf"
..



Betabeta source materials
============================================

There  is only  two supported  geometry source  layout: ``Basic`` and ``Realistic``.
It corresponds to a basic approximation or the realistic view of the source foil geometry,
as designed by the LAPP group (based on A. Jeremy work DocDB#4457 .

The realistic geometry model file
``@falaise:config/snemo/demonstrator/geometry/5.0/models/source_module/realistic/source_foils.geom``
describes the source foils used in the geometry.

based on the material description provided in :
``@falaise:config/snemo/demonstrator/geometry/materials/1.0/materials.def``



.. end
