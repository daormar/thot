# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Simulates a CAT interaction given a test corpus using a translator client

# Functions

########
print_desc()
{
    echo "thot_cat_using_client written by Daniel Ortiz"
    echo "thot_cat_using_client performs a CAT process using thot_client"
    echo "type \"thot_cat_using_client --help\" to get usage information."
}

########
version()
{
    echo "thot_cat_using_client is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_cat_using_client -i <string> [-p <int>] [-uid <int>]"
    echo "                      -t <string> -r <string>"
    echo "                      [-tr]"
#    echo "                      [-tre]"
    echo "                      [-of] [-nc] [-pm <string>] [--help] [--version]"
    echo ""
    echo " -i <string>             : IP address of the server."
    echo " -p <int>                : Server port."
    echo " -uid <int>              : Set user id."
    echo " -t <string>             : File with the sentences to translate."
    echo " -r <string>             : File with reference sentences."
    echo " -tr                     : Train models after each translation."
    # echo " -tre                    : Train error correcting model after each"
    # echo "                           translation."
    echo " -of                     : Execute only the first iteration."
    echo " -nc                     : Do not clear server data at exit."
    echo " -pm <string>            : Print server models at exit using"
    echo "                           string as prefix."
    echo " --help                  : Display this help and exit."
    echo " --version               : Output version information and exit."
    echo ""
    echo "Warning: The character sets in which the test and reference sentences"
    echo "         are given should be taken into account in order to appropriately"
    echo "         set the value of the LC_ALL environment variable. Otherwise"
    echo "         the computer assisted translation process may fail due to"
    echo "         problems when manipulating strings."
}

########
pipe_fail()
{
    # test if there is at least one command to exit with a non-zero status
    for pipe_status_elem in ${PIPESTATUS[*]}; do 
        if test ${pipe_status_elem} -ne 0; then 
            echo "Error while executing script, see server output" >&2
            return 1; 
        fi 
    done
    return 0
}

########
verify_connection()
{
    h=`cat $TMPHYPERR | $GREP "connect() error"`
    if [ "${h}" = "connect() error" ]; then
        echo "ERROR: Connection lost!" >&2
        exit 1
    fi
}

########
print_header()
{
    echo "<?xml version=\"1.0\" encoding=\"$LANG\"?>"
    cpu=`cat /proc/cpuinfo | grep "model name" | awk -F : '{print $2}'`
    cpu=`echo $cpu | sed -e s"/^ //g"`
    cpumhz=`cat /proc/cpuinfo | grep "cpu MHz" | awk -F : '{print $2}'`
    cpumhz=`echo $cpumhz | sed -e s"/^ //g"`
    version=`cat /proc/version`
    echo "<Experiment CPU_Model=\"$cpu\" CPU_MHz=\"$cpumhz\" OS=\"$version\" TASK=\"??\" Source=\"$1\" Target=\"$2\">"
    echo ""
}

########
print_tail()
{
    KSR=`echo "" | $AWK -v ks=$KS -v chars=$CHARS '{printf "%.2f",(ks/chars)*100}'`
    tott=`sum_ab $tottz $tottgz`
    niter=`sum_ab $MAacc $ngiter`
    avgt=`div_ab $tott $niter`
    avgtz=`div_ab $tottz $MAacc`
    avgtgz=`div_ab $tottgz $ngiter`
    avgtrtime=`div_ab $tottrtime ${numSent}`
    echo "<!--"
    echo "KSMR (KS+MA+MAacc/CHARS): $KSMR ($KS+$MA+${MAacc}/$CHARS)"
    echo "KSR (KS/CHARS): $KSR ($KS/$CHARS)"
    echo "ITER=0 TIME (MIN/AVG/MAX/TOT): ${mintz}s/${avgtz}s/${maxtz}s/${tottz}s"
    echo "ITER>0 TIME (MIN/AVG/MAX/TOT): ${mintgz}s/${avgtgz}s/${maxtgz}s/${tottgz}s"
    echo "TIME (AVG/TOT): ${avgt}s/${tott}s"
    if [ ${tr_given} -eq 1 ]; then
        echo "AVG. ONLINE TRAINING TIME: $avgtrtime"
    fi
    echo "--!>"
    echo "</Experiment>"
}

########
process_time_from_log()
{
    cat $1 | grep "Request latency" | $AWK '{printf"%.6f",$3}'
}

########
sum_ab()
{
    echo "" | $AWK -v a=$1 -v b=$2 '{printf"%.3f",a+b}'
}
########
div_ab()
{
    echo "" | $AWK -v a=$1 -v b=$2 '{printf"%.3f",a/b}'
}
########
min_ab()
{
    echo "" | $AWK -v a=$1 -v b=$2 '{if(a<=b) {printf"%.3f",a} else {printf"%.3f",b}}'
}
########
max_ab()
{
    echo "" | $AWK -v a=$1 -v b=$2 '{if(a>=b) {printf"%.3f",a} else {printf"%.3f",b}}'
}
########
len_str()
{
    echo "$1" | $AWK '{printf"%d",length($0)}' 
}
########
extend_pref()
{
    echo "" | $AWK -v ref="$1" -v hyp="$2" '{
                                         refsize=split(ref,refa,"")
                                         hypsize=split(hyp,hypa,"")
                                         if(refsize>hypsize) size=hypsize
                                         else size=refsize
                                         # Find the first different character
                                         for(i=1;i<=size;++i)
                                         {
                                           if(refa[i]!=hypa[i])
                                           {
                                             break
                                           }
                                           else
                                           {
                                            if(i==1) outstr=refa[i]
                                            else outstr=sprintf("%s%s",outstr,refa[i])
                                           }
                                         }
                                         if(i<=size) outstr=sprintf("%s%s",outstr,refa[i])
                                         else
                                         {
                                           if(i<=refsize)
                                           {
                                            outstr=sprintf("%s%s",outstr,refa[i])
                                           }
                                         }
                                         printf"%s\n",outstr
                                        }'
}

########
suffix()
{
    echo "" | $AWK -v pr="$1" -v newpref="$2" '{printf"%s",substr(newpref,length(pr)+1)}'
}

########
is_pref()
{
    echo "" | $AWK -v a="$1" -v b="$2" '{if(a==substr(b,1,length(a))){printf"1"}else{printf"0"}}'
}

########
is_opt()
{
    echo $1 | grep "\-[a-zA-Z]" | wc -l
}

########
get_hyp_with_prompt()
{
    aux=`echo "" | $AWK -v newpref="$2" '{printf"%s",substr(newpref,1,length(newpref)-1)}'`
    aux_without_badsym=`echo "${aux}" | sed -e s'/\\\/<BSLASH>/g' | sed -e s"/\//<SLASH>/g" | sed -e s"/\[/<OSBRACKET>/g" | sed -e s"/\]/<CSBRACKET>/g"`
    hyp_without_badsym=`echo "$1" | sed -e s'/\\\/<BSLASH>/g' | sed -e s"/\//<SLASH>/g" | sed -e s"/\[/<OSBRACKET>/g" | sed -e s"/\]/<CSBRACKET>/g"`
    hyp_with_prompt=`echo "${hyp_without_badsym}" | sed -e s"/^${aux_without_badsym}/${aux_without_badsym}^/g"`
    echo "${hyp_with_prompt}" | sed -e s'/<BSLASH>/\\/g' | sed -e s"/<SLASH>/\//g" | sed -e s"/<OSBRACKET>/\[/g" | sed -e s"/<CSBRACKET>/\]/g"
}

####################### main

# Print command line to the error output
echo "Cmd. line: $0 $*" >&2

ip_given=0
uid_given=0
sents_given=0
refs_given=0
tr_given=0
# tre_given=0
of=0
nc_given=0
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
        "-tr") tr_given="1"
            ;;
        # "-tre") tre_given="1"
        #     ;;
        "-of") of="1"
            ;;
        "-nc") nc_given="1"
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

if [ ${refs_given} -eq 0 ]; then
    echo "Error: file with references not given">&2
    exit 1
else
    if [ ! -f  "${reffile}" ]; then
        echo "Error: file ${reffile} with references does not exist">&2
        exit 1
    fi
fi

# parameters are ok

# Disable file name generation using metacharacters (globbing)
# NOTE: globbing may produce unexpected results when invoking some
# functions used during the interactive machine translation process
set -f

# Determine whether to skip user interactions
if [ ${of} -eq 1 ]; then
    SKIP_FOLLOW_ITERS="yes"
else
    SKIP_FOLLOW_ITERS="no"
fi

# Set maximum number of iterations
MAX_ITERS=200

# Print output header
print_header $testfile $reffile

# Translate corpus
if [ -f $testfile -a -f $reffile ]; then
    numSent=0
    tmpdir=/tmp
    TMPTIME=`mktemp ${tmpdir}/time.XXXXXX`
    TMPHYP=`mktemp ${tmpdir}/hyp.XXXXXX`
    TMPHYPERR=`mktemp ${tmpdir}/hyperr.XXXXXX`
    TMPHYPCOV=`mktemp ${tmpdir}/hypcov.XXXXXX`
    TMPRETRANSHYP=`mktemp ${tmpdir}/retranshyp.XXXXXX`
    TMPTRTIME=`mktemp ${tmpdir}/trtime.XXXXXX`
    trap "rm $TMPTIME $TMPHYP $TMPHYPERR $TMPHYPCOV $TMPRETRANSHYP $TMPTRTIME 2>/dev/null" EXIT
    KS=0
    MA=0
    CHARS=0
    MAacc=0 
    tottz=0
    mintz=100000
    maxtz=0
    tottgz=0
    mintgz=100000
    maxtgz=0
    tottrtime=0
    ngiter=0

    # Read $testfile line by line
    while read -r s; do
        # Obtain reference sentence
        numSent=`expr $numSent + 1`
        r=`head -${numSent} $reffile | tail -1`

        # Initialize iteration number
        iter_num=1

        # Initial iteration
        $bindir/thot_client -i $ip ${port_op} ${uid_op} -sc "$s" -v >$TMPHYP 2>$TMPHYPERR
        verify_connection

        # Update variables
        h=`cat $TMPHYP`
        inittime=`process_time_from_log $TMPHYPERR`
        tottz=`sum_ab $tottz $inittime`
        mintz=`min_ab $mintz $inittime`
        maxtz=`max_ab $maxtz $inittime`

        # Print iteration information
        echo "<TestSample number=\"$numSent\" InitializationTime=\"$inittime\">"
        echo "<source>            $s </source>"
        echo "<reference>         $r </reference>"
        echo "<hyp time=\"0.000000\">  ^$h </hyp>"
        initial_hyp=h

        # following iterations
        if [ "${SKIP_FOLLOW_ITERS}" != "yes" ]; then
            KSsent=0
            MAsent=0
            prev=""
            ref_ispref_hyp=`is_pref "$r" "$h"`
            while [ ${ref_ispref_hyp} -eq 0 -a ${iter_num} -lt ${MAX_ITERS} ]; do
                # Update iteration number
                iter_num=`expr ${iter_num} + 1`

                # Compute new prefix and previous and new lenghts
                new_pref=`extend_pref "$r" "$h"`
                prevl=`len_str "$prev"`
                new_prefl=`len_str "${new_pref}"`

                # Compute contributions to error measures
                diffe=`expr ${new_prefl} - ${prevl}`
                if [ $diffe -gt 1 ]; then
                    MA=`expr $MA + 1`
                    MAsent=`expr $MAsent + 1`
                fi
                ngiter=`expr $ngiter + 1`
                KSsent=`expr $KSsent + 1`
                suff=`suffix "$prev" "${new_pref}"`
                
                # Append new string to the prefix
                $bindir/thot_client -i $ip ${port_op} ${uid_op} -ap "$suff" -v>$TMPHYP 2>$TMPHYPERR
                verify_connection

                # Update variables
                h=`cat $TMPHYP`
                hprompt=`get_hyp_with_prompt "$h" "${new_pref}"`
                ittime=`process_time_from_log $TMPHYPERR`
                tottgz=`sum_ab $tottgz $ittime`
                mintgz=`min_ab $mintgz $ittime`
                maxtgz=`max_ab $maxtgz $ittime`

                # Print iteration information
                echo "<hyp time=\"$ittime\">  ${hprompt} </hyp>"

                # Compute end condition
                prev="${new_pref}"
                ref_ispref_hyp=`is_pref "$r" "$h"`
            done
        fi

        # Print warning if maximum number of iterations was exceeded
        if [ ${iter_num} -ge ${MAX_ITERS} ]; then
            echo "WARNING: maximum number of iterations exceeded for sentence number ${numSent}" >&2
        fi

        # Compute error-related statistics (only if maximum number of
        # iterations not exceeded)
        if [ ${iter_num} -lt ${MAX_ITERS} ]; then
            C=`len_str "$r"`
            CHARS=`expr $CHARS + $C`
            MAacc=`expr ${MAacc} + 1`
            mac=`expr ${MAacc} + $MA`
            macsent=`expr ${MAsent} + 1`
            KS=`expr $KS + ${KSsent}`
            KSMR=`echo "" | $AWK -v ks=$KS -v ma=$MA -v maacc=$MAacc -v chars=$CHARS '{printf"%.2f",((ks+ma+maacc)/chars)*100}'`
            KSMsent=`expr ${KSsent} + ${macsent}`
        else
            C=""
            KSsent=""
            macsent=""
            KSMsent=""
        fi

        # Execute training or adaptation processes if requested
        # check -tr option
        if [ ${tr_given} -eq 1 ]; then
            # train models after each translation
            $bindir/thot_client -i $ip ${port_op} ${uid_op} -tr "$s" "$r" -v 2>$TMPHYPERR
            trtime=`process_time_from_log $TMPHYPERR`
            echo "<train time=\"$trtime\">"
            tottrtime=`sum_ab $tottrtime $trtime`
        fi

        # verify model coverage after training/adaptation (if they were
        # requested)
        if [ ${tr_given} -eq 1 ]; then
            $bindir/thot_client -i $ip ${port_op} ${uid_op} -c "$s" "$r" -v> $TMPHYPCOV 2>$TMPHYPERR
            hcov=`cat $TMPHYPCOV`
            vercovtime=`process_time_from_log $TMPHYPERR`
            echo "<hcov time=\"$vercovtime\"> $hcov </hcov>"
            
            # translate source sentence again 
            $bindir/thot_client -i $ip ${port_op} ${uid_op} -sc "$s" -v> $TMPRETRANSHYP 2>$TMPHYPERR
            retranshyp=`cat $TMPRETRANSHYP`
            echo "<retrans> $retranshyp </retrans>"
        fi

        # # check -tre option
        # if [ ${tre_given} -eq 1 ]; then
        #     # train ec model after each translation
        #     $bindir/thot_client -i $ip ${port_op} ${uid_op} -tre "${initial_hyp}" "$r" -v 2>$TMPHYPERR
        #     trtime=`process_time_from_log $TMPHYPERR`
        #     echo "<train ecm time=\"$trtime\">"    
        # fi

        # print information about the translation
        echo "</TestSample>"
        echo ""
        echo "<!--(PARTIAL) Sent: ${numSent} ; Chars: ${C} ; key strokes: ${KSsent} ; mouse actions: ${macsent} ; KS+MA: ${KSMsent} -->"
        echo "<!--(ACUM)    Sent: ${numSent} ; Chars: ${CHARS} ; key strokes: ${KS} ; mouse actions: ${mac} ; KSMR: $KSMR -->"
        echo ""
    done < $testfile

    # Print server models if required
    if [ ${pm_given} -eq 1 ]; then
        ${bindir}/thot_client -i $ip ${port_op} ${uid_op} -o ${pm_out_pref} -v
    fi
    
    # Clear structures in the server (if -nc option was given, server
    # structures are not cleared)
    if [ ${nc_given} -eq 0 ]; then
        $bindir/thot_client -i $ip ${port_op} ${uid_op} -v -clear
    else
        echo "WARNING: server data structures were not cleared" >&2
    fi

    # Print experiment tail
    print_tail

    # Return 0
    exit 0

else
    # Clear structures in the server
    $bindir/thot_client -i $ip ${port_op} ${uid_op} -v -clear

    # Return 1
    exit 1        
fi
