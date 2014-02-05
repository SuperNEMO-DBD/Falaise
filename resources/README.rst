======================
Falaise resource files
======================

:Authors: François Mauger
:Date:    2013-10-18

.. contents::
   :depth: 3
..

Presentation
============

This is  the top  level directory  of all  resource files  provided by
Falaise.

Contents:

 * ``README.rst`` : this file
 * ``config`` : the top directory for all official configuration files
   used   by  the   simulation,  reconstruction   and  data   analysis
   processing.   Our  convention  is  to   organize  the  files  in  a
   meaningfull  hierarchy, that  enables configuration  versioning and
   reusability  of  versioned  resource  files.

   All  files  should  be  accessible from  within  Bayeux  file  path
   utilities (``datatools::fetch_path``, ``datatools::fetch_path_with_env``...)
   with the  following syntax: ::

     "@falaise:config/topic/subtopic/.../version/filename"

   Example  for   the  main  configuration  file   for  the  SuperNEMO
   demonstrator module virtual geometry: ::

     "@falaise:config/geometry/snemo/demonstrator/3.0/manager.conf"
