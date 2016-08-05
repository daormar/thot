# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import sys, getopt, codecs, itertools

import thot_smt_preproc as smtpr

##################################################
def print_help():
    print >> sys.stderr, "thot_decategorize -s <string> -t <string> -i <string> [--help]"
    print >> sys.stderr, ""
    print >> sys.stderr, "-s <string>    File with uncategorized source text"
    print >> sys.stderr, "-t <string>    File with target text to be decategorized"
    print >> sys.stderr, "-i <string>    File with hypothesis information extracted"
    print >> sys.stderr, "               from decoder's log file"
    print >> sys.stderr, "--help         Print this help message"

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
        print >> sys.stderr, "Error! -s parameter not given"
        sys.exit(2)

    if(t_given==False):
        print >> sys.stderr, "Error! -t parameter not given"
        sys.exit(2)

    if(i_given==False):
        print >> sys.stderr, "Error! -i parameter not given"
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
    print >> sys.stderr, "s is %s" % (sfilename)
    print >> sys.stderr, "t is %s" % (tfilename)
    print >> sys.stderr, "i is %s" % (hypfilename)

    # Read parallel files line by line
    running_words=0.0
    eddist=0.0
    for sline, tline, iline in itertools.izip(sfile,tfile,ifile):
        # Read source, target and hypothesis information
        sline=sline.strip("\n")
        tline=tline.strip("\n")
        iline=iline.strip("\n")

        decateg_line=smtpr.decategorize(sline,tline,iline)
        print decateg_line.encode("utf-8")

if __name__ == "__main__":
    main(sys.argv)
