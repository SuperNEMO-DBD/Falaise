############################################################
flsimrc-timestamper 
############################################################

Configuration
==============

The SimRC timestamper algorithm must be given two types of input
in order to generate a list of timestamps to be distributed to
simulated events :

* A list of runs
* An activity model taken from a list of models instantiated by a dedicated factory

Run list
--------

A list  of runs is  provided in a configuration  file using
the  ``datatools::multi_properties`` format.

**Example:**

.. code::
  
   #@key_label  run
   #@meta_label type

   [run="2201" type="snemo::rc::run_description"]
   #@description Run category
   category : string = "production"
   #@description Run period
   period : string = "[2022-01-01 00:00:00/2022-02-01 00:00:00)"
   #@description Total number of events collected during the run
   number_of_events : integer = 12412
   
   [run="2202" type="snemo::rc::run_description"]
   #@description Run category
   category : string = "production"
   #@description Run period
   period : string = "[2022-02-01 00:00:00/2022-03-01 00:00:00)"
   #@description Total number of events collected during the run
   number_of_events : integer = 7823

   [run="2203" type="snemo::rc::run_description"]
   #@description Run category
   category : string = "production"
   #@description Run period
   period : string = "[2022-03-01 00:00:00/2022-04-01 00:00:00)"
   #@description Total number of events collected during the run
   number_of_events : integer = 14234
   #@description Number of breaks during the run
   number_of_breaks : integer = 2
   #@description Period of the first break
   break_0.period : string = "[2022-03-05 00:00:00/2022-03-15 00:00:00)"
   #@description Period of the second break
   break_1.period : string = "[2022-03-17 12:00:00/2022-03-25 00:00:00)"

..


Activity factory models
-----------------------

A list  of activity models is  provided in a configuration  file using
the  ``datatools::multi_properties`` format.  Several activity  models
are available:

* ``snemo::physics_model::constant_activity_model``
* ``snemo::physics_model::decaying_activity_model``
* ``snemo::physics_model::scaled_activity_model``
* ``snemo::physics_model::sliced_activity_model``
* ``snemo::physics_model::per_period_activity_model``

**Example:**

.. code::
   
   #@key_label name
   #@meta_label type

   # Activity model with constant activity
   [name="constant_1Bq" type="snemo::physics_model::constant_activity_model"]
   #@description Activity of the model
   activity : real as activity = 1.0 Bq

   # Activity model with decaying activity
   [name="decaying-from-2023-01-01" type="snemo::physics_model::decaying_activity_model"]
   #@description Starting timestamp of the model
   starting_time     : string = "2023-01-01 00:00:00"
   #@description Activity at start time
   starting_activity : real as activity = 1.0 mBq
   #@description Half-life of the decaying process
   half_life         : real as time = 100.0 day

   # Scaled activity model
   [name="constant_1mBq" type="snemo::physics_model::scaled_activity_model"]
   model : string = "constant_1Bq"
   scale_factor : real = 1.e-3

   # Scaled activity model
   [name="constant_2mBq" type="snemo::physics_model::scaled_activity_model"]
   model : string = "constant_1Bq"
   scale_factor : real = 2.e-3

   # Scaled activity model
   [name="constant_10mBq" type="snemo::physics_model::scaled_activity_model"]
   model : string = "constant_1Bq"
   scale_factor : real = 1.e-2

   # Sliced activity model
   [name="constant_1Bq_march_2023" type="snemo::physics_model::sliced_activity_model"]
   model : string = "constant_1Bq"
   slice.period : string = "[2023-03-01 00:00:00 / 2023-04-01 00:00:00 )"

   # Per period activity model
   [name="per_runs" type="snemo::physics_model::per_period_activity_model"]
   number_of_slices : integer = 6
   slice_0.period : string = "[2023-01-01 00:00:00 / 2023-01-03 00:00:00 )"
   slice_0.model  : string = "constant_1mBq"
   slice_1.period : string = "[2023-01-03 00:00:00 / 2023-01-07 00:00:00 )"
   slice_1.model  : string = "constant_2mBq"
   slice_2.period : string = "[2023-01-07 00:00:00 / 2023-01-09 00:00:00 )"
   slice_2.model  : string = "constant_10mBq"
   slice_3.period : string = "[2023-01-09 00:00:00 / 2023-01-15 00:00:00 )"
   slice_3.model  : string = "constant_2mBq"
   slice_4.period : string = "[2023-01-15 00:00:00 / 2023-01-22 00:00:00 )"
   slice_4.activity : real as activity = 2.5 mBq
   slice_5.period : string = "[2023-01-22 00:00:00 / 2023-01-31 00:00:00 )"
   slice_5.activity : real as activity = 0.5 mBq
   
..
   

Principles
=================

The SimRC timestamper can use two generation modes for event timestamps:

* ``regular`` : the timestamps are sampled using a regular probability pattern
  during the sampling period,
* ``random`` : the timestamps are sampled using random pattern
  during the sampling period.

The output file use an ascii format described in the following:

* The first line is a magic line ``#!falaise::simrc::mc-event-timestamps``
* The second line is a special comment ``#@ntimestamps=N`` where ``N`` is the number of
  stored timestamps.
* Each following line describes a timestamp entry with the format:
 
  .. raw::
 
     ``EventCounter RunId Timestamp``
  ..
  
  where ``EventCounter`` is a positive integer, ``RunId`` is the ID of the run to which
  the simulated event is associated and ``Timestamp`` is the standard text representation
  of the timestamp assigned to the event.

  **Example:**
  
  .. raw::
  
     #!falaise::simrc::mc-event-timestamps
     #@ntimestamps=5
     0 0 2023-01-01 07:27:57.839004
     1 0 2023-01-01 11:49:44.151399
     2 0 2023-01-01 12:11:21.950998
     3 0 2023-01-02 17:27:04.442982
     4 0 2023-01-04 23:11:21.563365
  ..
  

Usage
=====

Print the online help:

.. code:: bash

   $ flsimrc-timestamper --help
...


**Example 1:** Regular sampling using a constant activity model selected from the
factory:

.. code:: bash

   $ flsimrc-timestamper \
     --run-list "runs.lis" \
     --activity-model-factory-config "activity_models.conf" \
     --activity-model "constant_1Bq" \
     --timestamp-generation-mode="regular" \
     --number-of-events=100000 \
     --event-timestamps-file "timestamps.data"
..

**Example 2:** Random sampling using a constant activity model selected from the
factory:

.. code:: bash

   $ flsimrc-timestamper \
     --run-list "runs.lis" \
     --activity-model-factory-config "activity_models.conf" \
     --activity-model "constant_1Bq" \
     --timestamp-generation-mode="random" \
     --timestamp-generation-random-seed=12345 \
     --number-of-events=100000 \
     --event-timestamps-file "timestamps.data"
..
 
.. end

   
