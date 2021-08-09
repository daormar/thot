# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import sys, getopt, codecs, itertools

import thot_smt_preproc as smtpr

##################################################
def print_help():
    print("thot_decategorize -s <string> -t <string> -i <string> [--help]", file=sys.stderr)
    print("", file=sys.stderr)
    print("-s <string>    File with uncategorized source text", file=sys.stderr)
    print("-t <string>    File with target text to be decategorized", file=sys.stderr)
    print("-i <string>    File with hypothesis information extracted", file=sys.stderr)
    print("               from decoder's log file", file=sys.stderr)
    print("--help         Print this help message", file=sys.stderr)

##################################################
def main(argv):
    # take parameters
    s_given=False
    t_given=False
    i_given=False
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hs:t:i:",["help","srcsents=","trgsents=","hypinfo="])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    if(len(opts)==0):
        print_help()
        sys.exit()
    else:
        for opt, arg in opts:
            if opt in ("-h", "--help"):
                print_help()
                sys.exit()
            elif opt in ("-s", "--srcsents"):
                sfilename = arg
                s_given=True
            elif opt in ("-t", "--trgsents"):
                tfilename = arg
                t_given=True
            elif opt in ("-i", "--hypinfo"):
                hypfilename = arg
                i_given=True

    # check parameters
    if(s_given==False):
        print("Error! -s parameter not given", file=sys.stderr)
        sys.exit(2)

    if(t_given==False):
        print("Error! -t parameter not given", file=sys.stderr)
        sys.exit(2)

    if(i_given==False):
        print("Error! -i parameter not given", file=sys.stderr)
        sys.exit(2)

    # open files
    if(s_given==True):
        # open file
        sfile = codecs.open(sfilename, 'r', "utf-8")

    if(t_given==True):
        # open file
        tfile = codecs.open(tfilename, 'r', "utf-8")

    if(i_given==True):
        # open file
        ifile = codecs.open(hypfilename, 'r', "utf-8")

    # print parameters
    print("s is %s" % (sfilename), file=sys.stderr)
    print("t is %s" % (tfilename), file=sys.stderr)
    print("i is %s" % (hypfilename), file=sys.stderr)

    # Read parallel files line by line
    running_words=0.0
    eddist=0.0
    for sline, tline, iline in zip(sfile,tfile,ifile):
        # Read source, target and hypothesis information
        sline=sline.strip("\n")
        tline=tline.strip("\n")
        iline=iline.strip("\n")

        decateg_line=smtpr.decategorize(sline,tline,iline)
        print(decateg_line.encode("utf-8"))

if __name__ == "__main__":
    main(sys.argv)
