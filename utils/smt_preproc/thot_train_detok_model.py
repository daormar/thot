# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import io, sys, getopt

import thot_smt_preproc as smtpr

##################################################
def print_help():
    print >> sys.stderr, "thot_train_detok_model -r <string> [-t <string>] [-n <int>]"
    print >> sys.stderr, "                       -o <string> [-v] [--help]"
    print >> sys.stderr, ""
    print >> sys.stderr, "-r <string>    File with raw text in the language of interest"
    print >> sys.stderr, "-t <string>    File with tokenized version of the raw text using"
    print >> sys.stderr, "               an arbitrary tokenizer"
    print >> sys.stderr, "-n <int>       Order of n-grams for language model"
    print >> sys.stderr, "-o <string>    Prefix of output files"
    print >> sys.stderr, "-v             Verbose mode"
    print >> sys.stderr, "--help         Print this help message"

##################################################
def main(argv):
    # take parameters
    r_given=False
    rfilename = ""
    t_given=False
    tfilename = ""
    n_given=False
    nval=3
    o_given=False
    opref= ""
    verbose=False
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hr:t:n:o:v",["help","rawfn=","tokfn=","nval=","opref="])
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
            elif opt in ("-r", "--rawfn"):
                rfilename = arg
                r_given=True
            elif opt in ("-t", "--tokfn"):
                tfilename = arg
                t_given=True
            elif opt in ("-n", "--nval"):
                nval = int(arg)
                n_given=True
            elif opt in ("-o", "--opref"):
                opref = arg
                o_given=True
            elif opt in ("-v", "--verbose"):
                verbose=True

    # check parameters
    if(r_given==False):
        print >> sys.stderr, "Error! -r parameter not given"
        sys.exit(2)

    if(o_given==False):
        print >> sys.stderr, "Error! -o parameter not given"
        sys.exit(2)

    # print parameters
    if(r_given==True):
        print >> sys.stderr, "r is %s" % (rfilename)

    if(t_given==True):
        print >> sys.stderr, "t is %s" % (tfilename)

    if(n_given==True):
        print >> sys.stderr, "n is %d" % (nval)

    if(o_given==True):
        print >> sys.stderr, "o is %s" % (opref)

    # open files
    if(r_given==True):
        # open file
        rfile = io.open(rfilename, 'r', encoding="utf-8")

    if(t_given==True):
        # open file
        tfile = io.open(tfilename, 'r', encoding="utf-8")

    # train translation model
    print >> sys.stderr, "Training translation model..."
    tmodel=smtpr.TransModel()
    if(t_given==True):
        tmodel.train_tok_tm_par_files(rfile,tfile,verbose)
    else:
        tmodel.train_tok_tm(rfile,verbose)

    # print translation model
    tmfile = io.open(opref+".tm", 'w', encoding='utf-8')
    tmodel.print_model_to_file(tmfile)

    # reopen files
    rfile.close()
    rfile = io.open(rfilename, 'r', encoding="utf-8")

    if(t_given==True):
        tfile.close()
        tfile = io.open(tfilename, 'r', encoding="utf-8")

    # train language model
    print >> sys.stderr, "Training language model..."
    lmodel=smtpr.LangModel()
    if(t_given==True):
        lmodel.train_tok_lm_par_files(rfile,tfile,nval,verbose)
    else:
        lmodel.train_tok_lm(rfile,nval,verbose)

    # print language model
    lmfile = io.open(opref+".lm", 'w', encoding='utf-8')
    lmodel.print_model_to_file(lmfile)

if __name__ == "__main__":

    # Call main function
    main(sys.argv)
