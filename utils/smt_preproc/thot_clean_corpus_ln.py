# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import sys, getopt, codecs
import math
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
    print >> sys.stderr, "-d <int>       Maximum number of standard deviations allowed in the"
    print >> sys.stderr, "               difference in length between the source and target"
    print >> sys.stderr, "               sentences (4 by default)"
    print >> sys.stderr, "--help         Print this help message"

##################################################
def compute_mean_stddev(slen_list,tlen_list):
    # Compute mean
    mean=0.0
    for k in xrange(len(slen_list)):
        mean+=slen_list[k]-tlen_list[k]
    mean=mean/len(slen_list)

    # Compute standard deviation
    stddev=0.0
    for k in xrange(len(slen_list)):
        diff=slen_list[k]-tlen_list[k]
        stddev+=(diff-mean)*(diff-mean)
    stddev=math.sqrt(stddev/len(slen_list))

    return (mean,stddev)

##################################################
def compute_mean_stddev_per_length(slen_list,tlen_list):
    # Compute means
    mean_per_length={}
    samples_per_length={}
    for k in xrange(len(slen_list)):
        slen=slen_list[k]
        if(slen not in samples_per_length.keys()):
            samples_per_length[slen]=1.0
            mean_per_length[slen]=slen_list[k]-tlen_list[k]
        else:
            samples_per_length[slen]+=1.0
            mean_per_length[slen]+=slen_list[k]-tlen_list[k]

    for k in samples_per_length.keys():
        mean_per_length[k]=mean_per_length[k]/samples_per_length[k]

    # Compute standard deviations
    stddev_per_length={}
    for k in xrange(len(slen_list)):
        slen=slen_list[k]
        diff=slen_list[k]-tlen_list[k]

        if(slen not in stddev_per_length.keys()):
            stddev_per_length[slen]=(diff-mean_per_length[slen])*(diff-mean_per_length[slen])
        else:
            stddev_per_length[slen]+=(diff-mean_per_length[slen])*(diff-mean_per_length[slen])

    for k in samples_per_length.keys():
        stddev_per_length[k]=math.sqrt(stddev_per_length[k]/samples_per_length[k])

    # Return result
    return (mean_per_length,stddev_per_length,samples_per_length)

##################################################
def main(argv):
    # take parameters
    minlen=1
    maxlen=80
    d_par=4
    s_given=False
    t_given=False
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hs:t:i:a:d:",["help","srcfn=","trgfn="])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    if(len(opts)==0):
        print_help()
        sys.exit(0)
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
        elif opt in ("-i", "--minl"):
            minlen = int(arg)
        elif opt in ("-a", "--maxl"):
            maxlen = int(arg)
        elif opt in ("-d", "--md"):
            d_par = int(arg)

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
    print >> sys.stderr, "d is %d" % (d_par)

    # open file
    if(s_given==True):
        # open file
        srcfile = codecs.open(srcfn, 'r', "utf-8")

    if(t_given==True):
        # open file
        trgfile = codecs.open(trgfn, 'r', "utf-8")

    # read parallel files line by line
    slen_list=[]
    tlen_list=[]
    for srcline, trgline in itertools.izip(srcfile,trgfile):
        srcline=srcline.strip("\n")
        src_word_array=srcline.split()
        trgline=trgline.strip("\n")
        trg_word_array=trgline.split()

        # Store sentence lengths
        slen_list.append(len(src_word_array))
        tlen_list.append(len(trg_word_array))

    # Compute statistics
    (mean,stddev)=compute_mean_stddev(slen_list,tlen_list)
    (mean_perl,stddev_perl,samples_perl)=compute_mean_stddev_per_length(slen_list,tlen_list)

    # Print line numbers
    min_num_samples=10
    for k in xrange(len(slen_list)):
        slen=slen_list[k]
        tlen=tlen_list[k]
#        print mean,stddev,";",slen,tlen,";",mean_perl[slen],stddev_perl[slen],";",samples_perl[slen]
        # Verify minimum and maximum length
        if (slen>=minlen and tlen>=minlen and slen<=maxlen and tlen<=maxlen):
            # Obtain difference in sentence length
            diff=slen-tlen
            # Obtain upper and lower limits for difference in sentence length
            if(samples_perl[slen]>=min_num_samples):
                uplim=mean_perl[slen]+d_par*stddev_perl[slen]
                lolim=mean_perl[slen]-d_par*stddev_perl[slen]
            else:
                uplim=mean+d_par*stddev
                lolim=mean-d_par*stddev

            # Verify difference in sentence length
            if(diff<=uplim and diff>=lolim):
                print k+1
            else:
                print >> sys.stderr,"lineno:",k+1,", slen:",slen,", tlen:",tlen
        else:
            print >> sys.stderr,"lineno:",k+1,", slen:",slen,", tlen:",tlen

if __name__ == "__main__":
    main(sys.argv)
