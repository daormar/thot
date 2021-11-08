# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import io, sys, getopt, itertools

##################################################
def print_help():
    print("thot_calc_wer -r <string> -t <string> [-v]", file=sys.stderr)
    print("", file=sys.stderr)
    print("-r <string>    File containing the reference sentences", file=sys.stderr)
    print("-t <string>    File containing the system translations", file=sys.stderr)
    print("-v             Verbose mode", file=sys.stderr)

##################################################
def levenshtein(s1, s2):
    # Always work with the longest string as the first input parameter
    if len(s1) < len(s2):
        return levenshtein(s2, s1)

    # len(s1) >= len(s2)
    if len(s2) == 0:
        return len(s1)

    # Initialize variables
    previous_row = list(range(len(s2) + 1))

    # Calculate edit distance
    for i, c1 in enumerate(s1):
        current_row = [i + 1]
        for j, c2 in enumerate(s2):
            insertions = previous_row[j + 1] + 1 # j+1 instead of j
                                                 # since previous_row
                                                 # and current_row are
                                                 # one character longer
                                                 # than s2
            deletions = current_row[j] + 1
            substitutions = previous_row[j] + (c1 != c2)
            current_row.append(min(insertions, deletions, substitutions))
        previous_row = current_row

    # Return result
    return previous_row[-1]

##################################################
def main(argv):
    # take parameters
    r_given=False
    t_given=False
    verbose=0
    filename = ""
    try:
        opts, args = getopt.getopt(sys.argv[1:],"r:t:v",["refsents=","syssents="])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    if(len(opts)==0):
        print_help()
        sys.exit()
    else:
        for opt, arg in opts:
            if opt in ("-r", "--refsents"):
                refsents = arg
                r_given=True
            elif opt in ("-t", "--syssents"):
                syssents = arg
                t_given=True
            elif opt in ("-v", "--verbose"):
                verbose=1

    # check parameters
    if(r_given==False):
        print("Error! -r parameter not given", file=sys.stderr)
        sys.exit(2)

    if(t_given==False):
        print("Error! -t parameter not given", file=sys.stderr)
        sys.exit(2)

    # open files
    if(r_given==True):
        # open file
        rfile = io.open(refsents, 'r', encoding="utf-8")

    if(t_given==True):
        # open file
        tfile = io.open(syssents, 'r', encoding="utf-8")

    # Read parallel files line by line
    running_words=0.0
    eddist=0.0
    for rline, tline in zip(rfile,tfile):
        # Read reference and test sentences
        rline=rline.strip("\n")
        ref_word_array=rline.split()
        tline=tline.strip("\n")
        test_word_array=tline.split()

        # Compute statistics
        running_words+=len(ref_word_array)
        sent_eddist=levenshtein(ref_word_array,test_word_array)
        eddist+=sent_eddist

        # Print verbose information
        if(verbose==1):
            sent_wer=float(sent_eddist)/float(len(ref_word_array))
            print(format(sent_wer,'g'),"|||",rline,"|||",tline)

    # Print WER
    WER=(eddist/running_words)
    print("WER:",format(WER,'g'),"(",format(eddist,'g'),"/",format(running_words,'g'),")")

if __name__ == "__main__":
    main(sys.argv)
