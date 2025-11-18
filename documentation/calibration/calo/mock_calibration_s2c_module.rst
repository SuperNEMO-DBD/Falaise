
Class ``snemo::processing::mock_calorimeter_s2c_module``:


Instance  initialized from  a ``datatools::properties``  object filled
from a parsed configuration file. Supported properties are:

.. code:: 
   
   [name="CalibrateCalorimeters" type="snemo::processing::mock_calorimeter_s2c_module"]
   #@config Parameters for the mock calorimeter calibration process

   #@description Default input bank
   SD_label  : string = "SD" 

   #@description Default output bank
   CD_label  : string = "CD"

   #@description Default geometry service
   Geo_label : string = "geometry" 

   #@description Seed for the internal PRNG to smear OM's response   
   random.seed : integer = 12345 

   #@description Trigger time window 
   cluster_time_width : real as time = 100 ns
    
   #@description Activate alpha quenching
   alpha_quenching : boolean = true
  
   #@description Store the hit OD of the original MC true hit
   store_mc_hit_id : boolean = false


   #@description List of OM types to be addressed by the calibration process
   hit_categories : string[3]  = "calo" "xcalo" "gveto"

   # Uniformity correction parameters:
   
   calorimeter_regime_database_path : string = \
     "@falaise:snemo/demonstrator/reconstruction/db/calorimeter_regime_database_v0.db"

   pol3d_parameters_mwall_8inch_path : string  = \
     "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_8inch.db"
 
   pol3d_parameters_mwall_5inch_path : string  = \
     "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_5inch.db"
  
   pol3d_parameters_xwall_path : string  = \
     "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_XW.db"
  
   pol3d_parameters_gveto_path : string  = \
     "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_GV.db"

..


Method ``process`` operates on a ``datatools::things`` object. It calls the 
``process_impl`` method which in turn calls:

 - digitizeHits(...): this method selects the list of MC calorimeter hits to be processed
   and compute the effective energy deposit seen by the OMs. It takes into account:

   - the status of the OM (real conditions), ignoring dead or off OMs
   - the energy quenching for alpha particles
   - the Birks & Cerenkov effect correction factor
   - the uniformity correction factor depending on the position of the hits in the scintillator block.

   For each hit, energy deposits including various corrections are stored as
   auxiliary properties ("edep", "edep_u", "edep_bc", "edep_bcu").

 - calibrateHits(...):  this method calibrates  calorimeter   hits  and  computes  a
   visible energy for each of them. Basic measured energy without correction is stored
   by default but additional properties are also stored:

   - "evis_u" and "sigma_evis_u" : includes uniformity correction
   - "evis_bc" and "sigma_evis_bc" : includes Birks & Cerenkov correction
   - "evis_bcu" and "sigma_evis_bcu" : includes both uniformity and Birks & Cerenkov corrections

 - triggerHits(...) :  apply some  basic trigger conditions  to select
   the final set of hits stored in the CD bank.

.. end
   
