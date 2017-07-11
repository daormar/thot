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
/* Module: thot_ll_weight_upd_nblist.cc                             */
/*                                                                  */
/* Definitions file: thot_ll_weight_upd_nblist.cc                   */
/*                                                                  */
/* Description: Implements a log-linear weight updater given a set  */
/*              of n-best lists.                                    */
/*                                                                  */
/********************************************************************/

/**
 * @file thot_ll_weight_upd_nblist.cc
 *
 * @brief Implements a log-linear weight updater given a set of n-best
 * lists.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseScorer.h"
#include "BaseLogLinWeightUpdater.h"

#include "DynClassFactoryHandler.h"
#include "awkInputStream.h"
#include "ErrorDefs.h"
#include "options.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//--------------- Constants ------------------------------------------

struct thot_llwu_nblist_pars
{
  Vector<float> llWeightVec;
  Vector<string> includeVarStr;
  Vector<bool> includeVarBool;
  std::string fileWithNbestLists;
  std::string fileWithReferences;
};

//--------------- Function Declarations ------------------------------

int handleParameters(int argc,
                     char *argv[],
                     thot_llwu_nblist_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   thot_llwu_nblist_pars& pars);
int checkParameters(thot_llwu_nblist_pars& pars);

int obtain_references(const thot_llwu_nblist_pars& pars,
                      Vector<std::string>& referenceVec);
int obtain_nblist_and_scr_comps_for_file(const thot_llwu_nblist_pars& pars,
                                         std::string nbfile,
                                         Vector<std::string>& nblist,
                                         Vector<Vector<double> >& scoreComps);
int obtain_nblists_and_scr_comps(const thot_llwu_nblist_pars& pars,
                                 Vector<Vector<string> >& nblistVec,
                                 Vector<Vector<Vector<double> > >& scoreCompsVec);
int update_ll_weights(const thot_llwu_nblist_pars& pars);
void printUsage(void);
void version(void);

//--------------- Global variables -----------------------------------

DynClassFactoryHandler dynClassFactoryHandler;
BaseScorer* baseScorerPtr;
BaseLogLinWeightUpdater* llWeightUpdaterPtr;

//--------------- Function Definitions -------------------------------

//--------------------------------
int main(int argc,char *argv[])
{
  thot_llwu_nblist_pars pars;

  if(handleParameters(argc,argv,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
        // Print parameters
    cerr<<"-w option is";
    for(unsigned int i=0;i<pars.llWeightVec.size();++i)
      cerr<<" "<<pars.llWeightVec[i];
    cerr<<endl;
    cerr<<"-nb option is "<<pars.fileWithNbestLists<<endl;
    cerr<<"-r option is "<<pars.fileWithReferences<<endl;
    cerr<<"-va option is";
    for(unsigned int i=0;i<pars.includeVarStr.size();++i)
      cerr<<" "<<pars.includeVarBool[i];
    cerr<<endl;

        // Initialize pointers
    int err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH,false);
    if(err==THOT_ERROR)
      return THOT_ERROR;

    baseScorerPtr=dynClassFactoryHandler.baseScorerDynClassLoader.make_obj(dynClassFactoryHandler.baseScorerInitPars);
    if(baseScorerPtr==NULL)
    {
      cerr<<"Error: BaseScorer pointer could not be instantiated"<<endl;
      return THOT_ERROR;
    }

    llWeightUpdaterPtr=dynClassFactoryHandler.baseLogLinWeightUpdaterDynClassLoader.make_obj(dynClassFactoryHandler.baseLogLinWeightUpdaterInitPars);
    if(llWeightUpdaterPtr==NULL)
    {
      cerr<<"Error: BaseLogLinWeightUpdater pointer could not be instantiated"<<endl;
      return THOT_ERROR;
    }

        // Link scorer to weight updater
    if(!llWeightUpdaterPtr->link_scorer(baseScorerPtr))
    {
      cerr<<"Error: Scorer class could not be linked to log-linear weight updater"<<endl;
      return THOT_ERROR;
    }
    
        // Update log-linear weights
    int retVal=update_ll_weights(pars);

        // Release pointers
    delete baseScorerPtr;
    delete llWeightUpdaterPtr;

        // Release class factories
    dynClassFactoryHandler.release_smt(false);

    return retVal;
  }
}

//--------------------------------
int handleParameters(int argc,
                     char *argv[],
                     thot_llwu_nblist_pars& pars)
{
  if(argc==1 || readOption(argc,argv,"--version")!=-1)
  {
    version();
    return THOT_ERROR;
  }
  if(readOption(argc,argv,"--help")!=-1)
  {
    printUsage();
    return THOT_ERROR;   
  }
  if(takeParameters(argc,argv,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    if(checkParameters(pars)==THOT_OK)
    {
      return THOT_OK;
    }
    else
    {
      return THOT_ERROR;
    }
  }
}

//--------------------------------
int takeParameters(int argc,
                   char *argv[],
                   thot_llwu_nblist_pars& pars)
{
      // Take -nb parameter
  int err=readSTLstring(argc,argv, "-nb", &pars.fileWithNbestLists);
  if(err==THOT_ERROR)
    return THOT_ERROR;
  
      // Take -r parameter
  err=readSTLstring(argc,argv, "-r", &pars.fileWithReferences);
  if(err==THOT_ERROR)
    return THOT_ERROR;

      // Obtain included variables
  err=readStringSeq(argc,argv, "-va", pars.includeVarStr);
  if(err==THOT_ERROR)
    return THOT_ERROR;
  else
  {
    for(unsigned int i=0;i<pars.includeVarStr.size();++i)
    {
      pars.includeVarBool.push_back(atoi(pars.includeVarStr[i].c_str()));
    }
  }

      // Obtain weight vector used to generate the n-best lists
  err=readFloatSeq(argc,argv, "-w", pars.llWeightVec);
  if(err==THOT_ERROR)
    return THOT_ERROR;

  return THOT_OK;
}

//--------------------------------
int checkParameters(thot_llwu_nblist_pars& pars)
{  
  if(pars.fileWithNbestLists.empty())
  {
    cerr<<"Error: parameter -nb not given!"<<endl;
    return THOT_ERROR;   
  }

  if(pars.fileWithReferences.empty())
  {
    cerr<<"Error: parameter -r not given!"<<endl;
    return THOT_ERROR;   
  }

  if(pars.includeVarStr.size()!=pars.llWeightVec.size())
  {
    cerr<<"Error: number of weights provided by -w and -va options are not equal!"<<endl;
    return THOT_ERROR;       
  }

  return THOT_OK;
}

//--------------------------------
int obtain_references(const thot_llwu_nblist_pars& pars,
                      Vector<std::string>& referenceVec)
{
      // Clear output variable
  referenceVec.clear();

      // Fill output variable
  awkInputStream awk;

  if(awk.open(pars.fileWithReferences.c_str())==THOT_ERROR)
  {
    cerr<<"Error while opening file "<<pars.fileWithReferences<<endl;
    return THOT_ERROR;
  }  
  
  while(awk.getln())
  {
    referenceVec.push_back(awk.dollar(0));
  }
  
  return THOT_OK;
}

//--------------------------------
int obtain_nblist_and_scr_comps_for_file(const thot_llwu_nblist_pars& pars,
                                         std::string nbfile,
                                         Vector<std::string>& nblist,
                                         Vector<Vector<double> >& scoreComps)
{
      // Clear output variables
  nblist.clear();
  scoreComps.clear();

        // Fill output variables
  awkInputStream awk;

  if(awk.open(nbfile.c_str())==THOT_ERROR)
  {
    cerr<<"Error while opening file "<<nbfile<<endl;
    return THOT_ERROR;
  }

  // cerr<<"**** Processing file"<<nbfile<<endl;
  
      // Read n-best file
  while(awk.getln())
  {
    if(awk.FNR>1)
    {
          // Read n-best list entry

          // Read score components
      Vector<double> scoreCompsForTrans;
      unsigned int i;
      for(i=3;i<=awk.NF;++i)
      {
        if(awk.dollar(i)=="|||")
        {
          ++i;
          break;
        }
        else
        {
          if(pars.includeVarStr.empty())
            scoreCompsForTrans.push_back(atof(awk.dollar(i).c_str()));
          else
            if(i-3<pars.includeVarStr.size() && pars.includeVarBool[i-3])
              scoreCompsForTrans.push_back(atof(awk.dollar(i).c_str()));
        }
      }

          // Read n-best translation
      std::string nbest;
      for(;i<=awk.NF;++i)
      {
        nbest+=awk.dollar(i);
        if(i<awk.NF)
          nbest+=" ";
      }

          // Extend output variables
      scoreComps.push_back(scoreCompsForTrans);
      nblist.push_back(nbest);

      // cerr<<"|||||||||"<<nbest<<"||||||"<<endl;
      // for(unsigned int i=0;i<scoreCompsForTrans.size();++i)
      // {
      //   cerr<<scoreCompsForTrans[i]<<" ";
      // }
      // cerr<<endl;
    }
  }
  
  return THOT_OK;
}

//--------------------------------
int obtain_nblists_and_scr_comps(const thot_llwu_nblist_pars& pars,
                                 Vector<Vector<string> >& nblistVec,
                                 Vector<Vector<Vector<double> > >& scoreCompsVec)
{
      // Clear output variables
  nblistVec.clear();
  scoreCompsVec.clear();

      // Fill output variables
  awkInputStream awk;

  if(awk.open(pars.fileWithNbestLists.c_str())==THOT_ERROR)
  {
    cerr<<"Error while opening file "<<pars.fileWithNbestLists<<endl;
    return THOT_ERROR;
  }
  
  while(awk.getln())
  {
    Vector<std::string> nblist;
    Vector<Vector<double> > scoreComps;

        // Obtain n-best list file name
    std::string nbfile=awk.dollar(0);

        // Process n-best list file
    int ret=obtain_nblist_and_scr_comps_for_file(pars,
                                                 nbfile,
                                                 nblist,
                                                 scoreComps);
    if(ret==THOT_ERROR)
    {
      return THOT_ERROR;
    }
    
        // Add n-best list and score components to output variables
    nblistVec.push_back(nblist);
    scoreCompsVec.push_back(scoreComps);
  }
  
  return THOT_OK;
}

//--------------------------------
int update_ll_weights(const thot_llwu_nblist_pars& pars)
{
  int retVal;
  Vector<std::string> referenceVec;
  Vector<Vector<string> > nblistVec;
  Vector<Vector<Vector<double> > > scoreCompsVec;
  
      // Obtain references
  retVal=obtain_references(pars,referenceVec);
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
  
      // Obtain n-best lists with their vectors of score components
  retVal=obtain_nblists_and_scr_comps(pars,nblistVec,scoreCompsVec);
  if(retVal==THOT_ERROR)
    return THOT_ERROR;

      // Generate vector with current weights
  Vector<double> currWeightsVec;
  for(unsigned int i=0;i<pars.llWeightVec.size();++i)
  {
    if(pars.includeVarBool.empty())
      currWeightsVec.push_back(pars.llWeightVec[i]);
    else
    {
      if(pars.includeVarBool[i])
        currWeightsVec.push_back(pars.llWeightVec[i]);
    }
  }

      // Update log-linear weights
  Vector<double> newWeightsVec;
  llWeightUpdaterPtr->updateClosedCorpus(referenceVec,
                                         nblistVec,
                                         scoreCompsVec,
                                         currWeightsVec,
                                         newWeightsVec);
      // Print result
  cout<<"Updated weights:";
  if(pars.includeVarBool.empty())
  {
    for(unsigned int i=0;i<newWeightsVec.size();++i)
      cout<<" "<<newWeightsVec[i];
    cout<<endl;
  }
  else
  {
    unsigned int j=0;
    for(unsigned int i=0;i<pars.includeVarBool.size();++i)
    {
      if(pars.includeVarBool[i])
      {
        if(j<newWeightsVec.size())
        {
          cout<<" "<<newWeightsVec[j];
          ++j;
        }
        else
        {
          cerr<<"Warning: j index out of range while printing solution"<<endl;
        }
      }
      else
        cout<<" 0";
    }
    cout<<endl;
  }
  
  return THOT_OK;
}

//--------------------------------
void printUsage(void)
{
  cerr<<"thot_ll_weight_upd_nblist -w <float> ... <float>"<<endl;
  cerr<<"                          [-va <bool> ... <bool>]"<<endl;
  cerr<<"                          -nb <string> -r <string>"<<endl;
  cerr<<"                          [--help] [--version]"<<endl;
  cerr<<endl;
  cerr<<"-w <float>...<float>     Weights used to generate the n-best lists."<<endl;
  cerr<<"-va <bool>...<bool>      Set variable values to be excluded or included."<<endl;
  cerr<<"                         Each value equal to 0 excludes the variable and values"<<endl;
  cerr<<"                         equal to 1 include the variable."<<endl;
  cerr<<"-nb <string>             File containing the names of files with n-best lists."<<endl;
  cerr<<"-r <string>              File with reference sentences associated to each"<<endl;
  cerr<<"                         n-best list."<<endl;
  cerr<<"--help                   Display this help and exit."<<endl;
  cerr<<"--version                Output version information and exit."<<endl;
}

//--------------------------------
void version(void)
{
  cerr<<"thot_ll_weight_upd_nblist is part of the thot package"<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
