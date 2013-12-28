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
  awkInputStream awk;
  
  if(awk.open(fileName)==ERROR)
  {
    cerr<<"Error while loading model file "<<fileName<<endl;
    return ERROR;
  }
  else
  {
        // Release previously stored model
    this->release();

    cerr<<"Loading model file "<<fileName<<endl;
    
        // Read model entries
    while(awk.getline())
    {
      if(awk.dollar(1)!="#")
      {
        if(awk.NF==3)
        {
              // Read entry
          std::string lmType=awk.dollar(1);
          std::string modelFileName=awk.dollar(2);
          std::string activeStr=awk.dollar(3);
          cerr<<"* Reading LM entry: "<<lmType<<" "<<modelFileName<<" "<<activeStr<<endl;

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

              // Store file name
          this->modelFileNameVec.push_back(modelFileName);

              // Check if model is active
          if(activeStr=="yes")
            this->modelIndex=this->modelPtrVec.size()-1;
        }
      }
    }
    if(modelIndex==INVALID_MODEL_INDEX)
    {
      cerr<<"Error: one of the models should be marked as active"<<endl;
      return ERROR;
    }
    else
      return OK;
  }
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
    if(awk.getline())
    {
          // Read weights
      for(unsigned int i=1;i<=awk.NF;++i)
      {
        _weights.push_back((float)atof(awk.dollar(i).c_str()));
      }
      awk.close();

          // Set weights
      setWeights(_weights);
      
      return OK;
    }
    else
    {
      cerr<<"Error while loading file with weights: "<<fileName<<endl;
      awk.close();
      return ERROR;
    }
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
    for(unsigned int i=0;i<lmTypeVec.size();++i)
    {
          // Print model entry

          // Obtain model file name
      stringstream ss;
      ss<<i;
      std::string currFileName=fileName;
      currFileName=currFileName+"."+ss.str();
      outF<<lmTypeVec[i]<<" "<<modelFileNameVec[i]<<" ";
      if(modelIndex==(int) i)
        outF<<"yes";
      else
        outF<<"no";
      outF<<endl;

          // Print model files
      int ret=modelPtrVec[i]->print(currFileName.c_str());
      if(ret==ERROR) return ERROR;
    }
    outF.close();	
    return OK;
  }
}

//---------------
bool IncrInterpNgramLM::printWeights(const char *fileName)
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
    for(unsigned int i=0;i<weights.size();++i)
    {
      outF<<weights[i];
    }
    outF<<endl;
    return OK;
  }
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
