
Build/installation of the CellularAutomatonTracker library
in a local directory for tests :

1. Setup CLHEP, Boost, ROOT and Gnuplot. You can use Cadfael for that.

2. Run the 'proc.sh' script :

  bash$ ./proc.sh [-D] [-r]

  use option '-D' to build a development (non minimal) version
  with the embedded 'CATutils' library and associated tools

  use option '-r' for a full rebuild from scratch; this removes 
  previous build/install directories.

--



