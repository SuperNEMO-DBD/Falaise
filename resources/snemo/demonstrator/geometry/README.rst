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
 
* ``variants/`` : Variant geometry configurations

  Contents:

  - ``1.0/`` : Variant geometry configuration ``1.0``

    Contents:
    
    + ``GeometryVariants.conf`` : Variant geometry registry
    + ``GeometryVariantRepository.conf`` : Variant geometry standalone repository
    + ``*.def`` : Various definition files for geometry variant models and parameters

  - ``2.0/`` : Variant geometry configuration ``2.0``

    Contents:

    + ``GeometryVariants.conf`` : Variant geometry registry
    + ``GeometryVariantRepository.conf`` : Variant geometry standalone repository
    + ``geometry.def`` : Specific definition file for the main geometry variant model


Testing the geometry
======================

The Falaise library must be built:

.. code:: shell
   
   $ bash testing/test-5.0.bash
..


.. end
   
