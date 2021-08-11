# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Translates a given corpus using a translator client

########
print_desc()
{
    echo "thot_smt_using_client written by Daniel Ortiz"
    echo "thot_smt_using_client performs a CAT process using thot_client"
    echo "type \"thot_smt_using_client --help\" to get usage information."
}

########
version()
{
    echo "thot_smt_using_client is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_smt_using_client -i <string> [-p <int>] [-uid <int>]"
    echo "                      -t <string> [-r <string>]"
    echo "                      [-pm <string>] [--help] [--version]"
    echo ""
    echo " -i <string>             : IP address of the server."
    echo " -p <int>                : Server port."
    echo " -uid <int>              : Set user id."
    echo " -t <string>             : File with the sentences to translate."
    echo " -r <string>             : File with reference sentences for online"
    echo "                           training."
    echo " -pm <string>            : Print server models at exit using"
    echo "                           string as prefix."
    echo " --help                  : Display this help and exit."
    echo " --version               : Output version information and exit."
    echo ""
    echo "Warning: The character sets in which the test and reference sentences"
    echo "         are given should be taken into account in order to appropriately"
    echo "         set the value of the LC_ALL environment variable."
}

########

# Print command line to the error output
echo "Cmd. line: $0 $*" >&2

ip_given=0
uid_given=0
sents_given=0
refs_given=0
pm_given=0

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-i") shift
            if [ $# -ne 0 ]; then
                ip=$1
                ip_given=1
            else
                ip_given=0
            fi
            ;;
        "-p") shift
            if [ $# -ne 0 ]; then
                port=$1
                port_op="-p ${port}"
            fi
            ;;
        "-uid") shift
            if [ $# -ne 0 ]; then
                uid=$1
                uid_op="-uid ${uid}"
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                testfile=$1
                sents_given=1
            fi
            ;;
        "-r") shift
            if [ $# -ne 0 ]; then
                reffile=$1
                refs_given=1
            fi
            ;;
        "-pm") shift
            if [ $# -ne 0 ]; then
                pm_out_pref=$1
                pm_given=1
            fi
            ;;
    esac
    shift
done

# verify parameters

if [ ${ip_given} -eq 0 ]; then
    echo "Error: ip address not given" >&2
    exit 1
fi

if [ ${sents_given} -eq 0 ]; then
    echo "Error: file with sentences not given">&2
    exit 1
else
    if [ ! -f  "${testfile}" ]; then
        echo "Error: file ${testfile} with test sentences does not exist">&2
        exit 1
    fi
fi

if [ ${refs_given} -eq 1 ]; then
    if [ ! -f  "${reffile}" ]; then
        echo "Error: file ${reffile} with references does not exist">&2
        exit 1
    fi
fi

# parameters are ok

# Translate corpus
numSent=0
while read -r s; do
    numSent=`expr $numSent + 1`

    if [ ${refs_given} -eq 1 ]; then
        r=`head -${numSent} "$reffile" | tail -1`
    fi

    # Translate sentence
    "$bindir"/thot_client -i $ip ${port_op} ${uid_op} -t "$s" || exit 1

    if [ ${refs_given} -eq 1 ]; then
        # train models after each translation
        "$bindir"/thot_client -i $ip ${port_op} ${uid_op} -tr "$s" "$r" || exit 1
    fi
done < $testfile

# Print server models if required
if [ ${pm_given} -eq 1 ]; then
    "${bindir}"/thot_client -i $ip ${port_op} ${uid_op} -o "${pm_out_pref}" || exit 1
fi
        
# Return 0
exit 0
