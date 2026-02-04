=====================================
Falaise Running Conditions
=====================================

This  is the  part  of  Falaise dedicated  to  the representation  and
management of the runnning condition (RC) of the SuperNEMO experiment.

* ``run_description.h``, ``run_description.cc`` : Description of a run
  with the ``run_description`` class

  - Run ID : unique identifier of the run (integer)
  - Run category (enum ``run_category``)
  - Period : the  data collection time interval the  run is associated
    with (class ``boost::date_time::time_period``)
  - Breaks  : an  array of  successive  breaks spread  during the  run
    period      (implemented     as      a     ``std::vector``      of
    ``boost::date_time::time_period``s)
  - Number of events : the total number of events in the run

* ``run_list.h``, ``run_list.cc`` : An ordered list of run descriptions

  - A dictionary of run descriptions (implemented as a map with run ID
    keys)
  - Span  :  the  spanning  period  which contains  the  full  set  of
    described runs
  - Duration : the sum of the durations of all runs
  - Number of events : the total number of events in the runs
  - Last run ID : the ID of the last run ID in the list

* ``calorimeter_om_status.h``, ``calorimeter_om_status.cc`` :

  - Class ``calorimeter_om_status_change_event`` 

  - Database table: list of calorimeter OM status change events

    ======= ===================== ============= ======================================
    OMID     TIMESTAMP             EVENT_TYPE     BIT
    int                            string         string
    ======= ===================== ============= ======================================
     1       2026-02-02 16:00:00   "set_bit"      "dead"
                                                  "off"
				                  "noisy"
				                  "no_bi_monitoring"
                                                  "no_li_monitoring"
				                  "bi_unstability_long"
				                  "bi_unstability_long"
				                  "bi_jump"
				                  "bi_screening_se_strips"
				                  "li_unstability_long"
				                  "bi_li_unconsistency_short"
				                  "bi_li_unconsistency_long"
    ======= ===================== ============= ======================================
     2       2026-02-02 16:00:00   "unset_bit"    idem   
    ======= ===================== ============= ======================================
     3       2026-02-02 16:00:00   "reset_bits"   <empty>   
    ======= ===================== ============= ======================================
     
.. end
   
