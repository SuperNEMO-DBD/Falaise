CellularAutomatonTracker README
===============================

CellularAutomatonTracker is a library dedicated to the
clustering of tracker Geiger hits in the SuperNEMO experiment. 

CellularAutomatonTracker is a component of the Channel aggregator :
  https://nemo.lpc-caen.in2p3.fr/wiki/Channel

Building/Installing CellularAutomatonTracker
--------------------------------------------
Prerequisites:
  CMake >= 2.8.x   (http://www.cmake.org)
  CLHEP >= 2.1.0.1 (http://proj-clhep.web.cern.ch/proj-clhep/)
  Boost >= 1.47 
Optional for development : 
  ROOT  >= 5.18.00 (http://root.cern.ch)

The Cadfael aggregator package can provide all these third-party dependencies:
  https://nemo.lpc-caen.in2p3.fr/wiki/Cadfael

Fast build and test :
---------------------

1 - Setup CLHEP and Boost
2 - Cd in the 'devel/LocalBuild/' directory
3 - Run :
  bash$ ./proc.sh
4 - Cd in the 'examples/' directory
5 - Setup Gnuplot
6 - Run :
  bash$ ./proc.sh

CMake installation :
--------------------

bash$ mkdir __build
bash$ cd  __build
bash$ cmake -DCMAKE_INSTALL_PREFIX:PATH=<CAT install root dir> ..
bash$ make
bash$ make install

Then check the DLLs built in '<CAT install root dir>/lib[64]' :
 - libCATAlgorithm.so
 - libmybhep.so

And also the include files :
  <CAT install root dir>/include/CellularAutomatonTracker/
  ├── CATAlgorithm
  └── mybhep

The CMake "Config" file is : 
  - <CAT install root dir>/lib[64]/cmake/CellularAutomatonTracker/CellularAutomatonTrackerConfig.cmake

Author: Federico Nova <nova@physics.utexas.edu>

--

