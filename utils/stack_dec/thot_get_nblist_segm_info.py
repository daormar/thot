# Author: Daniel Ortiz Mart\'inez
# *- python -*

# import modules
import io, sys, getopt, itertools

##################################################
def print_help():
    print >> sys.stderr, "thot_get_nblist_segm_info -n <string> -p <string>"
    print >> sys.stderr, ""
    print >> sys.stderr, "-n <string>    File containing n-best list"
    print >> sys.stderr, "-p <string>    Prefix of .wg and .idx files"

##################################################
def make_key(orig_state,dest_state):
    return orig_state+"->"+dest_state

##################################################
def load_wg_info(wgfile):
    lineno=1
    wginfo={}
    for line in wgfile:
        if (lineno>2):
            line=line.strip("\n")
            wg_info_entry_array=line.split("|||")
            first_field_array=wg_info_entry_array[0].split()
            orig_state=first_field_array[0]
            dest_state=first_field_array[1]
            key=make_key(orig_state,dest_state)
            wginfo[key]=wg_info_entry_array[-1]
        lineno+=1
    return wginfo
    
##################################################
def load_idx_info(idxfile):
    lineno=1
    idxinfo={}
    for line in idxfile:
        if (lineno>2):
            line=line.strip("\n")
            idx_info_entry_array=line.split()
            idxinfo[idx_info_entry_array[0]]=idx_info_entry_array[1]
        lineno+=1
    return idxinfo

##################################################
def obtain_trg_cuts(state_transitions_array,wginfo):
    trgpos=0
    for i in range(len(state_transitions_array)):
        trgpos+=len(wginfo[state_transitions_array[i]].split())
        if(i==0):
            trgcuts=str(trgpos)
        else:
            trgcuts+=" "+str(trgpos)
    return trgcuts

##################################################
def obtain_src_segm_given_coverages(prev_coverage,succ_coverage):
    reverse_prev_coverage=prev_coverage[::-1]
    prev_coverage_array=list(reverse_prev_coverage)
    reverse_succ_coverage=succ_coverage[::-1]
    succ_coverage_array=list(reverse_succ_coverage)
    different_pos=[]
    for i in range(len(succ_coverage_array)):
        if(i>=len(prev_coverage_array)):
            if(succ_coverage_array[i]=="1"):
                different_pos.append(i)
        else:
            if(prev_coverage_array[i]=="0" and succ_coverage_array[i]=="1"):
                different_pos.append(i)
    leftmost_pos=min(different_pos)
    rightmost_pos=max(different_pos)
    return "( "+str(leftmost_pos)+" , "+str(rightmost_pos)+" )"
    
##################################################
def obtain_src_segms(state_transitions_array,idxinfo):
    srcsegms=""
    for i in range(len(state_transitions_array)):
        prev_state=state_transitions_array[i].split("->")[0]
        succ_state=state_transitions_array[i].split("->")[1]
        prev_coverage=idxinfo[prev_state]
        succ_coverage=idxinfo[succ_state]
        if(i==0):
            srcsegms=obtain_src_segm_given_coverages(prev_coverage,succ_coverage)
        else:
            srcsegms+=" "+obtain_src_segm_given_coverages(prev_coverage,succ_coverage)
        
    return srcsegms
        
##################################################
def process_nbl_file(nblfile,wginfo,idxinfo):
    lineno=1
    for line in nblfile:
        if (lineno==1):
            line=line.strip("\n")
            print line.encode("utf-8")
        else:
            # Process n-best list entry
            line=line.strip("\n")
            nbl_entry_array=line.split("|||")
            state_transitions_array=nbl_entry_array[2].split()
            trgcuts=obtain_trg_cuts(state_transitions_array,wginfo)
            srcsegms=obtain_src_segms(state_transitions_array,idxinfo)

            # Print result
            print line.encode("utf-8"),"|||",srcsegms,"|",trgcuts
        lineno+=1
    
##################################################
def main(argv):
    # take parameters
    n_given=False
    p_given=False
    verbose=0
    filename = ""
    try:
        opts, args = getopt.getopt(sys.argv[1:],"n:p:v",["nblist=","prefix="])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    if(len(opts)==0):
        print_help()
        sys.exit()
    else:
        for opt, arg in opts:
            if opt in ("-n", "--nblist"):
                nblist = arg
                n_given=True
            elif opt in ("-p", "--prefix"):
                prefix = arg
                p_given=True

    # check parameters
    if(n_given==False):
        print >> sys.stderr, "Error! -n parameter not given"
        sys.exit(2)

    if(p_given==False):
        print >> sys.stderr, "Error! -p parameter not given"
        sys.exit(2)

    # open files
    if(n_given==True):
        # open file
        nblfile = io.open(nblist, 'r', encoding="utf-8")

    if(p_given==True):
        # open files
        wgfile = io.open(prefix+".wg", 'r', encoding="utf-8")
        idxfile = io.open(prefix+".idx", 'r', encoding="utf-8")

    # load word-graph information
    wginfo=load_wg_info(wgfile)
    
    # load idx file information
    idxinfo=load_idx_info(idxfile)
    
    # process n-best list file
    process_nbl_file(nblfile,wginfo,idxinfo)
        
if __name__ == "__main__":
    main(sys.argv)
