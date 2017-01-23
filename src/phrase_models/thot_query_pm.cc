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
/* Module: thot_query_pm.cc                                         */
/*                                                                  */
/* Definitions file: thot_query_pm.cc                               */
/*                                                                  */
/* Description: Executes queries against a given phrase model.      */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseModel.h"
#include "DynClassFileHandler.h"
#include "SimpleDynClassLoader.h"
#include <ErrorDefs.h>
#include <StrProcUtils.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "options.h"
#include "ctimer.h"
#include <math.h>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int TakeParameters(int argc,char *argv[]);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

char phraseModelFileName[256];
char phrasePairsFileName[256];
char phrase[256];
bool obtInvTrans;

//--------------- Function Definitions -------------------------------

int p_option(BasePhraseModel* pbModelPtr);
void print_trans_for_phrase(BasePhraseModel* pbModelPtr,
                            Vector<string> phraseVec);
int f_option(BasePhraseModel* pbModelPtr);
void printUsage(void);
int TakeParameters(int argc,char *argv[]);

//---------------
int main(int argc,char *argv[])
{     
  if(TakeParameters(argc,argv)==0)
  {
        // Initialize dynamic class file handler
    DynClassFileHandler dynClassFileHandler;
    if(dynClassFileHandler.load(THOT_MASTER_INI_PATH)==ERROR)
    {
      cerr<<"Error while loading ini file"<<endl;
      return ERROR;
    }
        // Define variables to obtain base class infomation
    std::string baseClassName;
    std::string soFileName;
    std::string initPars;

        // Obtain info for BasePhraseModel class
    baseClassName="BasePhraseModel";
    if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==ERROR)
    {
      cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<endl;
      cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<endl;
      return ERROR;
    }
   
        // Load class derived from BasePhraseModel dynamically
    SimpleDynClassLoader<BasePhraseModel> basePhraseModelDynClassLoader;
    if(!basePhraseModelDynClassLoader.open_module(soFileName))
    {
      cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<endl;
      return ERROR;
    }     
   
    BasePhraseModel* pbModelPtr=basePhraseModelDynClassLoader.make_obj(initPars);
    if(pbModelPtr==NULL)
    {
      cerr<<"Error: BasePhraseModel pointer could not be instantiated"<<endl;
      
      basePhraseModelDynClassLoader.close_module();
      return ERROR;
    }

    int ret;
    if(phrase[0]!=0)
    {
      ret=p_option(pbModelPtr);
    }
    else
    {
      ret=f_option(pbModelPtr);
    }

        // Release objects and close modules
    delete pbModelPtr;
    basePhraseModelDynClassLoader.close_module();
     
    return ret;
  }
  else
  {
    return ERROR;
  }
}

//---------------
int p_option(BasePhraseModel* pbModelPtr)
{
  if(pbModelPtr->load(phraseModelFileName)==0)
  {
    Vector<string> phraseVec;
    double total_time=0,elapsed_ant,elapsed,ucpu,scpu;
    ctimer(&elapsed_ant,&ucpu,&scpu);
    
        // Print parameters
    cerr<<"Phrase: "<<phrase<<endl;
    cerr<<"Inverse-translation flag: "<<obtInvTrans<<endl;

    phraseVec=StrProcUtils::stringToStringVector(phrase);

        // Generate all subphrases and obtain translation options for
        // each of them
    for(unsigned int i=0;i<phraseVec.size();++i)
    {
      for(unsigned int j=i;j<phraseVec.size();++j)
      {
        Vector<string> subPhraseVec;
        cout<<"* Translations for: \"";
        for(unsigned int k=i;k<=j;++k)
        {
          subPhraseVec.push_back(phraseVec[k]);
          if(k!=i) cout<<" ";
          cout<<phraseVec[k];
        }
        cout<<"\""<<endl;
    
        print_trans_for_phrase(pbModelPtr,subPhraseVec);
      }
    }

        // Obtain total time spent
    ctimer(&elapsed,&ucpu,&scpu);  
    total_time+=elapsed-elapsed_ant;

    cerr<<"Total retrieving time in secs: "<<total_time<<endl;
    
    return OK;
  }
  else
  {
    return ERROR;
  }
}

//---------------
void print_trans_for_phrase(BasePhraseModel* pbModelPtr,
                            Vector<string> phraseVec)
{
  NbestTableNode<PhraseTransTableNodeData> ttableNode;
  NbestTableNode<PhraseTransTableNodeData>::iterator ttableNodeIter;
  Vector<WordIndex>::iterator vecWordIndexIter;

  if(obtInvTrans)
  {
        // Retrieve translations
    pbModelPtr->strGetNbestTransFor_t_(phraseVec,ttableNode);
        // Print translations
    for(ttableNodeIter=ttableNode.begin();ttableNodeIter!=ttableNode.end();++ttableNodeIter)	 
    {
      for(vecWordIndexIter=ttableNodeIter->second.begin();vecWordIndexIter!=ttableNodeIter->second.end();++vecWordIndexIter)
        cout<<pbModelPtr->wordIndexToSrcString(*vecWordIndexIter)<<" ";  	 
      cout<<"||| "<< (float) ttableNodeIter->first<<endl;
    }
  }
  else
  {
        // Retrieve translations
    pbModelPtr->strGetNbestTransFor_s_(phraseVec,ttableNode);
        // Print translations
    for(ttableNodeIter=ttableNode.begin();ttableNodeIter!=ttableNode.end();++ttableNodeIter)	 
    {
      for(vecWordIndexIter=ttableNodeIter->second.begin();vecWordIndexIter!=ttableNodeIter->second.end();++vecWordIndexIter)
        cout<<pbModelPtr->wordIndexToTrgString(*vecWordIndexIter)<<" ";  	 
      cout<<"||| "<< (float) ttableNodeIter->first<<endl;
    }
  }
}

//---------------
int f_option(BasePhraseModel* pbModelPtr)
{
  awkInputStream awk;

      // Open input file
  if(awk.open(phrasePairsFileName)==ERROR)
  {
    cerr<<"Error in file with phrase pairs, file "<<phrasePairsFileName<<" does not exist.\n";
    return ERROR;
  }

      // Load model
  if(pbModelPtr->load(phraseModelFileName)==0)
  {
    Vector<std::string> fileSrcSentVec;
    Vector<std::string> fileTrgSentVec;
    LgProb lp;

        // Read input
    while(awk.getln())
    {
      if(awk.NF>=3)
      {
        unsigned int i=1; 
        fileSrcSentVec.clear();
        while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
        {
          fileSrcSentVec.push_back(awk.dollar(i)); 
          ++i;
        }
        ++i;
        fileTrgSentVec.clear();
        while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
        {
          fileTrgSentVec.push_back(awk.dollar(i));			   
          ++i; 
        }
            // Process sentence pair
        if(obtInvTrans)
        {
          lp=pbModelPtr->strLogps_t_(fileSrcSentVec,fileTrgSentVec);
        }
        else
        {
          lp=pbModelPtr->strLogpt_s_(fileSrcSentVec,fileTrgSentVec);     
        }
        cout<<awk.dollar(0)<<" ||| "<<lp<<endl;
        cerr<<awk.dollar(0)<<" ||| "<<lp.get_p()<<endl;
      }
    } 
    return OK;
  }
  else
  {
    return ERROR;
  }
}

//---------------
int TakeParameters(int argc,char *argv[])
{
 int err;
	
 if(argc<2)
 {
   printUsage();
   return 1;
 }
 
 /* Takes the model file name */
 err=readString(argc,argv, "-l", phraseModelFileName);
 if(err==-1)
 {
   printUsage();
   return 1;
 }
   
 /* Takes -p or -f option */
 phrase[0]=0;
 phrasePairsFileName[0]=0;
 err=readString(argc,argv, "-p", phrase);
 if(err==-1)
 {
   err=readString(argc,argv, "-f", phrasePairsFileName);
   if(err==-1)
   {
     printUsage();
     return 1;
   }
 }
 
 /* Verify inverse translation option */
 err=readOption(argc,argv, "-i");
 obtInvTrans=1;
 if(err==-1)
 {
   obtInvTrans=0;
 }
 
 return 0;  
}
 
//--------------------------------
void printUsage(void)
{
 cerr<<"Usage: thot_query_pm    -l <string> {-p <string> |\n";
 cerr<<"                        -f <string>} [-i]\n\n";
 cerr<<"-l <string>             Phrase model file name for load.\n";
 cerr<<"-p <string>             Obtain translations stored in the model for\n";
 cerr<<"                        \"string\" and all its sub-phrases.\n";
 cerr<<"-f <string>             Return log-prob for each phrase pair given in\n";
 cerr<<"                        the file \"string\".\n";
 cerr<<"                        File format: <src_phrase> ||| <trg_phrase>\n";
 cerr<<"-i                      Obtain inverse translations.\n\n";
}
