# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import sys, getopt, codecs
import itertools

##################################################
def print_help():
    print >> sys.stderr, "thot_clean_corpus_ln -s <string> -t <string> [-i <int>] [-a <int>]"
    print >> sys.stderr, "                     [-d <int>] [--help]"
    print >> sys.stderr, ""
    print >> sys.stderr, "-s <string>    File with source text"
    print >> sys.stderr, "-t <string>    File with target text"
    print >> sys.stderr, "-i <int>       Minimum sentence length (1 by default)"
    print >> sys.stderr, "-a <int>       Maximum sentence length (80 by default)"
    print >> sys.stderr, "-d <int>       Maximum difference between source and target sentence length"
    print >> sys.stderr, "               (15 by default)"
    print >> sys.stderr, "--help         Print this help message"

##################################################
def main(argv):
    # take parameters
    minlen=1
    maxlen=80
    maxdiff=15
    s_given=False
    t_given=False
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hs:t:i:a:d:",["help","srcfn=","trgfn="])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print_help()
            sys.exit()
        elif opt in ("-s", "--srcfn"):
            srcfn = arg
            s_given=True
        elif opt in ("-t", "--trgfn"):
            trgfn = arg
            t_given=True
        elif opt in ("-t", "--trgfn"):
            trgfn = arg
            t_given=True
        elif opt in ("-i", "--minl"):
            minlen = int(arg)
        elif opt in ("-a", "--maxl"):
            maxlen = int(arg)
        elif opt in ("-d", "--md"):
            maxdiff = int(arg)

    # check parameters
    if(s_given==False):
        print >> sys.stderr, "Error! -s parameter not given"
        sys.exit(2)

    if(t_given==False):
        print >> sys.stderr, "Error! -t parameter not given"
        sys.exit(2)

    # print parameters
    if(s_given==True):
        print >> sys.stderr, "s is %s" % (srcfn)

    if(t_given==True):
        print >> sys.stderr, "t is %s" % (srcfn)

    print >> sys.stderr, "i is %d" % (minlen)
    print >> sys.stderr, "a is %d" % (maxlen)
    print >> sys.stderr, "d is %d" % (maxdiff)

    # open file
    if(s_given==True):
        # open file
        srcfile = codecs.open(srcfn, 'r', "utf-8")

    if(t_given==True):
        # open file
        trgfile = codecs.open(trgfn, 'r', "utf-8")

    # read parallel files line by line
    lineno=1
    for srcline, trgline in itertools.izip(srcfile,trgfile):
        srcline=srcline.strip("\n")
        src_word_array=srcline.split()
        trgline=trgline.strip("\n")
        trg_word_array=trgline.split()

        # Initialize length-related variables
        slen=len(src_word_array)
        tlen=len(trg_word_array)
        if(slen>tlen):
            difflen=slen-tlen
        else:
            difflen=tlen-slen

        # Check whether current sentence pair appears to be ok or not
        if (slen>=minlen and tlen>=minlen and slen<=maxlen and tlen<=maxlen and difflen<=maxdiff):
            print lineno
        else:
            print >> sys.stderr, lineno,srcline.encode("utf-8"),"<->",trgline.encode("utf-8")

        # Increase lineno
        lineno=lineno+1

if __name__ == "__main__":
    main(sys.argv)
