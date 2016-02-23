# Author: Daniel Ortiz Mart\'inez
# *- python -*

# Given a GIZA alignment file (.A3.final) thot_conv_giza_alig_file
# allows to convert it into other formats.

# import modules
import sys, getopt, codecs

##################################################
def print_help():
    print >> sys.stderr, "Usage: thot_conv_giza_alig_file -g <string> -f <int>"
    print >> sys.stderr, ""
    print >> sys.stderr, "-g <string>  GIZA alignment file"    
    print >> sys.stderr, "-f <int>     Output file format,"
    print >> sys.stderr, "             1-> human"
    print >> sys.stderr, "             2-> moses"
    # print >> sys.stderr, "             3-> pair+alignment"
    # print >> sys.stderr, "             4-> GIZA inverted"
    # print >> sys.stderr, "             5-> Asymptote"
    print >> sys.stderr, ""

##################################################
def extract_src_info(src_info_array):
    # Initialize variables
    src_words=[]
    aligs=[]
    wamatrix={}

    # Iterate over src_info_array elements
    i=0
    while i<len(src_info_array):
        src_words.append(src_info_array[i])
        i+=2
        j=i
        while src_info_array[j]!="})":
            srcpos=len(src_words)-1
            trgpos=int(src_info_array[j])
            aligs.append((srcpos,trgpos))
            wamatrix[str(srcpos)+" "+str(trgpos)]=1
            j+=1
        i=j+1

    # Return result
    return (src_words,aligs,wamatrix)

##################################################
def conv_giza_file_to_human(gfile):
    # read giza file entry by entry
    while True:
        # Read three-line entry
        line1 = gfile.readline()
        line1=line1.strip("\n")

        line2 = gfile.readline()
        line2=line2.strip("\n")
        trgw_array=line2.split()

        line3 = gfile.readline()
        line3=line3.strip("\n")
        src_info_array=line3.split()

        if not line3: break 

        # Process entry

        # Extract information
        (srcw_array,aligs,wamatrix)=extract_src_info(src_info_array)

        # Print entry information
        ## print header
        print line1.encode("utf-8")
        print "trg:",line2.encode("utf-8")	
        print "src:",
        for j in range(1,len(srcw_array)):
            print srcw_array[j].encode("utf-8"),
        print ""
	   
        # ## print matrix
        # for j in range(1,len(srcw_array)):
        #     srcpos=len(srcw_array)-j
        #     for i in range(1,len(trgw_array)+1):
        #         trgpos=i
        #         if(str(srcpos)+" "+str(trgpos) in wamatrix.keys()):
        #             print " 1",
        #         else:
        #             print " 0",
        #     print "|",srcw_array[srcpos].encode("utf-8") #,srcpos,i

        # sep_str=""
        # for i in range(1,len(trgw_array)+1):
        #     sep_str=sep_str+"---"
        # print sep_str

        ## print matrix
        for i in range(1,len(trgw_array)+1):
            trgpos=len(trgw_array)-i+1
            for j in range(1,len(srcw_array)):
                srcpos=j
                if(str(srcpos)+" "+str(trgpos) in wamatrix.keys()):
                    print " 1",
                else:
                    print " 0",
            print "|",trgw_array[trgpos-1].encode("utf-8")

        sep_str=""
        for j in range(1,len(srcw_array)):
            sep_str=sep_str+"---"
        print sep_str

        ## print source words
        end=False
        l=0
        while not end:
            end=True
            for j in range(1,len(srcw_array)):
                srcc_array=list(srcw_array[j])
                if(l<len(srcc_array)):
                    print " "+srcc_array[l].encode("utf-8"),
                    end=False
                else:
                    print "  ",
            print "";
            l+=1

##################################################
def conv_giza_file_to_moses(gfile):

    # read giza file entry by entry
    while True:
        # Read three-line entry
        line1 = gfile.readline()
        line1=line1.strip("\n")

        line2 = gfile.readline()
        line2=line2.strip("\n")
        trgw_array=line2.split()

        line3 = gfile.readline()
        line3=line3.strip("\n")
        src_info_array=line3.split()

        if not line3: break 

        # Process entry

        # Extract information
        (srcw_array,aligs,wamatrix)=extract_src_info(src_info_array)

        # Print entry information
        for i in range(len(aligs)):
            srcpos=aligs[i][0]
            trgpos=aligs[i][1]
            if srcpos>0:
                print str(srcpos-1)+"-"+str(trgpos-1),

        print ""
        

##################################################
def main(argv):
    # take parameters
    g_given=False
    gfilename = ""
    f_given=False
    f_val= ""

    try:
        opts, args = getopt.getopt(sys.argv[1:],"g:f:",["help","gizafile=","format="])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    if(len(opts)==0):
        print_help()
        sys.exit()
    else:
        for opt, arg in opts:
            if opt in ("-g", "--gizafile"):
                gfilename = arg
                g_given=True
            elif opt in ("-f", "--format"):
                f_val = int(arg)
                f_given=True

    # check parameters
    if(g_given==False):
        print >> sys.stderr, "Error! -g parameter not given"
        sys.exit(2)

    if(f_given==False):
        print >> sys.stderr, "Error! -f parameter not given"
        sys.exit(2)

    # print parameters
    if(g_given==True):
        print >> sys.stderr, "g is %s" % (gfilename)

    if(f_given==True):
        print >> sys.stderr, "f is %s" % (f_val)

    # open files
    if(g_given==True):
        # open file
        gfile = codecs.open(gfilename, 'r', "utf-8")

    # process parameters
    if(f_val==1):
        conv_giza_file_to_human(gfile)
    elif(f_val==2):
        conv_giza_file_to_moses(gfile)
    elif(f_val==3):
        print >> sys.stderr, "Warning, option not implemented"
    elif(f_val==4):
        print >> sys.stderr, "Warning, option not implemented"
    elif(f_val==5):
        print >> sys.stderr, "Warning, option not implemented"
        

if __name__ == "__main__":

    # Call main function
    main(sys.argv)
