# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Obtains a length histogram of a given corpus file from 1 to lmax

corpus_length()
{
    local corpus=$1

    "$AWK" -v corpus="${corpus}" 'BEGIN{ 
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
                           }' "${corpus}"
}

if [ $# -lt 1 ]; then
    echo "Usage: thot_corpus_length -c <string>"
    echo ""
    echo "-c <string> : corpus to be processed"
else
    # Read parameters
    c_given=0
    while [ $# -ne 0 ]; do
        case $1 in
        "-c") shift
            if [ $# -ne 0 ]; then
                corpus=$1
                c_given=1
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
    corpus_length "$corpus"

fi
