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
 
/*********************************************************************/
/*                                                                   */
/* Module: thot_gen_sw_model.cc                                      */
/*                                                                   */
/* Definitions file: thot_gen_sw_model.cc                            */
/*                                                                   */
/* Description: Generates a single-word model from bilingual         */
/*              corpora.                                             */
/*                                                                   */   
/*********************************************************************/


//--------------- Include files ---------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#ifdef THOT_HAVE_LEVELDB_LIB 
#include "IncrLevelDbHmmP0AligModel.h"
#endif
#include "IncrHmmP0AligModel.h"
#include "_incrSwAligModel.h"
#include "BaseStepwiseAligModel.h"
#include "BaseSwAligModel.h"
#include "thot_gen_sw_model_pars.h"
#include "DynClassFileHandler.h"
#include "SimpleDynClassLoader.h"
#include "options.h"

//--------------- Constants -------------------------------------------

#define LR_1          1
#define LR_2          2
#define LR_3          3
#define FIXED_LR      0.5

//--------------- Function Declarations -------------------------------

int init_swm(int verbosity);
void release_swm(int verbosity);
int processParameters(thot_gen_sw_model_pars pars);
void emIters(thot_gen_sw_model_pars& pars,
             BaseSwAligModel<std::vector<Prob> >* swAligModelPtr,
             pair<unsigned int,unsigned int> wholeTrainRange,
             int verbosity);
float obtainLr(const thot_gen_sw_model_pars& pars,
               unsigned int stepNum);
int handleParameters(int argc,
                     char *argv[],
                     thot_gen_sw_model_pars& pars);
int takeParameters(int argc,
                   const std::vector<std::string>& argv_stl,
                   thot_gen_sw_model_pars& pars);
int checkParameters(thot_gen_sw_model_pars& pars);
void printParameters(thot_gen_sw_model_pars pars);
void printUsage(void);
void version(void);

//--------------- Type definitions ------------------------------------

SimpleDynClassLoader<BaseSwAligModel<std::vector<Prob> > > baseSwAligModelDynClassLoader;
BaseSwAligModel<std::vector<Prob> >* swAligModelPtr;

//--------------- Global variables ------------------------------------


//--------------- Function Definitions --------------------------------


//--------------- main function
int main(int argc,char *argv[])
{
  thot_gen_sw_model_pars pars;
    
  if(handleParameters(argc,argv,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    if(pars.v_given || pars.v1_given)
    {
      printParameters(pars);
    }
    return processParameters(pars);
  }
}

//---------------
int init_swm(int verbosity)
{
      // Initialize dynamic class file handler
  DynClassFileHandler dynClassFileHandler;
  if(dynClassFileHandler.load(THOT_MASTER_INI_PATH,verbosity)==THOT_ERROR)
  {
    std::cerr<<"Error while loading ini file"<<std::endl;
    return THOT_ERROR;
  }
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string soFileName;
  std::string initPars;

      ////////// Obtain info for BaseSwAligModel class
  baseClassName="BaseSwAligModel";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }
   
      // Load class derived from BaseSwAligModel dynamically
  if(!baseSwAligModelDynClassLoader.open_module(soFileName,verbosity))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }

  swAligModelPtr=baseSwAligModelDynClassLoader.make_obj(initPars);
  if(swAligModelPtr==NULL)
  {
    std::cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<std::endl;
    baseSwAligModelDynClassLoader.close_module();
    
    return THOT_ERROR;
  }

  return THOT_OK;
}

//---------------
void release_swm(int verbosity)
{
  delete swAligModelPtr;
  baseSwAligModelDynClassLoader.close_module(verbosity);
}

//--------------- processParameters function
int processParameters(thot_gen_sw_model_pars pars)
{
  int verbosity=0;

  if(init_swm(true)==THOT_ERROR)
    return THOT_ERROR;
  
      // Load model if -l option was given
  if(pars.l_given)
  {
        // Load model
    int ret=swAligModelPtr->load(pars.l_str.c_str());
    if(ret==THOT_ERROR)
    {
      release_swm(true);
      return THOT_ERROR;
    }
  }

      // Set maximum size in the dimension of n of the matrix of
      // expected values for incremental sw models
  _incrSwAligModel<std::vector<Prob> >* _incrSwAligModelPtr=dynamic_cast<_incrSwAligModel<std::vector<Prob> >*>(swAligModelPtr);
  if(_incrSwAligModelPtr)
  {
    if(pars.r_given)
    {
      _incrSwAligModelPtr->set_expval_maxnsize(pars.r);
    }
  }

      // Set p0 value if given and supported by the current alignment
      // model
  if(pars.np_given)
  {
    IncrHmmP0AligModel* incrHmmP0AligModelPtr=dynamic_cast<IncrHmmP0AligModel*>(swAligModelPtr);
    if(incrHmmP0AligModelPtr)
    {
      incrHmmP0AligModelPtr->set_hmm_p0(pars.np_val);
      std::cerr<<"NULL word alignment probability has been set to "<<pars.np_val<<std::endl;
    }
    else
    {
      std::cerr<<"Warning: -np option cannot be combined with the current alignment model"<<std::endl;
    }
  }

      // Set lexical smoothing interpolation factor if given and
      // supported by the current alignment model
  if(pars.lf_given)
  {
    _incrHmmAligModel* incrHmmAligModelPtr=dynamic_cast<_incrHmmAligModel*>(swAligModelPtr);
    if(incrHmmAligModelPtr)
    {
      incrHmmAligModelPtr->setLexSmIntFactor(pars.lf_val);
    }
    else
    {
      std::cerr<<"Warning: -lf option cannot be combined with the current lexical model"<<std::endl;
    }
  }

      // Set alignment smoothing interpolation factor if given and
      // supported by the current alignment model
  if(pars.af_given)
  {
    _incrHmmAligModel* incrHmmAligModelPtr=dynamic_cast<_incrHmmAligModel*>(swAligModelPtr);
    if(incrHmmAligModelPtr)
    {
      incrHmmAligModelPtr->setAlSmIntFactor(pars.af_val);
    }
    else
    {
      std::cerr<<"Warning: -af option cannot be combined with the current alignment model"<<std::endl;
    }
  }

  if(pars.s_given)
  {
#ifdef THOT_HAVE_LEVELDB_LIB 
        // Initialize model if necessary
    IncrLevelDbHmmP0AligModel* incrLevelDbHmmP0AligModelPtr = dynamic_cast<IncrLevelDbHmmP0AligModel*>(swAligModelPtr);
    if(incrLevelDbHmmP0AligModelPtr != NULL)
    {
      if(!pars.l_given)
      {
        std::cerr << "Initializing model with prefix " << pars.o_str << std::endl;
        incrLevelDbHmmP0AligModelPtr->init(pars.o_str.c_str());
      }
    }
#endif
        // Read sentence pairs
    std::string srctrgcFileName="";
    pair<unsigned int,unsigned int> pui;
    int ret=swAligModelPtr->readSentencePairs(pars.s_str.c_str(),
                                              pars.t_str.c_str(),
                                              srctrgcFileName.c_str(),
                                              pui);
    if(ret==THOT_ERROR)
    {
      release_swm(true);
      return THOT_ERROR;
    }
  }
  
      // Set verbosity value
  if(pars.v_given)
    verbosity=1;
  else
  {
    if(pars.v1_given) verbosity=2;
  }

  std::cerr<<"Starting EM iterations..."<<std::endl;
  
      // If -c given, initialize parameters uniformly
  if(!pars.eb_given && pars.c_given)
  {
    swAligModelPtr->trainAllSents(verbosity);
    if(!pars.nl_given)
    {
      pair<double,double> pdd=swAligModelPtr->loglikelihoodForAllSents(false);    
      std::cerr<<"Iter: "<<0<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<std::endl;
    }
    else std::cerr<<"Iter "<<0<<" completed"<<std::endl;
  }

      // Initialize range of sentences to train
  pair<unsigned int,unsigned int> wholeTrainRange;
  wholeTrainRange.first=0;
  if(swAligModelPtr->numSentPairs()>0)
    wholeTrainRange.second=swAligModelPtr->numSentPairs()-1;
  else
    wholeTrainRange.second=0;

      // EM iterations
  if(swAligModelPtr->numSentPairs()>0)
    emIters(pars,swAligModelPtr,wholeTrainRange,verbosity);
  
      // Print results
  int ret=swAligModelPtr->print(pars.o_str.c_str());
  if(ret==THOT_ERROR)
  {
    release_swm(true);
    return THOT_ERROR;
  }

      // Delete pointer
  release_swm(true);
  
  return THOT_OK;
}

//--------------- emIters function
void emIters(thot_gen_sw_model_pars& pars,
             BaseSwAligModel<std::vector<Prob> >* swAligModelPtr,
             pair<unsigned int,unsigned int> wholeTrainRange,
             int verbosity)
{
      // Execute EM iterations
  if(pars.i_given && pars.r_given && pars.in_given)
  {
        // -i, -r and -in options were given
    for(unsigned int j=wholeTrainRange.first;j<=wholeTrainRange.second+pars.r;++j)
    {
      for(unsigned int i=0;i<pars.numIter;++i)
      {
        int n=j-(i*(pars.r/pars.numIter));
        if(n>=0 && (unsigned int)n<=wholeTrainRange.second)
        {
          swAligModelPtr->trainSentPairRange(make_pair(n,n),verbosity);
        }
      }
    }
    if(!pars.nl_given)
    {
      pair<double,double> pdd=swAligModelPtr->loglikelihoodForPairRange(wholeTrainRange,false);    
      std::cerr<<"log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<std::endl;
    }
  }
  else
  {
        // -i, -r and -in options were not given simultaneously
    if(pars.mb_given)
    {
          // -mb parameter given
      bool end=false;
      unsigned int rangeId=0;
      unsigned int nIter=1;
      unsigned int stepNum=0;
      pair<unsigned int,unsigned int> trainRange;
      
      while(!end)
      {        
            // Set alpha value if appliable
        BaseStepwiseAligModel* swiseAligModelPtr=dynamic_cast<BaseStepwiseAligModel*>(swAligModelPtr);
        if(swiseAligModelPtr)
        {
          float lr=obtainLr(pars,stepNum);
          swiseAligModelPtr->set_nu_val(lr);
          if(verbosity)
            std::cerr<<"Learning rate has been set to "<<lr<<std::endl;
        }
        
            // Obtain train range
        trainRange.first=rangeId*pars.mb+wholeTrainRange.first;
        trainRange.second=trainRange.first+pars.mb-1;
        if(trainRange.second>=wholeTrainRange.second)
          trainRange.second=wholeTrainRange.second;
        
            // Execute training process
        if(verbosity)
          std::cerr<<"Executing mini-batch training over range "<<trainRange.first<<"-"<<trainRange.second<<" ..."<<std::endl;
        swAligModelPtr->trainSentPairRange(trainRange,verbosity);

            // Verify end of iteration condition
        ++rangeId;
        if(trainRange.first+pars.mb-1>=wholeTrainRange.second)
        {
          if(!pars.nl_given)
          {
            pair<double,double> pdd=swAligModelPtr->loglikelihoodForPairRange(wholeTrainRange,false);
            std::cerr<<"Iter: "<<nIter<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<std::endl;
          }
          else std::cerr<<"Iter "<<nIter<<" completed"<<std::endl;
          
          if(nIter<pars.numIter)
          {
            ++nIter;
            rangeId=0;
          }
          else end=true;
        }
            // Increase step number
        ++stepNum;
      }
    }
    else
    {
      for(unsigned int i=0;i<pars.numIter;++i)
      {
        if(pars.i_given)
        {
              // Execute incremental training
          for(unsigned int j=wholeTrainRange.first;j<=wholeTrainRange.second;++j)
          {
            swAligModelPtr->trainSentPairRange(make_pair(j,j),verbosity);
          }
          if(!pars.nl_given)
          {
            pair<double,double> pdd=swAligModelPtr->loglikelihoodForPairRange(wholeTrainRange,false);    
            std::cerr<<"Iter: "<<i+1<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<std::endl;
          }
          else std::cerr<<"Iter "<<i+1<<" completed"<<std::endl;
        }
        else
        {
          if(pars.eb_given)
          {
                // Execute efficient conventional training
            _incrSwAligModel<std::vector<Prob> >* _incrSwAligModelPtr=dynamic_cast<_incrSwAligModel<std::vector<Prob> >*>(swAligModelPtr);
            _incrSwAligModelPtr->efficientBatchTrainingForRange(wholeTrainRange,verbosity);
            if(!pars.nl_given)
            {
              pair<double,double> pdd=_incrSwAligModelPtr->loglikelihoodForPairRange(wholeTrainRange,false);    
              std::cerr<<"Iter: "<<i+1<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<std::endl;
            }
            else std::cerr<<"Iter "<<i+1<<" completed"<<std::endl;
          }
          else
          {
                // Execute conventional training
            swAligModelPtr->trainSentPairRange(wholeTrainRange,verbosity);
            if(!pars.nl_given)
            {
              pair<double,double> pdd=swAligModelPtr->loglikelihoodForPairRange(wholeTrainRange,false);    
              std::cerr<<"Iter: "<<i+1<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<std::endl;
            }
            else std::cerr<<"Iter "<<i+1<<" completed"<<std::endl;
          }
        }
      }
    }
  }
}

//--------------- obtainLr function
float obtainLr(const thot_gen_sw_model_pars& pars,
               unsigned int stepNum)
{
  if(pars.lr_given)
  {
        // Init variables
    float lr=FIXED_LR;
    unsigned int lr_type=(unsigned int) pars.lrPars[0];
    float par1=0;
    float par2=0;
    if(pars.lrPars.size()>=2) par1=pars.lrPars[1];
    if(pars.lrPars.size()>=3) par2=pars.lrPars[2];

        // Obtain learning rate
    switch(lr_type)
    {
      case LR_1: lr=par1;
        break;
      case LR_2: lr=1.0/(float)pow((float)stepNum+2,(float)par1);
        break;
      case LR_3: lr=par1/(1.0+((float)stepNum/par2));
        break;
    }
    return lr;
  }
  else return FIXED_LR;
}

//--------------- handleParameters function
int handleParameters(int argc,
                     char *argv[],
                     thot_gen_sw_model_pars& pars)
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
  std::vector<std::string> argv_stl=argv2argv_stl(argc,argv);
  if(takeParameters(argc,argv_stl,pars)==THOT_ERROR)
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

//--------------- takeparameters function
int takeParameters(int argc,
                   const std::vector<std::string>& argv_stl,
                   thot_gen_sw_model_pars& pars)
{
  int i=1;
  unsigned int matched;
  
  while(i<argc)
  {
    matched=0;
    
        // -s parameter
    if(argv_stl[i]=="-s" && !matched)
    {
      pars.s_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -s parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.s_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -t parameter
    if(argv_stl[i]=="-t" && !matched)
    {
      pars.t_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -t parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.t_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -l parameter
    if(argv_stl[i]=="-l" && !matched)
    {
      pars.l_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -l parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.l_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -n parameter
    if(argv_stl[i]=="-n" && !matched)
    {
      pars.n_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -n parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.numIter=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -nl parameter
    if(argv_stl[i]=="-nl" && !matched)
    {
      pars.nl_given=true;
      ++matched;
    }

        // -eb parameter
    if(argv_stl[i]=="-eb" && !matched)
    {
      pars.eb_given=true;
      ++matched;
    }

        // -i parameter
    if(argv_stl[i]=="-i" && !matched)
    {
      pars.i_given=true;
      ++matched;
    }

        // -c parameter
    if(argv_stl[i]=="-c" && !matched)
    {
      pars.c_given=true;
      ++matched;
    }

        // -r parameter
    if(argv_stl[i]=="-r" && !matched)
    {
      pars.r_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -r parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.r=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -in parameter
    if(argv_stl[i]=="-in" && !matched)
    {
      pars.in_given=true;
      ++matched;
    }

        // -mb parameter
    if(argv_stl[i]=="-mb" && !matched)
    {
      pars.mb_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -mb parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.mb=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -lr parameter
    if(argv_stl[i]=="-lr" && !matched)
    {
      pars.lr_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no values for -lr parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        ++matched;
        bool end=false;
        while(!end)
        {
          if(i==argc-1 || isOption(argv_stl[i+1].c_str()))
            end=true;
          else
          {
            pars.lrPars.push_back(atof(argv_stl[i+1].c_str()));
            ++i;
          }
        }
      }
    }

        // -np parameter
    if(argv_stl[i]=="-np" && !matched)
    {
      pars.np_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -np parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.np_val=atof(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

            // -lf parameter
    if(argv_stl[i]=="-lf" && !matched)
    {
      pars.lf_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -lf parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.lf_val=atof(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -af parameter
    if(argv_stl[i]=="-af" && !matched)
    {
      pars.af_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -af parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.af_val=atof(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -o parameter
    if(argv_stl[i]=="-o" && !matched)
    {
      pars.o_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -o parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.o_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -v parameter
    if(argv_stl[i]=="-v" && !matched)
    {
      pars.v_given=true;
      ++matched;
    }

        // -v1 parameter
    if(argv_stl[i]=="-v1" && !matched)
    {
      pars.v1_given=true;
      ++matched;
    }

        // Check if current parameter is not valid
    if(matched==0)
    {
      std::cerr<<"Error: parameter "<<argv_stl[i]<<" not valid."<<std::endl;
      return THOT_ERROR;
    }
    ++i;
  }
  return THOT_OK;
}

//--------------- checkParameters function
int checkParameters(thot_gen_sw_model_pars& pars)
{
  if(!pars.l_given)
  {
    if(!pars.s_given)
    {
      std::cerr<<"Error: -s parameter not given!"<<std::endl;
      return THOT_ERROR;
    }

    if(!pars.t_given)
    {
      std::cerr<<"Error: -t parameter not given!"<<std::endl;
      return THOT_ERROR;
    }
  }

  if(!pars.n_given)
  {
    std::cerr<<"Error: -n parameter not given!"<<std::endl;
    return THOT_ERROR;
  }

  if(!pars.o_given)
  {
    std::cerr<<"Error: -o parameter not given!"<<std::endl;
    return THOT_ERROR;
  }

  if(pars.eb_given)
  {
    if(pars.i_given || pars.c_given || pars.r_given || pars.mb_given || pars.in_given)
    {
      std::cerr<<"Error: parameter -eb cannot be combined with parameters -i, -c, -r, -mb and -in"<<std::endl;
      return THOT_ERROR;        
    }
  }

  if(pars.l_given && pars.c_given)
  {
    std::cerr<<"Error: parameter -l cannot be combined with parameter -c"<<std::endl;
    return THOT_ERROR;
  }

  if(pars.lr_given && !pars.mb_given)
  {
    std::cerr<<"Error: parameter -lr cannot be used without -mb parameter"<<std::endl;
    return THOT_ERROR;
  }
  
  if(pars.mb_given)
  {
    if(pars.eb_given || pars.i_given || pars.c_given || pars.r_given || pars.in_given)
    {
      std::cerr<<"Error: parameter -mb cannot be combined with parameters -i, -c, -r, -eb and -in"<<std::endl;
      return THOT_ERROR;        
    }
  }

  if(pars.r_given && !pars.i_given)
  {
    std::cerr<<"Error: parameter -r cannot be used without -i parameter"<<std::endl;
    return THOT_ERROR;
  }

  if(pars.in_given && !pars.i_given)
  {
    std::cerr<<"Error: parameter -in cannot be used without -i parameter"<<std::endl;
    return THOT_ERROR;
  }
  
      // Check invalid options when using non-incremental sw models
  if(init_swm(false)==THOT_ERROR)
    return THOT_ERROR;
      
  _incrSwAligModel<std::vector<Prob> >* _incrSwAligModelPtr=dynamic_cast<_incrSwAligModel<std::vector<Prob> >*>(swAligModelPtr);
  if(!_incrSwAligModelPtr)
  {
    if(pars.eb_given || pars.i_given || pars.c_given || pars.r_given || pars.mb_given || pars.in_given)
    {
      release_swm(false);
      std::cerr<<"Error: parameters -eb, -mb, -i, -c, -r and -in cannot be used with non-incremental single word models"<<std::endl;
      return THOT_ERROR;
    }
  }
  
  release_swm(false);
  
  return THOT_OK;
}

//--------------- printParameters function
void printParameters(thot_gen_sw_model_pars pars)
{
  if(!pars.l_given)
  {
    std::cerr<<"File with source sentences: "<<pars.s_str<<std::endl;
    std::cerr<<"File with target sentences: "<<pars.t_str<<std::endl;
  }
  else
    std::cerr<<"-l: "<<pars.l_str<<std::endl;
  std::cerr<<"Number of iterations: "<<pars.numIter<<std::endl;
  std::cerr<<"-nl: "<<pars.nl_given<<std::endl;
  std::cerr<<"-eb: "<<pars.eb_given<<std::endl;
  std::cerr<<"-i: "<<pars.i_given<<std::endl;
  std::cerr<<"-c: "<<pars.c_given<<std::endl;
  if(pars.r_given) std::cerr<<"-r: "<<pars.r<<std::endl;
  if(pars.mb_given) std::cerr<<"-mb: "<<pars.mb<<std::endl;
  if(pars.lr_given)
  {
    std::cerr<<"-lr: "<<(unsigned int)pars.lrPars[0]<<std::endl;
    for(unsigned int i=1;i<pars.lrPars.size();++i) std::cerr<<" "<<pars.lrPars[i];
    std::cerr<<std::endl;
  }
  std::cerr<<"-in: "<<pars.in_given<<std::endl;
  if(pars.np_given)
    std::cerr<<"-np: "<<pars.np_val<<std::endl;
  if(pars.lf_given)
    std::cerr<<"-lf: "<<pars.lf_val<<std::endl;
  if(pars.af_given)
    std::cerr<<"-af: "<<pars.af_val<<std::endl;
  std::cerr<<"Output files prefix: "<<pars.o_str<<std::endl;
  std::cerr<<"-v: "<<pars.v_given<<std::endl;
  std::cerr<<"-v1: "<<pars.v1_given<<std::endl;
}

//--------------- printUsage function
void printUsage(void)
{
  std::cerr<<"Usage: thot_gen_sw_model {[-s <string> -t <string>] [-l <string>]}\n";
  std::cerr<<"                      -n <int> [-nl]\n";
  std::cerr<<"                      [-eb | -mb <int> [-lr <int> [<float1>...<floatn>] ] \n";
  std::cerr<<"                      | -i [-c] [-r <int> [-in]] ]\n";
  std::cerr<<"                      [-np <float>] [-lf <float>] [-af <float>]\n";
  std::cerr<<"                      -o <string>\n";
  std::cerr<<"                      [-v|-v1] [--help] [--version]\n\n";
  std::cerr<<"-s <string>           File with source training sentences.\n";
  std::cerr<<"-t <string>           File with target training sentences.\n";
  std::cerr<<"-l <string>           Prefix of the model files to be loaded.\n";
  std::cerr<<"-n <int>              Number of EM iterations.\n";
  std::cerr<<"-nl                   Do not print the log-likelihood after each iteration\n";
  std::cerr<<"                      (saves computation time).\n";
  std::cerr<<"-eb                   Perform efficient batch training (saves memory).\n";
  std::cerr<<"                      NOTE: only available for incremental models.\n";
  std::cerr<<"-i                    Perform incremental training.\n";
  std::cerr<<"-c                    Start estimation with a conventional\n";
  std::cerr<<"                      EM iteration.\n";
  std::cerr<<"-r <int>              Restrict maximum size of matrix of\n";
  std::cerr<<"                      expected values in the dimension of n.\n";
  std::cerr<<"-mb <int>             Execute mini-batches of length <int>.\n";
  std::cerr<<"-lr <int> <f1...n>    Set learning-rate type. Depending on the lr\n";
  std::cerr<<"                      type, additional parameters are required.\n";
  std::cerr<<"                      1 -> fixed, <float1> required\n";
  std::cerr<<"                      2 -> liang, <float1> required\n";
  std::cerr<<"                      3 -> own, <float1> and <float2> required\n";
  std::cerr<<"-in                   Interlace EM iterations given the values of\n";
  std::cerr<<"                      -n and -r options.\n";
  std::cerr<<"-lf <float>           Set lexical smoothing interpolation factor, "<<DEFAULT_LEX_SMOOTH_INTERP_FACTOR<<" by\n";
  std::cerr<<"                      default (only available for HMM-based alignment models).\n";
  std::cerr<<"                      NOTE: this option has no effect when combined with\n";
  std::cerr<<"                      the -l option.\n";
  std::cerr<<"-af <float>           Set alignment smoothing interpolation factor, "<<DEFAULT_ALIG_SMOOTH_INTERP_FACTOR<<" by\n";
  std::cerr<<"                      default (only available for HMM-based alignment models).\n";
  std::cerr<<"                      NOTE: this option has no effect when combined with\n";
  std::cerr<<"                      the -l option.\n";
  std::cerr<<"-np <float>           Set probability value for the NULL word alignment, "<<DEFAULT_HMM_P0<<" by\n";
  std::cerr<<"                      default (only available for HMM-based alignment models\n";
  std::cerr<<"                      with fixed p0 probability).\n";
  std::cerr<<"                      NOTE: this option has no effect when combined with\n";
  std::cerr<<"                      the -l option.\n";
  std::cerr<<"-o <string>           Set prefix for output files.\n";
  std::cerr<<"-v | -v1              Verbose modes.\n";
  std::cerr<<"--help                Display this help and exit.\n";
  std::cerr<<"--version             Output version information and exit.\n";
}

//--------------- version function
void version(void)
{
  std::cerr<<"thot_gen_sw_model is part of the thot package "<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
