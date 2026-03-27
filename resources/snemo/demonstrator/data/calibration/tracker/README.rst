====================================================
Tracker cell Plasme Propagation Time table (PPT)
====================================================


Generate a mock template PPT table for a mock running period:

.. code:: shell

   $ python make_mock_cell_ppt_map.py > tracker_cell_ppt_map_mock-1.csv
..

Result in  a CSV-formatted file with  6 columns. The same  unique long
running period  is defined  for each  tracker cell.  The same  PPT and
associated error is set for all cells.

Example:

.. code:: shell

   $ less tracker_cell_ppt_map_mock1.csv
   #@title=Mock tracker cell plasma propagation time (PPT) table for test only
   #@author=F.Mauger
   #@date=2026-03-19
   #cell_num;start_time;stop_time;ppt_type;ppt[ns];ppt_error[ns]
   0;2025-04-01 00:00:00;2026-04-01 00:00:00;normal;55250.0;1250.0
   1;2025-04-01 00:00:00;2026-04-01 00:00:00;normal;55250.0;1250.0
   2;2025-04-01 00:00:00;2026-04-01 00:00:00;normal;55250.0;1250.0
   3;2025-04-01 00:00:00;2026-04-01 00:00:00;normal;55250.0;1250.0
   4;2025-04-01 00:00:00;2026-04-01 00:00:00;normal;55250.0;1250.0
   ...
   2033;2025-04-01 00:00:00;2026-04-01 00:00:00;normal;55250.0;1250.0
..

This  file can  be parsed  by the  ``snemo::tracker_cell_ppt_service``
service using the ``files`` mode at initialization.


Two mock files are provided in Falaise for testing the ``files`` mode.


