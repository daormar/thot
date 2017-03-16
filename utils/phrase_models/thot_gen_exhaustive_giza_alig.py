# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import io, sys, getopt, itertools

##################################################
def print_help():
    print >> sys.stderr, "thot_gen_exhaustive_giza_alig -s <string> -t <string>"
    print >> sys.stderr, ""
    print >> sys.stderr, "-s <string>    File with source sentences"
    print >> sys.stderr, "-t <string>    File with target sentences"

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
        print >> sys.stderr, "Error! -s parameter not given"
        sys.exit(2)

    if(t_given==False):
        print >> sys.stderr, "Error! -t parameter not given"
        sys.exit(2)

    # open files
    if(s_given==True):
        # open file
        sfile = io.open(srcsents, 'r', encoding="utf-8")

    if(t_given==True):
        # open file
        tfile = io.open(trgsents, 'r', encoding="utf-8")

    # Read parallel files line by line
    for sline, tline in itertools.izip(sfile,tfile):
        # Read reference and test sentences
        sline=sline.strip("\n")
        src_word_array=sline.split()
        tline=tline.strip("\n")
        trg_word_array=tline.split()

        # Print entry
        print "# 1"
        print tline.encode("utf-8")
        positions_list=range(1,len(trg_word_array)+1)
        positions=' '.join(str(e) for e in positions_list)
        print "NULL ({ })",
        for i in range(len(src_word_array)):
            print src_word_array[i].encode("utf-8"),"({",positions,"})",
        print

if __name__ == "__main__":
    main(sys.argv)
