=================================
Falaise plugins/modules directory
=================================



:Authors: François Mauger
:Date:    2014-04-24

.. contents::
   :depth: 3
..


Presentation
============

This is  the top  level directory  of all plugins/modules associated to
the Falaise library.

Contents:

 * ``README.rst`` : this file
 * ``_make_list_of_plugins_resource_files`` : a shell script that generated
   official list of published resource files for each module
 * Modules:

    * ``things2root/`` : a module that builds a Root tree from Falaise data models (ntuplizer)
    * ``CAT/`` : The CAT and SULTAN modules that build clusters of tracker hits
      as track candidates for particle trajectory fitting
    * ``TrackFit/`` : a module that fits clusters of tracker hits with
      linear or helical trajectories.
    * ``ChargedParticleTracking/`` : a module that interpret tracker
      trajectories given SuperNEMO detector *i.e.* determine electric charge,
      associate trajectory to calorimeter hits and extrapolate vertices.


Management of resource files
============================

Any  module may  provide some  resource files  (configuration or  data
files,  examples...).  We  use a  convention to  allow each  module to
publish   such    files   within    the   Falaise    framework.    The
``_make_list_of_plugins_resource_files``  script is  able to  scan the
``resources``  directory from  the base  source directory  of a  given
module. The list of all publishable resource files is built and stored
in the  ``resources/_resources.cmake`` file  in a format  suitable for
CMake  processing.  From  this file,  CMake is  able to  configure and
install all the resource files associated to the module.

Each time some  new resource files are added to  (or removed from) the
list of publishable resource files by  the developper of a module, the
``_make_list_of_plugins_resource_files`` script must be used to update
the *official* list: ::

  $ cd {Falaise trunk base dir}/modules
  $ ./_make_list_of_plugins_resource_files --only {ModuleName}

where  ``{Falaise  trunk  base  dir}``  is  the  Falaise  source  base
directory and ``{ModuleName}`` is the name of the target module.

Example for the MockTrackerClusterizer module: ::

  $ ./_make_list_of_plugins_resource_files --only MockTrackerClusterizer
  $ svn st MockTrackerClusterizer/resources/
  ...
  M       _resources.cmake
  ...

Note  that  all  files  or directories  starting  with  an  underscore
(examples:  ``_dummy.txt``, ``_backup.d``),  ``.svn`` directories  and
backup files with the ``~`` suffix are not publishable by convention.
