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
/* Module: IncrInterpNgramLM                                        */
/*                                                                  */
/* Definitions file: IncrInterpNgramLM.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrInterpNgramLM.h"

//--------------- Global variables -----------------------------------

//--------------- Function declarations 

//--------------- Constants


//--------------- Classes --------------------------------------------

//---------------
bool IncrInterpNgramLM::load(const char *fileName)
{
      // Load language model entries
  int retval=loadLmEntries(fileName);
  if(retval==ERROR) return ERROR;

      // Load weights
  std::string fileNameW=fileName;
  fileNameW=fileNameW+".weights";
  retval=loadWeights(fileNameW.c_str());
  if(retval==ERROR) return ERROR;

  return THOT_OK;
}

//---------------
bool IncrInterpNgramLM::loadLmEntries(const char *fileName)
{
  Vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(fileName,modelDescEntryVec)==THOT_OK)
  {
    for(unsigned int i=0;i<modelDescEntryVec.size();++i)
    {
      cerr<<"* Reading LM entry: "<<modelDescEntryVec[i].modelType<<" "<<modelDescEntryVec[i].absolutizedModelFileName<<" "<<modelDescEntryVec[i].statusStr<<endl;
      int ret=loadLmEntry(modelDescEntryVec[i].modelType,
                          modelDescEntryVec[i].absolutizedModelFileName,
                          modelDescEntryVec[i].statusStr);
      if(ret==ERROR)
        return ERROR;
    }
    return THOT_OK;
  }
  else
    return ERROR;     
}

//---------------
bool IncrInterpNgramLM::loadLmEntry(std::string lmType,
                                    std::string modelFileName,
                                    std::string statusStr)
{
      // Create pointer to model
  BaseNgramLM<Vector<WordIndex> >* lmPtr=createLmPtr(lmType);
  if(lmPtr==NULL)
    return ERROR;
  
      // Store file pointer
  modelPtrVec.push_back(lmPtr);

      // Add global to local map
  GlobalToLocalDataMap gtlDataMap;
  gtlDataMapVec.push_back(gtlDataMap);

      // Load model from file
  int ret=modelPtrVec.back()->load(modelFileName.c_str());
  if(ret==ERROR) return ERROR;
        
      // Store lm type
  lmTypeVec.push_back(lmType);

      // Store status
  modelStatusVec.push_back(statusStr);
  
      // Check if model is main
  if(statusStr=="main")
    modelIndex=modelPtrVec.size()-1;

  return THOT_OK;
}

//---------------
bool IncrInterpNgramLM::loadWeights(const char *fileName)
{
      // Open file with weights
  awkInputStream awk;
  if(awk.open(fileName)==ERROR)
  {
    cerr<<"Error, file with weights "<<fileName<<" cannot be read"<<endl;
    return ERROR;
  }  
  else
  {
    Vector<double> _weights;

    cerr<<"Loading weights from "<<fileName<<endl;
        // Read weights for each language model
    while(awk.getln())
    {
      if(awk.NF==1)
      {
        _weights.push_back((double)atof(awk.dollar(1).c_str()));
      }
    }
    awk.close();

        // Check if each model has its weight
    unsigned int numModels=lmTypeVec.size();
    if(numModels!=_weights.size())
    {
      cerr<<"Error, file "<<fileName<<" contains "<<_weights.size()<<" but "<<numModels<<" models were loaded"<<endl;
      return ERROR;
    }
    
        // Set weights
    setWeights(_weights);
      
    return THOT_OK;
  }
}

//--------------
bool IncrInterpNgramLM::print(const char *fileName)
{
  int ret=printLmEntries(fileName);
  if(ret==ERROR) return ERROR;

  ret=printWeights(fileName);
  if(ret==ERROR) return ERROR;  
  return THOT_OK;
}

//---------------
bool IncrInterpNgramLM::printLmEntries(const char *fileName)
{
  ofstream outF;

  outF.open(fileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing model to file."<<endl;
    return ERROR;
  }
  else
  {
        // Print header
    outF<<"thot lm descriptor"<<endl;

        // Print lm entries
    for(unsigned int i=0;i<lmTypeVec.size();++i)
    {
          // Print descriptor entry
      std::string currModelFileName=obtainFileNameForLmEntry(fileName,i);
      outF<<lmTypeVec[i]<<" "<<currModelFileName<<" "<<modelStatusVec[i]<<endl;

          // Print language model
      bool ret=printLm(fileName,i);
      if(ret==ERROR)
        return ERROR;
    }
    return THOT_OK;
  }
}

//---------------
bool IncrInterpNgramLM::printLm(const char* fileDescName,
                                unsigned int entry_index)
{
      // Obtain directory name for model entry
  std::string currDirName=obtainDirNameForLmEntry(fileDescName,entry_index);

      // Obtain model file name
  std::string currModelFileName=obtainFileNameForLmEntry(fileDescName,entry_index);

      // Check if directory already exists. Create directory when
      // necessary
  struct stat info;
  if(stat(currDirName.c_str(),&info) != 0)
  {
        // No file or directory with given name exists
        // Create directory
    int ret = mkdir(currDirName.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    if(ret!=0)
    {
      cerr<<"Error while printing model, directory "<<currDirName<<" could not be created."<<endl;
      return ERROR;
    }
  }
  else
  {
    if(info.st_mode & S_IFREG)
    {
          // A file with the same name existed
      cerr<<"Error while printing model, directory "<<currDirName<<" could not be created."<<endl;
      return ERROR;
    }
  }
      // Print model files
  return modelPtrVec[entry_index]->print(currModelFileName.c_str());
}

//---------------
std::string IncrInterpNgramLM::obtainFileNameForLmEntry(const std::string fileDescName,
                                                        unsigned int entry_index)
{
      // Obtain directory name for model entry
  std::string currDirName=obtainDirNameForLmEntry(fileDescName,entry_index);

      // Obtain model file name
  std::string currModelFileName=currDirName+"/trg.lm";

  return currModelFileName;
}

//---------------
std::string IncrInterpNgramLM::obtainDirNameForLmEntry(const std::string fileDescName,
                                                       unsigned int entry_index)
{
      // Obtain directory name for model entry
  std::string fileDescDirName=extractDirName(fileDescName);

      // Obtain directory name
  std::string currDirName=fileDescDirName+"/"+modelStatusVec[entry_index];

  return currDirName;
}

//---------------
bool IncrInterpNgramLM::printWeights(const char *fileName)
{
  int ret=printInterModelWeights(fileName);
  if(ret==ERROR)
    return ERROR;
  
  ret=printIntraModelWeights(fileName);
  if(ret==ERROR)
    return ERROR;

  return THOT_OK;
}

//---------------
bool IncrInterpNgramLM::printInterModelWeights(const char *fileName)
{
  ofstream outF;

  std::string weightsFileName=fileName;
  weightsFileName=weightsFileName+".weights";
  outF.open(weightsFileName.c_str(),ios::out);
  if(!outF)
  {
    cerr<<"Error while printing model to file."<<endl;
    return ERROR;
  }
  else
  {
    for(unsigned int i=0;i<weights.size();++i)
    {
      outF<<weights[i]<<endl;
    }
    return THOT_OK;
  }
} 

//---------------
bool IncrInterpNgramLM::printIntraModelWeights(const char *fileName)
{
      // Print lm entries
  for(unsigned int i=0;i<lmTypeVec.size();++i)
  {
        // Print descriptor entry
    std::string currModelFileName=obtainFileNameForLmEntry(fileName,i);
    
        // Print language model weights (if appliable)
    _incrJelMerNgramLM<Count,Count>* incrJelMerLmPtr=dynamic_cast<_incrJelMerNgramLM<Count,Count>* >(modelPtrVec[i]);
    if(incrJelMerLmPtr)
    {
      bool ret=incrJelMerLmPtr->printWeights(currModelFileName.c_str());
      if(ret==ERROR)
        return ERROR;
    }
  }
  return THOT_OK;
}

//---------------
int IncrInterpNgramLM::updateModelWeights(const char *corpusFileName,
                                          int verbose/*=0*/)
{      
      // Update weights for each model entry (if any)
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    if(verbose)
      cerr<<"Updating weights of "<<modelStatusVec[i]<<" language model..."<<endl;
    _incrJelMerNgramLM<Count,Count>* incrJelMerLmPtr=dynamic_cast<_incrJelMerNgramLM<Count,Count>* >(modelPtrVec[i]);
    if(incrJelMerLmPtr)
      incrJelMerLmPtr->updateModelWeights(corpusFileName,verbose);
    else
    {
      if(verbose)
        cerr<<"This model does not have weights to be updated"<<endl;
    }
  }

      // Update weights of model combination
  if(verbose)
    cerr<<"Updating weights of model combination..."<<endl;
  int ret=updateModelCombinationWeights(corpusFileName,verbose);
  if(ret==ERROR)
    return ERROR;
  
  return THOT_OK;
}

//---------------
int IncrInterpNgramLM::updateModelCombinationWeights(const char *corpusFileName,
                                                     int verbose/*=0*/)
{
      // Initialize downhill simplex input parameters
  Vector<double> initial_weights=weights;
  int ndim=initial_weights.size();
  double* start=(double*) malloc(ndim*sizeof(double));
  int nfunk=0;
  double* x=(double*) malloc(ndim*sizeof(double));
  double y;

      // Create temporary file
  FILE* tmp_file=tmpfile();
  
  if(tmp_file==0)
  {
    cerr<<"Error updating of Jelinek Mercer's language model weights, tmp file could not be created"<<endl;
    return ERROR;
  }
    
      // Execute downhill simplex algorithm
  int ret;
  bool end=false;
  while(!end)
  {
        // Set initial weights (each call to step_by_step_simplex starts
        // from the initial weights)
    for(unsigned int i=0;i<initial_weights.size();++i)
      start[i]=initial_weights[i];
    
        // Execute step by step simplex
    double curr_dhs_ftol=DBL_MAX;
    ret=step_by_step_simplex(start,ndim,DHS_INTERP_LM_FTOL,DHS_INTERP_LM_SCALE_PAR,NULL,tmp_file,&nfunk,&y,x,&curr_dhs_ftol,false);

    switch(ret)
    {
      case THOT_OK: end=true;
        break;
      case DSO_NMAX_ERROR: cerr<<"Error updating of Jelinek Mercer's language model weights, maximum number of iterations exceeded"<<endl;
        end=true;
        break;
      case DSO_EVAL_FUNC: // A new function evaluation is requested by downhill simplex
        double perp;
        int retEval=new_dhs_eval(corpusFileName,tmp_file,x,perp);
        if(retEval==ERROR)
        {
          end=true;
          break;
        }
            // Print verbose information
        if(verbose>=1)
        {
          cerr<<"niter= "<<nfunk<<" ; current ftol= "<<curr_dhs_ftol<<" (FTOL="<<DHS_INTERP_LM_FTOL<<") ; ";
          cerr<<"weights=";
          for(unsigned int i=0;i<weights.size();++i)
            cerr<<" "<<weights[i];
          cerr<<" ; perp= "<<perp<<endl; 
        }
        break;
    }
  }
  
      // Set new weights if updating was successful
  if(ret==THOT_OK)
  {
    Vector<double> _weights;
    for(unsigned int i=0;i<weights.size();++i)
      _weights.push_back(start[i]);
    setWeights(_weights);
  }
  else
  {
    Vector<double> _weights=initial_weights;
    setWeights(_weights);
  }
  
      // Clear variables
  free(start);
  free(x);
  fclose(tmp_file);

  if(ret!=THOT_OK)
    return ERROR;
  else
    return THOT_OK;
}

//---------------
int IncrInterpNgramLM::new_dhs_eval(const char *corpusFileName,
                                    FILE* tmp_file,
                                    double* x,
                                    double& obj_func)
{
  unsigned int numOfSentences;
  unsigned int numWords;
  LgProb totalLogProb;
  bool weightsArePositive=true;
  int retVal;
  Vector<double> _weights=weights;
  
      // Fix weights to be evaluated
  for(unsigned int i=0;i<_weights.size();++i)
  {
    _weights[i]=x[i];
    if(_weights[i]<0) weightsArePositive=false;
  }
  if(weightsArePositive)
  {
        // Set weights
    setWeights(_weights);
        
        // Obtain perplexity
    retVal=this->perplexity(corpusFileName,numOfSentences,numWords,totalLogProb,obj_func);
  }
  else
  {
    obj_func=DBL_MAX;
    retVal=THOT_OK;
  }
      // Print result to tmp file
  fprintf(tmp_file,"%g\n",obj_func);
  fflush(tmp_file);
      // step_by_step_simplex needs that the file position
      // indicator is set at the start of the stream
  rewind(tmp_file);

  return retVal;
}

//---------------
WordIndex IncrInterpNgramLM::getBosId(bool &found)const
{
  WordIndex bosid;
  found=globalStringToWordIndex(BOS_STR,bosid);
  return bosid;
}

//---------------
WordIndex IncrInterpNgramLM::getEosId(bool &found)const
{
  WordIndex eosid;
  found=globalStringToWordIndex(EOS_STR,eosid);
  return eosid;
}

//---------------
void IncrInterpNgramLM::setNgramOrder(int _ngramOrder)
{
  for(unsigned int i=0;i<modelPtrVec.size();++i)
    modelPtrVec[i]->setNgramOrder(_ngramOrder);
}

//---------------
unsigned int IncrInterpNgramLM::getNgramOrder(void)
{
  if(modelPtrVec.size()>0)
    return modelPtrVec[modelIndex]->getNgramOrder();
  else
    return 0;
}

//-------------------------
BaseNgramLM<Vector<WordIndex> >* IncrInterpNgramLM::createLmPtr(std::string tmType)
{
  SimpleDynClassLoaderMap::iterator iter=simpleDynClassLoaderMap.find(tmType);
  if(iter!=simpleDynClassLoaderMap.end())
  {
    return iter->second.make_obj("");
  }
  else
  {
        // Declare dynamic class loader instance
    SimpleDynClassLoader<BaseNgramLM<Vector<WordIndex> > > simpleDynClassLoader;
  
        // Open module
    bool verbosity=false;
    if(!simpleDynClassLoader.open_module(tmType,verbosity))
    {
      cerr<<"Error: so file ("<<tmType<<") could not be opened"<<endl;
      return NULL;
    }

        // Create tm file pointer
    BaseNgramLM<Vector<WordIndex> >* tmPtr=simpleDynClassLoader.make_obj("");
    if(tmPtr==NULL)
    {
      cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<endl;
      simpleDynClassLoader.close_module();
    
      return NULL;
    }
        // Store class loader in map
    simpleDynClassLoaderMap.insert(std::make_pair(tmType,simpleDynClassLoader));
    
    return tmPtr;
  }
}

//-------------------------
void IncrInterpNgramLM::deleteModelPointers(void)
{
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    delete modelPtrVec[i];
  }
  modelPtrVec.clear();
}

//-------------------------
void IncrInterpNgramLM::closeDynamicModules(void)
{
  SimpleDynClassLoaderMap::iterator iter;
  for(iter=simpleDynClassLoaderMap.begin();iter!=simpleDynClassLoaderMap.end();++iter)
    iter->second.close_module(false);
  simpleDynClassLoaderMap.clear();
}

//---------------
void IncrInterpNgramLM::clear(void)
{
  weights.clear();
  normWeights.clear();
  gtlDataMapVec.clear();
  modelIndex=INVALID_LMODEL_INDEX;
  encPtr->clear();
  deleteModelPointers();
  closeDynamicModules();
}

//---------------
IncrInterpNgramLM::~IncrInterpNgramLM()
{
  clear();
}
