======================================
SuperNEMO Geometry configuration
======================================

Testing the geometry
======================

CCIN2P3
--------

The Falaise library must be built but not necessarily installed.
From a Bash shell:

.. code:: shell

   $ snswmgr_load_setup bayeux@3.5.5
   $ cd {falaise src base dir}/resources/snemo/demonstrator/geometry
   $ bash testing/test-6.0.bash
   ...
   geomtools> help
   ...
..


Organization of the directory
================================

* ``6.0/`` : Static geometry configuration version ``6.0`` (current version)
* ``5.0/`` : Static geometry configuration version ``5.0``

  Contents:

  - ``GeometryManager.conf`` : Main geometry configuration file
  - ``GeometryServices.conf`` : Standalone services configuration file with geometry only
  - ``GeometryModels/`` : Various geometry models definition files
  - ``GeometryPlugins/`` : Various geometry plugins definition files
  - ``GeomIDMaps/`` : Various geometry mapping definition files
 
* ``variants/`` : Variant geometry configurations

  Contents:

  - ``4.0/`` : Variant geometry configuration ``4.0`` (from static version ``6.0``)

  - ``3.0/`` : Variant geometry configuration ``2.0`` for geometry configuration version ``5.1`` (OBSOLETE)

    Contents:

    + ``GeometryVariants.conf`` : Variant geometry registry
    + ``GeometryVariantRepository.conf`` : Variant geometry standalone repository
    + ``geometry.def`` : Specific definition file for the main geometry variant model
  - ``2.0/`` : Variant geometry configuration ``2.0`` for geometry configuration version ``5.0``

    Contents:

    + ``GeometryVariants.conf`` : Variant geometry registry
    + ``GeometryVariantRepository.conf`` : Variant geometry standalone repository
    + ``geometry.def`` : Specific definition file for the main geometry variant model

  - ``1.0/`` : Variant geometry configuration ``1.0``

    Contents:
    
    + ``GeometryVariants.conf`` : Variant geometry registry
    + ``GeometryVariantRepository.conf`` : Variant geometry standalone repository
    + ``*.def`` : Various definition files for geometry variant models and parameters

 

Testing the geometry
======================

CCIN2P3
--------

The Falaise library must be built but not necessarily installed.
From a Bash shell:

.. code:: shell

   $ snswmgr_load_stack falaise@2025-11-19
   $ cd $BX_FALAISE_INSTALL_DIR/resources/snemo/demonstrator/geometry
   $ bash testing/test-6.0.bash
   ...
   Falaise build dir: /sps/nemo/sw/snsw/2024/var/BxWork/falaise-5.1.2/build.d/BuildProducts
   ...
   geomtools> help
   ...
..


.. end
   
