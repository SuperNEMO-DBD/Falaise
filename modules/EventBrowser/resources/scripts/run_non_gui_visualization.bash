#!/usr/bin/env bash

if [ "x${SNVISUALIZATION_BIN_DIR}" = "x" ]; then
    echo "ERROR: SNVISUALIZATION_BIN_DIR is not defined !" >&2
    exit 0
fi

appname="run_non_gui_visualization.bash"
opwd=$(pwd)

function my_exit ()
{
    cd ${opwd}
    local error_status=$1
    if [ "x${error_status}" = "x" ]; then
	error_status=0
    fi
    if [ "x${error_status}" = "x0" ]; then
	echo "NOTICE: ${appname}: The script has terminated successfully." >&2
    else
	echo "ERROR: ${appname}: The script has terminated with error(s) !" >&2
    fi
    echo "NOTICE: Bye !" >&2
    exit ${error_status}
}

function print_help ()
{
    cat<<EOF

run_non_gui_visualization.bash -- Run SuperNEMO Visualization Tool wthout GUI

Usage:

  \${SNVISUALIZATION_DATA_DIR}/resources/scripts/run_non_gui_visualization.bash [OPTIONS...]

Options:

  --verbose : verbose mode

  --debug   : debug mode

  --input-files, -i <ARCHIVE PATH> : set the path to archive files

  --nevents <NB oF EVENTS> : set the number of events to be visualized

  --canvas-width  <WIDTH>   : set the canvas width
  --canvas-height <HEIGHT>  : set the canvas height

  --scratch-dir <PATH> : set the name of the working/storage directory (default /tmp/snvisu.d)

  --pdf-file    <PATH> : set the name of the produced PDF directory (default /tmp/snvisu.d/snvisu.pdf)

Examples:

  A test run :

   \${SNVISUALIZATION_DATA_DIR}/resources/scripts/run_non_gui_visualization.bash -i ${SNCORE_ROOT}/resources/snemo/samples/snemo_SD_CD_0.txt.gz

  A more detailed example :

   \${SNVISUALIZATION_DATA_DIR}/resources/scripts/run_non_gui_visualization.bash \\
     --focus-on-roi \\
     --nevents 10 \\
     --scratch-dir /tmp/${USER}/snvisualization/img \\
     --input-files ${SNCORE_ROOT}/resources/snemo/samples/snemo_SD_CD_0.txt.gz


EOF

    return
}

# Default settings :
help=0

unregistered_option=
tmpdir=/tmp/snvisu.d
keep_processed_events=0

pdf_file=${tmpdir}/snvisu.pdf

while [ -n "$1" ]; do
    token="$1"
    if [ "x${token:0:1}" = "x-" ]; then
	option=${token}
        if [ ${option} = "--help" ]; then
	    help=1
        elif [ ${option} = "--keep-processed-events" ]; then
	    keep_processed_events=1
	elif [ ${option} = "--scratch-dir" ]; then
	    shift 1
	    tmpdir="$1"
	elif [ ${option} = "--pdf-file" ]; then
	    shift 1
	    pdf_file="$1"
	else
            unregistered_option+="${option} "
	fi
    else
	unregistered_option+="${token} "
    fi
    shift 1
done

if [ ${help} -eq 1 ]; then
    print_help
    my_exit 0
fi

if [ ! -d ${tmpdir} ]; then
    echo "NOTICE: ${appname}: Creating the scratch dir='${tmpdir}' directory !" >&2
    mkdir -p ${tmpdir}
    if [ $? -ne 0 ]; then
	echo "ERROR: ${appname}: Cannot create directory '${tmpdir}' ! Abort !" >&2
	my_exit 1
    fi
fi
echo "NOTICE: ${appname}: Scratch working directory : '${tmpdir}'" >&2

################################################################################################

function produce_pdf_file ()
{
    local tex_file=${pdf_file/.pdf/.tex}
    cat << EOF > ${tex_file}
\documentclass{article}

\usepackage[english]{babel}
\usepackage{helvet}
\usepackage{a4wide}
\usepackage{graphicx}

\graphicspath{{${tmpdir}}}

\renewcommand{\tabcolsep}{2pt}
\renewcommand{\arraystretch}{1}

\begin{document}
\pagestyle{empty}
\sf
\hspace{-3.5cm}
\begin{tabular}{cc}
EOF

    local i_img=1
    local i_page=1
    for img in $(ls -rt *.eps); do
        cat << EOF >> ${tex_file}
\includegraphics[scale=0.5]{${img/.eps/}}
EOF
        let "remainder = 4*${i_page} - ${i_img}"

        if [ ${remainder} -eq 0 ]; then
            cat << EOF >> ${tex_file}
\end{tabular}
\newpage
\hspace{-3.5cm}
\begin{tabular}{cc}
EOF
            let "i_page++"
        elif [ ${remainder} -eq 2 ]; then
            cat << EOF >> ${tex_file}
\\\\
EOF
        else
            cat << EOF >> ${tex_file}
&
EOF
        fi

        let "i_img++"
    done

    cat << EOF >> ${tex_file}
\end{tabular}
\end{document}
EOF

    latex ${tex_file} > ${logfile} 2>&1
    dvipdfm ${tex_file/.tex/} > ${logfile} 2>&1
    # pdflatex ${tex_file}
    return 0
}


################################################################################################

echo "NOTICE: ${appname}: Change to the working directory..." >&2
cd ${tmpdir}
logfile="${tmpdir}/snemo_non_gui_visualization.log"

# Processing events within 'non_gui_event_browser' :
# 2012-11-29 XG: The '=' symbol between 'scratch_dir' and ${tmpdir} is
# really important because without it, the program will think
# ${tmpdir} is an input file (due to the use of positional args in
# boost::program_options)
echo "NOTICE: ${appname}: Running the SuperNEMO event browser... this can take a while..." >&2
${SNVISUALIZATION_BIN_DIR}/snemo_non_gui_browser \
    --scratch-dir=${tmpdir} ${unregistered_option}   \
    > ${logfile} 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: ${appname}: Something failed ! Have a look in ${logfile} log file" 1>&2
    my_exit 1
fi
echo "NOTICE: ${appname}: Pipeline is finished." >&2

# Collect events view in one PDF
echo "NOTICE: ${appname}: Producing the corresponding PDF file... this can (also) take a while..." >&2
produce_pdf_file
echo "NOTICE: ${appname}: PDF file done." >&2

if [ ${keep_processed_events} -eq 1 ]; then
    which tree > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        tree ${tmpdir}/
    fi
else
    rm -f ${tmpdir}/*.eps
fi

cat<<EOF
-------------------------------------------------------------------------------------------
Report for : \${SNVISUALIZATION_DATA_DIR}/resources/scripts/run_non_gui_visualization.bash
-------------------------------------------------------------------------------------------

Log file is:

  ${logfile}

EOF

cat<<EOF

To see the output PDF, please open ${pdf_file} with your favorite PDF viewer

-------------------------------------------------------------------------------------------

EOF
my_exit 0

# end
