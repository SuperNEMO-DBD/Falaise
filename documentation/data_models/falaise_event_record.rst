Falaise event record data model
===============================

The SuperNEMO pipeline event record data model
----------------------------------------------

The   SuperNEMO  pipeline   mechanism  is   implemented  through   the
``dpp::dpp_driver`` class.  This pipeline  uses a generic container as
its  core data  model,  namely the  ``datatools::things`` class.   The
*things*  class is  a  general  purpose container  (in  reality it  is
implemented as a  dictionary of *serializable objects*)  where one can
store arbitrary  *banks* of data.  The  ``datatools::things`` class is
serializable through the Bayeux/datatools and Bayeux/brio systems.

Each *bank* contains a specific data  model for a specific purpose and
is addressed  through an  unique name/label (a  ``std::string``).  For
example, a *things* object that represents a SuperNEMO or BiPo3 *event
record* may contain:

 * bank ``"EH"`` :  an *event header* data model  which stores general
   informations about an event record  (run and event IDs, date, time,
   flags...),   the    data   model   being   implemented    by   some
   ``event_header`` class (to be defined elsewhere).
 * bank ``"SD"`` : a *simulated_data*  bank which stores the output of
   a simulation  run (example:  ``flsimulate``), the data  model being
   implemented  by  the ``mctools::simulated_data``  class  (reference
   needed).

A data file that stores *event records* thus looks like (in pseudo-XML): ::

  <event_record_1 />
  <event_record_2 />
  <event_record_3 />
  <event_record_4 />
  ...
  <event_record_N />

where  *event  record*  records   are  stored  sequentially,  one  per
archive.  Each   event  record  stores  internally   several  banks  of
data. Example (in pseudo-XML): ::

  <event_record_1>
    <bank name="EH" type="event_header">
      Event header data model run number, event number...
    </bank>
    <bank name="SD" type="mctools::simulated_data">
      Simulated data model: vertex, primary event, true hits...
    </bank>
    <bank name="XXX" type="YYY">
      Some data ...
    </bank>
    ... other banks...
  </event_record_1>

I/O
---

The Bayeux/dpp  library provides two *data  processing module* classes
dedicated to I/O operations on *event record* objects:

  * the      ``dpp::input_module``:     loads      *event     records*
    (``datatools::things``    objects)   from    Bayeux/datatools   or
    Bayeux/brio files (autodetected from the file extension).
  * the     ``dpp::output_module``:     stores     *event     records*
    (``datatools::things`` objects) in Bayeux/datatools or Bayeux/brio
    files (autodetected from the file extension).

Thanks to the technology of  the Boost/Serialization library, we don't
need to know what is the  exact content of the serialized event record
objects, this is determined on-the-fly,  provided that the data models
stored  in  the *banks*  are  registered  classes through  a  specific
mechanism (reference needed).
