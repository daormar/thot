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
/* Module: thot_scorer.cc                                           */
/*                                                                  */
/* Definitions file: thot_scorer.cc                                 */
/*                                                                  */
/* Description: Calculates the scoring function implemented by the  */
/*              module given in master.ini                          */
/*                                                                  */
/********************************************************************/

/**
 * @file thot_scorer.cc
 *
 * @brief Calculates the scoring function implemented by the module
 * given in master.ini.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseScorer.h"

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

struct thot_scorer_pars
{
  std::string fileWithReferences;
  std::string fileWithSysSents;
};

//--------------- Function Declarations ------------------------------

int handleParameters(int argc,
                     char *argv[],
                     thot_scorer_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   thot_scorer_pars& pars);
int checkParameters(thot_scorer_pars& pars);

int obtain_references(const thot_scorer_pars& pars,
                      Vector<std::string>& referenceVec);
int calc_score(const thot_scorer_pars& pars);
void printUsage(void);
void version(void);

//--------------- Global variables -----------------------------------

DynClassFactoryHandler dynClassFactoryHandler;
BaseScorer* baseScorerPtr;

//--------------- Function Definitions -------------------------------

//--------------------------------
int main(int argc,char *argv[])
{
  thot_scorer_pars pars;

  if(handleParameters(argc,argv,pars)==ERROR)
  {
    return ERROR;
  }
  else
  {
        // Initialize pointers
    int err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH,false);
    if(err==ERROR)
      return ERROR;

    baseScorerPtr=dynClassFactoryHandler.baseScorerDynClassLoader.make_obj(dynClassFactoryHandler.baseScorerInitPars);
    if(baseScorerPtr==NULL)
    {
      cerr<<"Error: BaseScorer pointer could not be instantiated"<<endl;
      return ERROR;
    }

        // Calculate score
    int retVal=calc_score(pars);

        // Release pointers
    delete baseScorerPtr;

        // Release class factories
    dynClassFactoryHandler.release_smt(false);

    return retVal;
  }
}

//--------------------------------
int handleParameters(int argc,
                     char *argv[],
                     thot_scorer_pars& pars)
{
  if(argc==1 || readOption(argc,argv,"--version")!=-1)
  {
    version();
    return ERROR;
  }
  if(readOption(argc,argv,"--help")!=-1)
  {
    printUsage();
    return ERROR;   
  }
  if(takeParameters(argc,argv,pars)==ERROR)
  {
    return ERROR;
  }
  else
  {
    if(checkParameters(pars)==OK)
    {
      return OK;
    }
    else
    {
      return ERROR;
    }
  }
}

//--------------------------------
int takeParameters(int argc,
                   char *argv[],
                   thot_scorer_pars& pars)
{
  int err=readSTLstring(argc,argv, "-r", &pars.fileWithReferences);
  if(err==ERROR)
    return ERROR;

  err=readSTLstring(argc,argv, "-t", &pars.fileWithSysSents);
  if(err==ERROR)
    return ERROR;

  return OK;
}

//--------------------------------
int checkParameters(thot_scorer_pars& pars)
{  
  if(pars.fileWithReferences.empty())
  {
    cerr<<"Error: parameter -r not given!"<<endl;
    return ERROR;   
  }

  if(pars.fileWithSysSents.empty())
  {
    cerr<<"Error: parameter -t not given!"<<endl;
    return ERROR;   
  }

  return OK;
}

//--------------------------------
int obtain_references(const thot_scorer_pars& pars,
                      Vector<std::string>& referenceVec)
{
      // Clear output variable
  referenceVec.clear();

      // Fill output variable
  awkInputStream awk;

  if(awk.open(pars.fileWithReferences.c_str())==ERROR)
  {
    cerr<<"Error while opening file "<<pars.fileWithReferences<<endl;
    return ERROR;
  }  
  
  while(awk.getln())
  {
    referenceVec.push_back(awk.dollar(0));
  }
  
  return OK;
}

//--------------------------------
int obtain_sys_sentences(const thot_scorer_pars& pars,
                         Vector<std::string>& sysSentVec)
{
      // Clear output variable
  sysSentVec.clear();

      // Fill output variable
  awkInputStream awk;

  if(awk.open(pars.fileWithSysSents.c_str())==ERROR)
  {
    cerr<<"Error while opening file "<<pars.fileWithSysSents<<endl;
    return ERROR;
  }
  
  while(awk.getln())
  {
    sysSentVec.push_back(awk.dollar(0));
  }
  
  return OK;
}

//--------------------------------
int calc_score(const thot_scorer_pars& pars)
{
  int retVal;
  Vector<std::string> referenceVec;
  Vector<std::string> sysSentVec;
  
      // Obtain references
  retVal=obtain_references(pars,referenceVec);
  if(retVal==ERROR)
    return ERROR;

      // Obtain system sentences
  retVal=obtain_sys_sentences(pars,sysSentVec);
  if(retVal==ERROR)
    return ERROR;

      // Calculate score
  double score;
  baseScorerPtr->corpusScore(sysSentVec,
                             referenceVec,
                             score);
  
      // Print result
  cout<<"Score= "<<score<<endl;
  
  return OK;
}

//--------------------------------
void printUsage(void)
{
  cerr<<"thot_scorer              -r <string> -t <string>"<<endl;
  cerr<<"                         [--help] [--version]"<<endl;
  cerr<<endl;
  cerr<<"-r <string>              File with reference sentences."<<endl;
  cerr<<"-t <string>              File with system sentences."<<endl;
  cerr<<"--help                   Display this help and exit."<<endl;
  cerr<<"--version                Output version information and exit."<<endl;
}

//--------------------------------
void version(void)
{
  cerr<<"thot_scorer is part of the thot package"<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
