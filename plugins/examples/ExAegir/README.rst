===========================================================
ExAegir - Example Plugin for Falaise AEGIR mechanism
===========================================================

The  ExAegir  example  project  provides some  dummy  event  generator
classes.   At dynamic  loading, the  ExAegir plugin  library registers
automatically those classes in the  Bayeux event generator factory. It
also  register  the  installation  path to  its  resource  files  with
identifier     ``"exaegir"``     such     as    file     path     like
``"exaegir@data/spectrum1.data"``   can   be  resolved   through   the
``datatools::fetch_path_wth_env``.
   
