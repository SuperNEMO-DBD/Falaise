======================================
SuperNEMO Vertex configuration
======================================


Organization of the directory
================================

* ``6.0/`` : Static vertex generator configuration version ``6.0``

* ``5.0/`` : Static vertex generator configuration version ``5.0``

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

* ``5.1/`` : Static vertex generator configuration version ``5.1``
    
* ``variants/`` : Variant  vertex generator configurations

  Contents:

  - ``1.0/`` : Variant vertex generator  configuration ``1.0`` for static version ``5.0`` 

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

  - ``2.0/`` : Variant vertex generator  configuration ``2.0`` for static version ``5.1``

  - ``3.0/`` : Variant vertex generator  configuration ``3.0`` for static version ``6.0``


Testing the vertex generator
================================

The Falaise library must be built first.

Example1:

.. code:: shell
   
   $ bash testing/test-6.0.bash real_snrs1_source_full_foils_surface magenta vertexes-dd_foilsurf.data
   geomtools> load_display_data vertexes0 vertexes-dd_foilsurf.data 
   geomtools> display -yz [1000:0]
..

.. code:: shell
   
   $ bash testing/test-6.0.bash calo_curtain_surface_front magenta vertexes-dd_front.data
   $ bash testing/test-6.0.bash calo_curtain_surface_back blue vertexes-dd_back.data
   $ bash testing/test-6.0.bash calo_curtain_bulk red vertexes-dd_bulk.data
   geomtools> load_display_data vertexes2 vertexes-dd_back.data 
   geomtools> load_display_data vertexes1 vertexes-dd_front.data 
   geomtools> load_display_data vertexes0 vertexes-dd_bulk.data 
   geomtools> display -xy [1000:0]
..
