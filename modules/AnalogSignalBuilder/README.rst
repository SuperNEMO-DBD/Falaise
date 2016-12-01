============================================
Falaise Analog Signal Builder (ASB) plugin
============================================
:Authors: G.Oliviéro <goliviero@lpccaen.in2p3.fr>,
	  Y.Lemière <lemiere@lpccaen.in2p3.fr>,
	  F.Mauger <mauger@lpccaen.in2p3.fr>,
:Date:    29/11/2016
   ..

This Falaise plugin implements  the modelization of the analog
signals from the simulation output ("SD" bank). The produced
output is supposed to be stored in the "SSD" bank (*simulated signal data*).

UML diagram of the process :
-----------------------------------------

PRELIMINARY
::



                                        +------------------+
                                        |   Calo signal    |
                                        |   Calo signal    |
          |***********************|  /->|   Calo signal    |
       /->| SD_to_calo_signal_algo| /   |   Calo signal    |
      /   |***********************+/    |   Calo signal    |
     /                 |          /     +------------------+
    /                  v         /
 +-----+          |-------------|
 | SD  |          | Signal Data |
 +-----+          |-------------|
    \                    ^     \        +------------------+
     \                   |      \       |  Geiger signal   |
      \                  |       \      |  Geiger signal   |
       \                 |        \     |  Geiger signal   |
        \   |********************| \    |  Geiger signal   |
         \->|SD_to_GG_signal_algo|  \   |  Geiger signal   |
            |********************|   \->|  Geiger signal   |
                                        |  Geiger signal   |
                                        |  Geiger signal   |
                                        |  Geiger signal   |
                                        |  Geiger signal   |
                                        |  Geiger signal   |
                                        +------------------+
..
