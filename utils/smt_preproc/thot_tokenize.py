# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import io, sys, getopt

from thot_smt_preproc import tokenize

##################################################
def print_help():
    print("thot_tokenize -f <string> [--help]", file=sys.stderr)
    print("", file=sys.stderr)
    print("-f <string>    File with text to be tokenized (can be read from stdin)", file=sys.stderr)
    print("--help         Print this help message", file=sys.stderr)

##################################################
def main(argv):
    # take parameters
    f_given=False
    filename = ""
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hf:",["help","filename="])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print_help()
            sys.exit()
        elif opt in ("-f", "--filename"):
            filename = arg
            f_given=True

    # print parameters
    if(f_given==True):
        print("f is %s" % (filename), file=sys.stderr)

    # open file
    if(f_given==True):
        # open file
        file = io.open(filename, 'r', encoding='utf-8')
    else:
        # fallback to stdin
        file = io.open(sys.stdin.fileno(), 'r', encoding='utf-8')

    # read file line by line
    for line in file:
        line=line.strip("\n")
        tokens = tokenize(line)
        tok_sent = ' '.join(tokens)
        print(tok_sent)
    file.close()

if __name__ == "__main__":
    main(sys.argv)
