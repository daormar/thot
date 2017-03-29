# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Augments n-best list with a new column.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_augment_nblist written by Daniel Ortiz"
    echo "thot_augment_nblist rescore n-best list using an rnn-lm"
    echo "type \"thot_augment_nblist --help\" to get usage information"
}

########
usage()
{
    echo "thot_augment_nblist     -n <string> -a <string> -s <string>"
    echo ""
    echo "-n <string>             n-best list file."
    echo "-a <string>             Weight name."
    echo "-s <string>             File with scores."
    echo ""
}

########
augment_nblist()
{
    $AWK -F " [|][|][|] " -v wname=$wname -v scrfile=$scrfile 'BEGIN{
        nline=1
        while((getline < scrfile) > 0)
        {
         scr[nline]=$1
         ++nline
        }
     }
     {
       if(NR==1)
       {
         numw=split($1,line_arr,",")
         for(i=1;i<=numw;++i)
         {
          winfolen=split(line_arr[i],weight_info," ")
          weight[i]=weight_info[winfolen]
         }
         printf"%s , %s 1\n",$1,wname
       }
       else
       {
         numw=split($2,feat," ")
         score=0
         for(i=1;i<=numw;++i)
           score+=weight[i]*feat[i]
         rnnlm_scr=scr[NR-1]
         score+=rnnlm_scr
         printf"%f ||| %s %s ||| %s\n",score,$2,rnnlm_scr,$3
       }
     }' $nblistf
}

##################

if [ $# -eq 0 ]; then
    usage
    exit 1
fi

# Read parameters
n_given=0
a_given=0
s_given=0

while [ $# -ne 0 ]; do
    case $1 in
        "-n") shift
            if [ $# -ne 0 ]; then
                nblistf=$1
                n_given=1
            fi
            ;;
        "-a") shift
            if [ $# -ne 0 ]; then
                wname=$1
                a_given=1
            fi
            ;;
        "-s") shift
            if [ $# -ne 0 ]; then
                scrfile=$1
                s_given=1
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
    if [ ! -f ${nblistf} ]; then
        echo "Error! file ${nblistf} does not exist" >&2
        exit 1
    fi
fi

if [ ${a_given} -eq 0 ]; then
    echo "Error! -a parameter not given!" >&2
    exit 1
fi

if [ ${s_given} -eq 0 ]; then
    echo "Error! -s parameter not given!" >&2
    exit 1
else
    if [ ! -f ${scrfile} ]; then
        echo "Error! file ${scrfile} does not exist" >&2
        exit 1
    fi
fi

# Process paramters

augment_nblist
