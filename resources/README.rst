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

Files:

 * ``README.rst`` : this file
 * ``_make_list_of_resource_files``  (not published):  a shell  script
   which builds the list of resource  files for the CMake build system
   and  store  it in  the  ``_resources.cmake``  file.  All  files  or
   directories (including this  script) of which the  names start with
   an underscore character  should not be published and  should not be
   visible from the Falaise installation path.
 * ``_resources.cmake`` (not published): this CMake script defines the
   list  of resource  files for  the  CMake build  system through  the
   ``_falaise_list_of_resources_files`` variable. It  must be included
   in the  main Falaise  CMake script.   This file  is rebuilt  by the
   ``_make_list_of_resource_files` script each time there is it needed
   (adding or removing files): ::

    $ ./_make_list_of_resource_files
    $ ls -1 _resources.cmake
    _resources.cmake

Subdirectories:

 * ``config`` : the top directory for all official configuration files
   used   by  the   simulation,  reconstruction   and  data   analysis
   processing.   Our  convention  is  to   organize  the  files  in  a
   meaningfull  hierarchy, that  enables configuration  versioning and
   reusability  of  versioned  resource  files.

   All  files  should  be  accessible from  within  Bayeux  file  path
   utilities (``datatools::fetch_path``, ``datatools::fetch_path_with_env``...)
   with the  following syntax: ::

     "@falaise:config/topic/subtopic/.../version/filename"

   Example for the  main configuration file for  the SuperNEMO virtual
   geometry: ::

     "@falaise:config/geometry/snemo/demonstrator/3.0/manager.conf"
