# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import io, sys, getopt

import thot_smt_preproc as smtpr

##################################################
def print_help():
    print("thot_rec_translator -f <string> -m <string> [-i <float>]", file=sys.stderr)
    print("                    [-w \"<float1> ... <float4>\"] [-v] [--help]", file=sys.stderr)
    print("", file=sys.stderr)
    print("-f <string>    File with text to be recased (can be read from standard", file=sys.stderr)
    print("               input)", file=sys.stderr)
    print("-m <string>    Prefix of recaser model files", file=sys.stderr)
    print("-i <float>     Interpolation probability for language model", file=sys.stderr)
    print("-w \"...\"       Weight vector of length four:", file=sys.stderr)
    print("                <float1> : translation model weight", file=sys.stderr)
    print("                <float2> : phrase penalty model weight", file=sys.stderr)
    print("                <float3> : word penalty weight", file=sys.stderr)
    print("                <float4> : language model weight", file=sys.stderr)
    print("-v             Verbose mode", file=sys.stderr)
    print("--help         Print this help message", file=sys.stderr)

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
        print("Error! -m parameter not given", file=sys.stderr)
        sys.exit(2)

    # print parameters
    if(f_given==True):
        print("f is %s" % (filename), file=sys.stderr)

    if(m_given==True):
        print("m is %s" % (mpref), file=sys.stderr)

    if(i_given==True):
        print("i is %f" % (ival), file=sys.stderr)

    if(w_given==True):
        print("w is \"%s\"" % (weight_str), file=sys.stderr)

    # open files
    if(f_given==True):
        # open file
        file = io.open(filename, 'r', encoding="utf-8")
    else:
        # fallback to stdin
        file = io.open(sys.stdin.fileno(), 'r', encoding='utf8')

    # load translation model
    tmodel=smtpr.TransModel()
    tmfilename=mpref+".tm"
    tmfile = io.open(tmfilename, 'r', encoding="utf-8")
    print("Loading translation model from file",tmfilename,"...", file=sys.stderr)
    tmodel.load(tmfile)

    # load language model
    lmodel=smtpr.LangModel()
    lmfilename=mpref+".lm"
    lmfile = io.open(lmfilename, 'r', encoding="utf-8")
    print("Loading language model from file",lmfilename,"...", file=sys.stderr)
    lmodel.load(lmfile)
    lmodel.set_interp_prob(ival)

    # translate (detokenize)
    decoder=smtpr.Decoder(tmodel,lmodel,weights)
    print("Recasing...", file=sys.stderr)
    decoder.recase(file,verbose)

if __name__ == "__main__":

    # Call main function
    main(sys.argv)
