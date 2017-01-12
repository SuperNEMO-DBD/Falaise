================================
SuperNEMO virtual geometry setup
================================

:Authors: François Mauger, Xavier Garrido, Yves Lemière, Guillaume Oliviero
:Date:    2016-11-14

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

The geometry now takes into account  the use of 5" PMTs for bottom/top
rows of the calorimeter main walls.   Most of the geometry models have
been    defined   from    the    original   version    3.0   of    the
``snemo::demonstrator`` setup.

The beta-beta source  layout has been reviewed. Source  strips are now
made of pads with new source material defined. The source module frame
has been  made more realistic.  Calibration source (Bi207) can  now be
inserted at standard position from the 6 calibration tracks.

Magnetic  field can  be  enabled/disabled. Also  when  enable, we  can
choose between uniform  vertical B-field of mapped  field (using Steve
Snow's work).

This  setup  uses  the  new  ``basic/1.0``  materials  resource  files
published  in Bayeux  >= 2.1.  The materials  named ``std::XXXX``  are
considered obsolete and must not be used anymore.

We also have added a simple model for the external shielding with pure
iron walls surrounding the demonstrator module.

A preliminary model of the  ground (rock material is approximated with
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

   * ``XXX_categories.lis``   :  the   description  of   all  geometry
     categories  associated to  physical  volumes of  interest in  the
     geometry setup; this allows to build a dictionary of geometry IDs
     to enable automated location of volumes and navigation within the
     geometry model

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
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
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
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
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
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --load-dll Falaise \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
          --variant-gui \
	  --variant-store  "myprofile.conf" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"
..

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

.. code:: sh

      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::logging "warning" \
          --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --load-dll Falaise \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
 	  --variant-load "myprofile.conf" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"
..

.. code:: sh

      $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
        bxgeomtools_inspector \
          --datatools::logging "warning" \
          --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --load-dll Falaise \
          --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
          --variant-set "geometry:layout=HalfCommissioning" \
          --variant-set "magnetic_field:active=0" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"
..


Materials used in the geometry
============================================

a. Print the list of materials:

   From Falaise ``resources`` directory (source or installation):

.. code:: sh

   $ find ./config/snemo/demonstrator/geometry/4.0/models/ -name "*.geom" -exec grep "material.ref" \{\} \; | cut -d= -f2 | tr -d " \t\"" | sort | uniq
   ...

b. Search the file where a given material is used:

   From Falaise ``resources`` directory (source or installation):

.. code:: sh

   $ find ./config/snemo/demonstrator/geometry/4.0/models/ -name "*.geom" -exec grep -l "basic::PTFE" \{\} \;
   ...
..


Betabeta source materials
============================================

There  is only  one supported  geometry source  layout: ``Basic``.  It
corresponds to a realistic approximation  of the source foil geometry,
as designed by the LAPP group.   With this layout, you can change both
the thickness  of the source  pads and the associated  material. These
variant are defined in:

``@falaise:config/snemo/demonstrator/geometry/4.0/variants/models/source_betabeta.def``

- ``thickness`` ranges from 5 to 500 um (default is ``250 um``).
- ``material`` is chosen from the following set:

  - ``Ca48``
  - ``Se82`` (default value)
  - ``Nd150``

The                 geometry                model                 file
``@falaise:config/snemo/demonstrator/geometry/4.0/models/source_module/basic/source_pads.geom``
describes the source pads used in the geometry.

The               ``source_external_pad.basic.model``              and
``source_internal_pad.basic.model`` geometry  models use  the material
alias named ``bb_source_material.basic``.

The   ``bb_source_material.basic``  material   alias  is   defined  in
``@falaise:config/snemo/demonstrator/geometry/4.0/plugins/materials/material_aliases.def``. Depending
of  the  ``material``  variant  parameter selected  by  the  user,  it
corresponds to one of the following value:

- ``snemo::ca48::basic``: alias for the ``snemo::ca48::nemo3`` material
- ``snemo::se82::basic``: alias for the ``snemo::se82_enriched100_pva`` material
- ``snemo::nd150::basic``: alias for the ``snemo::snemo::nd150::nemo3`` material

These           materials           are           defined           in
``@falaise:config/common/geometry/materials/2.0/materials.def``    and
some        related        elements       are        defined        in
``@falaise:config/common/geometry/materials/2.0/elements.def``.



.. end
