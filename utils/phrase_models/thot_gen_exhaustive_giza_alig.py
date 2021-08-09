# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import io, sys, getopt, itertools

##################################################
def print_help():
    print("thot_gen_exhaustive_giza_alig -s <string> -t <string>", file=sys.stderr)
    print("", file=sys.stderr)
    print("-s <string>    File with source sentences", file=sys.stderr)
    print("-t <string>    File with target sentences", file=sys.stderr)

##################################################
def main(argv):
    # take parameters
    s_given=False
    t_given=False
    verbose=0
    filename = ""
    try:
        opts, args = getopt.getopt(sys.argv[1:],"s:t:v",["srcsents=","syssents="])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    if(len(opts)==0):
        print_help()
        sys.exit()
    else:
        for opt, arg in opts:
            if opt in ("-s", "--srcsents"):
                srcsents = arg
                s_given=True
            elif opt in ("-t", "--trgsents"):
                trgsents = arg
                t_given=True

    # check parameters
    if(s_given==False):
        print("Error! -s parameter not given", file=sys.stderr)
        sys.exit(2)

    if(t_given==False):
        print("Error! -t parameter not given", file=sys.stderr)
        sys.exit(2)

    # open files
    if(s_given==True):
        # open file
        sfile = io.open(srcsents, 'r', encoding="utf-8")

    if(t_given==True):
        # open file
        tfile = io.open(trgsents, 'r', encoding="utf-8")

    # Read parallel files line by line
    for sline, tline in zip(sfile,tfile):
        # Read reference and test sentences
        sline=sline.strip("\n")
        src_word_array=sline.split()
        tline=tline.strip("\n")
        trg_word_array=tline.split()

        # Print entry
        print("# 1")
        print(tline.encode("utf-8"))
        positions_list=list(range(1,len(trg_word_array)+1))
        positions=' '.join(str(e) for e in positions_list)
        print("NULL ({ })", end=' ')
        for i in range(len(src_word_array)):
            print(src_word_array[i].encode("utf-8"),"({",positions,"})", end=' ')
        print()

if __name__ == "__main__":
    main(sys.argv)
