# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import sys, getopt, nltk, codecs

import thot_smt_preproc as smtpr

##################################################
def print_help():
    print >> sys.stderr, "thot_rec_translator -f <string> -m <string> [-i <float>]"
    print >> sys.stderr, "                    [-w \"<float1> ... <float4>\"] [-v] [--help]"
    print >> sys.stderr, ""
    print >> sys.stderr, "-f <string>    File with text to be recased (can be read from standard"
    print >> sys.stderr, "               input)"
    print >> sys.stderr, "-m <string>    Prefix of recaser model files"
    print >> sys.stderr, "-i <float>     Interpolation probability for language model"
    print >> sys.stderr, "-w \"...\"       Weight vector of length four:"
    print >> sys.stderr, "                <float1> : translation model weight"
    print >> sys.stderr, "                <float2> : phrase penalty model weight"
    print >> sys.stderr, "                <float3> : word penalty weight"
    print >> sys.stderr, "                <float4> : language model weight"
    print >> sys.stderr, "-v             Verbose mode"
    print >> sys.stderr, "--help         Print this help message"

##################################################
def main(argv):

    # take parameters
    f_given=False
    filename = ""
    m_given=False
    mpref = ""
    i_given=False
    ival = smtpr._global_lm_interp_prob
    w_given=False
    weights = [1,1,1,1]
    verbose=False
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hf:m:i:w:v",["help","filename=","mpref=","interp=","weights="])
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
            elif opt in ("-f", "--filename"):
                filename = arg
                f_given=True
            elif opt in ("-m", "--mpref"):
                mpref = arg
                m_given=True
            elif opt in ("-i", "--interp"):
                ival = float(arg)
                i_given=True
            elif opt in ("-w", "--weights"):
                weight_str = arg
                weight_str_array=weight_str.split()
                weights=[]
                for i in range(len(weight_str_array)):
                    weights.append(float(weight_str_array[i]))
                w_given=True
            elif opt in ("-v", "--verbose"):
                verbose=True

    # check parameters
    if(m_given==False):
        print >> sys.stderr, "Error! -m parameter not given"
        sys.exit(2)

    # print parameters
    if(f_given==True):
        print >> sys.stderr, "f is %s" % (filename)

    if(m_given==True):
        print >> sys.stderr, "m is %s" % (mpref)

    if(i_given==True):
        print >> sys.stderr, "i is %f" % (ival)

    if(w_given==True):
        print >> sys.stderr, "w is \"%s\"" % (weight_str)

    # open files
    if(f_given==True):
        # open file
        file = codecs.open(filename, 'r', "utf-8")
    else:
        # fallback to stdin
        file=codecs.getreader("utf-8")(sys.stdin)
    
    # load translation model
    tmodel=smtpr.TransModel()
    tmfilename=mpref+".tm"
    tmfile = codecs.open(tmfilename, 'r', "utf-8")
    print >> sys.stderr, "Loading translation model from file",tmfilename,"..."
    tmodel.load(tmfile)

    # load language model
    lmodel=smtpr.LangModel()
    lmfilename=mpref+".lm"
    lmfile = codecs.open(lmfilename, 'r', "utf-8")
    print >> sys.stderr, "Loading language model from file",lmfilename,"..."
    lmodel.load(lmfile)
    lmodel.set_interp_prob(ival)
        
    # translate (detokenize)
    decoder=smtpr.Decoder(tmodel,lmodel,weights)
    print >> sys.stderr, "Recasing..."
    decoder.recase(file,verbose)

if __name__ == "__main__":

    # Call main function
    main(sys.argv)
