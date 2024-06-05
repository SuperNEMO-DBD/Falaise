======================================
SuperNEMO Geometry configuration
======================================


Organization of the directory
================================

* ``5.0/`` : Static geometry configuration version ``5.0``

  Contents:

  - ``GeometryManager.conf`` : Main geometry configuration file
  - ``GeometryServices.conf`` : Standalone services configuration file with geometry only
  - ``GeometryModels/`` : Various geometry models definition files
  - ``GeometryPlugins/`` : Various geometry plugins definition files
  - ``GeomIDMaps/`` : Various geometry mapping definition files

* ``5.1/`` : Static geometry configuration version ``5.1``
 
* ``variants/`` : Variant geometry configurations

  Contents:

  - ``1.0/`` : Variant geometry configuration ``1.0``

    Contents:
    
    + ``GeometryVariants.conf`` : Variant geometry registry
    + ``GeometryVariantRepository.conf`` : Variant geometry standalone repository
    + ``*.def`` : Various definition files for geometry variant models and parameters

  - ``2.0/`` : Variant geometry configuration ``2.0`` for geometry configuration version ``5.0``

    Contents:

    + ``GeometryVariants.conf`` : Variant geometry registry
    + ``GeometryVariantRepository.conf`` : Variant geometry standalone repository
    + ``geometry.def`` : Specific definition file for the main geometry variant model

  - ``3.0/`` : Variant geometry configuration ``2.0`` for geometry configuration version ``5.1``

    Contents:

    + ``GeometryVariants.conf`` : Variant geometry registry
    + ``GeometryVariantRepository.conf`` : Variant geometry standalone repository
    + ``geometry.def`` : Specific definition file for the main geometry variant model


Testing the geometry
======================

CCIN2P3
--------

The Falaise library must be built but not necessarily installed.
From a Bash shell:

.. code:: shell

   $ source /sps/nemo/sw/snswmgr/snswmgr.conf
   $ snswmgr_load_stack falaise_build@2023-07-01
   $ cd /sps/nemo/sw/snsw/2024/var/BxWork/falaise-5.1.2/Falaise-5.1.2/resources/snemo/demonstrator/geometry
   $ bash testing/test-5.0.bash
   ...
   Falaise build dir: /sps/nemo/sw/snsw/2024/var/BxWork/falaise-5.1.2/build.d/BuildProducts
   ...
   geomtools> help
   ...
..


.. end
   
