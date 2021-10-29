===============================================
Testing solution to Falaise SNRS integration
===============================================

:author: F.Mauger
:date: 2021-10-20
       
Geometry setup 
==============

Configuration updates
---------------------

* Modifications of the variant system in :

   - ``resources/snemo/demonstrator/geometry/variants/``

* Add new geometry model in :

   - ``resources/snemo/demonstrator/geometry/GeometryModels/source_module/snrs1/``

Tests
-----
  
#. Setup Falaise
#. Cd in the Falaise source directory
#. Configure a variant simulation setup using the realistic SNRS1 model:

   .. code:: shell
	     
      $ flsimulate-configure -o utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf
      $ cat utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf
      ...
   ..
   
#. Display the geometry using the legacy realistic *flat* variant setup:

   .. code:: shell

      $ falaiseInstallDir="/opt/sw/Falaise/install/develop"
      $ falaiseVersion="4.0.3"
      $ bxgeomtools_inspector \
          --logging "debug" \
	  --datatools::resource-path "falaise@${falaiseInstallDir}/share/Falaise-${falaiseVersion}/resources" \
	  --load-dll "Falaise@${falaiseInstallDir}/lib" \
	  --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
	  --variant-load "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_a.conf" \
	  --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" 
      geomtools> display -yz source_submodule.model.log
      geomtools> G -p 1130
      geomtools> display -yz [1130:0.34.0]
      geomtools> display -yz --output source_layout_flat_yz.png source_submodule.model.log
      geomtools> quit
   ..

   .. image:: images/source_layout_flat_yz.png
      :width: 80%

   ..
	 
#. Display the geometry using the realistic *SNRS1* variant setup:
  
   .. code:: shell

      $ falaiseInstallDir="/opt/sw/Falaise/install/develop"
      $ falaiseVersion="4.0.3"
      $ snrsInstallDir="/opt/sw/SuperNEMO-DBD/SNRS/install-pro"
      $ bxgeomtools_inspector \
          --logging "debug" \
	  --datatools::resource-path "falaise@${falaiseInstallDir}/share/Falaise-${falaiseVersion}/resources" \
	  --load-dll "Falaise@${falaiseInstallDir}/lib" \
	  --load-dll "snrs@${snrsInstallDir}/lib" \
	  --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
	  --variant-load "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf" \
	  --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" 
      geomtools> display -yz source_submodule.model.log
      geomtools> G -p 1130
      geomtools> display -yz [1130:0.34.0]
      geomtools> display -yz --output source_layout_snrs1_yz.png source_submodule.model.log
      geomtools> quit
   ..

   .. image:: images/source_layout_snrs1_yz.png
      :width: 80%

   ..

   .. image:: images/source_layout_snrs1_xy-a.png
      :width: 80%
	       
   ..
    
   .. image:: images/source_layout_snrs1_xy-b.png
      :width: 80%
	       
   ..
    
   .. image:: images/source_layout_snrs1_xy-c.png
      :width: 80%
	       
   ..
    
   .. image:: images/source_layout_snrs1_xy-d.png
      :width: 80%
	       
   ..


.. raw:: pdf
   
   PageBreak
..


Vertex generator setup
======================

Configuration updates
----------------------

* Modifications of the variant system in :

  * resources/snemo/demonstrator/geometry/variants/vertex/

* Modifications of the geometry model in :

  * resources/snemo/demonstrator/geometry/VertexModels/source_realistic/

Tests
-----

#. Configure a variant simulation setup using the realistic flat model:

   .. code:: shell
	     
      $ flsimulate-configure -o utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_a.conf
      $ cat utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_a.conf
      ... 
      [registry="geometry"]
      ...
      layout/if_basic/source_layout = "RealisticFlat"
      ...
      [registry="vertexes"]
      generator = "real_flat_source_strip_3_bulk"
      ...
   ..
   
#. Generate vertexes using the legacy realistic flat source variant setup:

   .. code:: shell

      $ falaiseInstallDir="/opt/sw/Falaise/install/develop"
      $ falaiseVersion="4.0.3"
      $ bxgenvtx_production \
          --logging "debug" \
	  --datatools::resource-path "falaise@${falaiseInstallDir}/share/Falaise-${falaiseVersion}/resources" \
	  --load-dll "Falaise@${falaiseInstallDir}/lib" \
	  --variant-config "@falaise:snemo/demonstrator/geometry/VertexGeneratorVariantRepository.conf" \
	  --variant-load "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_a.conf" \
	  --geometry-manager "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" \
	  --vertex-generator-manager "@falaise:snemo/demonstrator/geometry/VertexGeneratorManager.conf" \
	  --shoot \
	  --prng-seed 314159 \
	  --number-of-vertices 1000 \
	  --output-file "vertices_realistic_flat_source.data" \
	  --visu-spot-size "2 mm" \
	  --visu-spot-color "black" \
	  --visu-max-counts 1000 \
	  --visu-output-file "vertices_realistic_flat_source-dd.data.gz"
      $ gnuplot
      gnuplot> splot 'vertices_realistic_flat_source.data'
      gnuplot> set xrange [-1000:+1000]
      gnuplot> set yrange [-2500:+2500]
      gnuplot> set zrange [-1500:+1500]
      gnuplot> splot 'vertices_realistic_flat_source.data'
      gnuplot> quit
      $ bxgeomtools_inspector \
          --logging "debug" \
	  --datatools::resource-path "falaise@${falaiseInstallDir}/share/Falaise-${falaiseVersion}/resources" \
	  --load-dll "Falaise@${falaiseInstallDir}/lib" \
	  --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
	  --variant-load "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_a.conf" \
	  --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" 
      geomtools> load_display_data "vertices" "vertices_realistic_flat_source-dd.data.gz"
      geomtools> d -yz [1100:0]
      geomtools> display -yz --output source_vertex_flat_strip3_yz.png [1100:0]
   ..

   .. image:: images/source_vertex_flat_strip3_yz.png
      :width: 80%

   ..
 
   .. image:: images/source_vertex_flat_strip3_xy.png
      :width: 80%

   ..
   
#. Configure a variant simulation setup using the realistic SNRS1 model:

   .. code:: shell
	     
      $ flsimulate-configure -o utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf
      $ cat utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf
      ... 
      [registry="geometry"]
      ...
      layout/if_basic/source_layout = "RealisticSNRS1"
      ...
      [registry="vertexes"]
      generator = "real_snrs1_source_strip_3_bulk"
      ...
   ..
   
#. Generate vertexes using the realistic SNRS1 source variant setup:

   .. code:: shell

      $ falaiseInstallDir="/opt/sw/Falaise/install/develop"
      $ falaiseVersion="4.0.3"
      $ snrsInstallDir="/opt/sw/SuperNEMO-DBD/SNRS/install-pro"
      $ bxgenvtx_production \
          --logging "debug" \
	  --datatools::resource-path "falaise@${falaiseInstallDir}/share/Falaise-${falaiseVersion}/resources" \
	  --load-dll "Falaise@${falaiseInstallDir}/lib" \
	  --load-dll "snrs@${snrsInstallDir}/lib" \
	  --variant-config "@falaise:snemo/demonstrator/geometry/VertexGeneratorVariantRepository.conf" \
	  --variant-load "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf" \
	  --geometry-manager "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" \
	  --vertex-generator-manager "@falaise:snemo/demonstrator/geometry/VertexGeneratorManager.conf" \
	  --shoot \
	  --prng-seed 314159 \
	  --number-of-vertices 5000 \
	  --output-file "vertices_realistic_snrs1_source.data" \
	  --visu-spot-size "3 mm" \
	  --visu-max-counts 5000 \
	  --visu-spot-color "black" \
	  --visu-output-file "vertices_realistic_snrs1_source-dd.data.gz"
      $ gnuplot
      gnuplot> splot 'vertices_realistic_snrs1_source.data'
      gnuplot> set xrange [-1000:+1000]
      gnuplot> set yrange [-2500:+2500]
      gnuplot> set zrange [-1500:+1500]
      gnuplot> splot 'vertices_realistic_snrs1_source.data'
      gnuplot> quit
      $ bxgeomtools_inspector \
          --logging "debug" \
	  --datatools::resource-path "falaise@${falaiseInstallDir}/share/Falaise-${falaiseVersion}/resources" \
	  --load-dll "Falaise@${falaiseInstallDir}/lib" \
	  --load-dll "snrs@${snrsInstallDir}/lib" \
	  --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
	  --variant-load "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf" \
	  --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" 
      geomtools> load_display_data "vertices" "vertices_realistic_snrs1_source-dd.data.gz"
      geomtools> d -yz [1100:0]
      geomtools> d -xy [1100:0]
   ..

   .. image:: images/source_vertex_snrs1_strip3_yz.png
      :width: 80%

   ..

   .. image:: images/source_vertex_snrs1_strip3_xy.png
      :width: 80%

   ..

 
Simulation
==========

#. Create a simulation configuration file ``simu.conf``:

.. code:: 
	  
   [name="flsimulate" type="flsimulate::section"]
   numberOfEvents : integer = 10

   [name="flsimulate.simulation" type="flsimulate::section"]
   rngEventGeneratorSeed         : integer = 314159
   rngVertexGeneratorSeed        : integer = 765432
   rngGeant4GeneratorSeed        : integer = 123456
   rngHitProcessingGeneratorSeed : integer = 987654

   [name="flsimulate.variantService" type="flsimulate::section"]
   profile : string as path = "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf"

   [name="flsimulate.plugins" type="flsimulate::section"]
   plugins : string[1] = "snrs"
   snrs.directory : string as path = "@snrsLoader.libraries:"
..

#. Run flsimulate (we need to locate the SNRS library):


.. code:: shell

   $ export snrsInstallDir="/opt/sw/SuperNEMO-DBD/SNRS/install-pro"
   $ PATH=${snrsInstallDir}/bin:${PATH}
   $ export snrsInstallLibDir="$(snrs-config --libdir)"
   $ flsimulate \
     -c utilities/geometry/test-snrs-integration1/simu.conf \
     -o run_1.xml
..

or 

.. code:: shell

   $ falaiseInstallDir="/opt/sw/Falaise/install/develop"
   $ falaiseVersion="4.0.3"
   $ snrsInstallDir="/opt/sw/SuperNEMO-DBD/SNRS/install-pro"
   $ PATH=${snrsInstallDir}/bin:${PATH}
   $ flsimulate -V "debug"  \
     --mount-directory "snrsLoader.libraries@$(snrs-config --libdir)" \
     -c utilities/geometry/test-snrs-integration1/simu.conf \
     -o run_1.xml
   $ flvisualize \
     --load-dll "snrs@${snrsInstallDir}/lib" \
     --variant-profile "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1_b.conf" \
     --input-file "run_1.xml"
..

.. raw:: pdf
   
   PageBreak
..


   flvisualize display  of two  events with 1-MeV  electrons generated
   from the deformed ITEP #3 source foil:

   .. image:: images/source_falaise_mc_snrs1_strip3_bulk.png
      :width: 80%

   ..

   .. image:: images/source_falaise_mc_snrs1_strip3_bulk-2.png
      :width: 80%

   ..


  
.. end
   
