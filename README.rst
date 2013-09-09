================================================
Falaise C++ Library for the SuperNEMO experiment
================================================

:Authors: François Mauger; Xavier Garrido, Ben Morgan
:Date:    Today

.. contents::
   :depth: 3
..

About Falaise
=============

Falaise provides a collection of C++ classes and functions designed
for the simulation, recording and analysis of data for the SuperNEMO
experiment. This functionality is split into two main applications:

- FalaiseSim, the main detector simulation
- FalaiseRec, the main reconstruction application

FalaiseRec uses a pipeline architecture in which the pipeline stages
may be configured and added to at runtime via a plugin system.

TODO : other apps and APIs.


Naming
------
Falaise is named thus because Falaise_ is the town in Normandy where William
the Conqueror was born.

.. _Falaise: http://en.wikipedia.org/wiki/Falaise,_Calvados

Licensing
---------
Please study the file ``LICENSE.txt`` for the distribution terms and
conditions of use of Falaise.

Getting Help
============

If you have problems, questions, ideas or suggestions on Falaise or
any of its submodules, please contact the SuperNEMO Software Working
Group via the main development website https://nemo.lpc-caen.in2p3.fr/.

Getting Falaise
===============

You can obtain the Falaise source code from the main LPC Caen Subversion
repository. To checkout the trunk, simply do:

.. code:: sh

    $ svn co https://nemo.lpc-caen.in2p3.fr/svn/Falaise/trunk Falaise/trunk

You can of course checkout to a directory of your choosing. Tagged versions
can be listed by doing:

.. code:: sh

    $ svn ls https://nemo.lpc-caen.in2p3.fr/svn/Falaise/tags 

and checked out in similar fashion to the trunk. If you use a tag, you
should always take the newest version. However, Falaise is developed with
the aim of keeping the trunk stable, so you can use this with high
confidence.

Preparing Your System for Falaise Installation
==============================================

To install Falaise, you first need the following sofware:

Prerequisite Software
---------------------

-  **UNIX Operating System**:

   -  Linux
   -  Mac OS X 10.7 (Lion) or higher

We expect Falaise to work on most modern BSD based Unices or Linux
distributions derived from Fedora(RedHat) or Debian provided the
software listed below is installed. However, we can only fully support
and test the following at present:

-  SUSE Enterprise Linux 11SP2
-  Ubuntu 12.04LTS: http://www.ubuntu.com
-  Scientific Linux 5/6: http://www.scientificlinux.org
-  Mac OS X 10.7
-  Mac OS X 10.8

If you have problems on systems other than these, please contact us,
though support will be on a best effort basis.

-  **Core Software Required**:

   -  Subversion (only for development)
   -  CMake 2.8.0 or higher: http://www.cmake.org
   -  C/C++ compiler supporting at least C++03 standard
      (GNU/Clang/Intel)

On Linux, you should install these through the package manager for your
distribution. Some older Linux systems (SL/CentOS, especially on
clusters) may not provide CMake 2.8. If this is the case, then you
should download the latest Linux *binary .sh* file from:

-  http://www.cmake.org/cmake/resources/software.html

and follow the instructions on that page to install it.

On Mac OS X, simply install CMake from the latest ``Darwin64`` dmg
bundle available from:

-  http://www.cmake.org/cmake/resources/software.html

To obtain the other tools, simply install the latest version of Xcode
from the Mac App Store. After installation, you should also install the
Xcode command line tools via going to the menu Xcode->Preferences,
clicking on the Downloads tab, and then installing Command Line Tools.

-  **Core Libraries Required**

   -  Boost 1.53.0 or higher: http://www.boost.org
       - with serialization, filesystem, system libraries.
   -  Camp 0.7.1 : https://github.com/tegesoft/camp
   -  GSL 1.15 or higher: http://www.gnu.org/s/gsl
   -  CLHEP 2.1.3.2 or higher: http://proj-clhep.web.cern.ch
   -  Geant4 9.6.0 or higher: http://geant4.cern.ch
       - with GDML support enabled
   -  ROOT 5.34.0 or higher: http://root.cern.ch

For ease of use, LPC Caen provides the Cadfael SDK which bundles these
libraries for easy use by Bayeux and clients of Bayeux. You can use this
bundle, or, at your discretion, your own, or system, installs of these
packages.


   
Installing Falaise
==================
Falaise provides a CMake based build system. We'll assume for brevity
that you are using a UNIX system on the command line (i.e. Mac or Linux).
We'll also assume that you're going to use the Cadfael SDK to provide
the required third party packages.

To build Bayeux, simply do, from the directory in which this file
resides:

.. code:: sh

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_INSTALL_PREFIX=<where you want to install> -DCMAKE_PREFIX_PATH=<path to your Cadfael install> ..
    $ make && make install



Troubleshooting
===============
WIP

Using Falaise
=============

To be done.

Developing Falaise
==================

To be done.


