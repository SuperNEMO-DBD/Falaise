======================================
Falaise Digitization plugin
======================================
:Authors: F.mauger   <mauger@lpccaen.in2p3.fr>, G.Oliviéro <goliviero@lpccaen.in2p3.fr>, Y.Lemière  <lemiere@lpccaen.in2p3.fr>
:Date:    05/2015


This plugin for Falaise implements the trigger digitization for the SuperNEMO experiment.

Goals of the plugin :
---------------------
- Modelized with an high fidelity the electronic response and all the process from Simulated Data to the trigger board.
- Design the trigger cleverness to make a decision if there are some interesting events or nothing.
- Design the strategy for data readout. (not implemented yet)

UML diagram of the process : (can change)
-----------------------------------------

::
                    
                                                                                                Calo_TP_data
                                                                                          |----------------------|                               Calo_CTW_data
                                        |------------------|                              | |------------------| |                            |----------------|
                                        |   Calo signal    |                              | |     Calo TP      | |                            | |------------| |      (not implemented yet)
                                        |   Calo signal    |   |**********************|   | |     Calo TP      | |   |********************|   | |  Calo CTW  | |   |***************************|
          |**********************|   /->|   Calo signal    |-->| signal_2_calo_TP_algo|-->| |     Calo TP      | |-->| calo_TP_2_calo_CTW |-->| |  Calo CTW  | |-->|   Calo_trigger_algorithm  |
       /->| SD_2_calo_signal_algo|  /   |   Calo signal    |   |**********************|   | |     Calo TP      | |   |********************|   | |------------| |   |***************************|
      /   |**********************| /    |   Calo signal    |                              | |     Calo TP      | |                            |----------------|                                \ 
     /                 |          /     |------------------|                              | |------------------| |                                                                               \
    /                  v         /                                                        |----------------------|                                                                                \         
   |-----|          |-------------|                                                                                                                                                                \    (not implemented yet)
   | SD  |          | Signal Data |                                                                                                                                                              |******************|
   |-----|          |-------------|                                                       |----------------------|                                                                               | Trigger_decision |
    \                    ^     \        |------------------|                              | |------------------| |                                                                               |******************|
     \                   |      \       |  Geiger signal   |                              | |    Geiger TP     | |                                                                                 /
      \                  |       \      |  Geiger signal   |                              | |    Geiger TP     | |                            |----------------|                                  /
       \                 |        \     |  Geiger signal   |                              | |    Geiger TP     | |                            | |------------| |                                 /
        \   |********************| \    |  Geiger signal   |                              | |    Geiger TP     | |                            | | Geiger CTW | |                                /
         \->| SD_2_GG_sinal_algo |  \   |  Geiger signal   |   |**********************|   | |    Geiger TP     | |   |********************|   | | Geiger CTW | |   |***************************|
            |********************|   \->|  Geiger signal   |-->|  signal_2_GG_TP_algo |-->| |    Geiger TP     | |-->|  GG_TP_2_calo_CTW  |-->| | Geiger CTW | |-->| Tracker_trigger_algorithm |
                                        |  Geiger signal   |   |**********************|   | |    Geiger TP     | |   |********************|   | | Geiger CTW | |   |***************************|
                                        |  Geiger signal   |                              | |    Geiger TP     | |                            | | Geiger CTW | |
                                        |  Geiger signal   |                              | |    Geiger TP     | |                            | |------------| |
                                        |  Geiger signal   |                              | |    Geiger TP     | |                            |----------------|
                                        |  Geiger signal   |                              | |    Geiger TP     | |                              Geiger_CTW_data
                                        |------------------|                              | |------------------| |
                                                                                          |----------------------|
                                                                                               Geiger_TP_data
SD : Simulated Data
GG : Geiger		         
TP : Trigger Primitive
CTW : Crate Trigger Word


To run the examples, you have to export the environment variable $FALAISE_DIGITIZATION_TESTING_DIR and give the path to the testing directory of the Digitization module. 
(ex : export FALAISE_DIGITIZATION_TESTING_DIR=~/data/Falaise/Falaise-trunk/modules/Digitization/testing)


