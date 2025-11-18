===========================================
SuperNEMO Run description
===========================================

:author: F.Mauger (mauger@lpccaen.in2p3.fr)
:date: 2025-05-06
:version: 1.0

   
Introduction
==============

The  basic informations  used to  describe a  given SuperNEMO  run are
explained in  this document.  These  informations can be  fetched from
the  SuperNEMO ``nemo_rundb``  database from  the SuperNEMO  DB server
hosted ac CCIN2P3.   The Falaise library provides  a general service
(class ``snemo::db_service``)  to access  this server and  a dedicated
service (class ``snemo::run_info_service``)  to access the descriptions
of any SuperNEMO runs.

Additional informations about  the software interface can  be found in
the Falaise library:

- ``snemo/rc/run_status.h`` header file.
- ``snemo/rc/run_description.h`` header file.
- ``snemo/services/db_service.h`` header file.
- ``snemo/services/run_info_service.h`` header file.
  
Description of a raw run
===========================

  
A SuperNEMO raw run is described through a set of properties:

* Run number (or run ID) : a 32-bit signed integer

  - Normal values are positive: from ``0`` to ``0x7FFFFFFF`` (``2147483647``)
  - Special value ``-1`` means : *invalid run number*
  - Special value ``-2`` means : *all runs*
  - Other values are reserved for future usage

* Run category: an unsigned integer enumeration from 0 to 7

   - 0 (INDETERMINATE) = *invalid category*
   - 1 (TEST) = *test run*
   - 2 (COMMISSIONING) = *commissioning run*
   - 3 (PRODUCTION) = *production run* (for betabeta or background data collection)
   - 4 (CALIBRATION_1) = *calibration run* type 1 : Bi-207 calibration with SDS (Source Deployment System)
   - 5 (CALIBRATION_2) = *calibration run* type 2 : LI calibration (Light Injection System)
   - 6 (CALIBRATION_3) = *calibration run* type 3 : reference OMs calibration
   - 7 (CALIBRATION_4) = *calibration run* type 4 : OM pedestal calibration

* DAQ configuration: 
     
* Run period:

  The  period of  data collection  of the  run is  described with  the
  ``snemo::time::time_period``    type     (an    alias     for    the
  ``boost::posix_time::time_period`` class).  It  is defined the start
  and stop  times of  the run (``snemo::time::time_point``  type), its
  duration (``snemo::time::time_duration`` type).

  Time  classes   can  be  manipulated  through   dedicated  functions
  available from the ``snemo::time::time_utils.h`` header file and the
  ``boost::posix_time``                  headers                  (see
  ``https://www.boost.org/doc/libs/1_88_0/doc/html/date_time.html``).

* Run  status :  This is  a single  bitset implemented  from a  32-bit
  unsigned integer  (default to  0). Each bit  position has  a special
  meaning. Only the first 7 bits have been assigned a special
  meaning:

  - bit 0 : the run has calorimeter issues
  - bit 1 : the run has tracker issues
  - bit 2 : the run has gas issues
  - bit 3 : the run has anti radon factory issues
  - bit 4 : the run has SDS issues (Source Deployment System)
  - bit 5 : the run has been truncated (abnormal termination : aborted, crash...)
  - bit 6 : the run has other issues (other type of problem)
 
  A null/zero status means no issues for the run: *good* run.

  Additional bits could be assigned special meaning in the future.
  
* Number of  triggers: the total  number of triggers collected  during the
  run is implemented through an unsigned 32-bit integer (default to 0).

* Deadtime: the total dead time ot the run is  implemented through
  a ``snemo::time::time_duration`` instance (default to 00:00:00.000000).

 
Description of a UDD dataset
=============================

The UDD data model is used to describe the first step of reconstructed
events  from  raw  data  triggers.   Thanks  to  the  low  level  data
processing  programs in  the  SNFEE  package, the  raw  data model  is
processed using a multistep pipeline  ending with the so-called *Event
Builder* (RTD2RED) which produces *Raw Event Data* (RED):

.. code::
   
   [DAQ]-> (CBD) -[CBD2RHD]-> (RHD) -[RHDSort]-> (sRHD) -[RHD2RTD]-> (RTD)
..


.. code::

   (RTD) -[RTD2RED]-> (RED)
..


The SNREDBridge package (https://github.com/SuperNEMO-DBD/SNREDBridge) is used to convert
*Raw Event Data* (RED) from the SNFEE package to the UDD data model from Falaise:

.. code::

   (RED) -[red_bridge]-> (UDD)
..






  
The run info service
======================

The Falaise's *run  info* service gives access to a  map containing the
descriptions of  runs.  At  initialization, it connects  the SuperNEMO
database through a  Falaise's DB service and loads  specific tables to
fill a local  map in memory.  The  run info map is  then available for
data reconstruction or data analysis tasks.

This   service   has   a   default   ``"runInfo"``   name   (see   the
``snemo::service_info::runInfoServiceName()``   function)   and   type
``snemo::run_info_service``.   It  depends   on  a   database  service
(``snemo::db_service`` class).



.. end
   


   
