===============================================
Testing solution to Falaise SSNRS integration
===============================================

:author: F.Mauger
:date: 2021-09-21
       
Geometry setup
==============

Configuration updates
---------------------

* Modifications of the variant system in :

   - ``resources/snemo/demonstrator/geometry/variants/``

* Add new geometry model in :

   - ``resources/snemo/demonstrator/geometry/GeometryModels/source_module/snrs1//``

Tests
-----
  
#. Setup Falaise
#. Cd in the Falaise source directory
#. Configure a variant simulation setup using the SNRS model:

   .. code:: shell
	     
      $ flsimulate-configure -o utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1.conf
      $ cat utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1.conf
      ...
   ..
   
  
#. Display the geometry using the legacy Bi207 variant setup:

   .. code:: shell

      $ falaiseInstallDir="/opt/sw/Falaise/install/develop"
      $ falaiseVersion="4.0.3"
      $ snrsInstallDir="/opt/sw/SuperNEMO-DBD/SNRS/install-1.0.0"
      $ bxgeomtools_inspector \
          --logging "debug" \
	  --datatools::resource-path "falaise@${falaiseInstallDir}/share/Falaise-${falaiseVersion}/resources" \
	  --load-dll "Falaise@${falaiseInstallDir}/lib" \
	  --load-dll "snrs@${snrsInstallDir}/lib" \
	  --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
	  --variant-load "utilities/geometry/test-snrs-integration1/snemo_geom_test_snrs1.conf" \
	  --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" 
      geomtools> display source_submodule.model.log
      geomtools> quit
   ..

   // --datatools::resource-path "snrs@${snrsInstallDir}/share/SNRS/resources" \

.. raw:: pdf
   
   PageBreak
..
