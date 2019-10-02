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
Simply type ``flreconstruct --help``

To get the current version, type ``flreconstruct --version``

For more detail use ``-v``, e.g. ``flreconstruct --version -v``

To run, do ``flreconstruct -i input.txt -o output.txt``, where ``input.txt``
is the text file you want to process, and ``output.txt`` is the file you
want to write the processed text to.


Application Structure
=====================
At present a very dumb system is implement to illustrate several key
points:

- Where a file reader object is created/used
- Where a file writer object is created/used
- Where an event processor/pipeline/queue is created used.

The ``flreconstruct.cc`` file implements the main command line logic.
The ``main`` function simply calls the ``FLReconstructImpl`` function
(later, this can become an object if needed) with the command line
arguments. This function parses the command line, extracting the data and
handling errors (it's the later that takes the large amount of code).
Once parsing is complete and successful, it hands over the extracted
arguments to the ``FLProcessData``
function. This creates an instance of ``DumbReconstruction``, configures
it with command line parameters, and then runs it.

``DumbReconstruction`` is implemented in ``DumbReconstruction.h``. Ignore
everything apart from the ``DumbReconstruction::doReconstruction()``
method. This does the following:

- Create an reader object and event iterator

  - **This is where Ray's reader can come in**

- Create a writer object and event iterator

  - **See Yorck's proposal for reconstruction output**

- Create a functor for the pipeline (``FLPipeline``)

  - It's created via a factory function, which gives you the hook
    to enable it to be created and loaded at runtime based on
    a configuration file and loadable modules.

- Plug these together using ``std::transform`` as a controller of sorts

  - Read each Event from the Reader

  - Hand over Event to Pipeline for processing

  - Pipeline returns processed Event

  - Reader takes processed Event

``DumbReconstruction`` models events as ``char`` but you can see from the
logic, use of typedefs and the STL that any type can be accomodated.



