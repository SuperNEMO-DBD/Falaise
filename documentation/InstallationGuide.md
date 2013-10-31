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
  * At least 200MB for a source code check out
  * At least 4.5GB for a first from scratch build
  * At least 2.5GB for a complete installation
* Familiarity with the UNIX command line
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

* https://nemo.lpc-caen.in2p3.fr

We recommend using the following layout of code

~~~~~
SuperNEMO/
|-- lpc-caen.svn
|-- builds
|-- installs
~~~~~

where the `SuperNEMO` directory can be located where you wish, though must
be on a filesystem with the required free space.

To create this setup, open a terminal, choose a directory for your
SuperNEMO workspace and run the following.

~~~~~{.bash}
$ cd /path/to/your/chosen/workspace
$ mkdir -p SuperNEMO/{lpc-caen.svn, builds, installs}
$ cd SuperNEMO
$ ls
builds  installs  lpc-caen.svn
~~~~~

With the directories in place, we can checkout the code. This is done
via the `svn` command line client, and we be using a "sparse checkout"
pattern. Whilst this leads to a few more steps upfront, it provides
a very stable and well organised working copy.

Note that in the following, we demonstrate how create a checkout by the fastest and
easiest means possible. This means that you end up with:

* A read-only repository
  * You can update, view logs and create diffs and patches
  * You cannot commit changes

If you already have a developer's account at LPC Caen, then the following will also
work, simply remove the `--username` and `--no-auth-cache` arguments from the svn commands.

~~~~~{.bash}
$ pwd
/path/to/your/chosen/workspace/SuperNEMO
$ svn co https://nemo.lpc-caen.in2p3.fr/svn --username=visitor --no-auth-cache --depth empty lpc-caen.svn
~~~~~

At this point, you may see the following warning about certificates:

~~~~~{.bash}
Error validating server certificate for 'https://nemo.lpc-caen.in2p3.fr:443':
 - The certificate is not issued by a trusted authority. Use the
   fingerprint to validate the certificate manually!
 - The certificate hostname does not match.
 - The certificate has expired.
Certificate information:
 - Hostname: caedev.in2p3.fr
 - Valid: from Thu, 15 May 2008 11:52:02 GMT until Fri, 15 May 2009 11:52:02 GMT
 - Issuer: LPC Caen, CNRS, Caen, France, Fr
 - Fingerprint: ae:89:9e:38:e3:80:a7:a3:b2:1e:df:5e:eb:33:2d:ba:83:be:b7:11
(R)eject, accept (t)emporarily or accept (p)ermanently? p
~~~~~

Simply press `t` or `p` to accept the certificate. The process will then continue
by asking for the password for `visitor`, for which you should enter `anonymous` (
If you have accessed the LPC repository before, you may not see either the certificate
check or password prompt

~~~~~{.bash}
Password for 'visitor':
Checked out revision 13809.
~~~~~

The revision number you see will vary depending on the current state of development.


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
