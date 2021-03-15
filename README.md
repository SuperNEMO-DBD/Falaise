# Falaise C++ Library and Applications for the SuperNEMO experiment

[![Build Status](https://travis-ci.org/SuperNEMO-DBD/Falaise.svg?branch=develop)](https://travis-ci.org/SuperNEMO-DBD/Falaise)

Falaise provides the main computational environment for the simulation,
processing and analysis of data for the [SuperNEMO double beta decay search
experiment](http://nemo.in2p3.fr). The three main components are

- `libFalaise`: the core library.

- `flsimulate`: the main detector simulation application.

- `flreconstruct`: the main reconstruction application.

- `flvisualize`: the event/detector geometry display application

A pipeline architecture is used for `flreconstruct` in which the pipeline stages
may be configured and added to at runtime via a plugin system.
Code for pipeline stages is stored in the ``modules`` subdirectory
with each module (or set of modules) having its own directory.

Additional modules from external sources and individual contribution
can be used too.

# Installing and Using Falaise
If you are _using_ Falaise, i.e. running the applications to generate, process,
and analyse data, _then you only require an install of Falaise_. If you need to _develop_
Falaise, i.e. make changes to the code to fix issues or add new features, please
see [Developing and Testing Falaise](#developing-and-testing-falaise) below.

We recommend installing Falaise and its requirements as documented on our [fork of Homebrew](https://github.com/SuperNEMO-DBD/brew).
By default this will install the latest **stable release** of Falaise. All releases with details of the changes
introduced are listed on the [GitHub Releases page](https://github.com/SuperNEMO-DBD/Falaise/releases).
For production and general work, you should only use a **stable release** (i.e. a git tag using a version
number such as `v4.0.3`). These are _not_ guaranteed to be bug free, but have passed all
known/implemented tests at the time of their release (i.e. no known bugs at that time), with physics
related features validated to the same level.

[Docker](https://www.docker.com)/[Singularity](https://www.sylabs.io/singularity/)
Image installs are also available, both providing a complete suite of software and tools for using and developing Falaise and extension modules. We strongly recommend Docker/Singularity Images on systems which support these tools,
especially on institution systems/clusters such as CC-IN2P3, as they provide the most
reliable and reproducible Falaise installs.

For both native image installs we strongly recommend that you run the Falaise applications from within
the `snemo-shell` shell environment provided, which is started and exited by running:

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
for a full guide to running `flsimulate`, `flreconstruct`, and writing your own plugin modules.


# Building and Testing Falaise
If you wish to _develop_ Falaise, e.g. find and fix a bug, or add new functionality, then you
_will need to get the Falaise source code and build and test it yourself_. The first step here
is to install of the tools and libraries needed to build and test Falaise. The aforementioned
[SuperNEMO Homebrew](https://github.com/SuperNEMO-DBD/brew) installer or Docker/Singularity
installers can be used here for simplicity as they provide an install of all the needed software
for development. You can also install the full list of prerequisites using the package manager of
your choice:

- Linux or macOS Operating System
  - Supported Linux systems: CentOS7, Ubuntu 20.04LTS
  - Other Linux distributions are known to work, but are **not supported**
  - Supported macOS systems: 10.14/15 (Mojave, Catalina)
- GCC (>= 7), Clang (>=6) or Xcode >= 10
- [Git](https://git-scm.com)
- [CMake](https://cmake.org) 3.12 or higher
- [Doxygen](http://www.doxygen.org) 1.8 or higher
- [Bayeux](https://github.com/SuperNEMO-DBD/Bayeux) 3.4.1 or higher
- [Boost](https:/boost.org) 1.69.0 **only**
  - Must provide `program_options`, `thread`, `serialization`, `filesystem` and `system` components
- [Camp](https://github.com/tegesoft/camp) 0.7.1 or higher
- [GSL](http://www.gnu.org/s/gsl) 2 or higher
- [CLHEP](http://proj-clhep.web.cern.ch) 2.1.3.1 only
- [Geant4](http://geant4.cern.ch) 9.6.4 only
   - with GDML support enabled
- [ROOT](http://root.cern.ch) 6.16 or higher

With these in place, to get set up on your local system to develop Falaise you should then:

1. Create your own Fork of the [SuperNEMO-DBD/Falaise](https://github.com/SuperNEMO-DBD/Falaise) repository on GitHub
2. Clone your Fork to your local system
3. Build and test Falaise
4. Start developing and testing on a _Topic Branch_
5. Submit your _Topic Branch_ to [SuperNEMO-DBD/Falaise](https://github.com/SuperNEMO-DBD/Falaise) as a Pull Request

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for comprehensive guides and cheatsheets on this workflow. Here,
we will quickly demonstrate steps 2 and 3.

To get the source code from your Fork of Falaise locally, simply clone it:

```
$ git clone https://github.com/<yourgithubid>/Falaise.git Falaise.git
```

With the source code cloned, it can be configured and compiled by doing:

```
$ mkdir Falaise.build
$ cd Falaise.build
$ cmake -DFALAISE_ENABLE_TESTING=ON ../Falaise.git
```

Errors at this stage are likely to be due to missing/unfound packages. If this is the
case, `cmake` can be directed to look in specific places using the `CMAKE_PREFIX_PATH`
variable. For example, if `Boost` is installed in `$HOME/boost` and `GSL` in `$HOME/software/gsl`,
`cmake` would be run as:

```
$ cmake -DCMAKE_PREFIX_PATH="$HOME/boost;$HOME/software/gsl" -DFALAISE_ENABLE_TESTING=ON ../Falaise.git
```

After configuration is successful, the build is run by:

```
$ make -jN
```

Adjust `N` to the number of cores on your machine for a faster build. After a
successful build, unit tests can be run using `ctest`:

```
$ ctest -jN
```

This will run the tests in parallel and provide a summary report of successes and failures. You
can see full output from the tests, e.g. to check failures, by doing

```
$ ctest -jN -VV
```

See `man ctest` for further options.

On completion of the build, the Falaise programs, libraries and documentation are available
for direct use under a POSIX-style hierarchy under the `BuildProducts` subdirectory of
the directory in which you ran the build. For example,

```
$ ./BuildProducts/bin/flsimulate --help
```

Documentation is browsable by opening the main page in your browser.
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

The above only covers the basics of creating a local development setup, and you
should consult the full [CONTRIBUTING.md](CONTRIBUTING.md) for details of the
development, testing and Pull Request submission process.


## Stability of the `develop` branch

Please note that builds like the above **must not be used for production or analysis work**.
The `develop` branch that provides the foundation for new developments is used only to _integrate fixes and new functionality_. Whilst it is tested to the same level as releases through the Pull Request process, the development
cycle _may_ introduce new and as-yet unidentified bugs.


# Getting Help

If you have problems, questions, ideas or suggestions on Falaise or
any of its submodules, [just raise this on the project board](https://supernemo-dbd.github.io/Falaise/issues).

# Naming
Falaise is named thus because [Falaise is the town in Normandy](http://en.wikipedia.org/wiki/Falaise,_Calvados) where William
the Conqueror was born. Note this has nothing to do with SuperNEMO software!

# Licensing
Please study the file ``LICENSE.txt`` for the distribution terms and
conditions of use of Falaise.

# Contributors

Many thanks go to Falaise's [contributors](https://github.com/SuperNEMO-DBD/Falaise/graphs/contributors)
