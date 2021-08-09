# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Randomly reorders the lines of a file given a seed

shuffle()
{
    # Initialize variables
    _seed=$1
    _tdir=$2
    _file=$3

    # Shuffle file
    "$AWK" -v seed=${_seed} 'BEGIN{srand(seed)}{printf"%f %d %s\n",rand(),NR,$0}' ${_file} \
        | LC_ALL=C "$SORT" -k1n -k2n -T "${_tdir}" | "$AWK" '{for(i=3;i<NF;++i) printf"%s ",$i; printf"%s\n",$NF}'
}

thot_shuffle_alt()
{
    # Alternative implementation (it has a higher spatial complexity)

    # Initialize variables
    local seed=$1
    local file=$2
    
    # Thot_Shuffle file
    "$AWK" -v seed=$seed \
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

if [ $# -ne 2 -a $# -ne 3 ]; then
    echo "Usage: thot_shuffle <seed> <tmpdir> [<file>]"
else

    # Take parameters
    seed=$1
    tdir=$2

    if [ $# -eq 3 ]; then
        file=$3
    fi

    # Invoke shuffling function
    shuffle $seed "$tdir" "$file"

fi
