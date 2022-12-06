==================================
Falaise Reconstruction Application
==================================

What's Here
===========

The main  application for  reconstruction of  real or  simulation data
from  SuperNEMO.   It takes  input  from  the detector/simulation  and
outputs reconstructed events.  Any code specific to the reconstruction
application goes here as well.

How to Use It
=============
Simply type ``flreconstruct --help`` to get a full list of command line
options.

To get the current version, type ``flreconstruct --version``.

To run, do ``flreconstruct -i input``, where ``input``
is the simulation/detector file you want to process. By default, this
will dump information on each event to screen.

To process the events through a specific pipeline, do
``flreconstruct -i input -p script``, where ``input``
is the simulation/detector file you want to process and ``script`` is
the pipeline configuration script.

To process the events through a specific pipeline and write the
results to disk, do
``flreconstruct -i input -p script -o output``, where ``input``
is the simulation/detector file you want to process, ``script`` is
the pipeline configuration script and ``output`` is the name of
the output file to write to.

For all of the above cases, you can also pass the input file as the last
argument, e.g. ``flreconstruct -p script -o output input`.

Tasks For Alpha 2
=================
1. User should be able to get a list of the names of available pipeline
   modules (COMPLETED)
2. User should be able to get documentation on the configuration flags
   which a given module can be initialized with (COMPLETED)
3. User should be able to load and run custom modules in the pipeline
   (IN PROGRESS)
   - Requires use of ``datatools::library_loader``
   - Need to think about how user can specify libraries to use
     - We DO NOT want to use command line arguments for each library
       as used in ``dpp::dpp_driver``. It's confusing and error prone.
     - Know that dynamic loaders will use ``(DY)LD_LIBRARY_PATH`` to
       locate libraries (check implementation details, especially with
       regard to when this is used with respect to relative paths)
     - Several other places we can specify these paths, for example
       - pipeline script, if properties format allows
       - User "dot file"
       - Defaults could be in a application resource file
     - Means that a layered configuration is needed.
   - Secondary question : should the user be able to get the documentation
     on a custom module? Probably not initially, unless we allow
     specification of a command line argument pointing to the custom
     module, or via an environment variable (cf ``CMAKE_MODULE_PATH``)


