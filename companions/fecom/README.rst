===========================================================
 SuperNEMO Demonstrator Front-end Electronics Commissioning
===========================================================

Code dedicated to SuperNEMO Demonstrator Front-end Electronics Commissioning (FECOM)

Goal
====

* Read Raw Hits obtained during commissioning
* Serialize them in Binary format (data.bz2 for example)
* Do an event builder to 'pack' hits into 'Commissioning Events' and store them into a new bank 'HCRD'
* Read the 'HCRD' bank and do some analysis on hits / events
* Continue the chain from the 'HCRD' bank to the 'CD' Falaise bank


Repository organization
=======================

* documentation :

* resources : Resources useful for fecom process. Example configuration files for tracker and calo mapping between Front-End boards and Geometry. These files were produced during the commissioning of 02/2016.
  An other resource file is : Run_00.dat which is an example of what the DAQ calo and tracker gives during the commissioning. The FECOM library will parse these hits and convert them into C++ structures.

* src : location of all sources / header and testing programs of the FECOM library

* programs : Location of the main programs of the library :
  - hc_decode_sort_serialize.cxx : program which read / sort and serialize raw hits. You have to specify the input file. An example input file is provided in 'resources/data/Run_00.dat'. The standard output file produced is : "/tmp/decode_sort-output.data.bz2".
  - hc_event_builder.cxx : program which take as serialized hits. (input like "decode_sort-output.data.bz2"). It packs calorimeter and tracker raw hits into event thanks to specific L1 and L2 electronic gates. Calo and tracker mapping files are necessary because this programs do the link between electronich channels and geometric channel (Falaise style). It fills a new bank in a datatools::things call 'HCRD' standing for Half Commissioning Raw Data bank. The standard output produce is : "/tmp/hc_event_builder_serialized.brio".
  - hc_raw_data_to_calibrated_data.cxx : program to 'convert' the HCRD bank into the CD bank in order to go through the 'standard' Falaise reconstruction pipeline. Standard output is : "/tmp/output_hc2cd.brio".
  - hc_rd_bank_reader.cxx : skeleton program which can read the HCRD bank and browse Commissioning Events contained by the bank. This program does nothing in particular but can be used as a sleeve for an analysis program.
