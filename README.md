# Falaise C++ Library and Applications for the SuperNEMO experiment

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

# Installing Falaise
## Quickstart
If you simply wish to use or try out Falaise, we recommend installing it
using [our Home/Linuxbrew tap](https://github.com/SuperNEMO-dbd/homebrew-cadfael).
This will install everything you need, and includes the latest official
release of the software. Once installed, consult the [online documentation](https://supernemo-dbd.github.io/Falaise)
for a full guide to running the software and writing new plugin modules.

## Building and Installing from Source
To build Falaise on your machine for use or development, the following requirements
must be met:

- Linux or macOS System
  - Supported Linux systems: CentOS6/7, Ubuntu 14.04/16.04LTS
  - Suported macOS systems: 10.10/11/12 (Mavericks/El Capitan/Sierra)
- GCC (>= 4.0), Clang (>=3.5) or Xcode 7/8
- CMake 3.5 or higher
- Doxygen 1.8 or higher
- [Bayeux](https://github.com/SuperNEMO-DBD/Bayeux) 3.0 or higher
- [Boost](https:/boost.org] 1.63.0 or higher
  - Must provide program_options, serialization, filesystem, system components
- [Camp](https://github.com/tegesoft/camp) 0.7.1 or higher
- [GSL](http://www.gnu.org/s/gsl) 1.16 or higher
- [CLHEP](http://proj-clhep.web.cern.ch) 2.1.3.1 or higher
- [Geant4](http://geant4.cern.ch) 9.6.4 or higher
   - with GDML support enabled
- [ROOT](http://root.cern.ch) 6.08 or higher

Falaise requires use of the C++11 or higher standard, so all of the above packages
and their C++ dependencies must be built/installed using this standard. This is
to ensure binary compatibility.

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

Errors at this stage are likely to be due to missing/unfound packages. If this is the
case, `cmake` can be directed to look in specific places using the `CMAKE_PREFIX_PATH`
variable. For example, if `Boost` is installed in `$HOME/boost` and `GSL` in `$HOME/software/gsl`,
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

Adjust `<N>` to the number of cores on your machine for a faster build.
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

If you need to install Falaise, after building you can run

```
$ make install
```

to install everything in a standard POSIX style hierarchy under the directory
passed as ``CMAKE_INSTALL_PREFIX``.


# Getting Help

If you have problems, questions, ideas or suggestions on Falaise or
any of its submodules, [raise an issue](https://supernemo-dbd.github.io/Falaise/issues).


# Naming
Falaise is named thus because [Falaise is the town in Normandy](http://en.wikipedia.org/wiki/Falaise,_Calvados where William
the Conqueror was born. Note this has nothing to do with SuperNEMO software!

.. _`town in Normandy`: http://en.wikipedia.org/wiki/Falaise,_Calvados

# Licensing
Please study the file ``LICENSE.txt`` for the distribution terms and
conditions of use of Falaise.


# Contributing to Falaise
WIP

## Contributors

Steven Calvez, Xavier Garrido, François Mauger, Ben Morgan, Guillaume Oliviero
