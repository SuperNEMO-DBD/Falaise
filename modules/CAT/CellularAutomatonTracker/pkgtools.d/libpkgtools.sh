# -*- mode: shell-script; -*-
#
# libpkgtools.sh
#
#   A bash library used by the pkgtools utilities.
#
# Author: F.Mauger
# Date: 2009-11-20
# Last update: 2010-09-17
#

# if [ "x${PKGTOOLS_FORCE_RELOAD}" = "x1" -o "x${__init_lib_pkgtools}" = "x" ]; then
#     export __init_lib_pkgtools=1
# else
#     echo "WARNING: pkgtools library has already been loaded !" >&2
# fi

if [ "x" = "x" ]; then
    __pkgtools__os=$(uname -s)
    __pkgtools__arch=$(uname -m | sed 's/ //g')
    __pkgtools__distribution_family=
    __pkgtools__distribution=
    __pkgtools__distribution_version=


    __pkgtools__msg_use_color=1 # default: true
    __pkgtools__msg_use_date=0  # default: false
    __pkgtools__msg_split_lines=0 # default: false
    __pkgtools__msg_quiet=0 # default: false
    __pkgtools__msg_verbose=0 # default: false
    __pkgtools__msg_warning=1 # default: true
    __pkgtools__msg_debug=0 # default: false
    __pkgtools__msg_devel=0 # default: false

    __pkgtools__msg_funcname_deps=
    __pkgtools__msg_funcname=

#
# UI utilities:
#

    __pkgtools__ui_interactive=1 # default: true
    __pkgtools__ui_gui=0 # default: false
    __pkgtools__ui_gui_type="dialog"
    __pkgtools__ui_INVALID_YESNO=-1
    __pkgtools__ui_YES=1
    __pkgtools__ui_NO=0
    __pkgtools__ui_user_yesno=${__pkgtools__ui_INVALID_YESNO}
    __pkgtools__ui_EMPTY_STRING=""
    __pkgtools__ui_user_string=${__pkgtools__ui_EMPTY_STRING}
    __pkgtools__ui_INVALID_INTEGER=-2147483648
    __pkgtools__ui_user_integer=${__pkgtools__ui_INVALID_INTEGER}

    __pkgtools__ui_dialog_bin=dialog
    __pkgtools__temp_file=

    export __PKGTOOLS__VERDESC_OPEN="["
    export __PKGTOOLS__VERDESC_CLOSE="]"
    export __PKGTOOLS__SUBLIST_OPEN="("
    export __PKGTOOLS__SUBLIST_SEP=","
    export __PKGTOOLS__SUBLIST_CLOSE=")"

#
# Private utilities:
#

    function pkgtools__delete_temp_file ()
    {
	if [ "x${__pkgtools__temp_file}" != "x" ]; then
	    if [ -f ${__pkgtools__temp_file} ]; then
		rm -f ${__pkgtools__temp_file}
		__pkgtools__temp_file=""
	    fi
	fi
	return 0
    }


    function pkgtools__create_temp_file ()
    {
	pkgtools__delete_temp_file
	tmp_dir=/tmp/${USER}
	if [ ! -d ${tmp_dir} ]; then
	    mkdir -p ${tmp_dir}
	fi
	__pkgtools__temp_file=$(mktemp -p ${tmp_dir} __pkgtools_XXXXXX)
       	return 0
    }


    function pkgtools__get_temp_file ()
    {
	echo ${__pkgtools__temp_file}
	return 0
    }


    function __pkgtools__dump ()
    {
	cat 1>&2 <<EOF

libpkgtools dump:

  Current function hierarchy = '${__pkgtools__msg_funcname_deps}'
  Current function name      = '${__pkgtools__msg_funcname}'

EOF
	return 0
    }


    function pkgtools__is_float ()
    {
	if [ "x$1" = "x" ]; then
	    return 1
	fi
	token=$(pkgtools__to_lower $1)
	test_float_py_tmp="/tmp/libpkgtools__is_float.py"
	if [ ! -f ${test_float_py_tmp} ]; then
	    cat > ${test_float_py_tmp} <<EOF
import sys
try:
  a=float(sys.argv[1])
except:
  sys.exit (1)
sys.exit (0)
EOF
	    chmod a+rx ${test_float_py_tmp}
	    ###ls -l ${test_float_py_tmp}
	fi
	python ${test_float_py_tmp} ${token} > /dev/null 2>&1
	if [ $? -eq 0 ]; then
	    return 1
	fi
	return 0
    }


    function pkgtools__is_integer ()
    {
	if [ "x$1" = "x" ]; then
	    return 1 # false
	fi
	token=$(pkgtools__to_lower $1)
	test_int_py_tmp="/tmp/libpkgtools__is_integer.py"
	if [ ! -f ${test_int_py_tmp} ]; then
	    cat > ${test_int_py_tmp} <<EOF
import sys
try:
  a=int(sys.argv[1])
except:
  sys.exit (1)
sys.exit (0)
EOF
	    chmod a+rx ${test_int_py_tmp}
	    ###ls -l ${test_int_py_tmp}
	fi
	python ${test_int_py_tmp} ${token} > /dev/null 2>&1
	if [ $? -eq 0 ]; then
	    return 1 # false
	fi
	return 0 # true
    }


#
# UI utilities:
#

    function pkgtools__ui_interactive ()
    {
	__pkgtools__ui_interactive=1
	return 0
    }

    function pkgtools__ui_batch ()
    {
	__pkgtools__ui_interactive=0
	return 0
    }

    function pkgtools__ui_is_interactive ()
    {
	if [ "x${PKGTOOLS_BATCH}" != "x" ]; then
	    if  [ "x${PKGTOOLS_BATCH}" != "x0" ]; then
		return 1 # false;
	    fi
	fi

	if [ ${__pkgtools__ui_interactive} -eq 1 ]; then
	    return 0 # true
	fi
	return 1 # false
    }

    function pkgtools__ui_is_batch ()
    {
	pkgtools__ui_is_interactive
	if [ $? -eq 0 ]; then
	    return 1
	fi
	return 0 # true
    }

    function pkgtools__ui_is_gui ()
    {
	if [ ${__pkgtools__ui_gui} -eq 1 ]; then
	    return 0 # true
	fi
	return 1 # false
    }


    function pkgtools__ui_using_gui ()
    {
	__pkgtools__at_function_enter pkgtools__ui_using_gui
	pkgtools__ui_is_batch
	if [ $? -eq 0 ]; then
	    pkgtools__msg_warning "Forcing interactive mode !"
	    pkgtools__ui_interactive
	fi
	__pkgtools__ui_gui=1
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__ui_not_using_gui ()
    {
	__pkgtools__ui_gui=0
	return 0
    }

    # maybe unuseful
    function pkgtools__ui_set_user_integer ()
    {
	__pkgtools__at_function_enter pkgtools__ui_set_user_integer
	pkgtools__is_integer "$1"
	if [ $? -ne 0 ]; then
	    __pkgtools__ui_user_integer=${__pkgtools__ui_INVALID_INTEGER}
	    __pkgtools__at_function_exit
	    return 1
	fi
	__pkgtools__ui_user_integer=$1
	__pkgtools__at_function_exit
	return 0
    }


    # maybe unuseful
    function pkgtools__ui_reset_user_integer ()
    {
	__pkgtools__ui_user_integer=${__pkgtools__ui_INVALID_INTEGER}
	return 0
    }


    # maybe unuseful
    function pkgtools__ui_get_user_integer ()
    {
	__pkgtools__ui_user_integer=${__pkgtools__ui_INVALID_INTEGER}
	echo ${__pkgtools__ui_user_integer}
	return 0
    }

    function pkgtools__ui_set_user_yesno ()
    {
	__pkgtools__at_function_enter pkgtools__ui_set_user_yesno
	local yesno=$1
	if [ -z "${yesno}" ]; then
	    yesno=${__pkgtools__ui_INVALID_YESNO}
	elif [ ${yesno} -eq ${__pkgtools__ui_YES} ]; then
	    yesno=${__pkgtools__ui_YES}
	elif [ ${yesno} -eq ${__pkgtools__ui_NO} ]; then
	    yesno=${__pkgtools__ui_NO}
	else
	    pkgtools__msg_error "Invalid Yes/No value: '${yesno}'! "
	    __pkgtools__at_function_exit
	    return 1
	fi
	__pkgtools__ui_user_yesno=${yesno}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__ui_reset_user_yesno ()
    {
	__pkgtools__ui_user_yesno=${__pkgtools__ui_INVALID_YESNO}
	return 0
    }


    function pkgtools__ui_get_user_yesno ()
    {
	echo ${__pkgtools__ui_user_yesno}
	return 0
    }


    function pkgtools__ui_user_yesno_is_yes ()
    {
	if [ "x${__pkgtools__ui_user_yesno}" = "x${__pkgtools__ui_YES}" ]; then
	    return 0
	fi
	return 1
    }


    function pkgtools__ui_user_yesno_is_no ()
    {
	if [ "x${__pkgtools__ui_user_yesno}" = "x${__pkgtools__ui_NO}" ]; then
	    return 0
	fi
	return 1
    }


    function pkgtools__ui_ask_user_yesno ()
    {
	__pkgtools__at_function_enter pkgtools__ui_ask_user_yesno
	pkgtools__ui_reset_user_yesno
	local YES=${__pkgtools__ui_YES}
	local NO=${__pkgtools__ui_NO}

	local question="$1"
	if [ "x${question}" = "x" ]; then
	    question="Your choice"
	fi
	shift 1

	local default_choice=$(pkgtools__to_lower ${1:0:1})
	pkgtools__msg_devel "default_choice='${default_choice}'! "
	shift 1

	choices="Y|n"
	default_yesno=${YES}
	if [ "x${default_choice}" != "x" ]; then
	    if [ "x${default_choice:0:1}" == "xn" ]; then
		choices="y|N"
		local default_yesno=${NO}
	    elif [ "x${default_choice:0:1}" == "xy" ]; then
		choices="Y|n"
		local default_yesno=${YES}
	    else
		pkgtools__msg_error "Invalid default choice '${default_choice}'! "
		__pkgtools__at_function_exit
		return 1
	    fi
	fi
	pkgtools__msg_devel "choices='${choices}'! "

	pkgtools__ui_is_batch
	if [ $? -eq 0 ]; then
	    pkgtools__msg_warning "Using default answer '${default_yesno}' !"
	    pkgtools__ui_set_user_yesno ${default_yesno}
	    __pkgtools__at_function_exit
	    return 0
	fi

	if [ ${__pkgtools__ui_gui} -eq 0 ]; then
	    echo -n "${question} ? " 1>&2
	    echo -n "[${choices}] " 1>&2
	    local ANS
	    read ANS
	    pkgtools__msg_devel "ANS='${ANS}'! "
	    if [ "x${ANS}" == "x" ]; then
		yesno=${default_yesno}
	    else
		user_yesno=$(pkgtools__to_lower ${ANS:0:1})
		pkgtools__msg_devel "user_yesno='${user_yesno}'! "
		if [ "x${user_yesno}" == "xn" ]; then
		    yesno=${NO}
		elif [ "x${user_yesno}" == "xy" ]; then
		    yesno=${YES}
		else
		    pkgtools__msg_error "Invalid Yes/No answer '${user_yesno}'! "
		    __pkgtools__at_function_exit
		    return 1
		fi
	    fi
	else
	    if [ "x${__pkgtools__ui_gui_type}" = "xdialog" ]; then
		ftmp=$(mktemp -p. pkgtools.dialog.tmpXXXX)
		term_nl=$(stty size | cut -d' ' -f1)
		term_nc=$(stty size | cut -d' ' -f2)
		let max_nlines=term_nl-2
		let max_ncols=term_nc-4
		nl=$(echo -e "${question}" | wc -l)
		#echo "DEVEL ==== nl=${nl}" 1>&2
		let nlines=nl+4
		#echo "DEVEL ==== nlines=${nlines}" 1>&2
		if [ ${nlines} -gt ${max_nlines} ]; then
		    nlines=${max_nlines}
		fi
		if [ ${nlines} -lt 6 ]; then
		    nlines=6
		fi
		${__pkgtools__ui_dialog_bin} --title "pkgtools GUI" \
		    --yesno "${question} ?" ${nlines} ${max_ncols} 2> ${ftmp}
		yesno=$?
		rm -f ${ftmp} > /dev/null 2>&1
		if [ ${yesno} -lt 0 -o ${yesno} -gt 1 ]; then
		    pkgtools__msg_error "Invalid Yes/no answer !"
		    __pkgtools__at_function_exit
		    return 1
		fi
	    else
		pkgtools__msg_error "Unknow GUI user dialog '${__pkgtools__ui_gui_type}'!"
		__pkgtools__at_function_exit
		return 1
	    fi
	fi
	pkgtools__msg_devel "yesno='${yesno}'! "
	pkgtools__ui_set_user_yesno ${yesno}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__ui_set_user_string ()
    {
	__pkgtools__ui_user_string="$1"
	return 0
    }


    function pkgtools__ui_reset_user_string ()
    {
	__pkgtools__ui_user_string=""
	return 0
    }


    function pkgtools__ui_get_user_string ()
    {
	echo ${__pkgtools__ui_user_string}
	return 0
    }


    function pkgtools__ui_ask_user_string ()
    {
	__pkgtools__at_function_enter pkgtools__ui_ask_user_string
	pkgtools__ui_reset_user_string
	local title="$1"
	shift 1
	local default_string="$1"
	shift 1

	pkgtools__ui_is_batch
	if [ $? -eq 0 ]; then
	    pkgtools__msg_warning "Using default answer '${default_string}' !"
	    pkgtools__ui_set_user_string "${default_string}"
	    __pkgtools__at_function_exit
	    return 0
	fi

	local user_string=""
	if [ ${__pkgtools__ui_gui} -eq 0 ]; then
	    echo -n "${title} " 1>&2
	    if [ "x${default_string}" != "x" ]; then
		echo -n "[${default_string}] " 1>&2

	    fi
	    echo -n ": " 1>&2
	    local ANS
	    read ANS
	    if [ "x${ANS}" != "x" ]; then
		user_string="${ANS}"
	    else
		user_string="${default_string}"
	    fi
	else
	    if [ "x${__pkgtools__ui_gui_type}" = "xdialog" ]; then
		ftmp=$(mktemp -p. pkgtools.dialog.tmpXXXX)
		term_nl=$(stty size | cut -d' ' -f1)
		term_nc=$(stty size | cut -d' ' -f2)
		let max_nlines=term_nl-2
		let max_ncols=term_nc-4
		nl=$(echo -e "${title}" | wc -l)
		let nlines=nl+4
		if [ ${nlines} -gt ${max_nlines} ]; then
		    nlines=${max_nlines}
		fi
		${__pkgtools__ui_dialog_bin} --title "pkgtools GUI" \
		    --inputbox "${title} :" ${nlines} ${max_ncols} "${default_string}" 2> ${ftmp}
		user_string="$(cat ${ftmp})"
		rm -f ${ftmp}
	    else
		pkgtools__msg_error "Unknow GUI user dialog '${__pkgtools__ui_gui_type}'!"
		__pkgtools__at_function_exit
		return 1
	    fi
	fi
	pkgtools__ui_set_user_string "${user_string}"
	__pkgtools__at_function_exit
	return 0
    }


#
#
#

    function pkgtools__set_funcname ()
    {
	fname=$1
	###pkgtools__devel_msg "D set_funcname   ==> '${fname}'"
	if [ "x${fname}" != "x" ]; then
	    if [ "x${__pkgtools__msg_funcname_deps}" != "x" ]; then
		###pkgtools__devel_msg "set_funcname ==========> new level == '${fname}'"
		__pkgtools__msg_funcname_deps="${__pkgtools__msg_funcname_deps}@${fname}"
	    else
		###pkgtools__devel_msg "set_funcname ==========> first level == '${fname}'"
		__pkgtools__msg_funcname_deps=${fname}
	    fi
	    ###pkgtools__devel_msg "D                ==> __pkgtools__msg_funcname_deps == '${__pkgtools__msg_funcname_deps}'"
	fi
	__pkgtools__msg_funcname=${fname}
	return 0
    }


    function pkgtools__unset_funcname ()
    {
	fname=$(echo ${__pkgtools__msg_funcname_deps} | tr "@" "\n" | tail -1)
	###pkgtools__devel_msg "D unset_funcname ==> '${fname}'"
 	if [ "x${__pkgtools__msg_funcname_deps}" != "x" ]; then
	    nfuncs=$(echo ${__pkgtools__msg_funcname_deps} | tr '@' '\n' | wc -l)
 	    let ncut=nfuncs-1
 	    tmp=$(echo -n ${__pkgtools__msg_funcname_deps} | tr "@" "\n" | head -${ncut} | tr '\n' '@' | sed 's/@$//g')
	    if [ ${ncut} -eq 0 ]; then
		tmp=
	    fi
	    __pkgtools__msg_funcname_deps=${tmp}
 	fi
	###pkgtools__devel_msg "D                ==> '${__pkgtools__msg_funcname_deps}'"
	previous_fname=$(echo ${__pkgtools__msg_funcname_deps} | tr "@" "\n" | tail -1)
	__pkgtools__msg_funcname=${previous_fname}
	return 0
    }


    function pkgtools__no_funcname ()
    {
	__pkgtools__msg_funcname=
	__pkgtools__msg_funcname_deps=
	return 0
    }


    function __pkgtools__at_function_enter ()
    {
	pkgtools__set_funcname $1
	pkgtools__msg_devel "Entering..."
	return 0
    }


    function __pkgtools__at_function_exit ()
    {
	pkgtools__msg_devel "Exiting."
	pkgtools__unset_funcname
	return 0
    }


#
# General utilities:
#

    function pkgtools__to_lower ()
    {
	echo $1 | tr "[A-Z]" "[a-z]"
	return 0
    }


    function pkgtools__to_upper ()
    {
	echo $1 | tr "[a-z]" "[A-Z]"
	return 0
    }


    function pkgtools__get_dirname ()
    {
	path=$1
	shift 1
	if [ "x${path}" = "x" ]; then
	    return 1
	fi
	pymac=$(mktemp -p . pkgtools_XXXXX)
	cat > ${pymac} <<EOF
import os.path
print os.path.dirname ("${path}")
EOF
	python ${pymac}
	rm -f ${pymac}
	return 0
    }


    function pkgtools__get_basename ()
    {
	path=$1
	shift 1
	if [ "x${path}" = "x" ]; then
	    return 1
	fi
	basename ${path}
	return 0
    }


#
# Colorized message utilities:
#

    # Notes:
    # echo -e "\e[1;4;31;42m"
    # echo -e "\e[m"

    function pkgtools__msg_color_normal ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;39m" 1>&2
        fi
	return 0
    }


    function pkgtools__msg_color_bold ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[1;1m" 1>&2
        fi
	return 0
    }


    function pkgtools__msg_color_underline ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;38m" 1>&2
        fi
	return 0
    }


    function pkgtools__msg_color_red ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;31m" 1>&2
        fi
	return 0
    }


    function pkgtools__msg_color_bright_red ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;31m" 1>&2
        fi
	return 0
    }

    function pkgtools__msg_color_green ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;32m" 1>&2
        fi
	return 0
    }

    function pkgtools__msg_color_brown ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;33m" 1>&2
        fi
	return 0
    }

    function pkgtools__msg_color_blue ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;34m" 1>&2
        fi
	return 0
    }


    function pkgtools__msg_color_violet ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;35m" 1>&2
        fi
	return 0
    }

    function pkgtools__msg_color_grey ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[0;37m" 1>&2
        fi
	return 0
    }

    function pkgtools__msg_color_white ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[1;37m" 1>&2
        fi
	return 0
    }

    function pkgtools__msg_color_black ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[1;39m" 1>&2
        fi
	return 0
    }

    function pkgtools__msg_color_reverse ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[1;7m" 1>&2
        fi
	return 0
    }

    function pkgtools__msg_color_no_reverse ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[1;27m" 1>&2
        fi
	return 0
    }


    function pkgtools__msg_color_blink ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "" 1>&2
        fi
	return 0
    }


    function pkgtools__msg_color_no_blink ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "" 1>&2
        fi
	return 0
    }


    function pkgtools__msg_color__cancel ()
    {
        if [ ${__pkgtools__msg_use_color} -eq 1 ]; then
            echo -en "\\033[1;m" 1>&2
        fi
	return 0
    }


#     function pkgtools__msg_begin_highlight ()
#     {
#         pkgtools__msg_color_bright_green
#     }

#     function pkgtools__msg_end_highlight ()
#     {
#         pkgtools__msg_color_normal
#     }

    function pkgtools__highlight ()
    {
        pkgtools__msg_color_bright_red
	echo -en "$@" 1>&2
        pkgtools__msg_color_normal
	return 0
    }


#     function pkgtools__begin_error ()
#     {
#         pkgtools__msg_color_red
#     }


#     function pkgtools__end_error ()
#     {
#         pkgtools__msg_color_normal
#     }

#
# System check utilities:
#

    function pkgtools__get_os ()
    {
	echo "${__pkgtools__os}"
	return 0
    }


    function pkgtools__get_arch ()
    {
	echo "${__pkgtools__arch}"
	return 0
    }

    function pkgtools__knows_number_of_processors ()
    {
	if [ -f /proc/cpuinfo ]; then
	    nbprocs=$(cat /proc/cpuinfo | grep ^processor | wc -l )
	    if [ "x${nbprocs}" != "x" ]; then
		return 0
	    fi
	fi
	return 1
    }

    function pkgtools__get_number_of_processors ()
    {
	if [ -f /proc/cpuinfo ]; then
	    nbprocs=$(cat /proc/cpuinfo | grep ^processor | wc -l )
	    echo "${nbprocs}"
	    return 0
	fi
	echo ""
	return 1
    }

    function pkgtools__get_sys ()
    {
	echo "$(pkgtools__get_os)-$(pkgtools__get_arch)"
	return 0
    }


    function pkgtools__is_linux ()
    {
	test "$(pkgtools__get_os)" = "Linux" && return 0
	return 1
    }


    function pkgtools__is_darwin ()
    {
	test "$(pkgtools__get_os)" = "Darwin" && return 0
	return 1
    }


    function pkgtools__get_so_extension ()
    {
	pkgtools__is_darwin
	if [ $? -eq 0 ]; then
	    echo ".dylib"
	else
	    echo ".so"
	fi
	return 0
    }

    function pkgtools__get_usr_lib ()
    {
	if [ "X$(uname -m)" = "Xx86_64" ]; then
	    echo "/usr/lib64"
	else
	    echo "/usr/lib"
	fi
	return 0
    }

    function pkgtools__get_distribution_family ()
    {
	if [ "x${__pkgtools__distribution_family}" = "x" ]; then
	    pkgtools__is_linux
	    if [ $? -eq 0 ]; then
		if [ -f /etc/redhat-release ]; then
		    __pkgtools__distribution_family="RedHat"
		elif [ -f /etc/fedora-release ]; then
		    __pkgtools__distribution_family="RedHat"
		elif [ -f /etc/lsb-release ]; then
		    __pkgtools__distribution_family="Debian"
		else
		    __pkgtools__distribution_family="<unresolved>"
		fi
	    else
		pkgtools__is_darwin
		if [ $? -eq 0 ]; then
		    __pkgtools__distribution_family="Darwin"
		else
		    __pkgtools__distribution_family="<unresolved>"
		fi
	    fi
	fi
	echo "${__pkgtools__distribution_family}"
	return 0
    }


    function pkgtools__is_redhat_like ()
    {
	test "$(pkgtools__get_distribution_family)" = "RedHat" && return 0
	return 1
    }


    function pkgtools__is_debian_like ()
    {
	test "$(pkgtools__get_distribution_family)" = "Debian" && return 0
	return 1
    }


    function pkgtools__is_darwin_like ()
    {
	test "$(pkgtools__get_distribution_family)" = "Darwin" && return 0
	return 1
    }


    function pkgtools__get_distribution ()
    {
	if [ "x${__pkgtools__distribution}" = "x" ]; then
	    pkgtools__is_linux
	    if [ $? -eq 0 ]; then
		which lsb_release > /dev/null 2>&1
		if [ $? -eq 0 ]; then
		    __pkgtools__distribution=$(lsb_release --id | cut -d: -f2 | tr -d "[[:space:]]")
		else
		    pkgtools__is_redhat_like
		    if [ $? -eq 0 ]; then
			cat /etc/redhat-release | tr ' ' '\n' | grep "SL" > /dev/null 2>&1
			if [ $? -eq 0 ]; then
			    __pkgtools__distribution="SL"
			else
			    cat /etc/redhat-release | tr ' ' '\n' | grep "Fedora" > /dev/null 2>&1
			    if [ $? -eq 0 ]; then
				__pkgtools__distribution="Fedora"
			    else
				__pkgtools__distribution="<unresolved>"
			    fi
			fi
		    fi
		fi
	    else
		pkgtools__is_darwin
		if [ $? -eq 0 ]; then
		    __pkgtools__distribution="Darwin"
		else
		    __pkgtools__distribution="<unresolved>"
		fi
	    fi
	fi
	echo "${__pkgtools__distribution}"
	return 0
    }


    function pkgtools__is_fedora ()
    {
	test "$(pkgtools__get_distribution)" = "Fedora" && return 0
	return 1
    }


    function pkgtools__is_sl ()
    {
	test "$(pkgtools__get_distribution)" = "SL" && return 0
	return 1
    }


    function pkgtools__is_ubuntu ()
    {
	test "$(pkgtools__get_distribution)" = "Ubuntu" && return 0
	return 1
    }


    function pkgtools__get_distribution_version ()
    {
	if [ "x${__pkgtools__distribution_version}" = "x" ]; then
	    pkgtools__is_linux
	    if [ $? -eq 0 ]; then
		which lsb_release > /dev/null 2>&1
		if [ $? -eq 0 ]; then
		    __pkgtools__distribution_version=$(lsb_release --release | cut -d: -f2 | tr -d [[:space:]])
		else
		    pkgtools__is_redhat_like
		    if [ $? -eq 0 ]; then
			tmp=$(cat /etc/redhat-release | tr [[:space:]] ' ' | tr -s ' ' | tr ' ' '\n' | grep [[:digit:]].[[:digit:]]*)
			if [ "x${tmp}" != "x" ]; then
			    __pkgtools__distribution_version=${tmp}
			else
			    __pkgtools__distribution_version="<unresolved>"
			fi
		    fi
		fi
	    else
		pkgtools__is_darwin
		if [ $? -eq 0 ]; then
		    __pkgtools__distribution_version=$(uname -r)
		else
		    __pkgtools__distribution_version="<unresolved>"
		fi
	    fi
	fi
	echo "${__pkgtools__distribution_version}"
	return 0
    }

#
# Message log utilities:
#

    function pkgtools__msg_using_warning ()
    {
	pkgtools__msg_using_warning
	__pkgtools__msg_warning=1
 	return 0
    }

    function pkgtools__msg_not_using_warning ()
    {
	__pkgtools__msg_warning=0
	return 0
    }

    function pkgtools__msg_using_verbose ()
    {
	__pkgtools__msg_verbose=1
 	return 0
    }

    function pkgtools__msg_not_using_verbose ()
    {
	__pkgtools__msg_verbose=0
	return 0
    }

    function pkgtools__msg_using_quiet ()
    {
	pkgtools__msg_using_verbose
	__pkgtools__msg_quiet=1
 	return 0
    }

    function pkgtools__msg_not_using_quiet ()
    {
	__pkgtools__msg_quiet=0
	return 0
    }

    function pkgtools__msg_is_quiet ()
    {
	local quiet_ret=1 # false
	if [ "x${PKGTOOLS_MSG_QUIET}" != "x" ]; then
	    if [ "x${PKGTOOLS_MSG_QUIET}" != "x0" ]; then
		quiet_ret=0 # false
	    fi
	else
	    if [ ${__pkgtools__msg_quiet} -eq 1  ]; then
		quiet_ret=0 # true
	    fi
	fi
	return ${quiet_ret}
    }

    function pkgtools__msg_using_debug ()
    {
	__pkgtools__msg_debug=1
	return 0
    }

    function pkgtools__msg_not_using_debug ()
    {
	__pkgtools__msg_debug=0
	return 0
    }

    function pkgtools__msg_using_devel ()
    {
	__pkgtools__msg_devel=1
	return 0
    }

    function pkgtools__msg_not_using_devel ()
    {
	__pkgtools__msg_devel=0
	return 0
    }

    function pkgtools__msg_using_date ()
    {
	__pkgtools__msg_use_date=1
	return 0
    }

    function pkgtools__msg_not_using_date ()
    {
	__pkgtools__msg_use_date=0
	return 0
    }

    function pkgtools__msg_using_color ()
    {
	__pkgtools__msg_use_color=1
	return 0
    }

    function pkgtools__msg_not_using_color ()
    {
	__pkgtools__msg_use_color=0
	pkgtools__msg_color_normal
	return 0
    }

    function __pkgtools__base_msg_prefix ()
    {
	local log_file=
	if [ "x${PKGTOOLS_LOG_FILE}" != "x" ]; then
	    log_file=${PKGTOOLS_LOG_FILE}
	else
	    log_file=/dev/null
	fi
	local msg_prefix="$1"
	(
	    (
		echo -n "${msg_prefix}: "
	    ) | tee -a ${log_file}
	) 1>&2
	return 0
    }

    function __pkgtools__base_msg ()
    {
	local log_file=
	if [ "x${PKGTOOLS_LOG_FILE}" != "x" ]; then
	    log_file=${PKGTOOLS_LOG_FILE}
	else
	    log_file=/dev/null
	fi
	(
	    (
		if [ ${__pkgtools__msg_use_date} -eq 1 ]; then
		    date +%F-%T | tr -d '\n'
		    echo -n " @ "
		fi
		if [ "x${appname}" != "x" ]; then
		    echo -n "${appname}: "
		fi
		if [ "x${__pkgtools__msg_funcname}" != "x" ]; then
		    echo -n "${__pkgtools__msg_funcname}: "
		fi
		if [ ${__pkgtools__msg_split_lines} -eq 1 ]; then
		    echo ""
		    echo -n "  "
		fi
		echo "$@"
	    ) | tee -a ${log_file}
	) 1>&2
	return 0;
    }

    function __pkgtools__base_msg_2 ()
    {
	if [ ${__pkgtools__msg_use_date} -eq 1 ]; then
	    date +%F-%T | tr -d '\n' 1>&2
	    echo -n " -- " 1>&2
	fi
	if [ "x${appname}" != "x" ]; then
	    echo -n "${appname}: " 1>&2
	fi
	if [ "x${__pkgtools__msg_funcname}" != "x" ]; then
	    echo -n "${__pkgtools__msg_funcname}: " 1>&2
	fi
	echo "" 1>&2
	echo -n "  " 1>&2
	echo "$@" 1>&2
	echo "" 1>&2
	return 0
    }


    function pkgtools__msg_err ()
    {
	pkgtools__msg_color_red
	__pkgtools__base_msg_prefix "ERROR"
	__pkgtools__base_msg $@
	pkgtools__msg_color_normal

	pkgtools__ui_is_interactive
	if [ $? -ne 0 ]; then
	    return 0
	fi
	pkgtools__ui_is_gui
	if [ $? -eq 0 ]; then
	    message="$@"
	    ${__pkgtools__ui_dialog_bin} --title "pkgtools GUI" \
		     --colors --msgbox "\Z1ERROR:\n\Zn ${message}" 10 40
	    return 0
	fi
	return 0
    }


    function pkgtools__msg_error ()
    {
	pkgtools__msg_err $@
	return 0
    }


    function pkgtools__msg_warning ()
    {
	if [ ${__pkgtools__msg_warning} -eq 0 ]; then
	    return 0
	fi
	pkgtools__msg_color_violet
	__pkgtools__base_msg_prefix "WARNING"
	__pkgtools__base_msg  $@
	pkgtools__msg_color_normal

	pkgtools__ui_is_interactive
	if [ $? -ne 0 ]; then
	    return 0
	fi
	pkgtools__ui_is_gui
	if [ $? -eq 0 ]; then
	    message="$@"
	    ${__pkgtools__ui_dialog_bin} --title "pkgtools GUI" \
		     --colors --msgbox "\Z5WARNING:\n\Zn ${message}" 10 40
	    return 0
	fi
	return 0
    }


    function pkgtools__msg_info ()
    {
	pkgtools__msg_is_quiet
	if [ $? -eq 0 ]; then
	    return 0
	fi

	if [ ${__pkgtools__msg_verbose} -eq 0 ]; then
	    return 0
	fi

	pkgtools__msg_color_blue
	__pkgtools__base_msg_prefix "INFO"
	__pkgtools__base_msg  $@
	pkgtools__msg_color_normal

	pkgtools__ui_is_interactive
	if [ $? -ne 0 ]; then
	    return 0
	fi
	pkgtools__ui_is_gui
	if [ $? -eq 0 ]; then
	    message="$@"
	    ${__pkgtools__ui_dialog_bin} --title "pkgtools GUI" \
		     --colors --msgbox "\Z4\ZbINFO:\n\Zn ${message}" 10 40
	    return 0
	fi
	return 0
    }


    function pkgtools__msg_verbose ()
    {
	pkgtools__msg_info $@
	return 0
    }


    function pkgtools__msg_notice ()
    {
	pkgtools__msg_is_quiet
	if [ $? -eq 0 ]; then
	    return 0
	fi

	pkgtools__msg_color_blue
	__pkgtools__base_msg_prefix "NOTICE"
	__pkgtools__base_msg "$@"
	pkgtools__msg_color_normal

	pkgtools__ui_is_interactive
	if [ $? -ne 0 ]; then
	    return 0
	fi
	pkgtools__ui_is_gui
	if [ $? -eq 0 ]; then
	    message="$@"
	    term_nl=$(stty size | cut -d' ' -f1)
	    term_nc=$(stty size | cut -d' ' -f2)
	    let max_nlines=term_nl-3
	    let max_ncols=term_nc-4
	    nl=$(echo -e "${message}" | wc -l)
	    let nlines=nl+4
	    if [ ${nlines} -gt ${max_nlines} ]; then
		nlines=${max_nlines}
	    fi
	    if [ ${nlines} -lt 6 ]; then
		nlines=6
	    fi
	    ${__pkgtools__ui_dialog_bin} --title "pkgtools GUI" \
		     --colors --msgbox "\Z4NOTICE:\n\Zn ${message}" ${nlines} ${max_ncols}
	    return 0
	fi
	return 0
    }


    function pkgtools__msg_highlight_notice ()
    {
	pkgtools__msg_color_green
	__pkgtools__base_msg_prefix "NOTICE"
	__pkgtools__base_msg $@
	pkgtools__msg_color_normal

	pkgtools__ui_is_interactive
	if [ $? -ne 0 ]; then
	    return 0
	fi
	pkgtools__ui_is_gui
	if [ $? -eq 0 ]; then
	    message="$@"
	    ${__pkgtools__ui_dialog_bin} --title "pkgtools GUI" \
		     --colors --msgbox "\Z4\ZbNOTICE:\n\Zn ${message}" 10 40
	    return 0
	fi
	return 0
    }


    function pkgtools__msg_devel ()
    {
	if [ ${__pkgtools__msg_devel} -eq 0 ]; then
	    return 0
	fi
	ok=1
	if [ ${ok} -eq 1 ]; then
	    pkgtools__msg_color_reverse
	    __pkgtools__base_msg_prefix "DEVEL"
	    __pkgtools__base_msg $@
	    #__pkgtools__base_msg_2 $@
	    pkgtools__msg_color_no_reverse
	fi
	pkgtools__msg_color_normal
	return 0
    }


    function pkgtools__msg_debug ()
    {
	if [ ${__pkgtools__msg_debug} -eq 0 ]; then
	    return 0
	fi
	ok=1
	if [ ${ok} -eq 1 ]; then
	    pkgtools__msg_color_brown
	    __pkgtools__base_msg_prefix "DEBUG"
	    __pkgtools__base_msg  $@
	    pkgtools__msg_color_normal
	fi
	return 0
    }


#
# Version number utilities
#

    function pkgtools__get_version_major ()
    {
	__pkgtools__at_function_enter pkgtools__get_version_major
	local version="$1"
	if [ "x${version}" = "x" ]; then
	    pkgtools__msg_error "Missing version id!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	local version_major=$(echo "${version}." | tr "/_" "." | cut -d'.' -f 1)
	echo ${version_major}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_version_minor ()
    {
	__pkgtools__at_function_enter pkgtools__get_version_minor
	local version="$1"
	if [ "x${version}" = "x" ]; then
	    pkgtools__msg_error "Missing version id!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	local version_minor=$(echo "${version}." | tr "/_" "." | cut -d'.' -f 2)
	echo ${version_minor}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_version_patch_level ()
    {
	local version="$1"
	if [ "x${version}" = "x" ]; then
	    pkgtools__msg_error "Missing version id!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	local version_patch_level=$(echo "${version}." | tr "/_" "." | cut -d'.' -f 3)
	if [ "x${version_patch_leve}" = "x" ]; then
	    version_patch_level=0
	fi
	echo ${version_patch_level}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_version_int ()
    {
	__pkgtools__at_function_enter pkgtools__get_version_int
	local version="$1"
	if [ "x${version}" = "x" ]; then
	    pkgtools__msg_error "Missing version id!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	version_major=$(pkgtools__get_version_major ${version})
	version_minor=$(pkgtools__get_version_minor ${version})
	version_patch_level=$(pkgtools__get_version_patch_level ${version})
	let version_lib=version_major*100000+version_minor*100+version_patch_level
	echo ${version_lib}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__create_global_header_file ()
    {
	__pkgtools__at_function_enter pkgtools__create_global_header_file
	pkg_name="$1"
	if [ "x${pkg_name}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1

	pkg_version="$1"
	if [ "x${pkg_version}" = "x" ]; then
	    pkgtools__msg_error "Missing package version!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1

	pkg_global_file="$1"
	if [ "x${pkg_global_file}" = "x" ]; then
	    pkgtools__msg_error "Missing target global header file!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1

	pkg_name_lower=$(pkgtools__to_lower ${pkg_name})
	pkg_name_upper=$(pkgtools__to_upper ${pkg_name})

	version_header_file="${pack_name}/${pack_name}_version.h"

	cat>${pkg_global_file}<<EOF
// -*- mode: c++; -*-
/* ${pkg_name}.h global header file
 * Author(s):     ${USERNAME} <${USERMAIL}>
 * Creation date: $(date +%F)
 * Last modified:
 *
 *
 */

/*! \mainpage The ${pkg_name} package documentation
 *
 * \section intro_section Introduction
 *
 * This section is empty.
 *
 * \section contents_section Contents
 *
 * This section is empty.
 *
 * \section build_section Installation instructions
 *
 * Configuration, build, installation and setup are done thanks
 * to the \c './pkgtools.d/pkgtool' script.
 *
 * Usage:
 *  \verbatim
bash> ./pkgtools.d/pkgtool --help
\endverbatim
 *
 * Using bash, a typical install sequence is:
 *
 *  \verbatim
bash> ./pkgtools.d/pkgtool [options...] info
bash> ./pkgtools.d/pkgtool [options...] check [special options...]
bash> ./pkgtools.d/pkgtool [options...] configure  [special options...]
bash> ./pkgtools.d/pkgtool [options...] build
bash> ./pkgtools.d/pkgtool [options...] build bin
bash> ./pkgtools.d/pkgtool [options...] test
bash> ./pkgtools.d/pkgtool [options...] doc
bash> ./pkgtools.d/pkgtool [options...] install [special options...]
\endverbatim
 *
 * Test:
 *  \verbatim
bash> source ${pack_name_lower}.sh
bash> ${pack_name_lower}-config --help
\endverbatim
 *
 * Setup from your \c '~/.bashrc':
 *  \verbatim
export ${pack_name_upper}_ROOT=<installation base directory>
if [ -f \${${pack_name_upper}_ROOT}/${pack_name_lower}.sh ]; then
   source \${${pack_name_upper}_ROOT}/${pack_name_lower}.sh
else
   echo "ERROR: No setup for the ${pack_name} package !" >&2
fi
\endverbatim
 *
 * Reseting the package:
 *  \verbatim
bash> ./pkgtools.d/pkgtool [options...] reset
\endverbatim
 *
 *
 * \section misc_section Miscellaneous informations
 *
 * This section is empty.
 *
 */

#ifndef __${pkg_name}__${pack_name}_h
#define __${pkg_name}__${pack_name}_h 1

#include <${version_header_file}>

//#include <${pack_name}/dummy_${pack_name}.h>

EOF
	cat>>${pkg_global_file}<<EOF

#endif // __${pkg_name}__${pack_name}_h

// end of ${pkg_name}.h
EOF

	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__create_version_header_file ()
    {
	__pkgtools__at_function_enter pkgtools__create_version_header_file
	pkg_name="$1"
	if [ "x${pkg_name}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1

	pkg_version="$1"
	if [ "x${pkg_version}" = "x" ]; then
	    pkgtools__msg_error "Missing package version!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1

	pkg_version_file="$1"
	if [ "x${pkg_version_file}" = "x" ]; then
	    pkgtools__msg_error "Missing target version header file!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1
	if [ -f ${pkg_version_file} ]; then
	    pkgtools__msg_error " Version file '${pkg_version_file}' already exists!"
	    __pkgtools__at_function_exit
	    return 1
	fi

	pkg_name_lower=$(pkgtools__to_lower ${pkg_name})
	pkg_name_upper=$(pkgtools__to_upper ${pkg_name})
	let pkg_version_int=$(pkgtools__get_version_int ${pkg_version})

	cat>${pkg_version_file}<<EOF
// -*- mode: c++; -*-
/* ${pkg_name}_version.h version header file
 *
 *
 */

#ifndef __${pkg_name}__${pkg_name}_version_h
#define __${pkg_name}__${pkg_name}_version_h 1

//  ${pkg_name_upper}_VERSION % 100        is the patch level
//  ${pkg_name_upper}_VERSION / 100 % 1000 is the minor version
//  ${pkg_name_upper}_VERSION / 100000     is the major version

#define ${pkg_name_upper}_VERSION ${pkg_version_int}

//  ${pkg_name_upper}_LIB_VERSION must be defined to be the same as ${pkg_name_upper}_VERSION
//  but as a *string* in the form "x_y[_z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define ${pkg_name_upper}_LIB_VERSION "${pkg_version}"

#endif // __${pkg_name}__${pkg_name}_version_h

// end of ${pkg_name}_version.h
EOF

	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__create_version_file ()
    {
	__pkgtools__at_function_enter pkgtools__create_version_file
	pkg_version="$1"
	if [ "x${pkg_version}" = "x" ]; then
	    pkgtools__msg_error "Missing version id!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1

	pkg_version_file="$1"
	if [ "x${pkg_version_file}" = "x" ]; then
	    pkgtools__msg_error "Missing target version file!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1

	if [ -f ${pkg_version_file} ]; then
	    pkgtools__msg_error " Version file '${pkg_version_file}' already exists!"
	    __pkgtools__at_function_exit
	    return 1
	fi

	echo "${pkg_version}" > ${pkg_version_file}

	__pkgtools__at_function_exit
	return 0
    }


#
# Package identification utilities:
#
#
#   name[version description](component,,,)
#
#

    function pkgtools__get_package_name ()
    {
	__pkgtools__at_function_enter pkgtools__get_package_name
	packdesc=$@
	if [ "x${packdesc}" = "x" ]; then
	    pkgtools__msg_error "Missing package description!"
	    return 1
	fi
	tmp=$(echo "${packdesc}${__PKGTOOLS__SUBLIST_OPEN}" | cut -d"${__PKGTOOLS__SUBLIST_OPEN}" -f1)
	pkgtools__msg_devel "====> tmp='${tmp}'"

	tmp2=$(echo "${tmp}${__PKGTOOLS__VERDESC_OPEN}" | cut -d"${__PKGTOOLS__VERDESC_OPEN}" -f1)
	pkgtools__msg_devel "====> tmp2='${tmp2}'"

	the_packname=${tmp2}
	pkgtools__msg_devel "====> the_packname='${the_packname}'"

	echo ${the_packname}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_package_version_desc ()
    {
	__pkgtools__at_function_enter pkgtools__get_package_version_desc
	packdesc=$@
	if [ "x${packdesc}" = "x" ]; then
	    pkgtools__msg_error "Missing package description!"
	    return 1
	fi
	tmp=$(echo "${packdesc}${__PKGTOOLS__SUBLIST_OPEN}" | cut -d"${__PKGTOOLS__SUBLIST_OPEN}" -f1)
	pkgtools__msg_devel "tmp='${tmp}'"

	tmp2=$(echo "${tmp}${__PKGTOOLS__VERDESC_CLOSE}" | cut -d"${__PKGTOOLS__VERDESC_CLOSE}" -f1)
	pkgtools__msg_devel "tmp2='${tmp2}'"

	tmp3=$(echo "${tmp2}${__PKGTOOLS__VERDESC_OPEN}" | cut -d"${__PKGTOOLS__VERDESC_OPEN}" -f2)
	pkgtools__msg_devel "tmp3='${tmp3}'"

	pack_version_desc=${tmp3}
	echo ${pack_version_desc}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__is_package_version_valid ()
    {
	__pkgtools__at_function_enter pkgtools__is_package_version_valid
	version_to_be_checked=$1
	if [ "x${version_to_be_checked}" = "x" ]; then
	    pkgtools__msg_error "Missing version number to be checked"
	    __pkgtools__at_function_exit
	    return 1
	fi
	pkgtools__msg_devel "Version to be checked is : '${version_to_be_checked}'"
	verdesc=$2
	if [ "x${verdesc}" = "x" ]; then
	    pkgtools__msg_devel "Version '${version_to_be_checked}' is considered as valid!"
	    __pkgtools__at_function_exit
	    return 0
	fi
	pkgtools__msg_devel "Version requirement is : '${verdesc}'"

	if [ "x${verdesc}" = "x!" ]; then
	    pkgtools__msg_devel "Version '${version_to_be_checked}' is not valid!"
	    __pkgtools__at_function_exit
	    return 1
	fi

	if [ ! -x ${PKGTOOLS_ROOT}/programs/check_version.py ]; then
	    pkgtools__msg_error "No '\${PKGTOOLS_ROOT}/programs/check_version.py}' is available ! Cannot check the validity of the version number ! Assuming it is ok !"
	    __pkgtools__at_function_exit
	    return 1
	else
	    ${PKGTOOLS_ROOT}/programs/check_version.py ${version_to_be_checked} "${verdesc}"
	    if [ $? -ne 0 ]; then
		pkgtools__msg_devel "Version '${version_to_be_checked}' is not valid!"
		__pkgtools__at_function_exit
		return 1
	    fi
	fi
	__pkgtools__at_function_exit
	return 0
    }


#
# Software check utilities:
#

    function pkgtools__executable_exists ()
    {
        exe=$1
        if [ "x${exe}" = "x" ]; then
            pkgtools__msg_error "Missing binary name!"
            return 1
        fi

        which ${exe} 2>&1 > /dev/null
        if [ $? -ne 0 ]; then
            return 1
        fi

        return 0 # true
    }

    function pkgtools__check_python_module ()
    {
	__pkgtools__at_function_enter pkgtools__check_python_module
	pymodule="$1"
	if [ "x${pymodule}" = "x" ]; then
	    err_msg "do_check_python_package: missing Python module!"
	    __pkgtools__at_function_exit
	    return 2
	fi
	test_py=/tmp/check_python_${pymodule}.py
	error_code=0
	cat>${test_py}<<EOF
#!/usr/bin/env python

import sys

def main () :
  err = 0
  try:
    import ${pymodule}
    sys.stderr.write ("Python module '%s'\n" % (${pymodule}))
  except:
    sys.stderr.write ("Cannot import Python module '%s'\n" % (${pymodule}))
    err = 1
  return err

if __name__ == "__main__" :
    sys.exit (main ())

EOF
	python ${test_py} > /dev/null 2>&1
	if [ $? -ne 0 ]; then
	    error_code=1
	fi
	rm -f ${test_py}
	__pkgtools__at_function_exit
	return ${error_code}
    }


    function pkgtools__is_pkg_config_installed ()
    {
	__pkgtools__at_function_enter pkgtools__is_pkg_config_installed
	error_code=0
	which pkg-config > /dev/null 2>&1
	error_code=$?
	__pkgtools__at_function_exit
	return ${error_code}
    }


    function pkgtools__package_is_pkg_config_aware ()
    {
	__pkgtools__at_function_enter pkgtools__package_is_pkg_config_aware
	packname=$1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	pkgtools__is_pkg_config_installed
	if [ $? -ne 0 ]; then
	    pkgtools__msg_devel "pkg-config is not setup!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	error_code=0
	pkg-config --exists ${packname} >/dev/null 2>&1
	error_code=$?
	__pkgtools__at_function_exit
	return ${error_code}
    }


    function pkgtools__get_config_cflags_opt ()
    {
	__pkgtools__at_function_enter pkgtools__get_config_cflags_opt
	cfg=$1
	shift 1
	if [ "x${cfg}" = "x" ]; then
	    pkgtools__msg_error "Missing configuration tool name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	if [ "x${cfg}" = "xpkg-config" ]; then
	    echo "--cflags"
	else
	    echo "--cflags"
	fi
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_config_ldflags_opt ()
    {
	__pkgtools__at_function_enter pkgtools__get_config_ldflags_opt
	cfg=$1
	shift 1
	if [ "x${cfg}" = "x" ]; then
	    pkgtools__msg_error "Missing configuration tool name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	if [ "x${cfg}" = "xpkg-config" ]; then
	    echo "--libs"
	else
	    echo "--ldflags"
	fi
	__pkgtools__at_function_exit
	return 0
    }


    # Returns the name of the configuration script:
    #
    #   pkgtools__get_package_config_tool NAME [0|1]
    #
    # Examples:
    #
    #  pack_cfg=$(pkgtools__get_package_config_tool blahblah)
    #
    #  --> blahblah-config
    #
    #  pack_cfg=$(pkgtools__get_package_config_tool blahblah 1
    #
    #  --> pkg-config
    #
    function pkgtools__get_package_config_tool ()
    {
	__pkgtools__at_function_enter pkgtools__get_package_config_tool
	packdesc=$1
	shift 1
	if [ "x${packdesc}" = "x" ]; then
	    pkgtools__msg_error "Missing package description!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	packname=$(pkgtools__get_package_name ${packdesc})
	pack_cfg=

	# force pkg-config to be the configuration script:
	check_pkg_config=0
	if [ "x${1}" = "x1" ]; then
	    check_pkg_config=1
	fi

	if [ ${check_pkg_config} -eq 1 ]; then
	    pack_cfg=pkg-config
	fi

	if [ "x${pack_cfg}" = "x" ]; then
	    pack_lower=$(pkgtools__to_lower ${packname})
	    # special trick:
	    if [ "${pack_lower}" == "cernlib" ]; then
		pack_cfg=cernlib
	    elif [ "x"${pack_lower} == "mysql" ]; then
		pack_cfg=mysql_config
	    else
                # search for XXX-config tool:
		pack_cfg=${pack_lower}-config
	    fi
	fi
	echo ${pack_cfg}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__check_package_config_tool ()
    {
	__pkgtools__at_function_enter pkgtools__check_package_config_tool
	packname=$1
	shift 1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	pack_cfg=$(pkgtools__get_package_config_tool ${packname})
	if [ "x${pack_cfg}" != "x" ]; then
	    which ${pack_cfg} >/dev/null 2>&1
	    if [ $? -ne 0 ]; then
		pkgtools__msg_warning "Package has no active '${pack_cfg}' configuration tool!"
		__pkgtools__at_function_exit
		return 1
	    fi
	fi
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_package_cflags_command ()
    {
	__pkgtools__at_function_enter pkgtools__get_package_cflags_command
	packname=$1
	shift 1
	sublibs=$@

	if [ "x${packname}" = "x" ]; then
	    return 1
	fi
	pack_cfg=$(pkgtools__get_package_config_tool ${packname})
	if [ "x${pack_cfg}" = "x" ]; then
	    return 1
	fi

	pack_lower=$(pkgtools__to_lower ${packname})
	pack_cflags_com=""

        # tricks:
	cflags_opt=
	if [ "${pack_lower}" == "clhep" ]; then
	    cflags_opt=--include
	    pack_cflags_com="${pack_cfg} ${cflags_opt}"
	elif [ "${pack_lower}" == "cernlib" ]; then
	    cflags_opt=
	    pack_cfg=
	    pack_cflags_com=
	else
	    cflags_opt=$(pkgtools__get_config_cflags_opt ${packname})
	    pack_cflags_com="${pack_cfg} ${cflags_opt}"
	    if [ "x${sublibs}" != "x" ]; then

            # tricks:
		if [ "${pack_lower}" == "boost" ]; then
		    if [ "x${pack_cflags_com}" != "x" ]; then
			pack_cflags_com="${pack_cflags_com} ${sublibs}"
		    fi
		fi

	    fi
	fi
	echo ${pack_cflags_com}
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_package_ldflags_command ()
    {
	__pkgtools__at_function_enter pkgtools__get_package_ldflags_command
	packname=$1
	shift 1
	sublibs=$@

	if [ "x${packname}" = "x" ]; then
	    __pkgtools__at_function_exit
	    return 1
	fi
	pack_cfg=$(pkgtools__get_package_config_tool ${packname})
	if [ "x${pack_cfg}" = "x" ]; then
	    __pkgtools__at_function_exit
	    return 1
	fi

	pack_lower=$(pkgtools__to_lower ${packname})
	pack_cflags_com=""

        # tricks:
	ldflags_opt=--ldflags
	if [ "${pack_lower}" == "clhep" ]; then
	    ldflags_opt=--libs
	elif [ "${pack_lower}" == "root" ]; then
	    ldflags_opt=--libs
	elif [ "${pack_lower}" == "mysql" ]; then
	    ldflags_opt=--libs
	elif [ "${pack_lower}" == "gsl" ]; then
	    ldflags_opt=--libs
	elif [ "${pack_lower}" == "cernlib" ]; then
	    ldflags_opt=
	else
	    ldflags_opt=$(pkgtools__get_config_ldflags_opt ${packname})
	    pack_cflags_com="${pack_cfg} ${ldflags_opt}"
	fi

	pack_ldflags_com="${pack_cfg} ${ldflags_opt}"
	if [ "x${sublibs}" != "x" ]; then

            # tricks:
	    if [ "${pack_lower}" == "boost" ]; then
		if [ "x${pack_ldflags_com}" != "x" ]; then
		    pack_ldflags_com="${pack_ldflags_com} ${sublibs}"
		fi
	    fi

	    if [ "${pack_lower}" == "cernlib" ]; then
		if [ "x${pack_ldflags_com}" != "x" ]; then
		    pack_ldflags_com="${pack_ldflags_com} ${sublibs}"
		fi
	    fi

	fi
	echo ${pack_ldflags_com}
	__pkgtools__at_function_exit
	return 0
    }
    #export -f pkgtools__get_package_ldflags_command


    function pkgtools__get_pyver ()
    {
	__pkgtools__at_function_enter pkgtools__get_pyver
	which python > /dev/null 2>&1
	if [ $? -ne 0 ]; then
	    echo ""
	    __pkgtools__at_function_exit
	    return 1
	fi
	python -V 2>&1 | head -1 | cut -d' ' -f2
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_g77ver ()
    {
	__pkgtools__at_function_enter pkgtools__get_g77ver
	which g77 > /dev/null 2>&1
	if [ $? -ne 0 ]; then
	    echo ""
	    __pkgtools__at_function_exit
	    return 1
	fi
	g77 --version | head -1 | tr '[:space:]' ' ' | tr -s ' ' | cut -d')' -f2 | sed 's/^ //g' | cut -d ' ' -f1
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_gfortranver ()
    {
	__pkgtools__at_function_enter pkgtools__get_gfortranver
	which gfortran > /dev/null 2>&1
	if [ $? -ne 0 ]; then
	    echo ""
	    __pkgtools__at_function_exit
	    return 1
	fi
	gfortran --version | head -1 | tr '[:space:]' ' ' | tr -s ' ' | cut -d')' -f2 | sed 's/^ //g' | cut -d ' ' -f1
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_gccver ()
    {
	__pkgtools__at_function_enter pkgtools__get_gccver
	which gcc > /dev/null 2>&1
	if [ $? -ne 0 ]; then
	    echo ""
	    __pkgtools__at_function_exit
	    return 1
	fi
	gcc --version | head -1 | tr '[:space:]' ' ' | tr -s ' ' | cut -d')' -f2 | sed 's/^ //g' | cut -d ' ' -f1
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__get_sys_with_gccver ()
    {
	gccver=$(pkgtools__get_gccver | tr -d '.')
	echo "$(pkgtools__get_sys)-gcc${gccver}"
    }


    function pkgtools__check_package_by_executable ()
    {
	__pkgtools__at_function_enter pkgtools__check_package_by_executable
	packname=$1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	__pkgtools__at_function_exit
	return 1
    }


    function pkgtools__check_package_by_config ()
    {
	__pkgtools__at_function_enter pkgtools__check_package_by_config
	packname=$1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	pkgconf=$(pkgtools__get_package_config_tool gsl)
	which ${pkgconf} > /dev/null 2>&1
	if [ $? -ne 0 ]; then
	    pkgtools__msg_devel "'${pkgconf}' utility does not exists!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__check_package_by_root_env ()
    {
	__pkgtools__at_function_enter pkgtools__check_package_by_root_env
	packname=$1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
        pkgrootenv=$(eval "echo \$$(echo ${packname}_ROOT | tr 'a-z' 'A-Z')")
#	pkgrootenv=$(eval $(echo '${packname}'))
        if [ ! -n "${pkgrootenv}" ]; then
            pkgtools__msg_devel "Environment variable is not set !"
            __pkgtools__at_function_exit
            return 1
        fi

	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__check_package_always_suspicious ()
    {
	__pkgtools__at_function_enter pkgtools__check_package_always_suspicious
	packname=$1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	lpackname=$(pkgtools__to_lower ${packname} )
	upackname=$(pkgtools__to_upper ${packname})

	if [ "x${PKGTOOLS_CHECK_ALWAYS_SUSPICIOUS}" != "x" ]; then
	    always_suspicious_list=$(echo ${PKGTOOLS_CHECK_ALWAYS_SUSPICIOUS} | tr "[[:space:]]" ' ' | tr -s ' ')
	    pkgtools__msg_devel "List of packages considered as suspicious: '${always_suspicious_list}'"
	    for pn in ${always_suspicious_list}; do
		lpn=$(pkgtools__to_lower ${pn} )
		if [ "${lpackname}" = "${lpn}" ]; then
		    pkgtools__msg_devel "Package '${packname}' is suspicious!"
		    __pkgtools__at_function_exit
		    return 0
		fi
	    done
	fi
	__pkgtools__at_function_exit
	return 1
    }


    function pkgtools__check_package_always_bad ()
    {
	__pkgtools__at_function_enter pkgtools__check_package_always_bad
	packname=$1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	lpackname=$(pkgtools__to_lower ${packname} )
	upackname=$(pkgtools__to_upper ${packname})

	if [ "x${PKGTOOLS_CHECK_ALWAYS_BAD}" != "x" ]; then
	    always_bad_list=$(echo ${PKGTOOLS_CHECK_ALWAYS_BAD} | tr "[[:space:]]" ' ' | tr -s ' ')
	    pkgtools__msg_devel "List of packages not validated by default: '${always_bad_list}'"
	    for pn in ${always_bad_list}; do
		lpn=$(pkgtools__to_lower ${pn} )
		if [ "${lpackname}" = "${lpn}" ]; then
		    pkgtools__msg_devel "Package '${packname}' is considered as not setup!"
		    __pkgtools__at_function_exit
		    return 0
		fi
	    done
	fi
	__pkgtools__at_function_exit
	return 1
    }


    function pkgtools__check_package_always_ok ()
    {
	__pkgtools__at_function_enter pkgtools__check_package_always_ok
	packname=$1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	lpackname=$(pkgtools__to_lower ${packname} )
	upackname=$(pkgtools__to_upper ${packname})

	if [ "x${PKGTOOLS_CHECK_ALWAYS_OK}" != "x" ]; then
	    always_ok_list=$(echo ${PKGTOOLS_CHECK_ALWAYS_OK} | tr "[[:space:]]" ' ' | tr -s ' ')
	    pkgtools__msg_devel "List of packages validated by default: '${always_ok_list}'"
	    for pn in ${always_ok_list}; do
		lpn=$(pkgtools__to_lower ${pn} )
		if [ "${lpackname}" = "${lpn}" ]; then
		    pkgtools__msg_devel "Package '${packname}' is considered as  properly setup!"
		    __pkgtools__at_function_exit
		    return 0
		fi
	    done
	fi
	__pkgtools__at_function_exit
	return 1
    }


    function pkgtools__check_package ()
    {
	__pkgtools__at_function_enter pkgtools__check_package
	packname=$1
	if [ "x${packname}" = "x" ]; then
	    pkgtools__msg_error "Missing package name!"
	    __pkgtools__at_function_exit
	    return 1
	fi

        # First step:
        #
        # Some packages are not checked from their possible setup
        # but checked from possible list of:
        #
        # - always "good setup" packages:
        #   Example:
        #     export PKGTOOLS_CHECK_ALWAYS_OK="AAA BBB CCC... "
        #
        # - always "bad setup" packages:
        #   Example:
        #     export PKGTOOLS_CHECK_ALWAYS_BAD="UUU VVV XXX... "
        #
        # - always "suspicious setup" packages that
        #   need some manual check outside the 'pkgtools' tools.
        #   Example:
        #     export PKGTOOLS_CHECK_ALWAYS_SUSPICIOUS="PPP QQQ... "
        #

	pkgtools__check_package_always_bad ${packname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_devel "Package '${packname}' is considered as not setup!"
	    __pkgtools__at_function_exit
	    return 1
	fi

	pkgtools__check_package_always_ok ${packname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_devel "Package '${packname}' is considered as properly setup!"
	    __pkgtools__at_function_exit
	    return 0
	fi

	pkgtools__check_package_always_suspicious ${packname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_warning "Package '${packname}' is suspicious!"
	    pkgtools__msg_warning "Please check manually the settings of '${packname}'!"
	    __pkgtools__at_function_exit
	    return 0
	fi

        # Second step:
        #
        # Packages are checked from their possible setup:
        #
        # - existence of a 'XXX-config' executable
        #
        # - existence of a 'XXX_ROOT' environment variable
        #

	pkgtools__check_package_by_config ${packname}
	if [ $? -eq 0 ]; then
	    __pkgtools__at_function_exit
	    return 0
	fi

	pkgtools__check_package_by_root_env ${packname}
	if [ $? -eq 0 ]; then
	    __pkgtools__at_function_exit
	    return 0
	fi

	__pkgtools__at_function_exit
	return 1
    }


    function pkgtools__check_dependencies ()
    {
	__pkgtools__at_function_enter pkgtools__check_dependencies

	local pack_info="$1"
	shift 1

	if [ ! -f ${pack_info} ]; then
	    pkgtools__msg_warning "Cannot find 'package.info' file!"
	    __pkgtools__at_function_exit
	    return 0
	fi

	local skipped_dependencies="$@"
	if [ "x${skipped_dependencies}" != "x" ]; then
	    pkgtools__msg_debug "Will skip dependency check for ${skipped_dependencies}..."
	fi

	local deps=$(cat ${pack_info} | grep "dependencies=" | cut -d= -f2- | tr '\t' ' ' | tr -s ' ' | tr ';' '\n' | sed -e 's/ /___/g')
	pkgtools__msg_debug "Checking deps='${deps}'"
	for adep in ${deps}; do
	    local tdep=$(echo "${adep}@" | cut -d'@' -f1)
	    local dep=$(echo ${tdep} | sed -e 's/___//g')
	    local dep_name=$(echo "${dep}[" | cut -d'[' -f1 | tr -d ' ')

            # skip check for dependency within the 'skipped_dependencies' list:
	    echo "${skipped_dependencies}" | grep ${dep_name} >/dev/null 2>&1
	    if [ $? -eq 0 ]; then
		pkgtools__msg_notice "Do not check ${dep_name} setup..."
		continue
	    fi
	    pkgtools__msg_notice "Checking ${dep_name} setup..."
	    local dep_version_info=$(echo "${dep}[" | cut -d'[' -f2 | cut -d']' -f1)
	    local dep_sublibs=$(echo "${adep}@" | cut -d'@' -f2 | sed -e 's/^(//g' -e 's/)$//g')
	    local dep_version=$(echo "${adep}/" | cut -d'/' -f2)
	    local dep_lower=$(pkgtools__to_lower ${dep_name})
	    local dep_upper=$(pkgtools__to_upper ${dep_name})
	    local dep_cfg=$(get_dependency_package_config ${dep_name})
	    local sublibs=$(echo ${dep_sublibs} | tr "A-Z" "a-z" | tr -d [[:space:]] | tr ',' ' ')
	    pkgtools__msg_debug "Package dependency     =  '${dep}'"
	    pkgtools__msg_debug "- Package name         =  '${dep_name}'"
	    pkgtools__msg_debug "- Package version info =  '${dep_version_info}'"
	    pkgtools__msg_debug "- Package sublibs      =  '${dep_sublibs}'"
	    pkgtools__msg_debug "- Depends on package '${dep_name}' with version info '${dep_version_info}'"
	    if [ "x${dep_sublibs}" != "x" ]; then
		pkgtools__msg_debug "check_dependencies:     with libraries '${dep_sublibs}'"
	    fi
	    pkgtools__msg_debug "- Sublibs      =  '${sublibs}'"

	    local dep_excluded=0
	    local check_excl=$(echo ${dep_version_info} | tr -d [:space:])
	    if [ "x${check_excl}" = "x!" ]; then
		dep_excluded=1
	    fi
	    pkgtools__msg_debug "- Excluded     =  '${dep_excluded}'"

	    local has_config_script=0
	    local has_executable=0
	    pkgtools__msg_debug "- Checking ${dep_cfg} file..."
	    which ${dep_cfg} > /dev/null 2>&1
	    if [ $? -ne 0 ]; then
		has_config_script=0
		pkgtools__msg_info "Cannot find '${dep_cfg}' config script !"
	    else
		has_config_script=1
		pkgtools__msg_notice "Found '${dep_cfg}' config script !"
	    fi

	    if [ ${has_config_script} -eq 0 ]; then
		pkgtools__msg_debug "- Checking ${dep_name} executable..."
		local dep_exe=
		which ${dep_name} > /dev/null 2>&1
		if [ $? -ne 0 ]; then
		    has_executable=0
		    pkgtools__msg_info "Cannot find '${dep_name}' executable !"
		else
		    dep_exe=${dep_name}
		    has_executable=1
		    pkgtools__msg_notice "Found '${dep_name}' executable !"
		fi

		if [ "x${dep_name}" != "x${dep_lower}" ]; then
		    pkgtools__msg_debug "- Checking ${dep_lower} executable..."
		    which ${dep_lower} > /dev/null 2>&1
		    if [ $? -eq 0 ]; then
			dep_exe=${dep_lower}
			has_executable=1
			pkgtools__msg_notice "Found '${dep_lower}' executable !"
		    else
			pkgtools__msg_info "Cannot find '${dep_lower}' executable !"
		    fi
		fi
	    fi

	    if [ ${has_executable} -eq 0 -a ${has_config_script} -eq 0 ]; then
		if [ ${dep_excluded} -eq 0 ]; then
		    pkgtools__msg_error "Cannot check '${dep_name}' through some script nor executable !"
		    __pkgtools__at_function_exit
		    return 1
		fi
	    fi

	    dep_version=

	    if [ "x${dep_version}" == "x" ]; then
		if [ ${has_config_script} -eq 1 ]; then
		    ${dep_cfg} --version > /dev/null 2>&1
		    if [ $? -ne 0 ]; then
			pkgtools__msg_warning "Cannot retrieve version information from the '${dep_cfg}' script !"
		    else
			dep_version=$(${dep_cfg} --version 2>&1)
		    fi
		fi
	    fi

	    if [ "x${dep_version}" == "x" ]; then
		if [ ${has_executable} -eq 1 -a "x${dep_cfg}" != "x${dep_exe}" ]; then
		    ${dep_exe} --version > /dev/null 2>&1
		    if [ $? -ne 0 ]; then
			pkgtools__msg_warning "Cannot retrieve version information from the '${dep_exe}' executable !"
		    else
			dep_version=$(${dep_exe} --version 2>&1)
		    fi
		fi
	    fi

	    ver_str=""
	    if [ "x${dep_version}" != "x" ]; then
		ver_str=" (${dep_version})"
	    else
		pkgtools__msg_warning "Cannot retrieve version information for '${dep_name}'!"
	    fi

	    if [ ${dep_excluded} -eq 1 ]; then
		pkgtools__msg_error "${dep_name}${ver_str} should NOT be setup !"
		__pkgtools__at_function_exit
		return 1
	    fi

	    ver_str_num=$(echo "${dep_version}" | tr '_' '.' | tr '/' '.' | tr -c -d '0123456789\.')
	    pkgtools__msg_devel "extracted version number '${ver_str_num}'"
	    if [ "x${PKGTOOLS_ROOT}" != "x" ]; then
		if [ "x${ver_str_num}" != "x" ]; then
		    pkgtools__msg_notice "Check if version '${ver_str_num}' of '${dep_name}' is valid..."
		    pkgtools__is_package_version_valid "${ver_str_num}" "${dep_version_info}"
		    if [ $? -eq 0 ]; then
			pkgtools__msg_notice "${dep_name} version ${ver_str_num} is valid."
		    else
			pkgtools__msg_error "${dep_name} version ${ver_str_num} is not valid ! Requirement for ${dep_name} version number is ${dep_version_info} !"
			__pkgtools__at_function_exit
			return 1
		    fi
		fi
	    else
		pkgtools__msg_warning "Some 'pkgtools' utilities are not installed ! Cannot check the validity of this version number ! "
		pkgtools__msg_warning "Please consider to install 'pkgtools' !"
		pkgtools__msg_warning "Continue at your own risk !"
	    fi

	    pkgtools__msg_notice "${dep_name}${ver_str} seems to be properly setup !"

	done

	__pkgtools__at_function_exit
	return 0;
    }

    function pkgtools__cfg_has_property ()
    {
	__pkgtools__at_function_enter pkgtools__cfg_has_property

	cfg_file="$1"
	shift 1
	property_key="$1"
	if [ "x${property_key}" = "x" ]; then
	    pkgtools__msg_error "Missing '${property_key}'!"
	    __pkgtools__at_function_exit
	    return 1
	fi
	shift 1

	if [ ! -r ${cfg_file} ]; then
	    pkgtools__msg_error "Cannot read file '${cfg_file}'!"
	    __pkgtools__at_function_exit
	    return 1
	fi

	grep "^${property_key}=" ${cfg_file} > /dev/null 2>&1
	if [ $? -ne 0 ]; then
	    __pkgtools__at_function_exit
	    return 1;
	fi

	__pkgtools__at_function_exit
	return 0;
    }

    function pkgtools__remove_file ()
    {
	__pkgtools__at_function_enter pkgtools__remove_file

	filename="$1"
	if [ "x${filename}" = "x" ]; then
	    pkgtools__msg_error "Missing filename !"
	    __pkgtools__at_function_exit
	    return 1
	fi
	if [ -f ${filename} ]; then
	    rm -f ${filename}
	fi
	__pkgtools__at_function_exit
	return 0;
    }

    function pkgtools__cfg_load_property ()
    {
	__pkgtools__at_function_enter pkgtools__cfg_load_property

	cfg_file="$1"
	shift 1
	property_key="$1"
	shift 1

	if [ ! -r ${cfg_file} ]; then
	    pkgtools__msg_error "Cannot read file '${cfg_file}'!"
	    __pkgtools__at_function_exit
	    return 1
	fi

	property_value=$(cat ${cfg_file} | grep "^${property_key}=" )
        echo "${property_value}"

	__pkgtools__at_function_exit
	return 0;
    }


    function pkgtools__cfg_store_property ()
    {
	__pkgtools__at_function_enter pkgtools__cfg_store_property

	cfg_file="$1"
	shift 1
	property_key="$1"
	shift 1
	property_value="$@"

	if [ ! -f ${cfg_file} ]; then
	    touch ${cfg_file}
	fi

	if [ ! -w ${cfg_file} ]; then
	    pkgtools__msg_error "Cannot write file '${cfg_file}'!"
	    __pkgtools__at_function_exit
	    return 1
	fi

	echo "${property_key}=${property_value}" >> ${cfg_file}

	__pkgtools__at_function_exit
	return 0
    }

    function pkgtools__test ()
    {
	__pkgtools__at_function_enter pkgtools__test
	pkgtools__msg_notice "OS                   : $(pkgtools__get_os)"
	pkgtools__msg_notice "Arch                 : $(pkgtools__get_arch)"
	pkgtools__msg_notice "Distribution family  : $(pkgtools__get_distribution_family)"
	pkgtools__msg_notice "Distribution         : $(pkgtools__get_distribution)"
	pkgtools__msg_notice "Distribution version : $(pkgtools__get_distribution_version)"
	pkgtools__is_linux
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Is Linux."
	fi
	pkgtools__is_darwin
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Is Darwin."
	fi
	pkgtools__is_ubuntu
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Is Ubuntu."
	fi
	pkgtools__knows_number_of_processors
	if [ $? -eq 0 ]; then
	    local nbprocs=$(pkgtools__get_number_of_processors)
	    pkgtools__msg_notice "Number of processors : $(pkgtools__get_number_of_processors)"
	fi

	pkgtools__msg_notice "Shared library extension : $(pkgtools__get_so_extension)"

	pkgtools__msg_notice "GSL configure utility: $(pkgtools__get_package_config_tool gsl)"

	pkgtools__msg_notice "GSL cflags command: '$(pkgtools__get_package_cflags_command gsl)'"

	pkgtools__msg_notice "GSL ldflags command: '$(pkgtools__get_package_ldflags_command gsl)'"

	pkgtools__msg_notice "CERNLIB configure utility: $(pkgtools__get_package_config_tool CERNLIB)"

	pkgtools__msg_notice "CERNLIB cflags command: '$(pkgtools__get_package_cflags_command CERNLIB mathlib packlib kernlib geant3)'"

	pkgtools__msg_notice "CERNLIB ldflags command: '$(pkgtools__get_package_ldflags_command CERNLIB mathlib packlib kernlib geant3)'"

	pkgtools__check_python_module Tkinter
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Python TKinter is setup."
	else
	    pkgtools__msg_warning "Python TKinter is not setup."
	fi

	pkgtools__check_python_module numpy >/dev/null 2>&1
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Python numpy is setup."
	else
	    pkgtools__msg_warning "Python numpy is not setup."
	fi

	pkgtools__check_python_module blahblah >/dev/null 2>&1
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Python blahblah is setup."
	else
	    pkgtools__msg_warning "Python blahblah is not setup."
	fi

	file="/usr/include/stdlib.h"
	pkgtools__msg_notice "Base name of '${file}': '$(pkgtools__get_basename ${file})'"
	pkgtools__msg_notice "Dir name of '${file}': '$(pkgtools__get_dirname ${file})'"

	pkgtools__msg_notice "Python   version: '$(pkgtools__get_pyver)'"
	pkgtools__msg_notice "g77      version: '$(pkgtools__get_g77ver)'"
	pkgtools__msg_notice "gfortran version: '$(pkgtools__get_gfortranver)'"
	pkgtools__msg_notice "gcc      version: '$(pkgtools__get_gccver)'"


	pkgtools__msg_notice
	pkgtools__msg_notice "Manipulate package's description:"
	pkgtools__msg_notice

	pkdesc="geant4[>4.0.3 <=4.6.23 !4.1.45](lib1,lib2,lib3)"
	pkname=$(pkgtools__get_package_name ${pkdesc})
	pkverdesc=$(pkgtools__get_package_version_desc ${pkdesc})
	pkgtools__msg_notice "Package description is: '${pkdesc}'"
	pkgtools__msg_notice "Package name is       : '${pkname}'"
	pkgtools__msg_notice "Package version desc  : '${pkverdesc}'"
	check_ver=4.23
	pkgtools__is_package_version_valid "${check_ver}" "${pkverdesc}"
	if [ $? -eq 0 ]; then
	    pkgtools__msg_highlight_notice "Package version ${check_ver} is valid."
	else
	    pkgtools__msg_warning "Package version ${check_ver} is not valid."
	fi

	pkdesc="geant4"
	pkname=$(pkgtools__get_package_name ${pkdesc})
	pkverdesc=$(pkgtools__get_package_version_desc ${pkdesc})
	pkgtools__msg_notice "Package description is: '${pkdesc}'"
	pkgtools__msg_notice "Package name is: '${pkname}'"
	pkgtools__msg_notice "Package version desc  : '${pkverdesc}'"
	check_ver=4.23
	pkgtools__is_package_version_valid "${check_ver}" "${pkverdesc}"
	if [ $? -eq 0 ]; then
	    pkgtools__msg_highlight_notice "Package version ${check_ver} is valid."
	else
	    pkgtools__msg_warning "Package version ${check_ver} is not valid."
	fi

	pkdesc="geant4[=4.64]"
	pkname=$(pkgtools__get_package_name ${pkdesc})
	pkverdesc=$(pkgtools__get_package_version_desc ${pkdesc})
	pkgtools__msg_notice "Package description is: '${pkdesc}'"
	pkgtools__msg_notice "Package name is: '${pkname}'"
	pkgtools__msg_notice "Package version desc  : '${pkverdesc}'"
	check_ver=4.23
	pkgtools__is_package_version_valid "${check_ver}" "${pkverdesc}"
	if [ $? -eq 0 ]; then
	    pkgtools__msg_highlight_notice "Package version ${check_ver} is valid."
	else
	    pkgtools__msg_warning "Package version ${check_ver} is not valid."
	fi


	pkgtools__msg_notice
	pkgtools__msg_notice "Checking packages:"
	pkgtools__msg_notice

	pkgtools__is_pkg_config_installed
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "pkg-config is setup."
	else
	    pkgtools__msg_warning "pkg-config is not setup."
	fi

	PKGTOOLS_CHECK_ALWAYS_OK="Geant4"
	PKGTOOLS_CHECK_ALWAYS_BAD="Broken"
	PKGTOOLS_CHECK_ALWAYS_SUSPICIOUS="to_be_checked"

	pkname=geant4
	pkgtools__check_package ${pkname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Package '${pkname}' is setup."
	else
	    pkgtools__msg_warning "Package '${pkname}' is not setup."
	fi

	pkname=broken
	pkgtools__check_package ${pkname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Package '${pkname}' is setup."
	else
	    pkgtools__msg_warning "Package '${pkname}' is not setup."
	fi

	pkname=to_be_checked
	pkgtools__check_package ${pkname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Package '${pkname}' is setup."
	else
	    pkgtools__msg_warning "Package '${pkname}' is not setup."
	fi

	pkname=blahblah
	pkgtools__check_package ${pkname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Package '${pkname}' is setup."
	else
	    pkgtools__msg_warning "Package '${pkname}' is not setup."
	fi

	pkgtools__check_package_config_tool ${pkname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Found '${pkname}' configuration tool."
	else
	    pkgtools__msg_warning "Cannot find '${pkname}' configuration_tool."
	fi

	pkgtools__package_is_pkg_config_aware ${pkname}
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "pkg-config can manage the '${pkname}' package."
	else
	    pkgtools__msg_warning "pkg-config does not manage the '${pkname}' package."
	fi

	pkdesc=blahblah
	pkname=$(pkgtools__get_package_name ${pkdesc})
	pkgtools__msg_notice "${pkname} configure utility: '$(pkgtools__get_package_config_tool ${pkdesc})'"

	pkgtools__msg_notice "${pkname} cflags command:    '$(pkgtools__get_package_cflags_command ${pkname})'"

	pkgtools__msg_notice "${pkname} ldflags command:   '$(pkgtools__get_package_ldflags_command ${pkname})'"

	pkgtools__msg_notice "${pkname} configure utility: '$(pkgtools__get_package_config_tool ${pkname} 1)'"


	__pkgtools__at_function_exit
	return 0
    }


    function pkgtools__test_ui ()
    {
	__pkgtools__at_function_enter pkgtools__test_ui

	pkgtools__ui_ask_user_string "Enter the name of the package" "tugudu"
	if [ $? -ne 0 ]; then
	    pkgtools__msg_error "UI error!"
	else
	    pkname="$(pkgtools__ui_get_user_string)"
	    pkgtools__msg_notice "pkname == '${pkname}'"
	fi

	pkgtools__ui_ask_user_string "Enter the name of the package"
	if [ $? -ne 0 ]; then
	    pkgtools__msg_error "UI error!"
	else
	    pkname="$(pkgtools__ui_get_user_string)"
	    pkgtools__msg_notice "pkname == '${pkname}'"
	fi

	pkgtools__ui_ask_user_yesno "Do you C++" "y"
	if [ $? -ne 0 ]; then
	    pkgtools__msg_error "UI error!"
	else
	    pkgtools__ui_user_yesno_is_yes
	    if [ $? -eq 0 ]; then
		pkgtools__msg_highlight_notice "You are a C++ geek!"
	    else
		pkgtools__msg_warning "You should try a little C++!"
	    fi
	fi

	pkgtools__ui_ask_user_yesno "Do you Python"
	if [ $? -ne 0 ]; then
	    pkgtools__msg_error "UI error!"
	else
	    pkgtools__ui_user_yesno_is_yes
	    if [ $? -eq 0 ]; then
		pkgtools__msg_highlight_notice "I can guess what is your quest..."
	    else
		pkgtools__msg_warning "You don't know about the Meaning of Life!"
	    fi
	fi

	sint="12234"
	pkgtools__is_integer ${sint}
	if [ $? -ne 0 ]; then
	    pkgtools__msg_highlight_notice "${sint} matches an integer!"
	else
	    pkgtools__msg_warning "${sint} does not match an integer!"
	fi

	sint="-12234"
	pkgtools__is_integer ${sint}
	if [ $? -ne 0 ]; then
	    pkgtools__msg_highlight_notice "${sint} matches an integer!"
	else
	    pkgtools__msg_warning "${sint} does not match an integer!"
	fi

	sint="12+234"
	pkgtools__is_integer ${sint}
	if [ $? -ne 0 ]; then
	    pkgtools__msg_highlight_notice "${sint} matches an integer!"
	else
	    pkgtools__msg_warning "${sint} does not match an integer!"
	fi

	sint="aaaa12"
	pkgtools__is_integer ${sint}
	if [ $? -ne 0 ]; then
	    pkgtools__msg_highlight_notice "${sint} matches an integer!"
	else
	    pkgtools__msg_warning "${sint} does not match an integer!"
	fi

	sfloat="3.14259"
	pkgtools__is_float ${sfloat}
	if [ $? -ne 0 ]; then
	    pkgtools__msg_highlight_notice "${sfloat} matches a float!"
	else
	    pkgtools__msg_warning "${sfloat} does not match a float!"
	fi

	sfloat="1.6e-19"
	pkgtools__is_float ${sfloat}
	if [ $? -ne 0 ]; then
	    pkgtools__msg_highlight_notice "${sfloat} matches a float!"
	else
	    pkgtools__msg_warning "${sfloat} does not match a float!"
	fi

	sfloat="nan"
	sfloat="inf"
	sfloat="+inf"
	sfloat="-inf"
	pkgtools__is_float ${sfloat}
	if [ $? -ne 0 ]; then
	    pkgtools__msg_highlight_notice "${sfloat} matches a float!"
	else
	    pkgtools__msg_warning "${sfloat} does not match a float!"
	fi

	pkgtools__msg_error "This is a test error message!"

	__pkgtools__at_function_exit
	return 0
    }

fi

#    export __init_lib_pkgtools=1
#else
#    echo "WARNING: pkgtools library has already been loaded !" >&2
#fi

# end of libpkgtools.sh
