# Falaise C++ Library and Applications for the SuperNEMO experiment

[![Build Status](https://travis-ci.org/SuperNEMO-DBD/Falaise.svg?branch=develop)](https://travis-ci.org/SuperNEMO-DBD/Falaise)

Falaise provides the main computational environment for the simulation,
processing and analysis of data for the [SuperNEMO double beta decay search
experiment](http://nemo.in2p3.fr). The three main components are

- `libFalaise`: the core library.

- `flsimulate`: the main detector simulation application.

- `flreconstruct`: the main reconstruction application.

A pipeline architecture is used in which the pipeline stages
may be configured and added to at runtime via a plugin system.
Code for pipeline stages is stored in the ``modules`` subdirectory
with each module (or set of modules) having its own directory.

Additional modules from external sources and individual contribution
can be used too.

# Getting Falaise
## Quickstart
We recommend installing Falaise and its requirements as documented on our [fork of Homebrew]
(https://github.com/SuperNEMO-DBD/brew). Both build-from-source and Docker/Singularity
Image installs are available, both providing a complete suite for using and developing
Falaise and extension modules. We strongly recommend Docker/Singularity Images on systems
which support these tools, especially on institution systems/clusters such as CC-IN2P3, as
these provide the most reliable and reproducible environments.

For both systems, we strongly recommend that you run and develop Falaise from within the
`snemo-shell` environment provided, which is started and exited by running:

```console
$ brew snemo-shell
Homebrew >=1.7.1 (shallow or no git repository)
Supernemo-dbd/homebrew-core (git revision 15b2f; last commit 2019-02-27)
Type "brew ls --versions" to list available software
Type "exit" to deactivate the session
snemo-shell> flsimulate --version
...
snemo-shell> exit
$
```

Once installed and setup, consult the [online documentation](https://supernemo-dbd.github.io/Falaise)
for a full guide to running `flsimulate`, `flreconstruct`, and writing new plugin modules.


## Building, Testing and Installing from Source
To get the source code, either download a release tarball or to get the latest development,
do

```
$ git clone https://github.com/supernemo-dbd/Falaise.git Falaise.git
```

To build Falaise, do

```
$ mkdir Falaise.build
$ cd Falaise.build
$ cmake -DCMAKE_INSTALL_PREFIX=/where/you/want/to/install ../Falaise.git
...
```

If you wish to enable `make test` after building, add the following option to cmake.

```
$ cmake -DFALAISE_ENABLE_TESTING=ON
```

Errors at this stage are likely to be due to missing/unfound packages. If this is the
case, `cmake` can be directed to look in specific places using the `CMAKE_PREFIX_PATH`
variable (If you are running in an `snemo-shell` session, this is preset for you).
For example, if `Boost` is installed in `$HOME/boost` and `GSL` in `$HOME/software/gsl`,
`cmake` would be run as:

```
$ cmake -DCMAKE_PREFIX_PATH="$HOME/boost;$HOME/software/gsl" -DCMAKE_INSTALL_PREFIX=/some/path ../Falaise.git
```

The `CMAKE_INSTALL_PREFIX` variable tells CMake where the products of the build
should be installed. Ensure you have write permissions to this location.

After configuration is successful, the build is run by:

```
$ make -j<N>
```

Adjust `<N>` to the number of cores on your machine for a faster build. After a
successful build, unit tests can be run using the `test` target:

```
$ make test
```

On completion of the build, the Falaise programs, libraries and documentation are available
for use under a POSIX-style hierarchy under the `BuildProducts` subdirectory of
the directory in which you ran the build. For example,

```
$ ./BuildProducts/bin/flsimulate --help
```

Documentation for the build is viewable by opening the main page in your browser.
On macOS, the `open` command can be used:

```
$ open ./BuildProducts/share/Falaise-<VERSION>/Documentation/API/html/index.html
```

where ``<VERSION>`` is the current Falaise version (simply use tab-complete
if you are unsure of this)
which should open a new tab in your browser populated with the documentation
frontpage. On Linux, you can generally do the same by running

```
$ xdg-open ./BuildProducts/share/Falaise-<VERSION>/Documentation/API/html/index.html
```

though ``xdg-open`` may not always be present (``gnome-open`` may be used
instead, for example).

If you need to install Falaise, you can run

```
$ make install
```

to install everything in a standard POSIX style hierarchy under the directory
passed as ``CMAKE_INSTALL_PREFIX``.


## Full Prerequisites
To build and run Falaise on your machine, the following OS and Software must be
present:

- Linux or macOS System
  - Supported Linux systems: CentOS6/7, Ubuntu 16.04/18.04LTS
  - Other Linux distributions are known to work, but are not
    officially supported. However, patches are welcome to resolve encountered issues!
  - Suported macOS systems: 10.12/13/14 (Sierra/High Sierra/Mojave)
- GCC (>= 7), Clang (>=6) or Xcode >= 9
- [CMake](https://cmake.org) 3.12 or higher
- [Doxygen](http://www.doxygen.org) 1.8 or higher
- [Bayeux](https://github.com/SuperNEMO-DBD/Bayeux) 3.3.0 or higher
- [Boost](https:/boost.org) 1.63.0 only
  - Must provide `program_options`, `thread`, `serialization`, `filesystem` and `system` components
- [Camp](https://github.com/tegesoft/camp) 0.7.1 or higher
- [GSL](http://www.gnu.org/s/gsl) 2 or higher
- [CLHEP](http://proj-clhep.web.cern.ch) 2.1.3.1 only
- [Geant4](http://geant4.cern.ch) 9.6.4 only
   - with GDML support enabled
- [ROOT](http://root.cern.ch) 6.10 or higher

Falaise requires use of the C++11 or higher standard, so all of the above packages
and their C++ dependencies must be built/installed using this standard. This is
to ensure binary compatibility.

The [SuperNEMO brew system](https://github.com/SuperNEMO-DBD/brew) can install the
above, either as build-from-source or as Docker/Singularity Images.


# Getting Help

If you have problems, questions, ideas or suggestions on Falaise or
any of its submodules, [raise an issue](https://supernemo-dbd.github.io/Falaise/issues).

# Contributing to Falaise

Please see the [Contribution Guide](https://github.com/SuperNEMO-DBD/Falaise/blob/develop/CONTRIBUTING.md#)


# Naming
Falaise is named thus because [Falaise is the town in Normandy](http://en.wikipedia.org/wiki/Falaise,_Calvados) where William
the Conqueror was born. Note this has nothing to do with SuperNEMO software!

# Licensing
Please study the file ``LICENSE.txt`` for the distribution terms and
conditions of use of Falaise.



## Contributors

Many thanks go to Falaise's [contributors](https://github.com/SuperNEMO-DBD/Falaise/graphs/contributors)
