#!/usr/bin/env bash
# Author: F.Mauger 
# Date: 2022-11-07
# Description: A Bash script to build and install development version of Falaise on Ubuntu (20/22.04).

origPwd="$(pwd)"
rebuild=false
onlyConfigure=false
debug=false
falaiseSourceDir="${origPwd}"
falaiseVersion="develop"
snrsMinVersion="1.1.0"
bayeuxMinVersion="3.5.3"
installBaseDir="$(pwd)/_install.d"
buildBaseDir="$(pwd)/_build.d"
installDir="${installBaseDir}"
buildDir="${buildBaseDir}"
withTests=false
falaiseSuffix=""

function my_exit()
{
    local _errorCode=$1
    if [ "x${_errorCode}" = "x" ]; then
	_errorCode=0
    fi
    shift 1
    local _errorMsg="$@"
    if [ ${_errorCode} -ne 0 -a -n "${_errorMsg}" ]; then
	echo >&2 "[error] ${_errorMsg}"
    fi
    cd ${origPwd}
    exit ${_errorCode}
}

function my_usage()
{
    cat<<EOF

Options:

   --help                 : print help
   --debug                : activate debug mode
   --only-configure       : perform configuration stage only
   --rebuild              : rebuild Falaise froms scratch (discard previous build)
   --with-tests           : build and run Falaise's tests
   --falaise-version label : force an arbitrary version tag (default='develop')
   --falaise-suffix label  : use a suffix for the build/install dirs (default='')

EOF
    return
}


function cl_parse()
{
    while [ -n "$1" ]; do
	local arg="$1"
	if [ "${arg}" = "--help" ]; then
	    my_usage
	    my_exit 0
	elif [ "${arg}" = "--debug" ]; then
	    debug=true
	elif [ "${arg}" = "--with-tests" ]; then
	    withTests=true
	elif [ "${arg}" = "--rebuild" ]; then
	    rebuild=true
	elif [ "${arg}" = "--only-configure" ]; then
	    onlyConfigure=true
	elif [ "${arg}" = "--develop" ]; then
	    falaiseVersion="develop"
	elif [ "${arg}" = "--falaise-suffix" -o "${arg}" = "-s" ]; then
	    shift 1
	    falaiseSuffix="$1"
	fi
	shift 1
    done
    return 0
}

echo >&2 "[info] Falaise source dir : '${falaiseSourceDir}'"

cl_parse $@
if [ $? -ne 0 ]; then
    my_exit 1 "[error] Command line parsing failed!"
fi

# Check Linux distribution:
distribId=$(cat /etc/lsb-release | grep DISTRIB_ID | cut -d= -f2)
if [ "${distribId}" != "Ubuntu" ]; then
    my_exit 1 "Not an Ubuntu Linux!"
fi
distribRelease=$(cat /etc/lsb-release | grep DISTRIB_RELEASE | cut -d= -f2)
if [ "${distribRelease}" != "18.04" -a "${distribRelease}" != "20.04" -a "${distribRelease}" != "22.04" ]; then
    my_exit 1 "[error] Not an Ubuntu Linux version 18.04, 20.04 or 22.04! Abort!"
else
    echo >&2 "[info] Found Ubuntu Linux ${distribRelease}"
fi

# Check SNRS:
which snrs-config > /dev/null
if [ $? -ne 0 ]; then
    my_exit 1 "SNRS package is not installed and setup! Abort!"
fi
export snrsInstallDir="$(snrs-config --prefix)"
export snrsVersion="$(snrs-config --version)"
echo >&2 "[info] SNRS ${snrsVersion} found at: '${snrsInstallDir}'"
if [ "x${snrsVersion}" \< "x${snrsMinVersion}" ]; then
    my_exit 1 "SNRS version ${snrsVersion} is not supported!"
fi

# Check Bayeux:
which bxquery > /dev/null
if [ $? -ne 0 ]; then
    my_exit 1 "Bayeux package is not installed and setup! Abort!"
fi
export bayeuxInstallDir="$(bxquery --prefix)"
export bayeuxVersion="$(bxquery --version)"
echo >&2 "[info] Bayeux ${bayeuxVersion} found at: '${bayeuxInstallDir}'"
if [ "x${bayeuxVersion}" \< "x${bayeuxMinVersion}" ]; then
    my_exit 1 "Bayeux version ${bayeuxVersion} is not supported!"
fi

############################################

if [ "x${falaiseVersion}" = "x" ]; then
    falaiseVersion=develop
    echo >&2 "[info] Forcing Falaise version suffix : '${falaiseVersion}'..."
fi

if [ "x${falaiseSuffix}" != "x" ]; then
    installDir=${installBaseDir}/${falaiseVersion}${falaiseSuffix}
    buildDir=${buildBaseDir}/${falaiseVersion}${falaiseSuffix}
else
    installDir=${installBaseDir}/${falaiseVersion}
    buildDir=${buildBaseDir}/${falaiseVersion}
fi

# Check:
if [ ! -d ${falaiseSourceDir} ]; then
    my_exit 1 "Falaise source directory '${falaiseSourceDir}' does not exist! Abort!"
fi

falaiseSetupDir="${falaiseSourceDir}/_setup.d"
if [ ! -d ${falaiseSetupDir} ]; then
    mkdir -p ${falaiseSetupDir}
    if [ $? -ne 0 ]; then
	my_exit 1 "Falaise setup directory '${falaiseSetupDir}' was not created! Abort!"	
    fi
fi
falaiseSetupScript="${falaiseSetupDir}/falaise_${falaiseVersion}_run_env.bash"

echo >&2 "[info] distribId=${distribId}"
echo >&2 "[info] distribRelease=${distribRelease}"
echo >&2 "[info] falaiseSourceDir=${falaiseSourceDir}"
echo >&2 "[info] falaiseVersion=${falaiseVersion}"
echo >&2 "[info] falaiseSuffix=${falaiseSuffix}"
echo >&2 "[info] buildDir=${buildDir}"
echo >&2 "[info] installDir=${installDir}"
echo >&2 "[info] bayeuxVersion=${bayeuxVersion}"
echo >&2 "[info] snrsVersion=${snrsVersion}"

# my_exit 0

if [ -d ${installDir} ]; then
    rm -fr ${installDir}
fi

if [ ! -d ${buildDir} ]; then
    mkdir -p ${buildDir}
else
    if [ ${rebuild} = true ]; then
	echo >&2 "[info] Rebuilding Falaise..."
	rm -fr ${buildDir}
	mkdir -p ${buildDir}
    fi
fi

specialOptions=
# if [ ${useXxx} == true ]; then
#     specialOptions="-DCMAKE_FIND_ROOT_PATH:PATH=${xxxPath}"
# fi

bayeuxDir=$(bxquery --cmakedir)
echo >&2 "[info] bayeuxDir = '${bayeuxDir}'"
echo >&2 "[info] specialOptions = '${specialOptions}'"
echo >&2 "[info] falaiseSourceDir = '${falaiseSourceDir}'"

# my_exit 1 "Stop"

cd ${buildDir}
echo >&2 ""
echo >&2 "[info] Configuring..."
cmake \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:PATH="${installDir}" \
    ${specialOptions} \
    -DBayeux_DIR:PATH="${bayeuxDir}" \
    -DFALAISE_COMPILER_ERROR_ON_WARNING=ON \
    -DFALAISE_CXX_STANDARD="11" \
    -DFALAISE_ENABLE_TESTING=ON \
    -DFALAISE_WITH_DOCS=ON \
    -GNinja \
    ${falaiseSourceDir}
if [ $? -ne 0 ]; then
    my_exit 1 "Falaise ${falaiseVersion} configuration failed!"
fi

if [ ${onlyConfigure} = false ]; then

    echo >&2 ""
    echo >&2 "[info] Building..."
    ninja -j4
    if [ $? -ne 0 ]; then
	my_exit 1 "Falaise ${falaiseVersion} build failed!"
    fi

    if [ ${withTests} = true ]; then
	echo >&2 ""
	echo >&2 "[info] Testing..."
	ninja test
	if [ $? -ne 0 ]; then
	    my_exit 1 "Falaise ${falaiseVersion} testing failed!"
	fi
    fi

    echo >&2 ""
    echo >&2 "[info] Installing..."
    ninja install
    if [ $? -ne 0 ]; then
	my_exit 1 "[error] Falaise ${falaiseVersion} installation failed!"
    fi

    echo >&2 ""
    echo >&2 "[info] Setup..."
    cat > ${falaiseSetupScript} <<EOF
# -*- mode: shell; -*-	
function falaise_${falaiseVersion}_setup()
{
    if [ -n "\${FALAISE_VERSION}" ]; then
        echo >&2 "[warning] Falaise version '${FALAISE_VERSION}' is already setup"
    else
        export PATH="${installDir}/bin:\${PATH}"
        export FALAISE_VERSION="${falaiseVersion}"
        echo >&2 "[info] Falaise version '${falaiseVersion}' is now setup"
    fi  
    return 0	
}
export -f falaise_${falaiseVersion}_setup
EOF
    cat<<EOF

The setup script '${falaiseSetupScript}'  has been generated.  

You may copy it in your '~/.bash.d/' directory and source it from your
'~/.bashrc' startup script with the following instructions:

    if [ -f "~/.bash.d/$(basename ${falaiseSetupScript})" ]; then
        source ~/.bash.d/$(basename ${falaiseSetupScript})
    fi   

The  bash  function  'falaise_${falaiseVersion}_setup'  will  thus  be
available to  activate Falaise in  your Bash shell, provided  you have
also activated Bayeux and SNRS before using proper commands.

EOF

    falaiseInstallLog="${falaiseSetupDir}/install.log"
    if [ -f ${falaiseInstallLog} ]; then
	rm -f  ${falaiseInstallLog}
    fi
    touch ${falaiseInstallLog}
    echo "falaise-version=${falaiseVersion}" >> ${falaiseInstallLog}
    echo "build-dir=${buildDir}" >> ${falaiseInstallLog}
    echo "install-dir=${installDir}" >> ${falaiseInstallLog}
    echo "setup-dir=${falaiseSetupDir}" >> ${falaiseInstallLog}
fi

my_exit 0

# end
