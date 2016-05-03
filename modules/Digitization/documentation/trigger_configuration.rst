==============================================================
Trigger, CBs and FEB configuration for the Digitization module
==============================================================

:Authors: G.Oliviéro
:Contact: goliviero@lpccaen.in2p3.fr
:Revision: 0.1
:Date:   2016-05-02

.. contents::

Trigger configuration
=====================

The following information will configure the trigger. 

There are 2 types of configuration, the top level for
important and mandatory configuration and the devel level for developpers. Values for devel level will be 
fixed in the future : 


**Top level :**

 * Configuration mode : "Beta beta production", "Calibration", "Test / Customize" (run and calibration can be already configured and the user has to choose between several configuration mode or can do it's own (customize))

Trigger Board configuration
---------------------------

**Layout :**

 * Address [module][rack][crate][card_number]

Calorimeter configuration
-------------------------

**Top level :**
  
 * Calorimeter threshold to validate the L1 calo decision. Avalaible values : 0,1,2,3+ (set to 1 atm)

 * Boolean to inhibit or not both side coincidence
 * Boolean to inhibit or not single side coincidence

**Low level :**

 * Calorimeter circular buffer depth @ 25 ns. (set to 4 atm, 4 * 25 ns to wait other calorimeter event)


Tracker configuration
---------------------

**Top level :**

 * Five lookup tables (programmable memories) 

   * mem1 : A9D2 Sliding Zone Vertical memory
   * mem2 : A8D2 Sliding Zone Horizontal memory
   * mem3 : A8D2 Zone Vertical memory
   * mem4 : A6D3 Zone Horizontal memory
   * mem5 : A8D3 Zone Vertical for Horizontal memory

 * Boolean to inhibit or not both side coincidence
 * Boolean to inhibit or not single side coincidence


**Low level :**

 * Number of layers considered for the near source zone calculation (set to 4 layers atm)

Coincidence configuration
-------------------------

**Top level :**

 * Boolean to activate calorimeter / tracker coincidences
 * Boolean to activate tracker / tracker coincidences (to do)
 * Boolean to activate tracker / delayed tracker coincidences (to do)
 * Boolean to activate the DAVE coincidence algorithm (few cells close to the foil)
 

**Low level :**

 * Coincidence calorimeter gate size @ 1600 ns (set to 4 atm but it will be 6,7 or 8 * 1600 ns to wait the tracker for coincidences).
 * L2 coincidence gate size @ 1600 ns. To know how many CT 1600 the L2 decision is retained and when the Previous Event Record is built (set to 5 * 1600 ns atm)
 * Size of the vector of Previous Event Record (vector or circular buffer) during 1 ms

Control Board (CB) configuration
================================

**Layout :**

 * Address [module][rack][crate][card_number]

Tracker CB
----------

**Top level :**


**Low level :**


Calorimeter CB
--------------

**Top level :**


**Low level :**

 * Construct multiplicity and zoning from only HT or LT + HT

Front-End Board (FEB) configuration
===================================

**Layout :**

 * Address [module][rack][crate][card_number]

Tracker FEB
-----------

**Top level :**


**Low level :**

 * Activated geiger cells number : Number of clocktick 800 where a geiger cell is activated (set to 10 atm)

Calorimeter FEB
---------------

**Top level :**


**Low level :**

  * High and low thresholds
