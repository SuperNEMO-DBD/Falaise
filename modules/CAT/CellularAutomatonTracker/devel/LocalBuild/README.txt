
Build/installation of the CellularAutomatonTracker 
library in a local directory for tests :

1. Setup CLHEP, Boost, ROOT and Gnuplot. You can use Cadfael for that.
   cd in the '<CAT source_dir>/devel/LocalBuild' directory :
   
  bash$ cd <CAT source_dir>/devel/LocalBuild

2. Run the 'proc.bash' script :

  bash$ ./proc.bash' [-D] [-r]

  use option '-D' to build a development (non minimal) version
  with the embedded 'CATutils' library and associated tools

  use option '-r' for a full rebuild from scratch; this removes 
  previous build/install directories.

3. The library is thus installed in the '<CAT source_dir>/devel/LocalBuild/__install'
   directory :

   bash$ tree <CAT source_dir>/devel/LocalBuild/__install

4. Clean all :

   bash$ ./clean.sh

--
F.Mauger <mauger@lpccaen.in2p3.fr>



