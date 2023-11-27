===========================================================
ExAegir - Example Plugin for Falaise AEGIR mechanism
===========================================================

The ExAegir example  Falaise plugin project provides  some dummy event
generator classes  as an illustration  of the possibility  to register
and use non-official event generator  in Falaise.  At dynamic loading,
the ExAegir  plugin library  registers automatically these  classes in
the  Bayeux event  generator system  factory.  It  also registers  the
installation path to  some resource files needed by  the generators at
initialization stage.

The unique  identifier ``"exaegir"`` is  used such as file  paths like
``"exaegir@data/spectrum1.data"``   can   be  automatically   resolved
through   the   ``datatools::fetch_path_with_env``  Bayeux's   utility
function.

Mimicking  the layout  of the  ExAegir project  should allow  users to
implement new event generators usable within Falaise.
   
