# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import sys, getopt, codecs

import thot_smt_preproc as smtpr

##################################################
def print_help():
    print("thot_remove_xml_annotations -f <string> [--help]", file=sys.stderr)
    print("", file=sys.stderr)
    print("-f <string>    File with text containing xml annotations (can be read", file=sys.stderr)
    print("               from stdin)", file=sys.stderr)
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
        file = codecs.open(filename, 'r', "utf-8")
    else:
        # fallback to stdin
        file=codecs.getreader("utf-8")(sys.stdin)

    # read file line by line
    for line in file:
        line=line.strip("\n")
        line_without_xml_tags=smtpr.remove_xml_annotations(line)

        print(line_without_xml_tags)

if __name__ == "__main__":
    main(sys.argv)
