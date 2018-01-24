Manage resource files in Falaise   {#manageresourcefiles}
=================================

\tableofcontents

Introduction to resource files {#manageresourcefiles_intro}
==============================

Each version  of Falaise is  delivered with  a set of  resource files,
stored from the `resources/` directory.

A  resource file  is  typically  a *configuration  file*  used by  the
simulation  or  the reconstruction  pipeline  to  setup some  specific
components of the data processing  software chain. It may also consist
in a  *data* file that must  be loaded at program  startup to initiate
some algorithms or functionalities.

Most of the configuration files published in Falaise use a text format
compatible       with      the       ``datatools::properties``      or
``datatools::multi_properties``   classes  provided   by  the   Bayeux
library.

Resource files directory {#manageresourcefiles_resourcefilesdirectory}
========================

The  `resources/` source  directory  is  located as  the  root of  the
Falaise's source  tree.  After installation, the  resources files will
be  stored  from  the  ̀{Falaise  install  path}/share/Falaise-{falaise
version}/resources/` path.

The   `resources/`  source   directory  is   organized  by   topics  :
configuration files, examples, tags (URN  management). Each time a new
resource file has  to be published, it must be  stored in the relevant
subdirectory.

For                            example,                            the
``config/snemo/demonstrator/geometry/4.0/manager.conf``  file  is  the
top  configuration  file  for  the  SuperNEMO  demonstrator's  virtual
geometry model  identified with  the tag ``4.0``.   Should one  add an
improved geometry model typically tagged  ``4.1``, it would make sense
to     store    its     new     main     configuration    file     as:
``config/snemo/demonstrator/geometry/4.1/manager.conf``.

Usually,  building  a  new   configuration  for  a  given  application
(simulation, reconstruction...)  or  service (geometry...)  in Falaise
implies  publishing  several new  resource  files  at the  same  time.
Developers or  experts of the  new functionality have to  populate the
relevant files  in specific possibly new  subdirectories.

Resource file registration {#manageresourcefiles_resourcefileregistration}
==========================


Once stored in  the `resources/` source directory, the  new files must
be  registered in  Falaise.  Falaise  provides a  dedicated script  to
generate automatically the list of all resource files that are part of
a given  release: the `resources/_BuildResourceList.sh`  script.  This
script must be run  every time a resource file is  added in or removed
from  the  `resources/`  source  directory.   It  builds  the  updated
official list of resource files associated to a given Falaise release.
This list is stored in  the `resources/_resources.cmake` file which in
turn is read by CMake during the build & installation process.

Note that files of which the  names start with an underscore (example:
``_foo.txt``) are  *not* registered in  the official list  of resource
files.   This  convention  enables  to  store  in  the  ``resources/``
directory some  files that are not  intended to be published  but only
used by experts during the development process.

Details are given in the `resources/_README_DEVELOPERS.rst` file.

Resource file path in Falaise {#manageresourcefiles_resourcefilepathinfalaise}
=============================

Falaise  has   a  special   mechanism  to  automatically   locate  the
installation path of  any resource file.  A special syntax  is used to
describe  the path  of  a  resource file  relatively  to the  resource
installation top  directory. The syntax simply  uses the ``@falaise:``
prefix  in front  of  the relative  path of  the  resource file.   The
``datatools::fetch_path_with_env(std::string  &)`` function,  provided
by Bayeux, is used to resolve  the real installation path of the file.
Essentially, this prefix  acts as an environment  variable storing the
installation path of the resource file directory.

Example: the real path of the main configuration file of the SuperNEMO geometry model
tagged 4.0 can be addressed with the following string:
``"@falaise:config/geometry/snemo/demonstrator/3.0/manager.conf"``
