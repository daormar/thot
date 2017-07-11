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

#include "_incrSwAligModel.h"
#include "IncrHmmP0AligModel.h"
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
             BaseSwAligModel<Vector<Prob> >* swAligModelPtr,
             pair<unsigned int,unsigned int> wholeTrainRange,
             int verbosity);
float obtainLr(const thot_gen_sw_model_pars& pars,
               unsigned int stepNum);
int handleParameters(int argc,
                     char *argv[],
                     thot_gen_sw_model_pars& pars);
int takeParameters(int argc,
                   const Vector<std::string>& argv_stl,
                   thot_gen_sw_model_pars& pars);
int checkParameters(thot_gen_sw_model_pars& pars);
void printParameters(thot_gen_sw_model_pars pars);
void printUsage(void);
void version(void);

//--------------- Type definitions ------------------------------------

SimpleDynClassLoader<BaseSwAligModel<Vector<Prob> > > baseSwAligModelDynClassLoader;
BaseSwAligModel<Vector<Prob> >* swAligModelPtr;

//--------------- Global variables ------------------------------------


//--------------- Function Definitions --------------------------------


//--------------- main function
int main(int argc,char *argv[])
{
  thot_gen_sw_model_pars pars;
    
  if(handleParameters(argc,argv,pars)==ERROR)
  {
    return ERROR;
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
  if(dynClassFileHandler.load(THOT_MASTER_INI_PATH,verbosity)==ERROR)
  {
    cerr<<"Error while loading ini file"<<endl;
    return ERROR;
  }
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string soFileName;
  std::string initPars;

      ////////// Obtain info for BaseSwAligModel class
  baseClassName="BaseSwAligModel";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==ERROR)
  {
    cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<endl;
    cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<endl;
    return ERROR;
  }
   
      // Load class derived from BaseSwAligModel dynamically
  if(!baseSwAligModelDynClassLoader.open_module(soFileName,verbosity))
  {
    cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<endl;
    return ERROR;
  }

  swAligModelPtr=baseSwAligModelDynClassLoader.make_obj(initPars);
  if(swAligModelPtr==NULL)
  {
    cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
    baseSwAligModelDynClassLoader.close_module();
    
    return ERROR;
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

  if(init_swm(true)==ERROR)
    return ERROR;
  
      // Load model if -l option was given
  if(pars.l_given)
  {
        // Load model
    int ret=swAligModelPtr->load(pars.l_str.c_str());
    if(ret==ERROR)
    {
      release_swm(true);
      return ERROR;
    }
  }

      // Set maximum size in the dimension of n of the matrix of
      // expected values for incremental sw models
  _incrSwAligModel<Vector<Prob> >* _incrSwAligModelPtr=dynamic_cast<_incrSwAligModel<Vector<Prob> >*>(swAligModelPtr);
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
      cerr<<"NULL word alignment probability has been set to "<<pars.np_val<<endl;
    }
    else
    {
      cerr<<"Warning: -np option cannot be combined with the current alignment model"<<endl;
    }
  }

      // Set lexical smoothing interpolation factor if given and
      // supported by the current alignment model
  if(pars.lf_given)
  {
    IncrHmmAligModel* incrHmmAligModelPtr=dynamic_cast<IncrHmmAligModel*>(swAligModelPtr);
    if(incrHmmAligModelPtr)
    {
      incrHmmAligModelPtr->setLexSmIntFactor(pars.lf_val);
    }
    else
    {
      cerr<<"Warning: -lf option cannot be combined with the current lexical model"<<endl;
    }
  }

      // Set alignment smoothing interpolation factor if given and
      // supported by the current alignment model
  if(pars.af_given)
  {
    IncrHmmAligModel* incrHmmAligModelPtr=dynamic_cast<IncrHmmAligModel*>(swAligModelPtr);
    if(incrHmmAligModelPtr)
    {
      incrHmmAligModelPtr->setAlSmIntFactor(pars.af_val);
    }
    else
    {
      cerr<<"Warning: -af option cannot be combined with the current alignment model"<<endl;
    }
  }

  if(pars.s_given)
  {
        // Read sentence pairs
    std::string srctrgcFileName="";
    pair<unsigned int,unsigned int> pui;
    int ret=swAligModelPtr->readSentencePairs(pars.s_str.c_str(),
                                              pars.t_str.c_str(),
                                              srctrgcFileName.c_str(),
                                              pui);
    if(ret==ERROR)
    {
      release_swm(true);
      return ERROR;
    }
  }
  
      // Set verbosity value
  if(pars.v_given)
    verbosity=1;
  else
  {
    if(pars.v1_given) verbosity=2;
  }

  cerr<<"Starting EM iterations..."<<endl;
  
      // If -c given, initialize parameters uniformly
  if(!pars.eb_given && pars.c_given)
  {
    swAligModelPtr->trainAllSents(verbosity);
    if(!pars.nl_given)
    {
      pair<double,double> pdd=swAligModelPtr->loglikelihoodForAllSents(false);    
      cerr<<"Iter: "<<0<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<endl;
    }
    else cerr<<"Iter "<<0<<" completed"<<endl;
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
  swAligModelPtr->print(pars.o_str.c_str());

      // Delete pointer
  release_swm(true);
  
  return THOT_OK;
}

//--------------- emIters function
void emIters(thot_gen_sw_model_pars& pars,
             BaseSwAligModel<Vector<Prob> >* swAligModelPtr,
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
      cerr<<"log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<endl;
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
            cerr<<"Learning rate has been set to "<<lr<<endl;
        }
        
            // Obtain train range
        trainRange.first=rangeId*pars.mb+wholeTrainRange.first;
        trainRange.second=trainRange.first+pars.mb-1;
        if(trainRange.second>=wholeTrainRange.second)
          trainRange.second=wholeTrainRange.second;
        
            // Execute training process
        if(verbosity)
          cerr<<"Executing mini-batch training over range "<<trainRange.first<<"-"<<trainRange.second<<" ..."<<endl;
        swAligModelPtr->trainSentPairRange(trainRange,verbosity);

            // Verify end of iteration condition
        ++rangeId;
        if(trainRange.first+pars.mb-1>=wholeTrainRange.second)
        {
          if(!pars.nl_given)
          {
            pair<double,double> pdd=swAligModelPtr->loglikelihoodForPairRange(wholeTrainRange,false);
            cerr<<"Iter: "<<nIter<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<endl;
          }
          else cerr<<"Iter "<<nIter<<" completed"<<endl;
          
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
            cerr<<"Iter: "<<i+1<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<endl;
          }
          else cerr<<"Iter "<<i+1<<" completed"<<endl;
        }
        else
        {
          if(pars.eb_given)
          {
                // Execute efficient conventional training
            _incrSwAligModel<Vector<Prob> >* _incrSwAligModelPtr=dynamic_cast<_incrSwAligModel<Vector<Prob> >*>(swAligModelPtr);
            _incrSwAligModelPtr->efficientBatchTrainingForRange(wholeTrainRange,verbosity);
            if(!pars.nl_given)
            {
              pair<double,double> pdd=_incrSwAligModelPtr->loglikelihoodForPairRange(wholeTrainRange,false);    
              cerr<<"Iter: "<<i+1<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<endl;
            }
            else cerr<<"Iter "<<i+1<<" completed"<<endl;
          }
          else
          {
                // Execute conventional training
            swAligModelPtr->trainSentPairRange(wholeTrainRange,verbosity);
            if(!pars.nl_given)
            {
              pair<double,double> pdd=swAligModelPtr->loglikelihoodForPairRange(wholeTrainRange,false);    
              cerr<<"Iter: "<<i+1<<" , log-likelihood= "<<pdd.first<<" , norm-ll= "<<pdd.second<<endl;
            }
            else cerr<<"Iter "<<i+1<<" completed"<<endl;
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
    return ERROR;
  }
  if(readOption(argc,argv,"--help")!=-1)
  {
    printUsage();
    return ERROR;   
  }
  Vector<std::string> argv_stl=argv2argv_stl(argc,argv);
  if(takeParameters(argc,argv_stl,pars)==ERROR)
  {
    return ERROR;
  }
  else
  {
    if(checkParameters(pars)==THOT_OK)
    {
      return THOT_OK;
    }
    else
    {
      return ERROR;
    }
  }
}

//--------------- takeparameters function
int takeParameters(int argc,
                   const Vector<std::string>& argv_stl,
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
        cerr<<"Error: no value for -s parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -t parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -l parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -n parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -r parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -mb parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no values for -lr parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -np parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -lf parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -af parameter."<<endl;
        return ERROR;
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
        cerr<<"Error: no value for -o parameter."<<endl;
        return ERROR;
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
      cerr<<"Error: parameter "<<argv_stl[i]<<" not valid."<<endl;
      return ERROR;
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
      cerr<<"Error: -s parameter not given!"<<endl;
      return ERROR;
    }

    if(!pars.t_given)
    {
      cerr<<"Error: -t parameter not given!"<<endl;
      return ERROR;
    }
  }

  if(!pars.n_given)
  {
    cerr<<"Error: -n parameter not given!"<<endl;
    return ERROR;
  }

  if(!pars.o_given)
  {
    cerr<<"Error: -o parameter not given!"<<endl;
    return ERROR;
  }

  if(pars.eb_given)
  {
    if(pars.i_given || pars.c_given || pars.r_given || pars.mb_given || pars.in_given)
    {
      cerr<<"Error: parameter -eb cannot be combined with parameters -i, -c, -r, -mb and -in"<<endl;
      return ERROR;        
    }
  }

  if(pars.l_given && pars.c_given)
  {
    cerr<<"Error: parameter -l cannot be combined with parameter -c"<<endl;
    return ERROR;
  }

  if(pars.lr_given && !pars.mb_given)
  {
    cerr<<"Error: parameter -lr cannot be used without -mb parameter"<<endl;
    return ERROR;
  }
  
  if(pars.mb_given)
  {
    if(pars.eb_given || pars.i_given || pars.c_given || pars.r_given || pars.in_given)
    {
      cerr<<"Error: parameter -mb cannot be combined with parameters -i, -c, -r, -eb and -in"<<endl;
      return ERROR;        
    }
  }

  if(pars.r_given && !pars.i_given)
  {
    cerr<<"Error: parameter -r cannot be used without -i parameter"<<endl;
    return ERROR;
  }

  if(pars.in_given && !pars.i_given)
  {
    cerr<<"Error: parameter -in cannot be used without -i parameter"<<endl;
    return ERROR;
  }
  
      // Check invalid options when using non-incremental sw models
  if(init_swm(false)==ERROR)
    return ERROR;
      
  _incrSwAligModel<Vector<Prob> >* _incrSwAligModelPtr=dynamic_cast<_incrSwAligModel<Vector<Prob> >*>(swAligModelPtr);
  if(!_incrSwAligModelPtr)
  {
    if(pars.eb_given || pars.i_given || pars.c_given || pars.r_given || pars.mb_given || pars.in_given)
    {
      release_swm(false);
      cerr<<"Error: parameters -eb, -mb, -i, -c, -r and -in cannot be used with non-incremental single word models"<<endl;
      return ERROR;
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
    cerr<<"File with source sentences: "<<pars.s_str<<endl;
    cerr<<"File with target sentences: "<<pars.t_str<<endl;
  }
  else
    cerr<<"-l: "<<pars.l_str<<endl;
  cerr<<"Number of iterations: "<<pars.numIter<<endl;
  cerr<<"-nl: "<<pars.nl_given<<endl;
  cerr<<"-eb: "<<pars.eb_given<<endl;
  cerr<<"-i: "<<pars.i_given<<endl;
  cerr<<"-c: "<<pars.c_given<<endl;
  if(pars.r_given) cerr<<"-r: "<<pars.r<<endl;
  if(pars.mb_given) cerr<<"-mb: "<<pars.mb<<endl;
  if(pars.lr_given)
  {
    cerr<<"-lr: "<<(unsigned int)pars.lrPars[0]<<endl;
    for(unsigned int i=1;i<pars.lrPars.size();++i) cerr<<" "<<pars.lrPars[i];
    cerr<<endl;
  }
  cerr<<"-in: "<<pars.in_given<<endl;
  if(pars.np_given)
    cerr<<"-np: "<<pars.np_val<<endl;
  if(pars.lf_given)
    cerr<<"-lf: "<<pars.lf_val<<endl;
  if(pars.af_given)
    cerr<<"-af: "<<pars.af_val<<endl;
  cerr<<"Output files prefix: "<<pars.o_str<<endl;
  cerr<<"-v: "<<pars.v_given<<endl;
  cerr<<"-v1: "<<pars.v1_given<<endl;
}

//--------------- printUsage function
void printUsage(void)
{
  cerr<<"Usage: thot_gen_sw_model {[-s <string> -t <string>] [-l <string>]}\n";
  cerr<<"                      -n <numIter> [-nl]\n";
  cerr<<"                      [-eb | -mb <int> [-lr <type> [<float1>...<floatn>] ] \n";
  cerr<<"                      | -i [-c] [-r <int> [-in]] ]\n";
  cerr<<"                      [-np <float>] [-lf <float>] [-af <float>]\n";
  cerr<<"                      -o <string>\n";
  cerr<<"                      [-v|-v1] [--help] [--version]\n\n";
  cerr<<"-s <string>           File with source training sentences.\n";
  cerr<<"-t <string>           File with target training sentences.\n";
  cerr<<"-l <string>           Prefix of the model files to be loaded.\n";
  cerr<<"-n <int>              Number of EM iterations.\n";
  cerr<<"-nl                   Do not print the log-likelihood after each iteration\n";
  cerr<<"                      (saves computation time).\n";
  cerr<<"-eb                   Perform efficient batch training (saves memory).\n";
  cerr<<"                      NOTE: only available for incremental models.\n";
  cerr<<"-i                    Perform incremental training.\n";
  cerr<<"-c                    Start estimation with a conventional\n";
  cerr<<"                      EM iteration.\n";
  cerr<<"-r <int>              Restrict maximum size of matrix of\n";
  cerr<<"                      expected values in the dimension of n.\n";
  cerr<<"-mb <int>             Execute mini-batches of length <int>.\n";
  cerr<<"-lr <lrtype> <f1...n> Set learning-rate type. Depending on the lr\n";
  cerr<<"                      type, additional parameters are required.\n";
  cerr<<"                      1 -> fixed, <float1> required\n";
  cerr<<"                      2 -> liang, <float1> required\n";
  cerr<<"                      3 -> own, <float1> and <float2> required\n";
  cerr<<"-in                   Interlace EM iterations given the values of\n";
  cerr<<"                      -n and -r options.\n";
  cerr<<"-lf <float>           Set lexical smoothing interpolation factor, "<<DEFAULT_LEX_SMOOTH_INTERP_FACTOR<<" by\n";
  cerr<<"                      default (only available for HMM-based alignment models).\n";
  cerr<<"                      NOTE: this option has no effect when combined with\n";
  cerr<<"                      the -l option.\n";
  cerr<<"-af <float>           Set alignment smoothing interpolation factor, "<<DEFAULT_ALIG_SMOOTH_INTERP_FACTOR<<" by\n";
  cerr<<"                      default (only available for HMM-based alignment models).\n";
  cerr<<"                      NOTE: this option has no effect when combined with\n";
  cerr<<"                      the -l option.\n";
  cerr<<"-np <float>           Set probability value for the NULL word alignment, "<<DEFAULT_HMM_P0<<" by\n";
  cerr<<"                      default (only available for HMM-based alignment models\n";
  cerr<<"                      with fixed p0 probability).\n";
  cerr<<"                      NOTE: this option has no effect when combined with\n";
  cerr<<"                      the -l option.\n";
  cerr<<"-o <string>           Set prefix for output files.\n";
  cerr<<"-v | -v1              Verbose modes.\n";
  cerr<<"--help                Display this help and exit.\n";
  cerr<<"--version             Output version information and exit.\n";
}

//--------------- version function
void version(void)
{
  cerr<<"thot_gen_sw_model is part of the thot package "<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
