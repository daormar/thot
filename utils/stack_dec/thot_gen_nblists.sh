# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Utility to automate n-best list generation.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_gen_nblists written by Daniel Ortiz"
    echo "thot_gen_nblists generates n-best lists"
    echo "type \"thot_gen_nblists --help\" to get usage information"
}

########
version()
{
    echo "thot_gen_nblists is part of the exper package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_gen_nblists      [-pr <int>] -c <string> -s <string>"
    echo "                      [-n <int>] -o <string>"
    echo "                      [-qs <string>] [-tdir <string>] [-sdir <string>]"
    echo "                      [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>               Number of processors (1 by default)"
    echo "-c <string>             Configuration file"
    echo "-s <string>             File with source sentences"
    echo "-n <int>                Size of the n-best lists"
    echo "-o <string>             Output directory common to all processors."
    echo "-qs <string>            Specific options to be given to the qsub"
    echo "                        command (example: -qs \"-l pmem=1gb\")"
    echo "                        NOTES:"
    echo "                         a) ignore this if not using a PBS cluster"
    echo "                         b) -qs option may be crucial to ensure the correct"
    echo "                            execution of the tool. The main purpose of -qs"
    echo "                            is to reserve the required cluster resources."
    echo "                            If the necessary resources are not met the"
    echo "                            execution will abort."
    echo "                            Resources are reserved in different ways depending"
    echo "                            on the cluster software. For instance, if using SGE"
    echo "                            software, -qs \"-l h_vmem=1G,h_rt=10:00:00\","
    echo "                            requests 1GB of virtual memory and a time limit"
    echo "                            of 10 hours." 
    echo "-tdir <string>          Directory for temporary files (/tmp by default)."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-sdir <string>          Absolute path of a directory common to all"
    echo "                        processors. If not given, \$HOME will be used."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-debug                  After ending, do not delete temporary files"
    echo "                        (for debugging purposes)"
    echo "--help                  Display this help and exit."
    echo "--version               Output version information and exit."
    echo ""
    echo "NOTE: When executing the tool in PBS clusters, it is required that the"
    echo "      configuration file and all the files pointed by it are stored in"
    echo "      a place visible to all processors."
}

########
get_sentid()
{
    local_file=$1
    echo "${local_file}" | "$AWK" -F "." '{printf"%s",$1}' | "$AWK" -F "_" '{printf"%s",$2}'
}

########
obtain_nblists()
{
    # Generate translations and word graphs
    "$bindir"/thot_decoder -pr ${pr_val} -c "${cmdline_cfg}" -t "${scorpus}" \
        -o "${outd}"/wg/inputsent -wg "${outd}"/wg/devtrans \
        -sdir "$sdir" ${qs_opt} "${qs_par}" -v || { trap - EXIT ; return 1; }

    # Obtain n-best lists from word graphs
    for wgfile in "${outd}"/wg/devtrans*.wg; do
        basewgfile=`$BASENAME "$wgfile"`
        sentid=`get_sentid "${basewgfile}"`
        "${bindir}"/thot_wg_proc -w "$wgfile" -n ${n_val} \
            -o "${outd}"/nblist/inputsent_${sentid} 2>> "${outd}"/nblist/thot_wg_proc.log || return 1
    done

    # Save disk space (compress files)
    for file in "${outd}"/wg/devtrans*; do
        "${GZIP}" "$file"
    done
}

##################

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

# Read parameters
pr_given=0
pr_val=1
c_given=0
s_given=0
n_given=0
n_val=100
o_given=0
qs_given=0
tdir_given=0
tdir="/tmp"
sdir_given=0
sdir=$HOME
debug=0

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-pr") shift
            if [ $# -ne 0 ]; then
                pr_val=$1
                pr_given=1
            fi
            ;;
        "-c") shift
            if [ $# -ne 0 ]; then
                cmdline_cfg=$1
                c_given=1
            fi
            ;;
        "-s") shift
            if [ $# -ne 0 ]; then
                scorpus=$1
                s_given=1
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                n_val=$1
                n_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
            fi
            ;;
        "-qs") shift
            if [ $# -ne 0 ]; then
                qs_opt="-qs"
                qs_par="$1"
                qs_given=1
            else
                qs_given=0
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
                tdir_given=1
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1
                sdir_given=1
            fi
            ;;
        "-debug") debug=1
            debug_opt="-debug"
            ;;
    esac
    shift
done

# Check parameters
if [ ${c_given} -eq 0 ]; then
    echo "Error! -c parameter not given" >&2
    exit 1
else
    if [ ! -f "${cmdline_cfg}" ]; then
        echo "Error! file ${cmdline_cfg} does not exist" >&2
        exit 1
    fi
fi

if [ ${s_given} -eq 0 ]; then
    echo "Error! -s parameter not given!" >&2
    exit 1
else
    if [ ! -f "${scorpus}" ]; then
        echo "Error! file ${scorpus} does not exist" >&2
        exit 1
    fi
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d "${outd}" ]; then
        echo "Warning! output directory does exist" >&2 
        # echo "Error! output directory should not exist" >&2 
        # exit 1
    else
        mkdir -p "${outd}" || { echo "Error! cannot create output directory" >&2; return 1; }
    fi
fi

if [ ${tdir_given} -eq 1 ]; then
    if [ ! -d "${tdir}" ]; then
        echo "Error! directory ${tdir} does not exist" >&2
        exit 1            
    fi
fi

if [ ${sdir_given} -eq 1 ]; then
    if [ ! -d "${sdir}" ]; then
        echo "Error! directory ${sdir} does not exist" >&2
        exit 1            
    fi
fi

# Create additional directories
mkdir "${outd}"/wg
mkdir "${outd}"/nblist

# Obtain n-best lists
obtain_nblists || exit 1
