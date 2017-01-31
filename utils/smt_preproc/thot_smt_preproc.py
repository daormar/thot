# Author: Daniel Ortiz Mart\'inez
# coding=utf-8
# *- python -*

# import modules
import sys, codecs, math, re, Queue
import itertools
from heapq import heappush, heappop


# global variables
_global_n=2
_global_lm_interp_prob=0.5
_global_common_word_str="<common_word>"
_global_number_str="<number>"
_global_digit_str="<digit>"
_global_alfanum_str="<alfanum>"
_global_unk_word_str="<unk>"
_global_eos_str="<eos>"
_global_bos_str="<bos>"
_global_categ_set=frozenset([_global_common_word_str,_global_number_str,_global_digit_str,_global_alfanum_str])
_global_digits = re.compile('\d')
_global_alnum = re.compile('[a-zA-Z0-9]+')
_global_a_par=7
_global_maxniters=100000
_global_tm_smooth_prob=0.000001

# xml annotation variables
grp_ann = "phr_pair_annot"
src_ann = "src_segm"
trg_ann = "trg_segm"
dic_patt = u"(<%s>)[ ]*(<%s>)(.+?)(<\/%s>)[ ]*(<%s>)(.+?)(<\/%s>)[ ]*(<\/%s>)" % (grp_ann,
                                                                                  src_ann, src_ann,
                                                                                  trg_ann, trg_ann,
                                                                                  grp_ann)
len_ann = "length_limit"
len_patt = u"(<%s>)[ ]*(\d+)[ ]*(</%s>)" % (len_ann, len_ann)


_annotation = re.compile(dic_patt + "|" + len_patt)

##################################################
class TransModel:
    def __init__(self):
        self.st_counts={}
        self.s_counts={}

    #####
    def clear(self):
        self.st_counts.clear()
        self.s_counts.clear()

    #####
    def increase_count(self,src_words,trg_words,c):
        if(src_words in self.st_counts):
            if(trg_words in self.st_counts[src_words]):
                self.st_counts[src_words][trg_words]=self.st_counts[src_words][trg_words]+c
                self.s_counts[src_words]=self.s_counts[src_words]+c
            else:
                self.st_counts[src_words][trg_words]=1
                self.s_counts[src_words]=self.s_counts[src_words]+c
        else:
            self.st_counts[src_words]={}
            self.st_counts[src_words][trg_words]=c
            self.s_counts[src_words]=c

    #####
    def obtain_opts_for_src(self,src_words):
        if(src_words in self.st_counts):
            result=[]
            for opt in self.st_counts[src_words]:
                result.append(opt)
            return result
        else:
            return []

    #####
    def obtain_srctrg_count(self,src_words,trg_words):
        if(src_words in self.st_counts):
            if(trg_words in self.st_counts[src_words]):
                return self.st_counts[src_words][trg_words]
            else:
                return 0
        else:
            return 0

    #####
    def obtain_trgsrc_prob(self,src_words,trg_words):
        sc=self.obtain_src_count(src_words)
        if(sc==0):
            return 0
        else:
            stc=self.obtain_srctrg_count(src_words,trg_words)
            return float(stc)/float(sc)

    #####
    def obtain_trgsrc_prob_smoothed(self,src_words,trg_words):
        sc=self.obtain_src_count(src_words)
        if(sc==0):
            return _global_tm_smooth_prob
        else:
            stc=self.obtain_srctrg_count(src_words,trg_words)
            return (1-_global_tm_smooth_prob)*(float(stc)/float(sc))

    #####
    def obtain_src_count(self,src_words):
        if(src_words in self.s_counts):
            return self.s_counts[src_words]
        else:
            return 0

    #####
    def load(self,file):
        # Read file line by line
        for line in file:
            line=line.strip("\n")
            line_array=line.split()

            # Iterate over line elements
            src_words=""
            trg_words=""
            src=True
            for i in range(len(line_array)-1):
                if(src==True):
                    if(line_array[i]=="|||"):
                        src=False
                    else:
                        if(i==0):
                            src_words=line_array[i]
                        else:
                            src_words=src_words+" "+line_array[i]
                else:
                    if(line_array[i-1]=="|||"):
                        trg_words=line_array[i]
                    else:
                        trg_words=trg_words+" "+line_array[i]

            c=int(line_array[len(line_array)-1])

            # Increase count
            self.increase_count(src_words,trg_words,c)

    #####
    def print_model_to_file(self,file):
        for k1 in self.st_counts:
            for k2 in self.st_counts[k1]:
                file.write(u'%s ||| %s %d\n'%(k1,k2,self.st_counts[k1][k2]))

    #####
    def print_model(self):
        print "*** st_counts:"
        for k1 in self.st_counts:
            for k2 in self.st_counts[k1]:
                print k1.encode("utf-8"),"|||",k2.encode("utf-8"),self.st_counts[k1][k2]

        print "*** s_counts:"
        for k in self.s_counts:
            print k.encode("utf-8"),self.s_counts[k]

    #####
    def train_sent_tok(self,raw_word_array,tok_array,verbose):
        if(len(tok_array)>0):
            # train translation model for sentence
            i=0
            j=0
            prev_j=0
            error=False

            # Obtain transformed raw word array
            while(i<len(raw_word_array)):
                end=False
                str=""

                # process current raw word
                while(end == False):
                    if(raw_word_array[i]==str):
                        end=True
                    else:
                        if(j>=len(tok_array)):
                            error=True
                            end=True
                        else:
                            str=str+tok_array[j]
                            j=j+1

                # Check that no errors were found while processing current raw word
                if(error==True):
                    return False

                # update the translation model
                tm_entry_ok=True
                tok_words=transform_word(tok_array[prev_j])
                raw_word=transform_word(tok_array[prev_j])
                for k in range(prev_j+1,j):
                    tok_words=tok_words+" "+transform_word(tok_array[k])
                    raw_word=raw_word+transform_word(tok_array[k])
                    if(is_categ(transform_word(tok_array[k-1])) and
                       is_categ(transform_word(tok_array[k]))):
                        tm_entry_ok=False

                raw_words=raw_word

                if(tm_entry_ok==True):
                    self.increase_count(tok_words,raw_words,1)

                # update variables
                i=i+1
                prev_j=j

            # The sentence was successfully processed
            return True

    #####
    def train_tok_tm(self,file,verbose):

        # Initialize variables
        nsent=1

        # read raw file line by line
        for line in file:
            line=line.strip("\n")
            raw_word_array=line.split()
            tok_array=tokenize(line)

            if(verbose==True):
                print >> sys.stderr,"* Training tm for sentence pair:"
                print >> sys.stderr," raw:",line.encode("utf-8")
                print >> sys.stderr," tok:",
                for i in range(len(tok_array)):
                    print >> sys.stderr,tok_array[i].encode("utf-8"),
                print >> sys.stderr,""

            # Process sentence
            retval=self.train_sent_tok(raw_word_array,tok_array,verbose)
            if(retval==False):
                print >> sys.stderr, "Warning: something went wrong while training the translation model for sentence",nsent
            nsent+=1

    #####
    def train_tok_tm_par_files(self,rfile,tfile,verbose):

        # Initialize variables
        nsent=1

        # Read parallel files line by line
        for rline, tline in itertools.izip(rfile,tfile):
            rline=rline.strip("\n")
            raw_word_array=rline.split()
            tline=tline.strip("\n")
            tok_array=tline.split()

            if(verbose==True):
                print >> sys.stderr,"* Training tm for sentence pair:"
                print >> sys.stderr," raw:",line.encode("utf-8")
                print >> sys.stderr," tok:",
                for i in range(len(tok_array)):
                    print >> sys.stderr,tok_array[i].encode("utf-8"),
                print >> sys.stderr,""

            # Process sentence
            retval=self.train_sent_tok(raw_word_array,tok_array,verbose)
            if(retval==False):
                print >> sys.stderr, "Warning: something went wrong while training the translation model for sentence",nsent
            nsent+=1

    #####
    def train_sent_rec(self,raw_word_array,lc_word_array,verbose):
        for i in range(len(raw_word_array)):
            raw_word=raw_word_array[i]
            lc_word=lc_word_array[i]
            self.increase_count(lc_word,raw_word,1)

    #####
    def train_rec_tm(self,file,verbose):

        # read raw file line by line
        for line in file:
            line=line.strip("\n")
            raw_word_array=line.split()
            lc_word_array=lowercase(line).split()

            if(verbose==True):
                print >> sys.stderr,"* Training tm for sentence pair:"
                print >> sys.stderr," raw:",line.encode("utf-8")
                print >> sys.stderr," lc:",
                for i in range(len(lc_word_array)):
                    print >> sys.stderr,lc_word_array[i].encode("utf-8"),
                print >> sys.stderr,""

            # Process sentence
            self.train_sent_rec(raw_word_array,lc_word_array,verbose)

    #####
    def get_mon_hyp_state(self,hyp):
        if(len(hyp.data.coverage)==0):
            return 0
        else:
            return hyp.data.coverage[len(hyp.data.coverage)-1]

##################################################
class LangModel:

    def __init__(self):
        self.n=_global_n
        self.interp_prob=_global_lm_interp_prob
        self.ng_counts={}


    #####
    def clear(self):
        self.n=_global_n
        self.interp_prob=_global_lm_interp_prob
        self.ng_counts.clear()

    #####
    def set_n(self,n):
        self.n=n

    #####
    def get_n(self):
        return self.n

    #####
    def set_interp_prob(self,interp_prob):
        if(interp_prob>0.99):
            self.interp_prob=0.99
        elif(interp_prob<0):
            self.interp_prob=0
        else:
            self.interp_prob=interp_prob

    #####
    def get_interp_prob(self):
        return self.interp_prob

    #####
    def increase_ng_count(self,ngram,c):
        if(ngram in self.ng_counts):
            self.ng_counts[ngram]=self.ng_counts[ngram]+c
        else:
            self.ng_counts[ngram]=c

    #####
    def obtain_ng_count(self,ngram):
        if(ngram in self.ng_counts):
            return self.ng_counts[ngram]
        else:
            return 0

    #####
    def obtain_trgsrc_prob(self,ngram):
        if(ngram==""):
            return 1.0/self.obtain_ng_count("")
        else:
            hc=self.obtain_ng_count(self.remove_newest_word(ngram))
            if(hc==0):
                return 0
            else:
                ngc=self.obtain_ng_count(ngram)
                return float(ngc)/float(hc)

    #####
    def obtain_trgsrc_interp_prob(self,ngram):
        ng_array=ngram.split()
        if(len(ng_array)==0):
            return self.obtain_trgsrc_prob(ngram)
        else:
            return self.interp_prob * self.obtain_trgsrc_prob(ngram) + (1-self.interp_prob) * self.obtain_trgsrc_interp_prob(self.remove_oldest_word(ngram))

    #####
    def remove_newest_word(self,ngram):
        ng_array=ngram.split()
        if(len(ng_array)<=1):
            return ""
        else:
            result=ng_array[0]
            for i in range(1,len(ng_array)-1):
                result=result+" "+ng_array[i]
            return result

    #####
    def remove_oldest_word(self,ngram):
        ng_array=ngram.split()
        if(len(ng_array)<=1):
            return ""
        else:
            result=ng_array[1]
            for i in range(2,len(ng_array)):
                result=result+" "+ng_array[i]
            return result

    #####
    def train_word_array(self,word_array):
        # obtain counts for 0-grams
        self.increase_ng_count("",len(word_array))

        # create auxiliary array with special words
        word_array_aux=[]
        word_array_aux.append(_global_bos_str)
        for i in range(len(word_array)):
            word_array_aux.append(word_array[i])
        word_array_aux.append(_global_eos_str)

        # obtain counts for higher order n-grams
        for i in range(1,self.n+1):
            for j in range(len(word_array_aux)):
                # Obtain n-gram
                ngram_array=word_array_aux[j:j+i]
                # Convert array to string
                ngram=""
                for k in range(len(ngram_array)):
                    if(k==0):
                        ngram=ngram_array[k]
                    else:
                        ngram=ngram+" "+ngram_array[k]

                # increase counts of ngram string
                if(i!=1 or j!=0):
                    self.increase_ng_count(ngram,1)

    #####
    def load(self,file):

        # Init variables
        max_n=0

        # Read file line by line
        for line in file:
            line=line.strip("\n")
            line_array=line.split()

            # Update max_n
            if(max_n<len(line_array)-1):
                max_n=len(line_array)-1

            # Iterate over line elements
            ng_words=""
            for i in range(len(line_array)-1):
                if(i==0):
                    ng_words=line_array[i]
                else:
                    ng_words=ng_words+" "+line_array[i]

            c=int(line_array[len(line_array)-1])

            # Increase count
            self.increase_ng_count(ng_words,c)

            # Increase zero-gram count if necessary
            if(len(line_array)==2):
                self.increase_ng_count("",c)

        # Set n-gram order
        self.n=max_n
        print >> sys.stderr, "n-gram order:",self.n


    #####
    def print_model_to_file(self,file):
        for k in self.ng_counts:
            if(k!=""):
                file.write(u'%s %d\n'%(k,self.ng_counts[k]))

    #####
    def print_model(self):
        print "*** n value:",self.n
        print "*** ng_counts:"
        for k in self.ng_counts:
            print k.encode("utf-8"),self.ng_counts[k]

    #####
    def lm_preproc(self,trans_raw_word_array,lmvoc):
        # Do not alter words
        return trans_raw_word_array

    #####
    def lm_preproc_unk_word(self,trans_raw_word_array,lmvoc):
        # Introduce unknown word if necessary
        preproc_trans_raw_word_array=[]
        for i in range(len(trans_raw_word_array)):
            if(trans_raw_word_array[i] in lmvoc):
                preproc_trans_raw_word_array.append(trans_raw_word_array[i])
                lmvoc[trans_raw_word_array[i]]=lmvoc[trans_raw_word_array[i]]+1
            else:
                preproc_trans_raw_word_array.append(_global_unk_word_str)
                lmvoc[trans_raw_word_array[i]]=1

        return preproc_trans_raw_word_array

    #####
    def train_sent_tok(self,raw_word_array,tok_array,lmvoc,verbose):
        if(len(tok_array)>0):
            # train translation model for sentence
            i=0
            j=0
            prev_j=0
            error=False

            # Obtain transformed raw word array
            trans_raw_word_array=[]
            while(i<len(raw_word_array)):
                end=False
                str=""

                # process current raw word
                while(end == False):
                    if(raw_word_array[i]==str):
                        end=True
                    else:
                        if(j>=len(tok_array)):
                            error=True
                            end=True
                        else:
                            str=str+tok_array[j]
                            j=j+1

                # Check that no errors were found while processing current raw word
                if(error==True):
                    return False

                # update the language model
                tm_entry_ok=True
                tok_words=transform_word(tok_array[prev_j])
                raw_word=transform_word(tok_array[prev_j])
                for k in range(prev_j+1,j):
                    tok_words=tok_words+" "+transform_word(tok_array[k])
                    raw_word=raw_word+transform_word(tok_array[k])
                    if(is_categ(transform_word(tok_array[k-1])) and
                       is_categ(transform_word(tok_array[k]))):
                        tm_entry_ok=False

                trans_raw_word_array.append(raw_word)

                # update variables
                i=i+1
                prev_j=j

            # preprocess sentence (introduce the unknown word token)
            preproc_trans_raw_word_array=self.lm_preproc(trans_raw_word_array,lmvoc)

            if(verbose==True):
                print >> sys.stderr,"* Training lm for sentence:",
                for i in range(len(preproc_trans_raw_word_array)):
                    print >> sys.stderr,preproc_trans_raw_word_array[i].encode("utf-8"),
                print >> sys.stderr,""


            # train language model using the transformed raw word
            # sentence
            self.train_word_array(preproc_trans_raw_word_array)

            # The sentence was successfully processed
            return True

    #####
    def train_tok_lm(self,file,nval,verbose):

        # initialize variables
        lmvoc={}
        self.set_n(nval)
        nsent=1

        # read raw file line by line
        for line in file:
            line=line.strip("\n")
            raw_word_array=line.split()
            tok_array=tokenize(line)

            if(verbose==True):
                print >> sys.stderr,"* Training lm for sentence pair:"
                print >> sys.stderr," raw:",line.encode("utf-8")
                print >> sys.stderr," tok:",
                for i in range(len(tok_array)):
                    print >> sys.stderr,tok_array[i].encode("utf-8"),
                print >> sys.stderr,""

            # Process sentence
            retval=self.train_sent_tok(raw_word_array,tok_array,lmvoc,verbose)
            if(retval==False):
                print >> sys.stderr, "Warning: something went wrong while training the language model for sentence",nsent
            nsent+=1


    #####
    def train_tok_lm_par_files(self,rfile,tfile,nval,verbose):

        # initialize variables
        lmvoc={}
        self.set_n(nval)
        nsent=1

        # Read parallel files line by line
        for rline, tline in itertools.izip(rfile,tfile):
            rline=rline.strip("\n")
            raw_word_array=rline.split()
            tline=tline.strip("\n")
            tok_array=tline.split()

            if(verbose==True):
                print >> sys.stderr,"* Training lm for sentence pair:"
                print >> sys.stderr," raw:",line.encode("utf-8")
                print >> sys.stderr," tok:",
                for i in range(len(tok_array)):
                    print >> sys.stderr,tok_array[i].encode("utf-8"),
                print >> sys.stderr,""

            # Process sentence
            retval=self.train_sent_tok(raw_word_array,tok_array,lmvoc,verbose)
            if(retval==False):
                print >> sys.stderr, "Warning: something went wrong while training the language model for sentence",nsent
            nsent+=1

    #####
    def train(self,file,nval,verbose):

        # initialize variables
        lmvoc={}
        self.set_n(nval)

        # read raw file line by line
        for line in file:
            line=line.strip("\n")
            word_array=line.split()

            # Process sentence
            if(verbose==True):
                print >> sys.stderr,"* Training lm for sentence:",
                for i in range(len(word_array)):
                    print >> sys.stderr,word_array[i].encode("utf-8"),
                print >> sys.stderr,""

            # train language model for current sentence
            self.train_word_array(word_array)

    #####
    def get_lm_state(self,words):
        # Obtain array of previous words including BOS symbol
        words_array_aux=words.split()
        words_array=[]
        words_array.append(_global_bos_str)
        for i in range(len(words_array_aux)):
            words_array.append(words_array_aux[i])

        # Obtain history from array
        len_hwa=len(words_array)
        hist=""
        for i in range(self.get_n()-1):
            if(i<len(words_array)):
                word=words_array[len_hwa-1-i]
                if(hist==""):
                    hist=word
                else:
                    hist=word+" "+hist
        return hist

    #####
    def get_hyp_state(self,hyp):
        return self.get_lm_state(hyp.data.words)

##################################################
class BfsHypdata:
    def __init__(self):
        self.coverage=[]
        self.words=""
    def __str__(self):
        result="cov:"
        for k in range(len(self.coverage)):
            result=result+" "+str(self.coverage[k])
        result=result+" ; words: "+self.words.encode("utf-8")
        return result

##################################################
class Hypothesis:

    def __init__(self):
        self.score=0
        self.data=BfsHypdata()

    def __cmp__(self, other):
        return cmp(other.score,self.score)

##################################################
class PriorityQueue:

    def __init__(self):
        self.heap=[]

    def empty(self):
        return len(self.heap)==0

    def put(self,item):
        heappush(self.heap,item)

    def get(self):
        return heappop(self.heap)

##################################################
class StateInfoDict:

    def __init__(self):
        self.recomb_map={}

    def empty(self):
        return len(self.recomb_map)==0

    def insert(self,state_info,score):

        # Update recombination info
        if(state_info in self.recomb_map):
            if(score>self.recomb_map[state_info]):
                self.recomb_map[state_info]=score
        else:
            self.recomb_map[state_info]=score

    def get(self):
        return heappop(self.heap)

    def hyp_recombined(self,state_info,score):

        if(state_info in self.recomb_map):
            if(score<self.recomb_map[state_info]):
                return True
            else:
                return False
        else:
            return False

##################################################
class StateInfo:

    def __init__(self,tm_state,lm_state):
        self.tm_state=tm_state
        self.lm_state=lm_state

    def __hash__(self):
        return hash((self.tm_state, self.lm_state))

    def __eq__(self, other):
        return (self.tm_state, self.lm_state) == (other.tm_state, other.lm_state)

##################################################
def obtain_state_info(tmodel,lmodel,hyp):

    return StateInfo(tmodel.get_mon_hyp_state(hyp),lmodel.get_hyp_state(hyp))

##################################################
def transform_word(word):
    if(word.isdigit()==True):
        if(len(word)>1):
            return _global_number_str
        else:
            return _global_digit_str
    elif(is_number(word)==True):
        return _global_number_str
    elif(is_alnum(word)==True and bool(_global_digits.search(word))==True):
        return _global_alfanum_str
    elif(len(word)>5):
        return _global_common_word_str
    else:
        return word

##################################################
def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

##################################################
def is_alnum(s):
    res = _global_alnum.match(s)
    if(res==None):
        return False
    else:
        return True

##################################################
def areCategsPaired(categ_src_ann_words_array,categ_trg_ann_words_array):
    # Initialize dictionaries with number of category ocurrences
    num_categs_src={}
    num_categs_trg={}
    for categ in _global_categ_set:
        num_categs_src[categ]=0
        num_categs_trg[categ]=0        
    # Count source categories
    for i in range(len(categ_src_ann_words_array)):
        if(categ_src_ann_words_array[i] in _global_categ_set):
            num_categs_src[categ_src_ann_words_array[i]]+=1
    # Count target categories
    for i in range(len(categ_trg_ann_words_array)):
        if(categ_trg_ann_words_array[i] in _global_categ_set):
            num_categs_trg[categ_trg_ann_words_array[i]]+=1
    # Verify category ocurrence number equality
    for categ in num_categs_src:
        if(not num_categs_src[categ]==num_categs_trg[categ]):
            return False

    return True

##################################################
def categ_src_trg_annotation(src_ann_words,trg_ann_words):
    # Obtain array with source words (with a without categorization)
    src_ann_words_array=src_ann_words.split()
    categ_src_ann_words_array=[]
    for i in range(len(src_ann_words_array)):
        categ_src_ann_words_array.append(categorize_word(src_ann_words_array[i]))

    # Obtain array with target words (with a without categorization)
    trg_ann_words_array=trg_ann_words.split()
    categ_trg_ann_words_array=[]
    for i in range(len(trg_ann_words_array)):
        categ_trg_ann_words_array.append(categorize_word(trg_ann_words_array[i]))

    # Verify that categories are paired
    if(areCategsPaired(categ_src_ann_words_array,categ_trg_ann_words_array)):
        return categ_src_ann_words_array,categ_trg_ann_words_array
    else:
        return src_ann_words_array,trg_ann_words_array

##################################################
def categorize(sentence):
    skeleton = annotated_string_to_xml_skeleton(sentence)

    # Categorize words
    categ_word_array=[]
    curr_xml_tag=None
    for i in range(len(skeleton)):
        is_tag, word = skeleton[i]
        if(is_tag==True):
            # Treat xml tag
            if(word=='<'+len_ann+'>'):
                categ_word_array.append(word)
                curr_xml_tag="len_ann"
            elif(word=='<'+grp_ann+'>'):
                categ_word_array.append(word)
            elif(word=='<'+src_ann+'>'):
                curr_xml_tag="src_ann"
            elif(word=='<'+trg_ann+'>'):
                curr_xml_tag="trg_ann"
            elif(word=='</'+len_ann+'>'):
                categ_word_array.append(word)
                curr_xml_tag=None
            elif(word=='</'+grp_ann+'>'):
                categ_word_array.append(word)
            elif(word=='</'+src_ann+'>'):
                curr_xml_tag=None
            elif(word=='</'+trg_ann+'>'):
                curr_xml_tag=None
                categ_src_words,categ_trg_words=categ_src_trg_annotation(src_ann_words,trg_ann_words)
                # Add source phrase
                categ_word_array.append('<'+src_ann+'>')
                for i in range(len(categ_src_words)):
                    categ_word_array.append(categ_src_words[i])
                categ_word_array.append('</'+src_ann+'>')
                # Add target phrase
                categ_word_array.append('<'+trg_ann+'>')
                for i in range(len(categ_trg_words)):
                    categ_word_array.append(categ_trg_words[i])
                categ_word_array.append('</'+trg_ann+'>')
        else:
            # Categorize group of words
            if(curr_xml_tag==None):
                word_array=word.split()
                for j in range(len(word_array)):
                    categ_word_array.append(categorize_word(word_array[j]))
            elif(curr_xml_tag=="len_ann"):
                word_array=word.split()
                for j in range(len(word_array)):
                    categ_word_array.append(word)
            elif(curr_xml_tag=="src_ann"):
                src_ann_words=word
            elif(curr_xml_tag=="trg_ann"):
                trg_ann_words=word

    return u' '.join(categ_word_array)

##################################################
def categorize_word(word):
    if(word.isdigit()==True):
        if(len(word)>1):
            return _global_number_str
        else:
            return _global_digit_str
    elif(is_number(word)==True):
        return _global_number_str
    elif(is_alnum(word)==True and bool(_global_digits.search(word))==True):
        return _global_alfanum_str
    else:
        return word

##################################################
def is_categ(word):
    if(word in _global_categ_set):
        return True
    else:
        return False

##################################################
def extract_alig_info(hyp_word_array):
    # Initialize output variables
    srcsegms=[]
    trgcuts=[]

    # Scan hypothesis information
    info_found=False
    for i in range(len(hyp_word_array)):
        if(hyp_word_array[i]=="hypkey:" and hyp_word_array[i-1]=="|"):
            info_found=True
            i-=2
            break;

    if(info_found):
        # Obtain target segment cuts
        trgcuts_found=False
        while i>0:
            if(hyp_word_array[i]!="|"):
                trgcuts.append(int(hyp_word_array[i]))
                i-=1
            else:
                trgcuts_found=True
                i-=1
                break
        trgcuts.reverse()

        if(trgcuts_found):
            # Obtain source segments
            srcsegms_found=False
            while i>0:
                if(hyp_word_array[i]!="|"):
                    if(i>3):
                        srcsegms.append((int(hyp_word_array[i-3]),int(hyp_word_array[i-1])))
                    i-=5
                else:
                    srcsegms_found=True
                    break
            srcsegms.reverse()

    # Return result
    if(srcsegms_found):
        return (srcsegms,trgcuts)
    else:
        return ([],[])

##################################################
def extract_categ_words_of_segm(word_array,left,right):
    # Initialize variables
    categ_words=[]

    # Explore word array
    for i in range(left,right+1):
        if(is_categ(word_array[i]) or is_categ(categorize_word(word_array[i]))):
           categ_words.append((i,word_array[i]))

    # Return result
    return categ_words

##################################################
def decategorize(sline,tline,iline):
    src_word_array=sline.split()
    trg_word_array=tline.split()
    hyp_word_array=iline.split()

    # Extract alignment information
    srcsegms,trgcuts=extract_alig_info(hyp_word_array)

    # Iterate over target words
    output=""
    for trgpos in range(len(trg_word_array)):

        if(is_categ(trg_word_array[trgpos])):
            output+=decategorize_word(trgpos,src_word_array,trg_word_array,srcsegms,trgcuts)
        else:
            output+=trg_word_array[trgpos]
            
        if(trgpos<len(trg_word_array)-1):
            output+=" "
            
    return output

##################################################
def decategorize_word(trgpos,src_word_array,trg_word_array,srcsegms,trgcuts):
    # Check if there is alignment information available
    if(len(srcsegms)==0 or len(trgcuts)==0):
        return trg_word_array[i]
    else:
        # Scan target cuts
        for k in range(len(trgcuts)):
            if(k==0):
                if(trgpos+1<=trgcuts[k]):
                    trgleft=0
                    trgright=trgcuts[k]-1
                    break
            else:
                if(trgpos+1>trgcuts[k-1] and trgpos+1<=trgcuts[k]):
                    trgleft=trgcuts[k-1]
                    trgright=trgcuts[k]-1
                    break
        # Check if trgpos'th word was assigned to one cut
        if(k<len(trgcuts)):
            # Obtain source segment limits
            srcleft=srcsegms[k][0]-1
            srcright=srcsegms[k][1]-1
            # Obtain categorized words with their indices
            src_categ_words=extract_categ_words_of_segm(src_word_array,srcleft,srcright)
            trg_categ_words=extract_categ_words_of_segm(trg_word_array,trgleft,trgright)

            # Obtain decategorized word
            decateg_word=""
            curr_categ_word=trg_word_array[trgpos]
            curr_categ_word_order=0
            for l in range(len(trg_categ_words)):
                if(trg_categ_words[l][0]==trgpos):
                    break
                else:
                    if(trg_categ_words[l][1]==curr_categ_word):
                        curr_categ_word_order+=1

            aux_order=0
            for l in range(len(src_categ_words)):
                if(categorize_word(src_categ_words[l][1])==curr_categ_word):
                    if(aux_order==curr_categ_word_order):
                        decateg_word=src_categ_words[l][1]
                        break
                    else:
                        aux_order+=1

            # Return decategorized word
            if(decateg_word==""):
                return trg_word_array[trgpos]
            else:
                return decateg_word
        else:
            return trg_word_array[trgpos]

##################################################
class Decoder:

    def __init__(self,tmodel,lmodel,weights):
        # Initialize data members
        self.tmodel=tmodel
        self.lmodel=lmodel
        self.weights=weights

        # Checking on weight list
        if(len(self.weights)!=4):
            self.weights=[1,1,1,1]
        else:
            print >> sys.stderr, "Decoder weights:",
            for i in range(len(weights)):
                print >> sys.stderr, weights[i],
            print >> sys.stderr,""

        # Set indices for weight list
        self.tmw_idx=0
        self.phrpenw_idx=1
        self.wpenw_idx=2
        self.lmw_idx=3

    #####
    def opt_contains_src_words(self,src_words,opt):

        st=""
        src_words_array=src_words.split()
        for i in range(len(src_words_array)):
            st=st+src_words_array[i]

        if(st==opt):
            return True
        else:
            return False

    #####
    def tm_ext_lp(self,new_src_words,opt,verbose):

        lp=math.log(self.tmodel.obtain_trgsrc_prob_smoothed(new_src_words,opt))

        if(verbose==True):
            print >> sys.stderr,"  tm: logprob(",opt.encode("utf-8"),"|",new_src_words.encode("utf-8"),")=",lp

        return lp

    #####
    def pp_ext_lp(self,verbose):

        lp=math.log(1.0/math.e)

        if(verbose==True):
            print >> sys.stderr,"  pp:",lp

        return lp

    #####
    def wp_ext_lp(self,words,verbose):

        nw=len(words.split())

        lp=nw*math.log(1/math.e)

        if(verbose==True):
            print >> sys.stderr,"  wp:",lp

        return lp

    #####
    def lm_transform_word(self,word):
        # Do not alter word
        return word

    #####
    def lm_transform_word_unk(self,word):
        # Introduce unknown word
        if(self.lmodel.obtain_ng_count(word)==0):
            return _global_unk_word_str
        else:
            return word

    #####
    def lm_ext_lp(self,hyp_words,opt,verbose):

        ## Obtain lm history
        rawhist=self.lmodel.get_lm_state(hyp_words)
        rawhist_array=rawhist.split()
        hist=""
        for i in range(len(rawhist_array)):
            word=self.lm_transform_word(rawhist_array[i])
            if(hist==""):
                hist=word
            else:
                hist=hist+" "+word

        # Obtain logprob for new words
        lp=0
        opt_words_array=opt.split()
        for i in range(len(opt_words_array)):
            word=self.lm_transform_word(opt_words_array[i])
            if(hist==""):
                ngram=word
            else:
                ngram=hist+" "+word
            lp_ng=math.log(self.lmodel.obtain_trgsrc_interp_prob(ngram))
            lp=lp+lp_ng
            if(verbose==True):
                print >> sys.stderr,"  lm: logprob(",word.encode("utf-8"),"|",hist.encode("utf-8"),")=",lp_ng

            hist=self.lmodel.remove_oldest_word(ngram)

        return lp

    #####
    def expand(self,tok_array,hyp,new_hyp_cov,verbose):
        # Init result
        exp_list=[]

        # Obtain words to be translated
        new_src_words=""
        last_cov_pos=self.last_cov_pos(hyp.data.coverage)
        for i in range(last_cov_pos+1,new_hyp_cov+1):
            if(new_src_words==""):
                new_src_words=tok_array[i]
            else:
                new_src_words=new_src_words+" "+tok_array[i]

        # Obtain translation options
        opt_list=self.tmodel.obtain_opts_for_src(new_src_words)

        # If there are no options and only one source word is being covered,
        # artificially add one
        if(len(opt_list)==0 and len(new_src_words.split())==1):
            opt_list.append(new_src_words)

        # Print information about expansion if in verbose mode
        if(verbose==True):
            print >> sys.stderr,"++ expanding -> new_hyp_cov:",new_hyp_cov,"; new_src_words:",new_src_words.encode("utf-8"),"; num options:",len(opt_list)

        # Iterate over options
        for opt in opt_list:

            if(verbose==True):
                print >> sys.stderr,"   option:",opt.encode("utf-8")

            # Extend hypothesis

            ## Obtain new hypothesis
            bfsd_newhyp=BfsHypdata()

            # Obtain coverage for new hyp
            for k in range(len(hyp.data.coverage)):
                bfsd_newhyp.coverage.append(hyp.data.coverage[k])
            bfsd_newhyp.coverage.append(new_hyp_cov)

            # Obtain list of words for new hyp
            if(hyp.data.words==""):
                bfsd_newhyp.words=opt
            else:
                bfsd_newhyp.words=hyp.data.words
                bfsd_newhyp.words=bfsd_newhyp.words+" "+opt

            ## Obtain score for new hyp

            # Add translation model contribution
            tm_lp=self.tm_ext_lp(new_src_words,opt,verbose)
            w_tm_lp=self.weights[self.tmw_idx]*tm_lp

            # Add phrase penalty contribution
            pp_lp=self.pp_ext_lp(verbose)
            w_pp_lp=self.weights[self.phrpenw_idx]*pp_lp

            # Add word penalty contribution
            wp_lp=self.wp_ext_lp(opt,verbose)
            w_wp_lp=self.weights[self.wpenw_idx]*wp_lp

            # Add language model contribution
            lm_lp=self.lm_ext_lp(hyp.data.words,opt,verbose)
            w_lm_lp=self.weights[self.lmw_idx]*lm_lp

            # Add language model contribution for <bos> if hyp is
            # complete
            w_lm_end_lp=0
            if(self.cov_is_complete(bfsd_newhyp.coverage,tok_array)):
                lm_end_lp=self.lm_ext_lp(bfsd_newhyp.words,_global_eos_str,verbose)
                w_lm_end_lp=self.weights[self.lmw_idx]*lm_end_lp

            if(verbose==True):
                print >> sys.stderr, "   expansion ->","w. lp:",hyp.score+w_tm_lp+w_pp_lp+w_lm_lp+w_lm_end_lp,"; w. tm logprob:",w_tm_lp,"; w. pp logprob:",w_pp_lp,"; w. wp logprob:",w_wp_lp,"; w. lm logprob:",w_lm_lp,"; w. lm end logprob:",w_lm_end_lp,";",str(bfsd_newhyp)
                print >> sys.stderr, "   ----"

            # Obtain new hypothesis
            newhyp=Hypothesis()
            newhyp.score=hyp.score + w_tm_lp + w_pp_lp + w_wp_lp + w_lm_lp + w_lm_end_lp
            newhyp.data=bfsd_newhyp

            # Add expansion to list
            exp_list.append(newhyp)

        # Return result
        return exp_list

    #####
    def last_cov_pos(self,coverage):

        if(len(coverage)==0):
            return -1
        else:
            return coverage[len(coverage)-1]

    #####
    def hyp_is_complete(self,hyp,src_word_array):

        return self.cov_is_complete(hyp.data.coverage,src_word_array)

    #####
    def cov_is_complete(self,coverage,src_word_array):

        if(self.last_cov_pos(coverage)==len(src_word_array)-1):
            return True
        else:
            return False

    #####
    def obtain_nblist(self,src_word_array,nblsize,verbose):
        # Insert initial hypothesis in stack
        priority_queue=PriorityQueue()
        hyp=Hypothesis()
        priority_queue.put(hyp)

        # Create state dictionary
        stdict=StateInfoDict()
        stdict.insert(obtain_state_info(self.tmodel,self.lmodel,hyp),hyp.score)
        stdict.insert(obtain_state_info(self.tmodel,self.lmodel,hyp),hyp.score)

        # Obtain n-best hypotheses
        nblist=[]
        for i in xrange(nblsize):
            hyp=self.best_first_search(src_word_array,priority_queue,stdict,verbose)

            # Append hypothesis to nblist
            if(len(hyp.data.coverage)>0):
                nblist.append(hyp)

        # return result
        return nblist

    #####
    def obtain_detok_sent(self,tok_array,best_hyp):

        # Check if tok_array is not empty
        if(len(tok_array)>0):
            # Init variables
            result=""
            coverage=best_hyp.data.coverage
            # Iterate over hypothesis coverage array
            for i in range(len(coverage)):
                # Obtain leftmost source position
                if(i==0):
                    leftmost_src_pos=0
                else:
                    leftmost_src_pos=coverage[i-1]+1

                # Obtain detokenized word
                detok_word=""
                for j in range(leftmost_src_pos,coverage[i]+1):
                    detok_word=detok_word+tok_array[j]

                # Incorporate detokenized word to detokenized sentence
                if(i==0):
                    result=detok_word
                else:
                    result=result+" "+detok_word
            # Return detokenized sentence
            return result
        else:
            return ""

    #####
    def get_hypothesis_to_expand(self,priority_queue,stdict):

        while(True):
            if(priority_queue.empty()==True):
                return (True,Hypothesis())
            else:
                hyp=priority_queue.get()
                sti=obtain_state_info(self.tmodel,self.lmodel,hyp)
                if(stdict.hyp_recombined(sti,hyp.score)==False):
                    return(False,hyp)

    #####
    def best_first_search(self,src_word_array,priority_queue,stdict,verbose):
        # Initialize variables
        end=False
        niter=0

        if(verbose==True):
            print >> sys.stderr, "*** Starting best first search..."

        # Start best-first search
        while not end:
            # Obtain hypothesis to expand
            empty,hyp=self.get_hypothesis_to_expand(priority_queue,stdict)
            # Check if priority queue is empty
            if(empty):
                end=True
            else:
                # Expand hypothesis
                if(verbose==True):
                    print >> sys.stderr, "** niter:",niter," ; lp:",hyp.score,";",str(hyp.data)
                # Stop if the hypothesis is complete
                if(self.hyp_is_complete(hyp,src_word_array)==True):
                    end=True
                else:
                    # Expand hypothesis
                    for l in range(0,_global_a_par):
                        new_hyp_cov=self.last_cov_pos(hyp.data.coverage)+1+l
                        if(new_hyp_cov<len(src_word_array)):
                            # Obtain expansion
                            exp_list=self.expand(src_word_array,hyp,new_hyp_cov,verbose)
                            # Insert new hypotheses
                            for k in range(len(exp_list)):
                                # Insert hypothesis
                                priority_queue.put(exp_list[k])
                                # Update state info dictionary
                                sti=obtain_state_info(self.tmodel,self.lmodel,exp_list[k])
                                stdict.insert(sti,exp_list[k].score)

            niter=niter+1

            if(niter>_global_maxniters):
                end=True

        # Return result
        if(niter>_global_maxniters):
            if(verbose==True):
                print  >> sys.stderr, "Warning: maximum number of iterations exceeded"
            return Hypothesis()
        else:
            if(self.hyp_is_complete(hyp,src_word_array)==True):
                if(verbose==True):
                    print >> sys.stderr, "*** Best first search finished successfully after",niter,"iterations, hyp. score:",hyp.score
                hyp.score=hyp.score
                return hyp
            else:
                if(verbose==True):
                    print >> sys.stderr, "Warning: priority queue empty, search was unable to reach a complete hypothesis"
                return Hypothesis()

    #####
    def detokenize(self,file,verbose):
        # read raw file line by line
        lineno=0
        for line in file:
            # Obtain array with tokenized words
            lineno=lineno+1
            line=line.strip("\n")
            tok_array=line.split()
            nblsize=1
            if(verbose==True):
                print >> sys.stderr,""
                print >> sys.stderr,"**** Processing sentence: ",line.encode("utf-8")

            if(len(tok_array)>0):
                # Transform array of tokenized words
                trans_tok_array=[]
                for i in range(len(tok_array)):
                    trans_tok_array.append(transform_word(tok_array[i]))

                # Obtain n-best list of detokenized sentences
                nblist=self.obtain_nblist(trans_tok_array,nblsize,verbose)

                # Print detokenized sentence
                if(len(nblist)==0):
                    print line.encode("utf-8")
                    print >> sys.stderr, "Warning: no detokenizations were found for sentence in line",lineno
                else:
                    best_hyp=nblist[0]
                    detok_sent=self.obtain_detok_sent(tok_array,best_hyp)
                    print detok_sent.encode("utf-8")
            else:
                print ""

    #####
    def recase(self,file,verbose):
        # read raw file line by line
        lineno=0
        for line in file:
            # Obtain array with tokenized words
            lineno=lineno+1
            line=line.strip("\n")
            lc_word_array=line.split()
            nblsize=1
            if(verbose==True):
                print >> sys.stderr,""
                print >> sys.stderr,"**** Processing sentence: ",line.encode("utf-8")

            if(len(lc_word_array)>0):
                # Obtain n-best list of detokenized sentences
                nblist=self.obtain_nblist(lc_word_array,nblsize,verbose)

                # Print recased sentence
                if(len(nblist)==0):
                    print line.encode("utf-8")
                    print >> sys.stderr, "Warning: no recased sentences were found for sentence in line",lineno
                else:
                    best_hyp=nblist[0]
                    print best_hyp.data.words.encode("utf-8")
            else:
                print ""

##################################################
class TokenizerSimple:

    def __init__(self):
        self.RX = re.compile(r'(\w+)|([^\w\s]+)', re.U)

    def tokenize(self, s):
        aux = filter(None, self.RX.split(s))
        return filter(None, [s.strip() for s in aux])

##################################################
class TokenizerComplex:

  def __init__(self):
    digits = r'([\d\.,]+)'
    hiphen_words = r'([^\W\d_]+\-[^\W\d_]+)'
    acronyms_short = r'([^\W\d_]{,3}\.)'
    acronyms_long = r'([^\W\d_]{,3}\.[^\W\d_]{,3}\.)'
    alfanum = r'(\w+)'
    non_alpha = r'([^\W\d_]+)'
    self.RX = re.compile(r'%s|%s|%s|%s|%s|%s' % (digits, hiphen_words, 
                                                 acronyms_long, acronyms_short, 
                                                 alfanum, non_alpha), re.U)

  def tokenize(self, s):
    aux = filter(None, self.RX.split(s))
    return filter(None, [s.strip() for s in aux])
##################################################
default_atoms = [
    "'em",
    "'ol",
    "vs.",
    "Ms.",
    "Mr.",
    "Dr.",
    "Mrs.",
    "Messrs.",
    "Gov.",
    "Gen.",
    "Mt.",
    "Corp.",
    "Inc.",
    "Co.",
    "co.",
    "Ltd.",
    "Bros.",
    "Rep.",
    "Sen.",
    "Jr.",
    "Rev.",
    "Adm.",
    "St.",
    "a.m.",
    "p.m.",
    "1a.m.",
    "2a.m.",
    "3a.m.",
    "4a.m.",
    "5a.m.",
    "6a.m.",
    "7a.m.",
    "8a.m.",
    "9a.m.",
    "10a.m.",
    "11a.m.",
    "12a.m.",
    "1am",
    "2am",
    "3am",
    "4am",
    "5am",
    "6am",
    "7am",
    "8am",
    "9am",
    "10am",
    "11am",
    "12am",
    "p.m.",
    "1p.m.",
    "2p.m.",
    "3p.m.",
    "4p.m.",
    "5p.m.",
    "6p.m.",
    "7p.m.",
    "8p.m.",
    "9p.m.",
    "10p.m.",
    "11p.m.",
    "12p.m.",
    "1pm",
    "2pm",
    "3pm",
    "4pm",
    "5pm",
    "6pm",
    "7pm",
    "8pm",
    "9pm",
    "10pm",
    "11pm",
    "12pm",
    "Jan.",
    "Feb.",
    "Mar.",
    "Apr.",
    "May.",
    "Jun.",
    "Jul.",
    "Aug.",
    "Sep.",
    "Sept.",
    "Oct.",
    "Nov.",
    "Dec.",
    "Ala.",
    "Ariz.",
    "Ark.",
    "Calif.",
    "Colo.",
    "Conn.",
    "Del.",
    "D.C.",
    "Fla.",
    "Ga.",
    "Ill.",
    "Ind.",
    "Kans.",
    "Kan.",
    "Ky.",
    "La.",
    "Md.",
    "Mass.",
    "Mich.",
    "Minn.",
    "Miss.",
    "Mo.",
    "Mont.",
    "Nebr.",
    "Neb.",
    "Nev.",
    "N.H.",
    "N.J.",
    "N.M.",
    "N.Y.",
    "N.C.",
    "N.D.",
    "Okla.",
    "Ore.",
    "Pa.",
    "Tenn.",
    "Va.",
    "Wash.",
    "Wis.",
    ":)",
    "<3",
    ";)",
    "(:",
    ":(",
    "-_-",
    "=)",
    ":/",
    ":>",
    ";-)",
    ":Y",
    ":P",
    ":-P",
    ":3",
    "=3",
    "xD",
    "^_^",
    "=]",
    "=D",
    "<333",
    ":))",
    ":0",
    "-__-",
    "xDD",
    "o_o",
    "o_O",
    "V_V",
    "=[[",
    "<33",
    ";p",
    ";D",
    ";-p",
    ";(",
    ":p",
    ":]",
    ":O",
    ":-/",
    ":-)",
    ":(((",
    ":((",
    ":')",
    "(^_^)",
    "(=",
    "o.O",
    "a.",
    "b.",
    "c.",
    "d.",
    "e.",
    "f.",
    "g.",
    "h.",
    "i.",
    "j.",
    "k.",
    "l.",
    "m.",
    "n.",
    "o.",
    "p.",
    "q.",
    "s.",
    "t.",
    "u.",
    "v.",
    "w.",
    "x.",
    "y.",
    "z.",
    "i.e.",
    "I.e.",
    "I.E.",
    "e.g.",
    "E.g.",
    "E.G."
]


_default_word_chars =                           \
        u"-&"                                   \
        u"0123456789"                           \
        u"ABCDEFGHIJKLMNOPQRSTUVWXYZ"           \
        u"abcdefghijklmnopqrstuvwxyz"           \
        u"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞß"      \
        u"àáâãäåæçèéêëìíîïðñòóôõöøùúûüýþÿ"      \
        u"ĀāĂăĄąĆćĈĉĊċČčĎďĐđĒēĔĕĖėĘęĚěĜĝĞğ"     \
        u"ĠġĢģĤĥĦħĨĩĪīĬĭĮįİıĲĳĴĵĶķĸĹĺĻļĽľĿŀŁł"  \
        u"ńŅņŇňŉŊŋŌōŎŏŐőŒœŔŕŖŗŘřŚśŜŝŞşŠšŢţŤťŦŧ" \
        u"ŨũŪūŬŭŮůŰűŲųŴŵŶŷŸŹźŻżŽžſ"             \
        u"ΑΒΓΔΕΖΗΘΙΚΛΜΝΟΠΡΣΤΥΦΧΨΩΪΫ"            \
        u"άέήίΰαβγδεζηθικλμνξοπρςστυφχψω"


class Tokenizer:

    def __init__(self, atoms=default_atoms, word_chars=_default_word_chars):
        """Initializer.

        Atom is a string that won't be split into separate tokens.  Longer
        atoms take precedence over their prefixes, e.g.: if 'a' and 'ab' are
        passed as atoms 'ab' will be returned.
        """

        atoms = map(re.escape, sorted(atoms, key=len, reverse=True))
        word_chars = re.escape(word_chars)

        self.re = re.compile("(?:" + "|".join(
            [a+"\\b" for a in atoms] +
            ["\\b[0-9]+[,\.][0-9]+[a-zA-Z]+\\b",
             "\\b[0-9]+[,\.][0-9]+\\b",
             "[%s]+(?:'[sS])?" % (word_chars),
             "\s+"
            ]) + ")", flags=re.I)

    def tokenize(self, text):
        """Tokenizes text :: string -> [strings].

        Concatenation of returned tokens should yields.
        """

        # Preprocessing step to avoid special case of all words concatenared with hyphens
        # adidas-climalite-mens-l-white-black-tank-top-nwt-msrp-basketball
        if len(text.split()) == 1 and text.count('-') > 1:
            text = text.replace('-', ' ')


        # Following is a safeguard that guarantees that concatenating resultant
        # tokens yield original text.  It fills gaps between matches returned
        # by findall().  Ideally, it shouldn't be needed.  findall() should
        # return all required substrings.
        #
        # However, REs are tricky, msitakes hapenn, therefore we choose to be
        # defensive:
        matches = self.re.findall(text)
        tokens = []
        p = 0
        for match in matches:
            #print "M ->", match
            mp = text[p:].find(match)
            if mp != 0:
                missed = text[p:p+mp]
                #print "m ->", missed
                tokens.append(missed)
                #_log.warning(u'Tokenizer missed substring "{}"'.format(missed))
            p = p + mp + len(match)
            tokens.append(match)

        if p < len(text):
            tokens.append(text[p:])

        return filter(lambda s: s.strip(), tokens)


##################################################
def tokenize(string):
    tokenizer = Tokenizer()
    skel = annotated_string_to_xml_skeleton(string)
    for idx, (is_tag, txt) in enumerate(skel):
        if is_tag:
            skel[idx][1] = [ skel[idx][1] ]
        else:
            skel[idx][1] = tokenizer.tokenize(txt)
    return xml_skeleton_to_tokens(skel)

##################################################
def xml_skeleton_to_tokens(skeleton):
    """
    Joins back the elements in a skeleton to return a list of tokens
    """
    annotated = []
    for _, tokens in skeleton:
        annotated.extend(tokens)
    return annotated

##################################################
def lowercase(string):
    #return str.lower()
    skel = annotated_string_to_xml_skeleton(string)
    for idx, (is_tag, txt) in enumerate(skel):
        if is_tag:
            skel[idx][1] = txt.strip()
        else:
            skel[idx][1] = txt.lower().strip()
    return xml_skeleton_to_string(skel)

##################################################
def xml_skeleton_to_string(skeleton):
    """
    Joins back the elements in a skeleton to return an annotated string
    """
    return u" ".join(txt for _,txt in skeleton)

##################################################
def annotated_string_to_xml_skeleton(annotated):
    """
    Parses a string looking for XML annotations
    returns a vector where each element is a pair (is_tag, text)
    """
    offset = 0
    skeleton = list()
    for m in _annotation.finditer(annotated):
        if offset < m.start():
            skeleton.append( [False, annotated[offset:m.start()]] )
        offset = m.end()
        g = m.groups()
        dic_g = filter(None, g[0:8])
        len_g = filter(None, g[8:11])
        ann = None
        if dic_g:
            ann = [[True, dic_g[0]],
                     [True, dic_g[1]], [False, dic_g[2]], [True, dic_g[3]],
                     [True, dic_g[4]], [False, dic_g[5]], [True, dic_g[6]],
                   [True, dic_g[7]]]
        elif len_g:
            ann = [[True, len_g[0]], [False, len_g[1]], [True, len_g[2]]]
        else:
            sys.stderr.write('WARNING:\n - s: %s\n - g: %s\n' % (annotated,g))
        if ann is not None:
            skeleton.extend(ann)
    if offset < len(annotated):
        skeleton.append( [False, annotated[offset:]] )
    return skeleton

##################################################
def remove_xml_annotations(annotated):
    xml_tags = set(['<'+src_ann+'>', '</'+len_ann+'>', '</'+grp_ann+'>'])
    skeleton = annotated_string_to_xml_skeleton(annotated)
    tokens = list()
    for i in range(len(skeleton)):
        is_tag, text = skeleton[i]
        token = text.strip()
        if not is_tag and token:
            if i == 0:
                tokens.append(token)
            else:
                ant_is_tag, ant_text = skeleton[i-1]
                if not ant_is_tag or (ant_is_tag and 
                                      ant_text.strip() in xml_tags):
                    tokens.append(token)
    return u' '.join(tokens)

##################################################
