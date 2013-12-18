# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Randomly reorders the lines of a file given a seed

shuffle()
{
    # Initialize variables
    local seed=$1
    local file=$2

    # Shuffle file
    $AWK -v seed=$seed 'BEGIN{srand(seed)}{printf"%f %d %s\n",rand(),NR,$0}' $file \
        | $SORT -k1g -k2n | $AWK '{for(i=3;i<NF;++i) printf"%s ",$i; printf"%s\n",$NF}'
}

thot_shuffle_alt()
{
    # Alternative implementation (it has a higher spatial complexity)

    # Initialize variables
    local seed=$1
    local file=$2
    
    # Thot_Shuffle file
    $AWK -v seed=$seed \
        'function random(b) {return rand()*b}
  BEGIN{
         i=0
       }
       {
         # Store line of file
         line[i]=$0
         i=i+1
       }
    END{
         # Generate random numbers
         num_lines=NR
         srand(seed)
         for(i=0;i<num_lines;++i) 
         {
           vec_lines[i]=i
         }

         # Determine new ordering for the lines
         j=num_lines-1
         for(i=0;i<num_lines;++i) 
         {
           # Obtain next line number
           n=int(random(j+1))
           next_line_num=vec_lines[n]
           vec_lines[n]=vec_lines[j]
           j=j-1

           # Print line
           printf"%s\n",line[next_line_num]
         }

       }' $file
}

if [ $# -ne 1 -a $# -ne 2 ]; then
    echo "Usage: thot_shuffle <seed> [<file>]"
else

    # Take parameters
    seed=$1

    if [ $# -eq 2 ]; then
        file=$2
    fi

    # Invoke shuffling function
    shuffle $seed $file

fi
