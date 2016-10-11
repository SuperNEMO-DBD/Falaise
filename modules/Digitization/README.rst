======================================
Falaise Digitization plugin
======================================
:Authors: G.Oliviéro <goliviero@lpccaen.in2p3.fr>,
	  Y.Lemière <lemiere@lpccaen.in2p3.fr>,
	  F.Mauger <mauger@lpccaen.in2p3.fr>,
:Date:    05/2015


This Falaise plugin implements  the digitization algorithms applied to
the simulated data of the SuperNEMO experiment.

Goals of the plugin :
---------------------
- Modelization  with  high  fidelity  of the  electronic  response  on
  simulated   data,  including   signal  waveform   modeling,  trigger
  functionnalites, and data readout processing.
- Design  the trigger  functionalities and  strategies to  improve the
  selection of events of interest.

UML diagram of the process :
-----------------------------------------

::

                                                                                                 Calo_TP_data
                                                                                           |----------------------|                                Calo_CTW_data
                                        |------------------|                               | |------------------| |                             |----------------|
                                        |   Calo signal    |                               | |     Calo TP      | |                             | |------------| |
                                        |   Calo signal    |   |***********************|   | |     Calo TP      | |   |*********************|   | |  Calo CTW  | |   |***************************|
          |***********************|  /->|   Calo signal    |-->| signal_to_calo_TP_algo|-->| |     Calo TP      | |-->| calo_TP_to_calo_CTW |-->| |  Calo CTW  | |-->|   Calo_trigger_algorithm  |
       /->| SD_to_calo_signal_algo| /   |   Calo signal    |   |***********************|   | |     Calo TP      | |   |*********************|   | |------------| |   |***************************|
      /   |***********************|/    |   Calo signal    |                               | |     Calo TP      | |                             |----------------|              |                 \
     /                 |          /     |------------------|                               | |------------------| |                                                             |                  \
    /                  v         /                                                         |----------------------|                                     |*******************************|           \
   |-----|          |-------------|                                                                                                                     |  Coincidence_trigger_algorithm|------      \
   | SD  |          | Signal Data |                                                                                                                     |*******************************|      |  |******************|
   |-----|          |-------------|                                                        |----------------------|                                                             |               --| Trigger_decision |
    \                    ^     \        |------------------|                               | |------------------| |                                                             |                 |******************|
     \                   |      \       |  Geiger signal   |                               | |    Geiger TP     | |                                                             |                    /
      \                  |       \      |  Geiger signal   |                               | |    Geiger TP     | |                             |----------------|              |                   /
       \                 |        \     |  Geiger signal   |                               | |    Geiger TP     | |                             | |------------| |              |                  /
        \   |********************| \    |  Geiger signal   |                               | |    Geiger TP     | |                             | | Geiger CTW | |              |                 /
         \->|SD_to_GG_signal_algo|  \   |  Geiger signal   |   |***********************|   | |    Geiger TP     | |   |*********************|   | | Geiger CTW | |   |***************************|
            |********************|   \->|  Geiger signal   |-->|  signal_to_GG_TP_algo |-->| |    Geiger TP     | |-->|  GG_TP_to_calo_CTW  |-->| | Geiger CTW | |-->| Tracker_trigger_algorithm |
                                        |  Geiger signal   |   |***********************|   | |    Geiger TP     | |   |*********************|   | | Geiger CTW | |   |***************************|
                                        |  Geiger signal   |                               | |    Geiger TP     | |                             | | Geiger CTW | |
                                        |  Geiger signal   |                               | |    Geiger TP     | |                             | |------------| |
                                        |  Geiger signal   |                               | |    Geiger TP     | |                             |----------------|
                                        |  Geiger signal   |                               | |    Geiger TP     | |                               Geiger_CTW_data
                                        |------------------|                               | |------------------| |
                                                                                           |----------------------|
                                                                                                Geiger_TP_data

SD : Simulated Data
SSD : Simulated Signal Data
SDD : Simulated Digitized Data
GG : Geiger
TP : Trigger Primitive
CTW : Crate Trigger Word


To  run the  examples, you  have  to export  the environment  variable
$FALAISE_DIGITIZATION_TESTING_DIR  and give  the path  to the  testing
directory    of   the    Digitization    module.     (ex   :    export
FALAISE_DIGITIZATION_TESTING_DIR=~/data/Falaise/Falaise-trunk/modules/Digitization/testing)
