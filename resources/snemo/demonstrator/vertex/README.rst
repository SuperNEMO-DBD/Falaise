======================================
SuperNEMO Vertex configuration
======================================


Organization of the directory
================================

* ``5.0/`` : Static decay generator configuration version ``5.0``

  Contents:

  - ``VertexGeneratorManager.conf`` : Main  configuration file for the
    vertex generator manager
  - ``VertexModels/`` : For vertex generators definition files

    + ``*.conf`` : Various vertex generators definitions
    * ``sds_bi207_*.conf``  :  Specific  configurations  for  weighted
      vertex  generators for  Bi-207  calibration  sources (SDS).  See
      Miroslav  Macko's thesis  (DocDB  #4816, page  77, Figure  3.27,
      Table  3.7).  Numbering  scheme as  well as  effective activities
      have  been checked  and  fixed by  Y.Lemière  and F.Mauger  from
      Miroslav's             inputs             (see             issue
      https://github.com/SuperNEMO-DBD/Falaise/issues/237 ).
	
    + ``source_basic/`` : For specific  vertex generators definitions
      using the basic source layout
    + ``source_realistic/`` : For specific  vertex generators definitions
      using the realistic source layouts

      * ``flat/`` : For specific  vertex generators definitions
	using the realistic flat source strips
      * ``snrs1/`` : For specific  vertex generators definitions
	using the realistic bent ITEP-like source strips (use package SNRS)
    
* ``variants/`` : Variant  vertex generator configurations

  Contents:

  - ``1.0/`` : Variant  decay generator  configuration ``1.0``

    Contents:
    
    + ``VertexGeneratorVariants.conf``  :   Variant  vertex  generator
      registry
    + ``VertexGeneratorVariantRepository.conf``   :   Variant   vertex
      generator standalone repository
    + ``VertexGeneratorVariantsGDM.conf``   :    Global   dependencies
      management    configuration    for    expressing    conditionnal
      availability  of  some  vertex  generator with  regards  to  the
      geometry variant setup.
    + ``*.conf``  :  Various  definition files  for  vertex  generator
      variant models and parameters
    + ``vertexes_generators.csv``   :   List   of   available   vertex
      generators for the variant system


Testing the vertex generator
================================

The Falaise library must be built first.

.. code:: shell
   
   $ bash testing/test-5.0.bash
..
