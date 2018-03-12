# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Augments n-best list with a new column.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_remove_nbl_component written by Daniel Ortiz"
    echo "thot_remove_nbl_component rescore n-best list using an rnn-lm"
    echo "type \"thot_remove_nbl_component --help\" to get usage information"
}

########
usage()
{
    echo "thot_remove_nbl_component -c <string>"
    echo ""
    echo "-c <string>              Component name."
    echo ""
}

########
remove_nbl_component()
{
    $AWK -F " [|][|][|] " -v wname="${compname}w" \
     '{
       if(NR==1)
       {
         # Obtain weights, weight names and index of removed weight
         removed_weight_idx=-1
         numw=split($1,line_arr," , ")
         for(i=1;i<=numw;++i)
         {
          winfolen=split(line_arr[i],weight_info," ")
          weight[i]=weight_info[winfolen]
          weight_name[i]=weight_info[winfolen-1]
          if(weight_name[i]==wname)
            removed_weight_idx=i
         }
         # Print new header line for n-best list
         printf"#"
         for(i=1;i<=numw;++i)
         {
          if(i!=removed_weight_idx)
          {
           printf" %s %s",weight_name[i],weight[i]
           if(i==numw)
             is_last=1
           else
           { 
             if(i==numw-1 && removed_weight_idx==numw)
              is_last=1
             else
              is_last=0
           }
           if(!is_last)
            printf(" ,")
          }
         }
         printf"\n"
       }
       else
       {
         # Obtain new score
         numw=split($2,feat," ")
         score=0
         for(i=1;i<=numw;++i)
         {
           if(i!=removed_weight_idx)
            score+=weight[i]*feat[i]
         }
         # Print entry
         printf"%f |||",score
         for(i=1;i<=numw;++i)
         {
          if(i!=removed_weight_idx)
          {
            printf" %f",feat[i]
          }
         }
         printf" ||| %s",$3
         if(NF==4)
          printf" ||| %s",$4
         printf"\n"
       }
     }'
}

##################

if [ $# -eq 0 ]; then
    usage
    exit 1
fi

# Read parameters
c_given=0
n_given=0

while [ $# -ne 0 ]; do
    case $1 in
        "-c") shift
            if [ $# -ne 0 ]; then
                compname=$1
                c_given=1
            fi
            ;;
    esac
    shift
done

# Check parameters

if [ ${c_given} -eq 0 ]; then        
    echo "Error! -c parameter not given!" >&2
    exit 1
fi

# Process paramters

remove_nbl_component
