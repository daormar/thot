/*
thot package for statistical machine translation
Copyright (C) 2013 Daniel Ortiz-Mart\'inez
 
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with this program; If not, see <http://www.gnu.org/licenses/>.
*/
 
/********************************************************************/
/*                                                                  */
/* Module: thot_aligner                                             */
/*                                                                  */
/* Definitions file: thot_aligner.cc                                */
/*                                                                  */
/* Description: Implements a phrase-based alignent system           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "SmtModelTypes.h"
#include "MultiStackTypes.h"
#include "StackDecSwModelTypes.h"
#include "ctimer.h"  // Module for obtain the elapsed time
#include "options.h"
#include "ErrorDefs.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

//--------------- Constants ------------------------------------------

#define PALIG_W_DEFAULT 10

#ifdef MULTI_STACK_USE_GRAN
 #define PALIG_S_DEFAULT 1024
#else
 #define PALIG_S_DEFAULT 64
#endif

#define PALIG_A_DEFAULT 10
#define PALIG_E_DEFAULT 10
#define PALIG_I_DEFAULT 1
#define PALIG_G_DEFAULT 0
#define PALIG_H_DEFAULT NO_HEURISTIC

//--------------- Function Declarations ------------------------------

int init_translator(void);
void release_translator(void);
int align_corpus(void);
Vector<string> stringToStringVector(string s);
void version(void);
void print_alig_a3_final(std::string srcstr,
                         std::string trgstr,
                         CURR_MODEL_TYPE::Hypothesis hyp,
                         unsigned int sentNo);
int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printConfig(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

CURR_MODEL_TYPE *pbtModelPtr;
CURR_MSTACK_TYPE<CURR_MODEL_TYPE>* translatorPtr;
bool p_option;
bool c_option;
bool monotoneSearch;
bool bf;
float W;
int A,E,U,S,I,G,heuristic,verbosity,bestTrans;
std::string sourceSentencesFile;
std::string refSentencesFile;
std::string languageModelFileName;
std::string transModelPref;
std::string wordGraphFileName;
float wgPruningThreshold;
unsigned int sentenceNo=0,numWords=0;	
Vector<float> weightVec;

//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc, char *argv[])
{
  if(TakeParameters(argc,argv)==OK)
  {
        // init translator    
    if(init_translator()==ERROR)
    {      
      cerr<<"Error during the initialization of the translator"<<endl;
      return ERROR;
    }
    else
    {
      unsigned int ret;

      ret=align_corpus();
      release_translator();
      if(ret==ERROR) return ERROR;
      else return OK;
    }
  }
  else return ERROR;
}

//--------------- init_translator function
int init_translator(void)
{
  int err;
  
  cerr<<"\n- Initializing model and test corpus...\n\n";

  pbtModelPtr=new CURR_MODEL_TYPE();

  err=pbtModelPtr->loadLangModel(languageModelFileName.c_str());
  if(err==ERROR)
  {
    delete pbtModelPtr;
    return ERROR;
  }

  err=pbtModelPtr->loadAligModel(transModelPref.c_str());
  if(err==ERROR)
  {
    delete pbtModelPtr;
    return ERROR;
  }

      // Set heuristic
  pbtModelPtr->setHeuristic(heuristic);

      // Set weights
  pbtModelPtr->setWeights(weightVec);
  pbtModelPtr->printWeights(cerr);
  cerr<<endl;

      // Set monotone search flag
  if(monotoneSearch) pbtModelPtr->setMonotoneSearch();
  else pbtModelPtr->resetMonotoneSearch();

      // Set model parameters
  pbtModelPtr->set_W_par(W);
  pbtModelPtr->set_A_par(A);
  pbtModelPtr->set_E_par(E);
  pbtModelPtr->set_U_par(U);
  pbtModelPtr->setVerbosity(verbosity);

      // Create a translator instance
  translatorPtr=new CURR_MSTACK_TYPE<CURR_MODEL_TYPE>();

      // Link translation model
  translatorPtr->link_smt_model(pbtModelPtr);
    
      // Set translator parameters
  translatorPtr->set_S_par(S);
  translatorPtr->set_I_par(I);
#ifdef MULTI_STACK_USE_GRAN
  translatorPtr->set_G_par(G);
#endif
      // Enable best score pruning if the decoder is not going to obtain
      // n-best translations or word-graphs
  if(bestTrans==1 && wgPruningThreshold==DISABLE_WORDGRAPH)
    translatorPtr->useBestScorePruning(true);

      // Set breadthFirst flag
  translatorPtr->set_breadthFirst(bf);

#ifndef THOT_DISABLE_REC
      // Enable word graph according to wgPruningThreshold
  if(wordGraphFileName!="")
  {
    
    if(wgPruningThreshold!=DISABLE_WORDGRAPH)
      translatorPtr->enableWordGraph();    
  }
#endif
      // Set translator verbosity
  translatorPtr->setVerbosity(verbosity);

  return OK;
}

//--------------- release_translator function
void release_translator(void)
{
  delete pbtModelPtr;
  delete translatorPtr;
}

//--------------- TranslateTestCorpus template function

int align_corpus(void)
{
  CURR_MODEL_TYPE::Hypothesis result;     // Results of the translation
  CURR_MODEL_TYPE::Hypothesis anotherTrans;     // Another results of the translation
  int sentNo=0;    
  double elapsed_ant,elapsed,ucpu,scpu,total_time=0;
      
  ifstream testCorpusFile;                // Test corpus file stream
  ifstream refCorpusFile;                 // reference corpus file stream
  string srcSentenceString,trgSentenceString,s;
  

      // Open test corpus file
  testCorpusFile.open(sourceSentencesFile.c_str());
  if(testCorpusFile.fail())
  {
    cerr<<"Error while opening file with test sentences "<<sourceSentencesFile<<endl;
    return ERROR;
  }
  testCorpusFile.seekg(0, ios::beg);

      // Open ref corpus file
  refCorpusFile.open(refSentencesFile.c_str());
  if(refCorpusFile.fail())
  {
    cerr<<"Error while opening file with references "<<refSentencesFile<<endl;
    return ERROR;
  }
  refCorpusFile.seekg(0, ios::beg);

  cerr<<"\n- Processing corpora...\n\n";

  if(!testCorpusFile)
  {
    cerr<<"Test corpus error!"<<endl;
    return ERROR;
  }
  else
  {
        // Align corpus sentences
    while(!testCorpusFile.eof())
    {
      getline(testCorpusFile,srcSentenceString);
      getline(refCorpusFile,trgSentenceString);

      if(srcSentenceString!="")
      {
        ++sentNo;
        
        if(verbosity)
        {
          cerr<<sentNo<<endl<<srcSentenceString<<endl;
          ctimer(&elapsed_ant,&ucpu,&scpu);
        }
       
            //------- Align sentence
        if(p_option)
        {
              // Translate with prefix
          result=translatorPtr->translateWithPrefix(srcSentenceString,trgSentenceString);
        }
        else
        {
          if(c_option)
          {
                // Verify model coverage
            result=translatorPtr->verifyCoverageForRef(srcSentenceString,trgSentenceString);
          }
          else
          {
                // Translate with reference
            result=translatorPtr->translateWithRef(srcSentenceString,trgSentenceString);
          }
        }
            //--------------------------
        if(verbosity) ctimer(&elapsed,&ucpu,&scpu);
               
        print_alig_a3_final(srcSentenceString,trgSentenceString,result,sentNo);
          
        if(verbosity)
        {
          pbtModelPtr->printHyp(result,cerr,verbosity);
#         ifdef THOT_STATS
          translatorPtr->printStats();
#         endif

          cerr<<"- Elapsed Time: "<<elapsed-elapsed_ant<<endl<<endl;
          total_time+=elapsed-elapsed_ant;
        }
#ifndef THOT_DISABLE_REC
            // Print wordgraph if the -wg option was given
        if(wordGraphFileName!="")
        {
          char wgFileNameForSent[256];
          sprintf(wgFileNameForSent,"%s_%06d",wordGraphFileName.c_str(),sentNo);
          translatorPtr->pruneWordGraph(wgPruningThreshold);
          translatorPtr->printWordGraph(wgFileNameForSent);
        }
#endif
#ifdef THOT_ENABLE_GRAPH
        char printGraphFileName[256];
        ofstream outS;
        sprintf(printGraphFileName,"sent%d.graph_file",sentNo);
        outS.open(printGraphFileName,ios::out);
        if(!outS) cerr<<"Error while printing search graph to file."<<endl;
        else
        {
          translatorPtr->printSearchGraphStream(outS);
          outS<<"Stack ID. Out\n";
          translatorPtr->printGraphForHyp(result,outS);
          outS.close();        
        }
#endif        
            //------- Obtain additional alignments
        for(int n=0;n<bestTrans-1;++n)
        {
          if(verbosity)
          {
            ctimer(&elapsed_ant,&ucpu,&scpu);
            cerr<<"Additional alignments "<<n+1<<endl;
          }
          anotherTrans=translatorPtr->getNextTrans();
          if(!pbtModelPtr->isComplete(anotherTrans))
          {
            cerr<<"No more additional translations!"<<endl;
            break;
          }
          print_alig_a3_final(srcSentenceString,trgSentenceString,anotherTrans,sentNo);
          if(verbosity)
          {
            ctimer(&elapsed,&ucpu,&scpu);
            pbtModelPtr->printHyp(anotherTrans,cerr,verbosity);
            cerr<<" * Time: " << elapsed-elapsed_ant << " secs\n";
          }
        }
        if(bestTrans>1)
        {
          cout<<"<eonb>"<<endl;
        }
            //--------------------------------------
      }    
    }
    testCorpusFile.close(); 
  }

  if(verbosity)
  {
    cerr<<"- Time per sentence: "<<total_time/sentNo<<endl;
  }

  return OK;
}

//---------------------------------------
void print_alig_a3_final(std::string srcstr,
                         std::string trgstr,
                         CURR_MODEL_TYPE::Hypothesis hyp,
                         unsigned int sentNo)
{
  CURR_MODEL_TYPE::Hypothesis::DataType dataType;
  Vector<std::string> sysTrgVec;
  Vector<std::string> trgVec;
    
  sysTrgVec=pbtModelPtr->getTransInPlainTextVec(hyp);
  trgVec=stringToStringVector(trgstr);
  dataType=hyp.getData();
  cout<<"# "<<sentNo <<" ; Align. score= "<<hyp.getScore()<<endl;
  cout<<srcstr<<endl;
  cout<<"NULL ({ })";
  if(sysTrgVec!=trgVec && !p_option)
  {
        // If the alignment is incomplete, align each target word with
        // each source word
    unsigned int srcsize=stringToStringVector(srcstr).size();
    for(unsigned int i=0;i<trgVec.size();++i)
    {
      cout<<" "<<trgVec[i]<<" ({ ";
      for(unsigned int j=1;j<=srcsize;++j) cout<<j<<" ";
      cout<<"})";
    }
    cout<<endl;
  }
  else
  {
    unsigned int i;
    i=1;
    for(unsigned int k=0;k<dataType.targetSegmentCuts.size();++k)
    {
      for(;i<=dataType.targetSegmentCuts[k];++i)
      {
        cout<<" "<<sysTrgVec[i-1]<<" ({ ";
        for(unsigned int j=dataType.sourceSegmentation[k].first;j<=dataType.sourceSegmentation[k].second;++j)
        {
          cout<<j<<" ";
        }
        cout<<"})";
      }
    }
    cout<<endl;
  }
}

//--------------- TakeParameters function
int TakeParameters(int argc,char *argv[])
{
 char s[512];
 int err;

 if(argc==1)
 {
   version();
   return ERROR;   
 }

 err=readOption(argc,argv,"--help");
 if(err!=-1)
 {
   printUsage();
   return ERROR;   
 }      

     /* Verify --version option */
 err=readOption(argc,argv, "--version");
 if(err!=-1)
 {
   version();
   return ERROR;
 }

     /* Verify --config option */
 err=readOption(argc,argv, "--config");
 if(err!=-1)
 {
   printConfig();
   return ERROR;
 }

 err=readOption(argc,argv,"-p");
 p_option=true;
 if(err==-1)
 {
   p_option=false;
 }    

 err=readOption(argc,argv,"-c");
 c_option=true;
 if(err==-1)
 {
   c_option=false;
 }    
 else
 {
   if(p_option)
   {
     cerr<<"Error: -p and -c options cannot be given simultaneously"<<endl;
     return ERROR;
   }
 }
 
     // Takes number of best translations 
 err=readInt(argc,argv, "-b", &(bestTrans));
 if(err==-1)
 {
   bestTrans=1;
 }  

     // Takes nBestTrans 
 err=readFloat(argc,argv, "-W", &W);
 if(err==-1)
 {
   W=PALIG_W_DEFAULT;
 }

     // Takes S parameter 
 err=readInt(argc,argv, "-S", &(S));
 if(err==-1)
 {
   S=PALIG_S_DEFAULT;
 }  

     // Takes A parameter 
 err=readInt(argc,argv, "-A", &A);
 if(err==-1)
 {
   A=PALIG_A_DEFAULT;
 }

     // Takes E parameter 
 err=readInt(argc,argv, "-E", &E);
 if(err==-1)
 {
   E=PALIG_E_DEFAULT;
 }

     // Takes U parameter 
 err=readInt(argc,argv, "-U", &U);
 if(err==-1)
 {
   U=MAX_SENTENCE_LENGTH_ALLOWED;
 }

     // Takes N parameter 
 err=readInt(argc,argv, "-I", &I);
 if(err==-1)
 {
   I=PALIG_I_DEFAULT;
 }

     // Takes I parameter 
 err=readInt(argc,argv, "-G", &G);
 if(err==-1)
 {
   G=PALIG_G_DEFAULT;
 }

     // Takes h parameter 
 err=readInt(argc,argv, "-h", &heuristic);
 if(err==-1)
 {
   heuristic=PALIG_H_DEFAULT;
 }

     // Take language model file name
 err=readString(argc,argv, "-lm", s);
 if(err==-1)
 {
   cerr<<"Error: parameter -lm not given!"<<endl;
   printUsage();
   return ERROR;   
 }
 else languageModelFileName=s;

     // Take read table prefix 
 err=readString(argc,argv, "-tm", s);
 if(err==-1)
 {
   cerr<<"Error: parameter -tm not given!"<<endl;
   printUsage();
   return ERROR;   
 }
 else transModelPref=s;
 
     // Take file name with the test sentences 
 err=readString(argc,argv, "-t",s);
 if(err==-1)
 {
   cerr<<"Error: parameter -t not given!"<<endl;
   printUsage();
   return ERROR;   
 }
 else sourceSentencesFile=s;

     // Take file name with the reference sentences
 err=readString(argc,argv, "-r",s);
 if(err==-1)
 {
   cerr<<"Error: parameter -r not given!"<<endl;
   printUsage();
   return ERROR;   
 }
 else refSentencesFile=s;

        // Take -bf option
 err=readOption(argc,argv,"-bf");
 bf=1;
 if(err==-1)
 {
   bf=0;
 }      

     // Take -mon parameter
 err=readOption(argc,argv,"-mon");
 monotoneSearch=true;
 if(err==-1)
 {
   monotoneSearch=false;
 }    

     // Take -we parameter
 err=readFloatSeq(argc,argv, "-we", weightVec);
 if(err==-1)
 {
   weightVec.clear();
 }     

      // Take -wg parameter
 err=readString(argc,argv, "-wg", s);
 if(err==-1)
 {
   wordGraphFileName="";
   wgPruningThreshold=DISABLE_WORDGRAPH;
 }
 else
 {
   wordGraphFileName=s;

       // Take -wgp parameter 
   err=readFloat(argc,argv, "-wgp", &wgPruningThreshold);
   if(err==-1)
   {
     wgPruningThreshold=UNLIMITED_DENSITY;
   }
 }
 
     // Take verbosity parameter
 err=readOption(argc,argv,"-v");
 if(err==-1)
 {
       // -v not found
   err=readOption(argc,argv,"-v1");
   if(err==-1)
   {
         // -v1 not found
     err=readOption(argc,argv,"-v2");
     if(err==-1)
     {
           // -v2 not found
       verbosity=0;
     }
     else
     {
           // -v2 found
       verbosity=3;
     }
   }
   else
   {
         // -v1 found
     verbosity=2;
   }
 }
 else
 {
       // -v found
   verbosity=1;
 }

 cerr<<"p option: "<<p_option<<endl;
 cerr<<"c option: "<<c_option<<endl;
 cerr<<"b: "<<bestTrans<<endl;
 cerr<<"W: "<<W<<endl;   
 cerr<<"S: "<<S<<endl;   
 cerr<<"A: "<<A<<endl;
 cerr<<"E: "<<E<<endl;
 cerr<<"U: "<<U<<endl;   
 cerr<<"I: "<<I<<endl;
#ifdef MULTI_STACK_USE_GRAN
 cerr<<"G: "<<G<<endl;
#endif
 cerr<<"h: "<<heuristic<<endl;
 cerr<<"bf: "<<bf<<endl;
 cerr<<"monotone search: "<<monotoneSearch<<endl;
 cerr<<"weight vector:";
 for(unsigned int i=0;i<weightVec.size();++i)
   cerr<<" "<<weightVec[i];
 cerr<<endl;
 cerr<<"lmfile: "<<languageModelFileName<<endl;   
 cerr<<"tm files prefix: "<<transModelPref<<endl;   
 cerr<<"test file: "<<sourceSentencesFile<<endl;   
 cerr<<"ref file: "<<refSentencesFile<<endl;
 if(wordGraphFileName!="")
 {
   cerr<<"word graph file prefix: "<<wordGraphFileName<<endl;
   if(wgPruningThreshold==UNLIMITED_DENSITY)
     cerr<<"word graph pruning threshold: word graph density unrestricted"<<endl;
   else
     cerr<<"word graph pruning threshold: "<<wgPruningThreshold<<endl;
 }
 else
 {
   cerr<<"word graph file prefix not given (wordgraphs will not be generated)"<<endl;
 }
 cerr<<"verbosity level: "<<verbosity<<endl;
   
 return OK;
}

//--------------- stringToStringVector function
Vector<string> stringToStringVector(string s)
{
 Vector<string> vs;	
 string aux="";
 unsigned int i;	

 for(i=0;i<s.size();++i)
    {
	 if(s[i]!=' ') aux+=s[i];
         else if(aux!="") {vs.push_back(aux); aux="";}		 
	}
 
 if(aux!="") vs.push_back(aux); 	
 return vs;	
}

//--------------- printConfig() function
void printConfig(void)
{
  CURR_MODEL_TYPE model;

  cerr <<"* Aligner configuration:"<<endl;
      // Print translation model information
  cerr<< "  - Statistical machine translation model type: "<<CURR_MODEL_LABEL<<endl;
  if(strlen(CURR_MODEL_NOTES)!=0)
  {
    cerr << "  - Model notes: "<<CURR_MODEL_NOTES<<endl;
  }
  cerr<<"  - Weights for the smt model and their default values: ";
  model.printWeights(cerr);
  cerr<<endl;

      // Print language model information
  cerr << "  - Language model type: "<<THOT_CURR_LM_LABEL<<endl;
  if(strlen(THOT_CURR_LM_NOTES)!=0)
  {
    cerr << "  - Language model notes: "<<THOT_CURR_LM_NOTES<<endl;
  }

      // Print phrase-based model information
  cerr << "  - Phrase-based model type: "<<THOT_CURR_PBM_LABEL<<endl;
  if(strlen(THOT_CURR_PBM_NOTES)!=0)
  {
    cerr << "  - Phrase-based model notes: "<<THOT_CURR_PBM_NOTES<<endl;
  }

      // Print single-word model information
  cerr << "  - Single-word model type: "<<CURR_SW_MODEL_LABEL<<endl;
  if(strlen(CURR_SW_MODEL_NOTES)!=0)
  {
    cerr << "  - Single-word model notes: "<<CURR_SW_MODEL_NOTES<<endl;
  }

      // Print decoding algorithm information
  cerr << "  - Translator type: "<<CURR_MSTACK_LABEL<<endl;
  if(strlen(CURR_MSTACK_NOTES)!=0)
  {
    cerr << "  - Translator notes: "<<CURR_MSTACK_NOTES<<endl;
  }
  cerr << endl;
}

//--------------- printUsage() function
void printUsage(void)
{
  cerr << "thot_aligner   -tm <transModelPref> -lm <langModelFile> -t <testSentFile>"<<endl;
  cerr << "               -r <refSentFile> [-p|-c] [-b <int>] [-W <float>]"<<endl;
  cerr << "               [-S <int>] [-A <int>] [-E <int>] [-U <int>] [-I <int>]"<<endl;
  cerr << "               [-G <int>] [-h <int>] [-bf] [-mon]"<<endl;
  cerr << "               [-we <float> ... <float>]"<<endl;
#ifndef THOT_DISABLE_REC
  cerr << "               [-wg <wordGraphFile> [-wgp <float>] ]"<<endl;
#endif
  cerr << "               [-v|-v1|-v2] [--help] [--version] [--config]"<<endl<<endl;
  cerr << " -tm <transModelPref> : Prefix of the translation model files."<<endl;
  cerr << " -lm <langModelFile>  : Language model file name."<<endl;
  cerr << " -t <testSentFile>    : File with the test sentences."<<endl;
  cerr << " -r <refSentFile>     : File with the reference sentences."<<endl;
  cerr << " -p                   : Treat the reference sentences as prefixes."<<endl;
  cerr << " -c                   : Verify model coverage for the reference sentence."<<endl;
#ifdef THOT_DISABLE_REC
  cerr << " -b <int>             : Obtain <int>-best translations."<<endl;
#else
  cerr << " -b <int>             : Obtain <int>-best translations (the <int>-best"<<endl;
  cerr << "                        recombined hypotheses are obtained). To obtain"<<endl;
  cerr << "                        real n-best lists generate word graphs first with the"<<endl;
  cerr << "                        -wg option."<<endl;
#endif
  cerr << " -W <float>           : Maximum number of translation options/Threshold"<<endl;
  cerr << "                        ("<<PALIG_W_DEFAULT<<" by default)."<<endl;
  cerr << " -S <int>             : S parameter ("<<PALIG_S_DEFAULT<<" by default)."<<endl;    
  cerr << " -A <int>             : A parameter ("<<PALIG_A_DEFAULT<<" by default)."<<endl;
  cerr << " -E <int>             : E parameter ("<<PALIG_E_DEFAULT<<" by default)."<<endl;
  cerr << " -U <int>             : Maximum number of jumped words (unrestricted by"<<endl;
  cerr << "                        default)."<<endl;
  cerr << " -I <int>             : Number of hypotheses expanded at each iteration"<<endl;
  cerr << "                        ("<<PALIG_I_DEFAULT<<" by default)."<<endl;
#ifdef MULTI_STACK_USE_GRAN
  cerr << " -G <int>             : Granularity parameter ("<<PALIG_G_DEFAULT<<"by default)."<<endl;
#else
  cerr << " -G <int>             : Parameter not available with the given configuration."<<endl;
#endif
  cerr << " -h <int>             : Heuristic function used: "<<NO_HEURISTIC<<"->None, "<<LOCAL_T_HEURISTIC<<"->LOCAL_T, "<<endl;
  cerr << "                        "<<LOCAL_TD_HEURISTIC<<"->LOCAL_TD ("<<PALIG_H_DEFAULT<<" by default)."<<endl;
  cerr << " -bf                  : Execute a breadth-first algorithm."<<endl;
  cerr << " -mon                 : Perform a monotone search."<<endl;
  cerr << " -we <float>...<float>: Set model weights, the number of weights and their"<<endl;
  cerr << "                        meaning depends on the model type (use --config option)."<<endl;
#ifndef THOT_DISABLE_REC
  cerr << " -wg <wordGraphFile>  : Print word graph after each translation, the prefix" <<endl;
  cerr << "                        of the files is given as parameter."<<endl;
  cerr << " -wgp <float>         : Prune word-graph using the given threshold.\n";
  cerr << "                        Threshold=0 -> no pruning is performed.\n";
  cerr << "                        Threshold=1 -> only the best arc arriving to each\n";
  cerr << "                                       state is retained.\n";
  cerr << "                        If not given, the number of arcs is not\n";
  cerr << "                        restricted.\n";
#endif
  cerr << "                        not restricted."<<endl;
  cerr << " -v|-v1|-v2           : verbose modes."<<endl;
  cerr << " --help               : Display this help and exit."<<endl;
  cerr << " --version            : Output version information and exit."<<endl;
  cerr << " --config             : Show current configuration."<<endl;
}

//--------------- version function
void version(void)
{
  cerr<<"thot_aligner is part of the thot package "<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
