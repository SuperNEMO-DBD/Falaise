===============================================================
Falaise/trunk installation report on (X)ubuntu 14.04 LTS (64bits)
===============================================================

* Author: Francois Mauger, LPC Caen <mauger@lpccaen.in2p3.fr>
* Date:    2015-06-21
* Last update: 2015-10-05


This is  a report about a  manual installation of Falaise/trunk  on my
64bit Ubuntu 14.04 system.

The target system
=================

Architecture: ::

  $ uname -a
  Linux mauger-laptop 3.13.0-55-generic #94-Ubuntu SMP Thu Jun 18 00:27:10 UTC 2015 x86_64 x86_64 x86_64 GNU/Linux

Processors: ::

  $ cat /proc/cpuinfo | grep "model name"
  model name	: Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
  model name	: Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
  model name	: Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
  model name	: Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz

Linux version: ::

  $ cat /etc/lsb-release
  DISTRIB_ID=Ubuntu
  DISTRIB_RELEASE=14.04
  DISTRIB_CODENAME=trusty
  DISTRIB_DESCRIPTION="Ubuntu 14.04.3 LTS"


GCC version: ::

  $ g++ --version | head -1
  g++ (Ubuntu 4.8.4-2ubuntu1~14.04) 4.8.4


Prerequisite
============

See my previous reports about Bayeux/trunk.

You may have installed (system) Bayeux/trunk alone for other activities.
Anyway Falaise/trunk comes with its own Bayeux.
This Falaise setup is built using explicitely a system Bayeux setup.


Installation of Falaise (trunk)
===============================

Set the software base directory where there is enough storage capacity to host
Falaise (> 1 GB). I choose here to use a simple environment variable ``SW_WORK_DIR``: ::

  $ export SW_WORK_DIR=/data/sw

You should adapt this base directory to your own system, for example: ::

  $ export SW_WORK_DIR=${HOME}/Software

Then create a few working directories: ::

  $ mkdir -p ${SW_WORK_DIR}
  $ mkdir ${SW_WORK_DIR}/Falaise          # This one is the base working directory for Falaise
  $ mkdir ${SW_WORK_DIR}/Falaise/Source   # This one will host the source code
  $ mkdir ${SW_WORK_DIR}/Falaise/Binary   # This one will host the build and installation directories

Download Falaise/trunk source files: ::

  $ cd ${SW_WORK_DIR}/Falaise/Source
  $ svn co https://nemo.lpc-caen.in2p3.fr/svn/Falaise/trunk Falaise-trunk
  $ cd Falaise-trunk

Configure Falaise:

  1. Make sure Bayeux is setup on your system. If you follow the Bayeux installation report
     available from the Bayeux wiki page (https://nemo.lpc-caen.in2p3.fr/wiki/Software/Bayeux)
     you just have to invoke: ::

      $ do_bayeux_dev_setup

  2. Create a build directory and cd in it: ::

      $ mkdir -p ${SW_WORK_DIR}/Falaise/Binary/Falaise-trunk/Build-Linux-x86_64
      $ cd ${SW_WORK_DIR}/Falaise/Binary/Falaise-trunk/Build-Linux-x86_64

  3. Invoke CMake to configure Falaise: ::

      $ cmake \
       -DCMAKE_BUILD_TYPE:STRING=Release \
       -DCMAKE_INSTALL_PREFIX:PATH=${SW_WORK_DIR}/Falaise/Binary/Falaise-trunk/Install-Linux-x86_64 \
       -DCMAKE_FIND_ROOT_PATH:PATH="$(bxquery --prefix);${SW_WORK_DIR}/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64" \
       -DFalaise_ENABLE_TESTING=ON \
       -DFalaise_BUILD_DOCS=ON \
       -DFalaise_USE_SYSTEM_BAYEUX=ON \
       ${SW_WORK_DIR}/Falaise/Source/Falaise-trunk

Build (using 4 processors to go faster): ::

  $ make -j4
  ...

Quick check after build
=========================

.. code:: sh

    $ ./BuildProducts/bin/flsimulate \
       --number 1  \
       --experiment "tracker_commissioning" \
       --event-generator "muon.cosmic.sea_level.toy" \
       --vertex-generator "experimental_hall_roof" \
       --output-file "snemo_tc_muons_roof.xml"
    $ ./BuildProducts/bin/flreconstruct \
       --input-file "snemo_tc_muons_roof.xml"

    $ ./BuildProducts/bin/flsimulate \
       --number 1  \
       --experiment "demonstrator" \
       --event-generator "Se82.0nubb" \
       --vertex-generator "source_strips_bulk" \
       --output-file "snemo_demonstrator_0nubb_source.xml"
    $ ./BuildProducts/bin/flreconstruct \
       --input-file "snemo_demonstrator_0nubb_source.xml"

Installation
====================

Simple run: ::

  $ make -j4 install


Setup your environment for Falaise
==================================

I prefer here to explicitely *load/setup* the Falaise environment from my Bash shell
with a dedicated function defined in my ``~/.bashrc`` startup file: ::

  # The base directory of all the software (convenient path variable):
  export SW_WORK_DIR=/data/sw

  # The Falaise/trunk setup function:
  function do_falaise_trunk_setup()
  {
    do_bayeux_dev_setup # Automatically load the Bayeux dependency
    if [ -n "${FALAISE_INSTALL_DIR}" ]; then
        echo "ERROR: Falaise/trunk is already setup !" >&2
        return 1
    fi
    export FALAISE_INSTALL_DIR=${SW_WORK_DIR}/Falaise/Binary/Falaise-trunk/Install-Linux-x86_64
    export PATH=${FALAISE_INSTALL_DIR}/bin:${PATH}
    echo "NOTICE: Falaise/trunk is now setup !" >&2
    return;
  }
  export -f do_falaise_trunk_setup

  # Special alias:
  alias do_falaise_dev_setup="do_falaise_trunk_setup"

When I want to use pieces of software from Falaise, I run::

  $ do_falaise_dev_setup
