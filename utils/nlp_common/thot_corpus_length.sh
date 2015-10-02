# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Obtains a length histogram of a given corpus file from 1 to lmax

corpus_length()
{
    local_corpus=$1
    local_lmax=$2

    $AWK -v corpus=${local_corpus} -v lmax=${local_lmax} 'BEGIN{ 
                            while( (getline <corpus) > 0)
                            {
                             if(lmax<NF) lmax=NF;
                            }
                            for(i=0;i<=lmax;++i)
                              frec[i]=0
                            total=0
                            total_l=0.0
                            acum=0
                           }
                           {
                            frec[NF]=frec[NF]+1
                            total=total+1
                            total_l=total_l+NF
                           }
                       END { 
                            printf "#   l  freq    %%    cumf   cum%%\n"
                            for(i=0;i<=lmax;++i) 
                            {
                             acum+=frec[i]
                             printf "  %3d %5d %4.1f %7d  %5.1f\n",i,frec[i],(frec[i]/total)*100,acum,(acum/total)*100
                            }
                            printf "\n# Total: %d\n",total
                            printf "# Mean length: %f\n\n",total_l/total
                           }' ${local_corpus}
}

if [ $# -lt 1 ]; then
    echo "Usage: thot_corpus_length -c <string> [-lmax <int>]"
    echo ""
    echo "-c <string> : corpus to be processed"
    echo "-lmax <int> : maximum length (100 by default)"
else
    # Read parameters
    c_given=0
    lmax_given=0
    lmax=100
    while [ $# -ne 0 ]; do
        case $1 in
        "-c") shift
            if [ $# -ne 0 ]; then
                corpus=$1
                c_given=1
            fi
            ;;
        "-lmax") shift
            if [ $# -ne 0 ]; then
                lmax=$1
                lmax_given=1
            fi
            ;;
        esac
        shift
    done

    # Check parameters
    if [ ${c_given} -eq 0 ]; then
        echo "Error! -c parameter not given" >&2
        exit 1
    fi

    # Process input parameters
    corpus_length $corpus $lmax    

fi
