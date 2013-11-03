Installing Falaise from Source
==============================

\tableofcontents

Introduction {#intro}
============
This walkthrough is intended to get you up and running with Falaise
as fast as possible. It therefore adopts a specific layout of where
working copies of sources, builds of these sources and installs of the
builds. You do not have to follow this layout, but we recommend it
as a fast and convenient way to organise things.

Falaise is currently at the Alpha stage of development, so you should consider
this document to be a work in progress. We welcome constructive comments,
criticisms and suggestions for improvement, and these should be directed to
the SuperNEMO Software Working Group.

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
Falaise is developed using the Subversion repository hosted by 
[LPC Caen](https://nemo.lpc-caen.in2p3.fr). Whilst you need a full account
to commit to the repository, we will be using the provided read-only access.

To work with the software, we recommend using the following layout of code
in directories:

~~~~~
SuperNEMO/
|-- lpc-caen.svn
|-- builds
|-- installs
~~~~~

where the `SuperNEMO` directory can be located where you wish, though it must
be on a filesystem with the required free space.

To create this setup, open a terminal, choose a directory for your
SuperNEMO workspace and run the following.

~~~~~
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

Note that in the following, we demonstrate how create a checkout by the fastest and easiest means possible. This means that you end up with:

* A read-only repository
  * You can update, view logs and create diffs and patches
  * You cannot commit changes

If you already have a developer's account at LPC Caen, then the following 
instructions will also work, simply remove the `--username` and 
`--no-auth-cache` arguments from the svn commands.

~~~~~
$ pwd
/path/to/your/chosen/workspace/SuperNEMO
$ svn co https://nemo.lpc-caen.in2p3.fr/svn --username=visitor --no-auth-cache --depth empty lpc-caen.svn
~~~~~

At this point, you may see the following warning about certificates:

~~~~~
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

Simply press `t` or `p` to accept the certificate. The process will 
then continue by asking for the password for `visitor`, for which you 
should enter `anonymous` (If you have accessed the LPC repository before, 
you may not see either the certificate check or password prompt).

~~~~~
Password for 'visitor':
Checked out revision 13809.
~~~~~

The revision number you see will vary depending on the current state of development. If you now list the contents of the `lpc-caen.svn` directory
it will be empty apart from a `.svn` directory. We now checkout the parts
we need, specifically the `Cadfael` Software Development Kit (SDK) and
the `Falaise` project.

~~~~~
$ pwd
/path/to/your/chosen/workspace/SuperNEMO
$ cd lpc-caen.svn
$ svn up --set-depth immediates Cadfael Falaise
A    Cadfael
A    Cadfael/trunk
A    Cadfael/branches
A    Cadfael/tags
Updated to revision 13851.
A    Falaise
A    Falaise/trunk
A    Falaise/branches
A    Falaise/tags
Updated to revision 13851.
~~~~~

As above, the revision number will depend on the current state of development.
Each LPC project is organised used the standard Subversion directory layout 
of `trunk` for the mainline of development, `branches` for major revision work
and bug fixes, and `tags` for stable released versions. 

An `ls` of the above directories will show that, again, they are empty. 
However, if you use Subversion's list command, e.g.

~~~~~
$ svn ls Cadfael/trunk
CMakeLists.txt
CadfaelVersion.txt
License.txt
NEWS.rst
README.rst
cmake/
docs/
ports/
testing/
~~~~~

you will get a listing of the contents in the actual repository.
This is what we mean by a "sparse checkout". Such a working copy allows you
to query the repository for what is available, and then pick and choose 
the parts you want. 

Finally, we checkout the trunk of Cadfael, and the Alpha 1 tag of Falaise

~~~~~
$ svn up --set-depth infinity Cadfael/trunk
A    Cadfael/trunk/License.txt
...
A    Cadfael/trunk/testing/ImportedTargets/CMakeLists.txt
Updated to revision 13851.
$ svn ls Falaise/tags
...
$ svn up --set-depth infinity Falaise/tags/..
~~~~~

The basic source code working copy is now in place. For more details on
using Subversion, we recommend the [Subversion Red Bean Guide](http://svnbook.red-bean.com/).


Building/Installing Cadfael {#cadfael}
===========================
Falaise uses several third party libraries to implement its functionality,
specifically

- [Boost C++ Libraries](http://www.boost.org) 1.48.0 and higher
- [CAMP C++ Reflection Library](https://github.com/tegesoft/camp) 0.7.1 and higher
- [CLHEP C++ HEP Library](http://proj-clhep.web.cern.ch/proj-clhep/) 2.1.3.1 and higher
- [GNU Scientific Library](http://www.gnu.org/software/gsl/) 1.15 and higher
- [ROOT C++ HEP Toolkit](http://root.cern.ch) 5.34.00 and higher
- [Geant4 C++ Toolkit for Detector Simulations](http://geant4.cern.ch) 9.6.1 and higher 

You can reuse existing installations of these libraries to build Falaise,
but for the easiest and most robust installation, we recommend using the
`Cadfael` Software Development Kit. This bundles the configuration, build
and installation of these libraries into a single easy to use toolchain.

Cadfael uses the [CMake](http://www.cmake.org) tool for driving the
configure, build and install of the above libraries.


Once you have installed Cadfael successfully, you can remove the build
directory to save disk space. As Cadfael updates are expected to be relatively
infrequent, this "one-shot" installation is generally the easiest way to
manage the SDK.


Building/Installing Falaise {#buildinstall}
===========================

Using Falaise {#using}
=============
The separation of build and install directories adopted above allows Falaise
to be used from either. This is most useful for developers, but 

If built with the defaults, then Falaise will have had Doxygen based
documentation built and installed. You can open this documentation by
pointing your web browser to the frontpage located at:

~~~~~
SuperNEMO/
|-- lpc-caen.svn
|-- builds
|-- installs
~~~~~

A quick way to open these on Mac OS X systems is to run

~~~~~
$ open path/to/index.html
~~~~~

which will open the file your preferred HTML viewer. 
On Linux, this is can be done via

~~~~~
$ xdg-open path/to/index.html
~~~~~

though the existence of the `xdg-open` program may be dependent on the
system you are running on. 


Developing Falaise {#developing}
==================
Development of Falaise falls into two main categories

* Implementing features in the Falaise core
* Implementing new plug-in modules for use in `flreconstruct`

The first of these is generally, though not exclusively, requires a
developer account on the LPC Caen Trac system (one can also create and 
submit patches). Development of plugins does not require a developer account,
though wil do should your module become part of the core.

We still recommend using the simple three level layout described earlier
as the sparse checkout allows you full and easy access to all repository
information. You can also easily add your own repositories if you are
doing private development, e.g.

~~~~~
SuperNEMO/
|-- lpc-caen.svn
|-- my-repo.git
|-- builds
|-- installs
~~~~~

Further documentation on this topic will be added in Alpha 2.

