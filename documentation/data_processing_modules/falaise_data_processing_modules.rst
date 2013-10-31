Falaise data processing modules
===============================

Concept
-------

To do.



Official data processing modules in Falaise
-------------------------------------------

To do.


Creating a new data model
-------------------------

Consider  a new  ``foo_module`` class to be used in the SuperNEMO pipeline
system. In order to be
usable within the Falaise pipeline, this ``foo_module`` class must:

 * inherit the ``dpp::base_module`` mother class,
 * have public constructor and destructor,
 * provide the ``foo_module::initialize(...)`` method with special signature,
 * provide the ``foo_module::reset()`` method,
 * provide the ``foo_module::process(datatools::things &)`` method,
 * register itself in the Bayeux/dpp module registration system.
 * provide a documentation through the Bayeux/datatools OCD interface.

A     simple     example     is    provided     in     the     Falaise
``documentation/data_processing_modules/examples/ex01`` folder.
