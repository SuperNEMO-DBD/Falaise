Falaise CAT plugin
==================

This  plugin for  Falaise implement  some processing  modules for  the
SuperNEMO reconstruction pipeline.

It  uses the  CellularAutomatonTracker  (CAT) by  F.  Nova, a  library
dedicated to  the clustering of  tracker Geiger hits in  the SuperNEMO
experiment.

Two clustering algorithms are available:

 * ``CAT`` : the CellularAutomatonTracker algorithm
 * ``SULTAN`` : the Legendre Transform based algorithm
