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
 * ``_BuildResourceList.sh``  (not published):  a shell  script
   which builds the list of resource  files for the CMake build system
   and stores it in  the  ``_resources.cmake``  file.  All  files  or
   directories (including this  script) of which the  names start with
   an underscore character  should not be published and  should not be
   visible from the Falaise installation path.
 * ``_resources.cmake`` (not published): this CMake script defines the
   list of published resource files for the CMake build system through
   the  ``Falaise_RESOURCES``  variable. This file should be rebuilt
   by the ``_BuildResourceList.sh`` script each time this is it
   needed during the development cycle (adding or removing files).
   This rebuilding can be done by hand by running the script directly ::

    $ ./_BuildResourceList.sh
    $ ls -1 _resources.cmake
    _resources.cmake

   or by running the script through your build tool via the
   ``falaise_resources`` custom target ::

    $ make falaise_resources

 * ``config`` : the top directory for all official configuration files
   used   by  the   simulation,  reconstruction   and  data   analysis
   processing. See the ''README.rst'' file for details.
