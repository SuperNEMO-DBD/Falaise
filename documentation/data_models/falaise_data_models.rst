Falaise data models
===================


Official data models in Falaise
-------------------------------

 * Event record container: ``datatools::things`` class from the Bayeux/datatools library.
 * Bank data:

   * Event header data:

     * Bank key: ``EH``
     * Class: ``snemo::models::event_header`` (from the Falaise library/sncore)

   * Simulated data:

     * Bank key: ``SD``
     * Class: ``mctools::simulated_data`` (from the Bayeux/mctools library)

   * list to be completed...

Creating a new data model
-------------------------

The SuperNEMO pipeline making use of the generic ``datatools::things``
container class  to represent  an *event record*,  one must  respect a
mandatory interface to be able to store  a given data model in a given
bank of the event record.

Consider  a new  ``foo`` class  being a  data model  of the  SuperNEMO
experiment to be stored in a bank  of the event record. In order to be
usable within the Falaise pipeline, this ``foo`` class must:

 * inherit the ``datatools::i_serializable`` interface,
 * have public default constructor and destructor,
 * provide a template ``foo::serialize`` method,
 * register itself in the Boost/Serialization system.

Additionally,   we   must   properly  instantiate   some   templatized
serialization  code  for the  ``foo``  class  in the  Falaise  library
or in some plugin shared library.

A     full     example     is      provided     in     the     Falaise
``documentation/data_models/examples/ex01`` folder.
