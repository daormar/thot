# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Filter those entries n-best list file not satisfying translation
# constraints.

########
print_desc()
{
    echo "thot_filter_nblist written by Daniel Ortiz"
    echo "thot_filter_nblist filters entries in n-best list file"
    echo "type \"thot_filter_nblist --help\" to get usage information"
}

########
version()
{
    echo "thot_filter_nblist is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_filter_nblist   -n <string> -p <string> [--help] [--version]"
    echo ""
    echo "-n <string>          File containing n-best list."
    echo "-p <string>          Prefix of .wg and .idx files."
    echo "--help               Display this help and exit."
    echo "--version            Output version information and exit."
    echo ""
}

########
get_src_sent_with_metadata()
{
    _idxfile=$1
    "$HEAD" -1 "${_idxfile}" | "$AWK" '{for(i=4;i<=NF;++i){printf "%s",$i; if(i!=NF) printf" "} printf"\n"}'
}

########
get_trg_sents()
{
    _nblfile=$1
    "$AWK" -F " [|][|][|] " '{if(FNR>1) print $4}' "${_nblfile}"
}

########
get_alig_info()
{
    _segm_info=$1
    "$AWK" -F " [|][|][|] " '{if(FNR>1) print $NF}' "${_segm_info}"
}

########
filter_nblist()
{
    _nblfile=$1
    _checkinginfo=$2

    "$AWK" -v chkinfo="${_checkinginfo}" 'BEGIN{
      nline=1
      while((getline < chkinfo) > 0)
      {
        if($0=="CONSTRAINTS NOT SATISFIED")
          constr_satisfied[nline]=0
        else
          constr_satisfied[nline]=1
        ++nline
      }
    }
    {
      if(FNR==1)
        print $0
      else
      {
        if(constr_satisfied[FNR-1]==1)
          print $0        
      }
    }' "${_nblfile}"
}

##################

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

# Read parameters
n_given=0
p_given=0

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                nblfile=$1
                n_given=1
            fi
            ;;
        "-p") shift
            if [ $# -ne 0 ]; then
                prefix=$1
                p_given=1
            fi
            ;;
    esac
    shift
done

# Check parameters
if [ ${n_given} -eq 0 ]; then
    echo "Error! -n parameter not given!" >&2
    exit 1
else
    if [ ! -f "${nblfile}" ]; then
        echo "Error! file ${nblfile} does not exist" >&2
        exit 1
    fi
fi

if [ ${p_given} -eq 0 ]; then
    echo "Error! -p parameter not given!" >&2
    exit 1
else
    if [ ! -f "${prefix}.wg" ]; then
        echo "Error! file ${prefix}.wg does not exist" >&2
        exit 1
    fi

    if [ ! -f "${prefix}.idx" ]; then
        echo "Error! file ${prefix}.idx does not exist" >&2
        exit 1
    fi
fi

TDIR=`"${MKTEMP}" -d /tmp/thot_filter_nblist_XXXXXX`

# Remove temp directories on exit
trap 'rm -rf "$TDIR" 2>/dev/null' EXIT

# Process parameters

# Obtain segmentation info for n-best list entries
"${bindir}"/thot_get_nblist_segm_info -n "${nblfile}" -p "${prefix}" > "$TDIR"/segm_info

# Obtain source sentence with metadata
get_src_sent_with_metadata "${prefix}".idx > "$TDIR"/srcsent

# Obtain target sentences
get_trg_sents "${nblfile}" > "$TDIR"/trgsents

# Obtain alignment info
get_alig_info "$TDIR"/segm_info > "$TDIR"/aliginfo

# Obtain constraint checking info
"${bindir}"/thot_check_constraints -s "$TDIR"/srcsent -t "$TDIR"/trgsents -a "$TDIR"/aliginfo > "$TDIR"/checkinginfo

# Filter n-best list file
filter_nblist "${nblfile}" "$TDIR"/checkinginfo
