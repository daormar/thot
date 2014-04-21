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
/* Module: thot_decoder                                             */
/*                                                                  */
/* Definitions file: thot_decoder.cc                                */
/*                                                                  */
/* Description: Implements a translation system which translates a  */
/*              test corpus using a multiple-stack decoder          */
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

#define PMSTACK_W_DEFAULT 10

#ifdef MULTI_STACK_USE_GRAN
 #define PMSTACK_S_DEFAULT 128
#else
 #define PMSTACK_S_DEFAULT 10
#endif

#define PMSTACK_A_DEFAULT 10
#define PMSTACK_I_DEFAULT 1
#define PMSTACK_G_DEFAULT 0
#define PMSTACK_H_DEFAULT LOCAL_TD_HEURISTIC
#define PMSTACK_NOMON_DEFAULT 0

//--------------- Function Declarations ------------------------------

int init_translator(void);
void release_translator(void);
int translate_corpus(void);
Vector<string> stringToStringVector(string s);
void version(void);
int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printConfig(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

CURR_MODEL_TYPE *pbtModelPtr;
CURR_MSTACK_TYPE<CURR_MODEL_TYPE>* translatorPtr;
bool be;
float W;
int A,nomon,S,I,G,heuristic,verbosity;
double timeLimit;
std::string sourceSentencesFile;
std::string languageModelFileName;
std::string transModelPref;
std::string wordGraphFileName;
std::string outFile;
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

      ret=translate_corpus();
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

      // Set model parameters
  pbtModelPtr->set_W_par(W);
  pbtModelPtr->set_A_par(A);
      // Set non-monotonicity level
  if(nomon==0)
  {
    pbtModelPtr->setMonotoneSearch();
  }
  else
  {
    pbtModelPtr->resetMonotoneSearch();
    pbtModelPtr->set_U_par(nomon);
  }
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
  if(wgPruningThreshold==DISABLE_WORDGRAPH)
    translatorPtr->useBestScorePruning(true);

      // Set breadthFirst flag
  translatorPtr->set_breadthFirst(!be);

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

int translate_corpus(void)
{
  CURR_MODEL_TYPE::Hypothesis result;     // Results of the translation
  CURR_MODEL_TYPE::Hypothesis anotherTrans;     // Another results of the translation
  int sentNo=0;    
  double elapsed_ant,elapsed,ucpu,scpu,total_time=0;
      
  ifstream testCorpusFile;                // Test corpus file stream
  string srcSentenceString,s;
  
    
      // Open test corpus file
  testCorpusFile.open(sourceSentencesFile.c_str());    
  testCorpusFile.seekg(0, ios::beg);

  cerr<<"\n- Translating test corpus sentences...\n\n";

  if(!testCorpusFile)
  {
    cerr<<"Test corpus error!"<<endl;
    return ERROR;
  }
  else
  {
        // Open output file if required
    ofstream outS;
    if(!outFile.empty())
    {
      outS.open(outFile.c_str(),ios::out);
      if(!outS) cerr<<"Error while opening output file."<<endl;
    }
    
        // Translate corpus sentences
    while(!testCorpusFile.eof())
    {
      getline(testCorpusFile,srcSentenceString); 
      if(srcSentenceString!="")
      {
        ++sentNo;
        
        if(verbosity)
        {
          cerr<<sentNo<<endl<<srcSentenceString<<endl;
          ctimer(&elapsed_ant,&ucpu,&scpu);
        }
       
            //------- Translate sentence
        result=translatorPtr->translate(srcSentenceString,timeLimit);

            //--------------------------
        if(verbosity) ctimer(&elapsed,&ucpu,&scpu);

        if(outFile.empty())
          cout<<pbtModelPtr->getTransInPlainText(result)<<endl;
        else
          outS<<pbtModelPtr->getTransInPlainText(result)<<endl;
          
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
        ofstream graphOutS;
        sprintf(printGraphFileName,"sent%d.graph_file",sentNo);
        graphOutS.open(printGraphFileName,ios::out);
        if(!graphOutS) cerr<<"Error while printing search graph to file."<<endl;
        else
        {
          translatorPtr->printSearchGraphStream(graphOutS);
          graphOutS<<"Stack ID. Out\n";
          translatorPtr->printGraphForHyp(result,graphOutS);
          graphOutS.close();        
        }
#endif        
      }    
    }
        // Close output file
    if(!outFile.empty())
    {
      outS.close();
    }

        // Close test corpus file
    testCorpusFile.close();
  }

  if(verbosity)
  {
    cerr<<"- Time per sentence: "<<total_time/sentNo<<endl;
  }

  return OK;
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

     // Takes W 
 err=readFloat(argc,argv, "-W", &W);
 if(err==-1)
 {
   W=PMSTACK_W_DEFAULT;
 }

     // Takes S parameter 
 err=readInt(argc,argv, "-S", &(S));
 if(err==-1)
 {
   S=PMSTACK_S_DEFAULT;
 }

     // Takes A parameter 
 err=readInt(argc,argv, "-A", &A);
 if(err==-1)
 {
   A=PMSTACK_A_DEFAULT;
 }

     // Takes U parameter 
 err=readInt(argc,argv, "-nomon", &nomon);
 if(err==-1)
 {
   nomon=PMSTACK_NOMON_DEFAULT;
 }

     // Takes I parameter 
 err=readInt(argc,argv, "-I", &I);
 if(err==-1)
 {
   I=PMSTACK_I_DEFAULT;
 }

     // Takes I parameter 
 err=readInt(argc,argv, "-G", &G);
 if(err==-1)
 {
   G=PMSTACK_G_DEFAULT;
 }

     // Takes h parameter 
 err=readInt(argc,argv, "-h", &heuristic);
 if(err==-1)
 {
   heuristic=PMSTACK_H_DEFAULT;
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
 
     // Take file name with the sentences to be translated 
 err=readString(argc,argv, "-t",s);
 if(err==-1)
 {
   cerr<<"Error: parameter -t not given!"<<endl;
   printUsage();
   return ERROR;   
 }
 else sourceSentencesFile=s;

      // Take output file name
 err=readString(argc,argv, "-o",s);
 if(err!=-1)
 {
   outFile=s;
 }
 
       // read -be option
 err=readOption(argc,argv,"-be");
 if(err==-1)
 {
   be=0;
 }      
 else
 {
   be=1;
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

     // read maxtime if given
 err=readDouble(argc,argv, "-maxtime", &timeLimit);
 if(err==-1)
 {
   timeLimit=0;
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

 cerr<<"W: "<<W<<endl;   
 cerr<<"S: "<<S<<endl;   
 cerr<<"A: "<<A<<endl;
 cerr<<"I: "<<I<<endl;
#ifdef MULTI_STACK_USE_GRAN
 cerr<<"G: "<<G<<endl;
#endif
 cerr<<"h: "<<heuristic<<endl;
 cerr<<"be: "<<be<<endl;
 cerr<<"nomon: "<<nomon<<endl;
 cerr<<"weight vector:";
 for(unsigned int i=0;i<weightVec.size();++i)
   cerr<<" "<<weightVec[i];
 cerr<<endl;
 cerr<<"time limit: "<<timeLimit<<endl;   
 cerr<<"lmfile: "<<languageModelFileName<<endl;   
 cerr<<"tm files prefix: "<<transModelPref<<endl;
 cerr<<"test file: "<<sourceSentencesFile<<endl;
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

  cerr <<"* Translator configuration:"<<endl;
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
  cerr << "thot_decoder        -tm <string> -lm <string> -t <string> [-o <string>]"<<endl;
  cerr << "                    [-W <float>] [-S <int>] [-A <int>]"<<endl;
  cerr << "                    [-I <int>] [-G <int>] [-h <int>]"<<endl;
  cerr << "                    [-be] [ -nomon <int>] [-we <float> ... <float>]"<<endl;
#ifndef THOT_DISABLE_REC
  cerr << "                    [-wg <string> [-wgp <float>] ]"<<endl;
#endif  
  cerr << "                    [-maxtime <float>] [-v|-v1|-v2]"<<endl;
  cerr << "                    [--help] [--version] [--config]"<<endl<<endl;
  cerr << " -tm <string>         : Prefix of the translation model files."<<endl;
  cerr << " -lm <string>         : Language model file name."<<endl;
  cerr << " -t <string>          : File with the test sentences."<<endl;
  cerr << " -o <string>          : File to store translations (if not given, they are"<<endl;
  cerr << "                        printed to the standard output)."<<endl;
  cerr << " -W <float>           : Maximum number of translation options/Threshold"<<endl;
  cerr << "                        ("<<PMSTACK_W_DEFAULT<<" by default)."<<endl;
  cerr << " -S <int>             : S parameter ("<<PMSTACK_S_DEFAULT<<" by default)."<<endl;    
  cerr << " -A <int>             : A parameter ("<<PMSTACK_A_DEFAULT<<" by default)."<<endl;
  cerr << " -I <int>             : Number of hypotheses expanded at each iteration"<<endl;
  cerr << "                        ("<<PMSTACK_I_DEFAULT<<" by default)."<<endl;
#ifdef MULTI_STACK_USE_GRAN
  cerr << " -G <int>             : Granularity parameter ("<<PMSTACK_G_DEFAULT<<"by default)."<<endl;
#else
  cerr << " -G <int>             : Parameter not available with the given configuration."<<endl;
#endif
  cerr << " -h <int>             : Heuristic function used: "<<NO_HEURISTIC<<"->None, "<<LOCAL_T_HEURISTIC<<"->LOCAL_T, "<<endl;
  cerr << "                        "<<LOCAL_TD_HEURISTIC<<"->LOCAL_TD ("<<PMSTACK_H_DEFAULT<<" by default)."<<endl;
  cerr << " -be                  : Execute a best-first algorithm (breadth-first search is"<<endl;
  cerr << "                        is executed by default)."<<endl;
  cerr << " -nomon <int>         : Perform a non-monotonic search, allowing the decoder to"<<endl;
  cerr << "                        skip up to <int> words from the last aligned source"<<endl;
  cerr << "                        words. If <int> is equal to zero, then a monotonic"<<endl;
  cerr << "                        search is performed ("<<PMSTACK_NOMON_DEFAULT<<" is the default value)."<<endl;
  cerr << " -we <float>...<float>: Set model weights, the number of weights and their"<<endl;
  cerr << "                        meaning depends on the model type (use --config option)."<<endl;
#ifndef THOT_DISABLE_REC
  cerr << " -wg <string>         : Print word graph after each translation, the prefix" <<endl;
  cerr << "                        of the files is given as parameter."<<endl;
  cerr << " -wgp <float>         : Prune word-graph using the given threshold.\n";
  cerr << "                        Threshold=0 -> no pruning is performed.\n";
  cerr << "                        Threshold=1 -> only the best arc arriving to each\n";
  cerr << "                                       state is retained.\n";
  cerr << "                        If not given, the number of arcs is not\n";
  cerr << "                        restricted.\n";
#endif
  cerr << " -maxtime <float>     : Maximum translation time in seconds."<<endl;
  cerr << " -v|-v1|-v2           : verbose modes."<<endl;
  cerr << " --help               : Display this help and exit."<<endl;
  cerr << " --version            : Output version information and exit."<<endl;
  cerr << " --config             : Show current configuration."<<endl;
}

//--------------- version function
void version(void)
{
  cerr<<"thot_decoder is part of the thot package "<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
