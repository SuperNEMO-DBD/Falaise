==================================
Falaise Reconstruction Application
==================================

What's Here
===========

The main application for reconstruction of raw data from SuperNEMO.
It takes input from the detector/simulation and outputs reconstructed
events.
Any code specific to the reconstruction application goes here as well.

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

