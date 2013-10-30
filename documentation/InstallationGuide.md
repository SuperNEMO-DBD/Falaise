Installing Falaise from Source
==============================

\tableofcontents

Introduction {#intro}
============
TODO : To keep things together, am putting what will go into DocDB
docs here. So yes, this is a scratchpad of sorts

This walkthrough is intended to get you up and running with Falaise
as fast as possible. It therefore adopts a specific layout of where
working copies of sources, builds of these sources and installs of the
builds. You do not have to follow this layout, but we recommend it
as a fast and convenient way to organise things.

Preparing Your System for Installation {#preparation}
======================================
To install the Falaise stack, you first need the following:

* UNIX Operating System
  * Linux
  * Mac OS X 10.7/8 (Lion/Mountain Lion) (10.6 and 10.9 should work, but are not tested)
* Several GB of free disk space:
  * At least 4.5GB for a complete build
  * At least 2.5GB for a complete installation
* Required Software
  * Subversion 1.6 or above
  * CMake 2.8.8 or above
  * Doxygen 1.8 or above
  * C/C++ compiler supporting at least the C++03 standard
    * GCC 4.5 or above, and Clang 3.1 or above are recommended
    * If available, you should use a C++11 compliant compiler
      * GCC 4.7 or higher, Clang 3.3 or higher
      * This is purely for the best forward compatibility, it is not a requirement.
  * BZip2 Library and Headers
  * zlib Library and Headers

We recommend that the Required Software are installed via the relevant
package manager (e.g. yum, apt on Linux, MacPorts, Homebrew on Mac OS X)
for your system, unless the version requirement listed is not met.

We expect the Falaise stack to compile and run on most modern BSD or Linux
based systems provided the listed Required Software is installed. Note
however that this list is a work in progress, and sections below will
highlight where problems may occur with missing software, and how to
report and resolve these issues.


Getting Falaise {#getting}
===============
Falaise is developed using the Subversion repository located at LPC Caen.

* https://nemo.lpc-caen.in2p3.fr/svn

We recommend using the following layout of code

~~~~~
SuperNEMO/
|-- lpc-caen.svn
|-- builds
|-- installs
~~~~~


Building/Installing Falaise {#buildinstall}
===========================

Using Falaise {#using}
=============

Developing Falaise {#developing}
==================
You will need to get an account on the LPC Trac system.

We still recommend using the simple three level layout described earlier
as the sparse checkout allows you full and easy access to all repository
information.
