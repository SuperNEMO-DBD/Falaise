============================================================================
Sample program to read and scan UDD bank from SuperNEMO raw data records
============================================================================

Description
============

The ``UddAnalysis`` module is dedicated to the processing of SuperNEMO
event records and the extraction  of *unified digitized data* from the
UDD bank. The processing of UDD data simply consists in the extraction
of various attributes found in  the *unified digitized* data structure
(see   the   ``snemo::datamodel::unified_digitized_data``   class   in
Falaise)  and  the printing  of  these  informations in  the  terminal
(optional).

Contents:

* ``UddAnalysis.hpp``   and   ``UddAnalysis.cpp``   :   source   files
  implementing the ``UddAnalysis`` module
* ``fluddscan.cxx``  :  program that  reads  an  input raw  data  file
  (typically   using   the   BRIO   storage  format)   and   apply   a
  ``UddAnalysis`` module instance on the event records


Usage
=====

Options
--------

* ``-V label`` : set verbosity to a given logging level (ex: "debug")
* ``-F integer`` : set the first event number to be processed (starts at 1 as default value)
* ``-N integer`` : set the maximum number of scanned event records (> 0)
* ``-P`` : print the UDD data extracted from the event record
* ``-i file`` : set the input raw udD data file (ex: BRIO format)  

Example
----------

.. code:: shell

   $ fluddscan   -V "debug"   -F 1  -N 10   -P   -i snemo_run-1047_udd.brio
..

TODO
============

- Add comments to better describe each attributes in the UDD format


.. end
