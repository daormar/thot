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

//--------------- Constants ------------------------------------------

struct thot_llwu_nblist_pars
{
  std::vector<float> llWeightVec;
  std::vector<std::string> includeVarStr;
  std::vector<bool> includeVarBool;
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
                      std::vector<std::string>& referenceVec);
int obtain_nblist_and_scr_comps_for_file(const thot_llwu_nblist_pars& pars,
                                         std::string nbfile,
                                         std::vector<std::string>& nblist,
                                         std::vector<std::vector<double> >& scoreComps);
int obtain_nblists_and_scr_comps(const thot_llwu_nblist_pars& pars,
                                 std::vector<std::vector<std::string> >& nblistVec,
                                 std::vector<std::vector<std::vector<double> > >& scoreCompsVec);
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
    std::cerr<<"-w option is";
    for(unsigned int i=0;i<pars.llWeightVec.size();++i)
      std::cerr<<" "<<pars.llWeightVec[i];
    std::cerr<<std::endl;
    std::cerr<<"-nb option is "<<pars.fileWithNbestLists<<std::endl;
    std::cerr<<"-r option is "<<pars.fileWithReferences<<std::endl;
    std::cerr<<"-va option is";
    for(unsigned int i=0;i<pars.includeVarStr.size();++i)
      std::cerr<<" "<<pars.includeVarBool[i];
    std::cerr<<std::endl;

        // Initialize pointers
    int err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH,false);
    if(err==THOT_ERROR)
      return THOT_ERROR;

    baseScorerPtr=dynClassFactoryHandler.baseScorerDynClassLoader.make_obj(dynClassFactoryHandler.baseScorerInitPars);
    if(baseScorerPtr==NULL)
    {
      std::cerr<<"Error: BaseScorer pointer could not be instantiated"<<std::endl;
      return THOT_ERROR;
    }

    llWeightUpdaterPtr=dynClassFactoryHandler.baseLogLinWeightUpdaterDynClassLoader.make_obj(dynClassFactoryHandler.baseLogLinWeightUpdaterInitPars);
    if(llWeightUpdaterPtr==NULL)
    {
      std::cerr<<"Error: BaseLogLinWeightUpdater pointer could not be instantiated"<<std::endl;
      return THOT_ERROR;
    }

        // Link scorer to weight updater
    if(!llWeightUpdaterPtr->link_scorer(baseScorerPtr))
    {
      std::cerr<<"Error: Scorer class could not be linked to log-linear weight updater"<<std::endl;
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
    std::cerr<<"Error: parameter -nb not given!"<<std::endl;
    return THOT_ERROR;   
  }

  if(pars.fileWithReferences.empty())
  {
    std::cerr<<"Error: parameter -r not given!"<<std::endl;
    return THOT_ERROR;   
  }

  if(pars.includeVarStr.size()!=pars.llWeightVec.size())
  {
    std::cerr<<"Error: number of weights provided by -w and -va options are not equal!"<<std::endl;
    return THOT_ERROR;       
  }

  return THOT_OK;
}

//--------------------------------
int obtain_references(const thot_llwu_nblist_pars& pars,
                      std::vector<std::string>& referenceVec)
{
      // Clear output variable
  referenceVec.clear();

      // Fill output variable
  awkInputStream awk;

  if(awk.open(pars.fileWithReferences.c_str())==THOT_ERROR)
  {
    std::cerr<<"Error while opening file "<<pars.fileWithReferences<<std::endl;
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
                                         std::vector<std::string>& nblist,
                                         std::vector<std::vector<double> >& scoreComps)
{
      // Clear output variables
  nblist.clear();
  scoreComps.clear();

        // Fill output variables
  awkInputStream awk;

  if(awk.open(nbfile.c_str())==THOT_ERROR)
  {
    std::cerr<<"Error while opening file "<<nbfile<<std::endl;
    return THOT_ERROR;
  }

  // std::cerr<<"**** Processing file"<<nbfile<<std::endl;
  
      // Read n-best file
  while(awk.getln())
  {
    if(awk.FNR>1)
    {
          // Read n-best list entry

          // Read score components
      std::vector<double> scoreCompsForTrans;
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

      // std::cerr<<"|||||||||"<<nbest<<"||||||"<<std::endl;
      // for(unsigned int i=0;i<scoreCompsForTrans.size();++i)
      // {
      //   std::cerr<<scoreCompsForTrans[i]<<" ";
      // }
      // std::cerr<<std::endl;
    }
  }
  
  return THOT_OK;
}

//--------------------------------
int obtain_nblists_and_scr_comps(const thot_llwu_nblist_pars& pars,
                                 std::vector<std::vector<std::string> >& nblistVec,
                                 std::vector<std::vector<std::vector<double> > >& scoreCompsVec)
{
      // Clear output variables
  nblistVec.clear();
  scoreCompsVec.clear();

      // Fill output variables
  awkInputStream awk;

  if(awk.open(pars.fileWithNbestLists.c_str())==THOT_ERROR)
  {
    std::cerr<<"Error while opening file "<<pars.fileWithNbestLists<<std::endl;
    return THOT_ERROR;
  }
  
  while(awk.getln())
  {
    std::vector<std::string> nblist;
    std::vector<std::vector<double> > scoreComps;

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
  std::vector<std::string> referenceVec;
  std::vector<std::vector<std::string> > nblistVec;
  std::vector<std::vector<std::vector<double> > > scoreCompsVec;
  
      // Obtain references
  retVal=obtain_references(pars,referenceVec);
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
  
      // Obtain n-best lists with their vectors of score components
  retVal=obtain_nblists_and_scr_comps(pars,nblistVec,scoreCompsVec);
  if(retVal==THOT_ERROR)
    return THOT_ERROR;

      // Generate vector with current weights
  std::vector<double> currWeightsVec;
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
  std::vector<double> newWeightsVec;
  llWeightUpdaterPtr->updateClosedCorpus(referenceVec,
                                         nblistVec,
                                         scoreCompsVec,
                                         currWeightsVec,
                                         newWeightsVec);
      // Print result
  std::cout<<"Updated weights:";
  if(pars.includeVarBool.empty())
  {
    for(unsigned int i=0;i<newWeightsVec.size();++i)
      std::cout<<" "<<newWeightsVec[i];
    std::cout<<std::endl;
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
          std::cout<<" "<<newWeightsVec[j];
          ++j;
        }
        else
        {
          std::cerr<<"Warning: j index out of range while printing solution"<<std::endl;
        }
      }
      else
        std::cout<<" 0";
    }
    std::cout<<std::endl;
  }
  
  return THOT_OK;
}

//--------------------------------
void printUsage(void)
{
  std::cerr<<"thot_ll_weight_upd_nblist -w <float> ... <float>"<<std::endl;
  std::cerr<<"                          [-va <bool> ... <bool>]"<<std::endl;
  std::cerr<<"                          -nb <string> -r <string>"<<std::endl;
  std::cerr<<"                          [--help] [--version]"<<std::endl;
  std::cerr<<std::endl;
  std::cerr<<"-w <float>...<float>     Weights used to generate the n-best lists."<<std::endl;
  std::cerr<<"-va <bool>...<bool>      Set variable values to be excluded or included."<<std::endl;
  std::cerr<<"                         Each value equal to 0 excludes the variable and values"<<std::endl;
  std::cerr<<"                         equal to 1 include the variable."<<std::endl;
  std::cerr<<"-nb <string>             File containing the names of files with n-best lists."<<std::endl;
  std::cerr<<"-r <string>              File with reference sentences associated to each"<<std::endl;
  std::cerr<<"                         n-best list."<<std::endl;
  std::cerr<<"--help                   Display this help and exit."<<std::endl;
  std::cerr<<"--version                Output version information and exit."<<std::endl;
}

//--------------------------------
void version(void)
{
  std::cerr<<"thot_ll_weight_upd_nblist is part of the thot package"<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
