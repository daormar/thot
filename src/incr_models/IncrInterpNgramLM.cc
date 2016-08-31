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
      // Load weights
  int retval=loadLmEntries(fileName);
  if(retval==ERROR) return ERROR;

      // Load weights
  std::string fileNameW=fileName;
  fileNameW=fileNameW+".weights";
  retval=loadWeights(fileNameW.c_str());
  if(retval==ERROR) return ERROR;

  return OK;
}

//---------------
bool IncrInterpNgramLM::loadLmEntries(const char *fileName)
{
  std::string mainFileName;
  if(fileIsDescriptor(fileName,mainFileName))
  {
    awkInputStream awk;
    if(awk.open(fileName)==ERROR)
    {
      cerr<<"Error while loading descriptor file "<<fileName<<endl;
      return ERROR;
    }
    else
    {
          // Release previously stored model
      this->release();

      cerr<<"Loading model file "<<fileName<<endl;

          // Discard first line (it is used to identify the file as a
          // descriptor)
      awk.getln();
    
          // Read entries for each language model
      while(awk.getln())
      {
        if(awk.dollar(1)!="#")
        {
          if(awk.NF==3)
          {
                // Read entry
            std::string lmType=awk.dollar(1);
            std::string modelFileName=awk.dollar(2);
            std::string statusStr=awk.dollar(3);
            std::string absolutizedModelFileName=this->absolutizeModelFileName(fileName,modelFileName);
            cerr<<"* Reading LM entry: "<<lmType<<" "<<absolutizedModelFileName<<" "<<statusStr<<endl;
            loadLmEntry(lmType,absolutizedModelFileName,statusStr);
          }
        }
      }
          // Check if main model was found
      if(modelIndex!=0)
      {
        cerr<<"Error: the first model entry should be marked as main"<<endl;
        return ERROR;
      }
      else
        return OK;
    }
  }
  else
  {
    cerr<<"Error while loading descriptor file "<<fileName<<endl;
    return ERROR;
  }
}

//---------------
bool IncrInterpNgramLM::loadLmEntry(std::string lmType,
                                    std::string modelFileName,
                                    std::string statusStr)
{
      // Create lm file pointer
  BaseIncrEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,Count,Count>* biecmPtr=NULL;
  if(lmType=="jm") biecmPtr=new IncrJelMerNgramLM;
  if(lmType=="cjm") biecmPtr=new CacheIncrJelMerNgramLM;
  if(biecmPtr==NULL) return ERROR;

      // Store file pointer
  modelPtrVec.push_back(biecmPtr);

      // Add global to local maps
  GlobalToLocalSrcDataMap gtlSrcDataMap;
  gtlSrcMapVec.push_back(gtlSrcDataMap);

  GlobalToLocalTrgDataMap gtlTrgDataMap;
  gtlTrgMapVec.push_back(gtlTrgDataMap);

      // Load model from file
  int ret=this->modelPtrVec.back()->load(modelFileName.c_str());
  if(ret==ERROR) return ERROR;
        
      // Store lm type
  lmTypeVec.push_back(lmType);

      // Store status
  modelStatusVec.push_back(statusStr);
  
      // Check if model is main
  if(statusStr=="main")
    this->modelIndex=this->modelPtrVec.size()-1;

  return OK;
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
    Vector<float> _weights;

    cerr<<"Loading weights from "<<fileName<<endl;
        // Read weights for each language model
    while(awk.getln())
    {
      if(awk.NF==1)
      {
        _weights.push_back((float)atof(awk.dollar(1).c_str()));
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
      
    return OK;
  }
}

//--------------
bool IncrInterpNgramLM::print(const char *fileName)
{
  int ret=printLmEntries(fileName);
  if(ret==ERROR) return ERROR;

  ret=printWeights(fileName);
  if(ret==ERROR) return ERROR;  
  return OK;
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
    return OK;
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
    int ret = mkdir(currDirName.c_str(), S_IRUSR | S_IWUSR);
    if(ret!=0)
    {
      cerr<<"Error while printing model, directory "<<currDirName<<" could not be created."<<endl;
      return ERROR;
    }
  }
  else
  {
    if(info.st_mode & S_IFMT)
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

  return OK;
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
    return OK;
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
  return OK;
}

//---------------
Prob IncrInterpNgramLM::pTrgGivenSrc(const Vector<WordIndex>& s,
                                     const WordIndex& t)
{
  Prob p=0;
      
  for(unsigned int i=0;i<this->modelPtrVec.size();++i)
  {
    p+=(Prob)this->normWeights[i]*((Prob)this->modelPtrVec[i]->pTrgGivenSrc(mapGlobalToLocalSrcData(i,s),
                                                                            mapGlobalToLocalTrgData(i,t)));
  }
  return p;
}

//---------------
int IncrInterpNgramLM::updateModelWeights(const char *corpusFileName,
                                          int verbose/*=0*/)
{
      // TBD
  return OK;
}

//---------------
WordIndex IncrInterpNgramLM::getBosId(bool &found)const
{
  WordIndex bosid;
  found=HighTrg_to_Trg(BOS_STR,bosid);
  return bosid;
}

//---------------
WordIndex IncrInterpNgramLM::getEosId(bool &found)const
{
  WordIndex eosid;
  found=HighTrg_to_Trg(EOS_STR,eosid);
  return eosid;
}

//---------------
void IncrInterpNgramLM::setNgramOrder(int _ngramOrder)
{
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    _incrNgramLM<Count,Count>* _ilmPtr;
    _ilmPtr=dynamic_cast<_incrNgramLM<Count,Count>*>(modelPtrVec[i]);
    if(_ilmPtr!=NULL)
      _ilmPtr->setNgramOrder(_ngramOrder);
  }
}

//---------------
unsigned int IncrInterpNgramLM::getNgramOrder(void)
{
  if(this->modelPtrVec.size()>0)
  {
    _incrNgramLM<Count,Count>* _ilmPtr;
    _ilmPtr=dynamic_cast<_incrNgramLM<Count,Count>*>(modelPtrVec[modelIndex]);
    if(_ilmPtr!=NULL)
      return _ilmPtr->getNgramOrder();
    else
      return 0;
  }
  else
    return 0;
}

//---------------
IncrInterpNgramLM::~IncrInterpNgramLM()
{
}

//------------------------------
