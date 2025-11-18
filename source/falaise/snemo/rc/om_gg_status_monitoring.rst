===================================================================
SuperNEMO Geiger cells and optical modules' status modeling
===================================================================


:author: F.Mauger
:date: 2025-07-21


Generalities
============

The purpose of this document is to describe the informations needed to
be  able   to  regularly   publish  the   status  of   each  SuperNEMO
demonstrator's detection unit (tracker cells and optical modules).
       
We propose to provide list of  *records* that describe some changes in
the status  of optical modules or  geiger cells. These records  can be
stored  in files  and  then loaded  in some  dedicated  tables of  the
database to make them available within the Falaise software.


Such a record will consist in a simple data model which describes:

1. the identifier of the detection unit :

  - the cell number, a conventional  positive integer from 0 to 2033,
    corresponding   to  geometry   IDs   from  ``[1203:0.0.0.0]``   to
    ``[1203:0.1.8.112]``            (see             ID            map
    ``/pbs/home/n/nemoprod/SuperNEMO/snemo_gg_num_gid_map.csv``     at
    CCIN2P3)
  - or the OM number, a conventional  positive integer from 0 to 711,
    corresponding   to  geometry   IDs   from  ``[1301:0.0.0.0]``   to
    ``[1251:0.1.1.15]``            (see             ID            map
    ``/pbs/home/n/nemoprod/SuperNEMO/snemo_om_num_gid_map.csv``     at
    CCIN2P3)

2. the start timestamp of the period of validity using ISO 8601 format
   (example: ``2025-04-23 15:45:37``)

  *Note:* The conversion of the  number of seconds elapsed since Epoch
  into the ISO  8601 format can be done with  the following command in
  the shell

  .. code-block:: shell

     $ date --date='@1752767639' +'%Y-%m-%d %H:%M:%S'
     2025-07-17 17:53:59
  ..
  
3. the stop timestamp of the period  of validity using ISO 8601 format
   (example: ``2025-04-23 21:13:56``)
4. the status flag  in the form of a 16-bit  bitset (unsigned integer,
   example: ``0000000000110010``)

   By default: a zero bitset status  means that the unit works without
   any issue.

   
Tracker cell status
====================

Meaning of Geiger cell status bits
-----------------------------------

- bit 0 (LSB): dead cell
- bit 1 : off cell
- bit 2 : cell with missing anode signal
- bit 3 : cell with missing bottom cathode signal
- bit 4 : cell with missing top cathode signal
- bit 5 : noisy cell
- bit 6 : on trip cell
- bits 7 to 14 : *reserved for future usage*
- bit 15 (MSB): cell with other issues

Examples:

- the status of a good cell: ``0000000000000000``
- the status of a dead cell: ``0000000000000001``
- the status of a off cell:  ``0000000000000010``
- the status of a cell with no anode signal:  ``0000000000000100``
- the status of a noisy cell with no top cathode signal:  ``0000000000110000``
- the status of a cell with unknown issue(s):  ``1000000000000000``


Examples of Geiger cell status records
---------------------------------------

Using  CSV format  with  semicolon (``;``)  as  separator (leading  an
trailing spaces are not considered):

.. code-block:: csv
   
   ID  ;PeriodStart        ;PeriodStop         ;StatusBits      ;
   0   ;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000000;
   1   ;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000001;
   2   ;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000010;
   3   ;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000100;
   ...
   45  ;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000100000;
   ...
   2033;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000001000000;
..


  

Optical module status
=======================

Meaning of optical module status bits
----------------------------------------

- bit 0 (LSB): dead OM
- bit 1 : off OM
- bit 2 : noisy OM
- bit 3 : OM with no monitoring from Bi runs
- bit 4 : OM with no monitoring from LI runs
- bit 5 : OM with long term unstability after Bi runs
- bit 6 : OM with jumps after Bi runs
- bit 7 : OM with screening Se bent source strip (biased gain calibration)
- bit 8 : OM with long term unstability after LI runs
- bit 9 : OM with short term unconsistency between Bi and LI runs
- bit 10 : OM with long term unconsistency between Bi and LI runs
- bits 11 to 14 : *reserved for future usage*
- bit 15 (MSB): OM with other issues


Examples:

- the status of a good OM: ``0000000000000000``
- the status of a noisy OM with jumps : ``0000000001000100``
- the status of an OM with unexpected issues : ``1000000000000000``
  

Examples of OM status records
---------------------------------------

Using CSV format with semicolon (``;``) as separator:

.. code:: csv
   
   ID ;PeriodStart        ;PeriodStop         ;StatusBits      ;
   0  ;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000000;
   1  ;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000001;
   ...
   563;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000100;
..


Examples of OM energy calibration records
----------------------------------------------------

Basically, this  could be the  same format than above  with additional
fields to describe the result of the fit of the calibration parameters:

#. fit identifier [type: integer (>=0)]:

   - 0 : no fit
   - 1 : fit of type 1 (the one used now, likely a Chi-square based fit with 2 free parameters)
   - k : fit of type k (for future usage)
    
#. next fields (depends on the fit model).

   Proposition for fit with ID=1 as a Chi2-based fit of a linear model ``E = A * QDC + B`` :

   6 fields:

   - A [type: float] : gain
   - A_err [type: float]  : 1-sigma error on gain
   - B [type: float] : pedestal
   - B_err [type: float] : 1-sigma error on pedestal
   - Chi2  [type: float] : chi-square value
   - NDOF [type: integer (>=0)] : number of degrees of freedom 

|
|

Using CSV format with semicolon (``;``) as separator:

.. code:: csv
   
   ID ;PeriodStart        ;PeriodStop         ;StatusBits      ;FitID;A    ;A_err;B   ;B_err;Chi2   ;NDOF;
   23 ;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000000;1    ;0.176;0.008;0.21;0.45 ;24.51  ;43  ;
   45 ;2025-04-23 15:45:37;2025-04-23 21:13:56;0001100000000000;0    ;     ;     ;    ;     ;       ;    ;
   563;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000001;1    ;0.276;0.038;2.3 ;0.21 ;2345.32;41  ;
..

In the  example above, on  record at ID=45, there  is no fit  that was
performed, so the FitID=0 and  empty following fields.  For the record
at ID=563, the fit result is  provided, but the quality is low because
of a very  large Chi2 value; also the b  value is significantly larger
than 0.

This model  has the advantage that  if, in the future,  we use another
fit model for the OM calibration (call it FitDI=2 by convention), with
more fitted parameters because of the  use of a non linear formula, it
can be stored also. Example:

.. code:: csv
   
   ID ;PeriodStart        ;PeriodStop         ;StatusBits      ;FitID;PAR1 ;PAR1_err;PAR2;PAR2_err;PAR3 ;PAR3_err;P-value;
   234;2025-04-23 15:45:37;2025-04-23 21:13:56;0000000000000000;2    ;0.166;0.012   ;0.23;0.03    ;34.45;1.34    ;2.3e-1;
..

.. end

   

      
	 
