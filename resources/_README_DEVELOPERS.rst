======================
Falaise resource files
======================

:Authors: François Mauger
:Date:    2014-01-30

.. contents::
   :depth: 3
..

Presentation
============

This is  the top  level directory  of all  resource files  provided by
Falaise.

Files:

 * ``_README_DEVELOPERS.rst`` : this file (not published).
 * ``README.rst``  :  description of  the  published  contents of  the
   resource directory.
 * ``_make_list_of_resource_files``  (not published):  a shell  script
   which builds the list of resource  files for the CMake build system
   and  store  it in  the  ``_resources.cmake``  file.  All  files  or
   directories (including this  script) of which the  names start with
   an underscore character  should not be published and  should not be
   visible from the Falaise installation path.
 * ``_resources.cmake`` (not published): this CMake script defines the
   list of published resource files for the CMake build system through
   the  ``_falaise_list_of_resources_files``  variable.   It  must  be
   included in the main Falaise CMake script.  This file is rebuilt by
   the ``_make_list_of_resource_files``  script each  time this  is it
   needed during the development cycle (adding or removing files): ::

    $ ./_make_list_of_resource_files
    $ ls -1 _resources.cmake
    _resources.cmake
 * ``config`` : the top directory for all official configuration files
   used   by  the   simulation,  reconstruction   and  data   analysis
   processing. See the ''README.rst'' file for details.
