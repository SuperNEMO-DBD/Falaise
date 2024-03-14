======================================
Falaise flpcd2cpcd Application
======================================

:Author: F.Mauger
:Date: 2024-03-14


What's Here
===========

The main application for the  conversion of UDD+pCD trigger-driven raw
data  to event-driven  data before  the recosntruction  pipeline.

This algorithm works  like an *Event Builder*.  It is  able to split a
raw data record  made of several calorimeter and  tracker hits (stored
in "UDD"  and "pCD" banks) into  several raw event records.   For this
purpose, it uses some clustering  informations generated and stored in
the        trigger-driven        "pCD"       bank        by        the
``snemo::processing::udd2pcd_module`` module (precalibration).


Usage
======

Online help is available through:

.. code:: sh

   $ flpcd2cpcd --help
..


Example
===============

Run the ``flpcd2cpcd`` event builder:

.. code:: sh

   $ flpcd2cpcd --config flpcd2cpcd.conf \
      --input-file snemo_run-724_udd+pcd.brio \
      --output-file snemo_run-724_udd+pcd-eb.brio
..

Where:

* ``snemo_run-724_udd+pcd.brio``  is a  raw  trigger-driven data  input file
  processed    through    the    ``snemo::processing::udd2pcd_module``
  precalibration module. It must contains three banks generated from
  DAQ raw file (the so-called *RED* format):

  -  "EH" : *Event Header*
  -  "UDD" : *Unified Digitized Data*
  -  "pCD" : *Precalibrated Data*

* ``snemo_run-724_udd+pcd-eb.brio``  is a event-driven data output file
  which should contain again the  following three banks:

  - "EH"   :    *Event   Header*    with   a   special    flag   named
     ``"event_builder"``                    (see                   the
     ``sndem::labels::event_builder_key()``         function        in
     ``falaise/snemo/datamodels/data_model.cc``   source  file).   The
     original run  number is preserved from  the input file but  a new
     event number is set.

     Also properties referencing the source trigger-driven data record *event number* are
     set.
     
  -  "UDD" : *Unified Digitized Data* with only hits associated to an unique cluster.
  -  "pCD" : *Precalibrated Data with* only hits associated to an unique cluster.

* ``flpcd2cpcd.conf`` :  the main configuration file could look like:

  .. code::

     #@description SuperNEMO Demonstrator pCDToCpCD processing
     #@key_label  "name"
     #@meta_label "type"

     [name="flpcd2cpcd" type="flreconstruct::section"]
   
     [name="flpcd2cpcd.variantService" type="flreconstruct::section"]
     # profile : string as path = "variant.profile"      

     [name="flpcd2cpcd.services" type="flreconstruct::section"]

     [name="flpcd2cpcd.plugins" type="flreconstruct::section"]
  ..

However, the program can be run without a specific configuration file:

  .. code:: sh
	    
     $ flpcd2cpcd \
       --input-file snemo_run-724_udd+pcd.brio \
       --output-file snemo_run-724_udd+pcd-eb.brio
  ..

Note also that in  principle, the ``flpcd2cpcd`` program should  be able to
automatically find  useful metadata  associated to  the input  file in order
to define some general processing context :

* ``experimentalSetupUrn`` (example: ``"urn:snemo:demonstrator:setup:2.0"``)
* ``reconstructionSetupUrn`` (example: ``"urn:snemo:demonstrator:reconstruction:3.0"``)
* ``configUrn`` (example: ``"urn:snemo:demonstrator:setup:variants:service:2.0"``)


.. end
   
