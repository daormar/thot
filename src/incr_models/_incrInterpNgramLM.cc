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
/* Module: _incrInterpNgramLM                                       */
/*                                                                  */
/* Definitions file: _incrInterpNgramLM.cc                          */
/*                                                                  */
/********************************************************************/

//--------------- Include files --------------------------------------

#include "_incrInterpNgramLM.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//---------------
_incrInterpNgramLM::_incrInterpNgramLM()
{
  modelIndex=INVALID_LMODEL_INDEX;
  this->encPtr=new lm_ienc;
}

//---------------
Count _incrInterpNgramLM::cHist(const Vector<WordIndex>& vu)
{
  BaseIncrNgramLM<Vector<WordIndex> >* ilmPtr=dynamic_cast<BaseIncrNgramLM<Vector<WordIndex> >* > (modelPtrVec[modelIndex]);
  if(ilmPtr)
    return ilmPtr->cHist(mapGlobalToLocalWidxVec(modelIndex,vu));
  else
  {
    cerr<<"Warning: cHist() function not available for this model!"<<endl;
    return 0;
  }
}

//---------------
Count _incrInterpNgramLM::cNgram(const WordIndex& w,
                                 const Vector<WordIndex>& vu)
{
  BaseIncrNgramLM<Vector<WordIndex> >* ilmPtr=dynamic_cast<BaseIncrNgramLM<Vector<WordIndex> >* > (modelPtrVec[modelIndex]);
  if(ilmPtr)
    return ilmPtr->cNgram(mapGlobalToLocalWidx(modelIndex,w),mapGlobalToLocalWidxVec(modelIndex,vu));
  else
  {
    cerr<<"Warning: cNgram() function not available for this model!"<<endl;
    return 0;
  }
}

//---------------
Count _incrInterpNgramLM::cHistStr(const Vector<std::string>& rq)
{
      // Process history
  Vector<WordIndex> vu;
  bool found=globalStrVecToWidxVec(rq,vu);
  if(!found)
    return 0;
  
  BaseIncrNgramLM<Vector<WordIndex> >* ilmPtr=dynamic_cast<BaseIncrNgramLM<Vector<WordIndex> >* > (modelPtrVec[modelIndex]);
  if(ilmPtr)
  {
    return ilmPtr->cHist(vu);
  }
  else
  {
    cerr<<"Warning: cHistStr() function not available for this model!"<<endl;
    return 0;
  }
}

//---------------
Count _incrInterpNgramLM::cNgramStr(const std::string& s,
                                    const Vector<std::string>& rq)
{
      // Process history
  Vector<WordIndex> vu;
  bool found=globalStrVecToWidxVec(rq,vu);
  if(!found)
    return 0;

      // Process new word
  WordIndex w;
  if(!globalStringToWordIndex(s,w))
    return 0;

  BaseIncrNgramLM<Vector<WordIndex> >* ilmPtr=dynamic_cast<BaseIncrNgramLM<Vector<WordIndex> >* > (modelPtrVec[modelIndex]);
  if(ilmPtr)
    return ilmPtr->cNgram(w,vu);
  else
  {
    cerr<<"Warning: cNgramStr() function not available for this model!"<<endl;
    return 0;
  }
}

//---------------
void _incrInterpNgramLM::incrCountsOfNgramStr(const std::string& s,
                                              const Vector<std::string>& rq,
                                              Count c)
{  
  BaseIncrNgramLM<Vector<WordIndex> >* ilmPtr=dynamic_cast<BaseIncrNgramLM<Vector<WordIndex> >* > (modelPtrVec[modelIndex]);
  if(ilmPtr)
    ilmPtr->incrCountsOfNgramStr(s,rq,c);
  else
    cerr<<"Warning: incrCountsOfNgramStr() function not implemented!"<<endl;
}

//---------------
void _incrInterpNgramLM::incrCountsOfNgram(const WordIndex& w,
                                           const Vector<WordIndex>& vu,
                                           Count c)
{
  BaseIncrNgramLM<Vector<WordIndex> >* ilmPtr=dynamic_cast<BaseIncrNgramLM<Vector<WordIndex> >* > (modelPtrVec[modelIndex]);
  if(ilmPtr)
    ilmPtr->incrCountsOfNgram(mapGlobalToLocalWidx(modelIndex,w),mapGlobalToLocalWidxVec(modelIndex,vu),c);
  else
    cerr<<"Warning: incrCountsOfNgramStr() function not implemented!"<<endl;
}

//---------------
LgProb _incrInterpNgramLM::getNgramLgProb(WordIndex w,
                                          const Vector<WordIndex>& vu)
{
  double lp=SMALL_LG_NUM;
    
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    double lpmodel=(double) log(normWeights[i])+ (double) modelPtrVec[i]->getNgramLgProb(mapGlobalToLocalWidx(i,w),
                                                                                         mapGlobalToLocalWidxVec(i,vu));
    lp=MathFuncs::lns_sumlog(lp,lpmodel);
//    cerr<<lp<<" "<<lpmodel<<" "<<normWeights[i]<<" "<<modelPtrVec[i]->getNgramLgProb(mapGlobalToLocalWidx(i,w),mapGlobalToLocalWidxVec(i,vu))<<endl;
  }
//  cerr<<"***"<<endl;
  return lp;
}

//---------------
LgProb _incrInterpNgramLM::getNgramLgProbStr(string s,
                                             const Vector<string>& rq)
{
      // Process history
  Vector<WordIndex> vu;
  bool found=globalStrVecToWidxVec(rq,vu);
  if(!found)
    return SMALL_LG_NUM;

      // Process new word
  WordIndex w;
  if(!globalStringToWordIndex(s,w))
    return SMALL_LG_NUM;

      // Return log-prob
  return getNgramLgProb(w,vu);
}

//---------------
LgProb _incrInterpNgramLM::getLgProbEnd(const Vector<WordIndex>& vu)
{
  bool found;
  
  return getNgramLgProb(this->getEosId(found),vu);  
}

//---------------
LgProb _incrInterpNgramLM::getLgProbEndStr(const Vector<string>& rq)
{
  return getNgramLgProbStr("</s>",rq);
}

//---------------
bool _incrInterpNgramLM::getStateForWordSeq(const Vector<WordIndex>& wordSeq,
                                            Vector<WordIndex>& state)
{
  state=wordSeq;
  return true;
}

//---------------
void _incrInterpNgramLM::getStateForBeginOfSentence(Vector<WordIndex> &state)
{
  Vector<WordIndex> keySeq;
  int i;
  bool found;
  
  for(i=0;i<(int)this->getNgramOrder()-1;++i)
    state.push_back(this->getBosId(found));  
}

//---------------
LgProb _incrInterpNgramLM::getNgramLgProbGivenState(WordIndex w,
                                                    Vector<WordIndex> &state)
{
  LgProb lp;

  lp=getNgramLgProb(w,state);
  for(unsigned int i=1;i<state.size();++i) state[i-1]=state[i];
  if(state.size()>0) state[state.size()-1]=w;
  return lp;
}

//---------------
LgProb _incrInterpNgramLM::getNgramLgProbGivenStateStr(std::string s,
                                                       Vector<WordIndex> &state)
{
 WordIndex w;
	
 w=stringToWordIndex(s);
 return getNgramLgProbGivenState(w,state);    
}

//---------------
LgProb _incrInterpNgramLM::getLgProbEndGivenState(Vector<WordIndex> &state)
{
  LgProb lp;
  bool found;
  
  lp=getLgProbEnd(state);
  for(unsigned int i=1;i<state.size();++i) state[i-1]=state[i];
  if(state.size()>0) state[state.size()-1]=this->getEosId(found);
  return lp;   
}

//---------------
bool _incrInterpNgramLM::existSymbol(string s)const
{
  WordIndex w;
  
  return globalStringToWordIndex(s,w);  
}

//---------------
WordIndex _incrInterpNgramLM::addSymbol(std::string s)
{
  WordIndex w=encPtr->genHTrgCode(s);
  encPtr->addHTrgCode(s,w);
  return w;
}

//---------------
unsigned int _incrInterpNgramLM::getVocabSize(void)
{
  return encPtr->sizeTrg();
}

//---------------
WordIndex _incrInterpNgramLM::stringToWordIndex(string s)const
{
  WordIndex w;
  
  bool found=globalStringToWordIndex(s,w);
  if(!found) w=UNK_SYMBOL;
    
  return w;
}

//---------------
string _incrInterpNgramLM::wordIndexToString(WordIndex w)const
{
  std::string s;
  
  bool found=encPtr->Trg_to_HighTrg(w,s);
  if(!found) s=UNK_SYMBOL_STR;
  
  return s;
}

//---------------
bool _incrInterpNgramLM::loadVocab(const char *fileName)
{
  cerr<<"Warning: loadVocab() function not implemented!"<<endl;
  return ERROR;
}

//---------------
bool _incrInterpNgramLM::printVocab(const char *fileName)
{
  cerr<<"Warning: printVocab() function not implemented!"<<endl;
  return ERROR;
}

//---------------
void _incrInterpNgramLM::clearVocab(void)
{
  cerr<<"Warning: clearVocab() function not implemented!"<<endl;
}

//---------------
size_t _incrInterpNgramLM::size(void)
{
  size_t s=0;
  
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    s+=modelPtrVec[i]->size();
  }
  return s;
}

//---------------
void _incrInterpNgramLM::clear(void)
{
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    modelPtrVec[i]->clear();
  }
  weights.clear();
  normWeights.clear();
  gtlDataMapVec.clear();
  modelIndex=INVALID_LMODEL_INDEX;
  encPtr->clear();
}

//---------------
void _incrInterpNgramLM::release(void)
{
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    delete modelPtrVec[i];
  }
  modelPtrVec.clear();
  weights.clear();
  normWeights.clear();
  gtlDataMapVec.clear();
  modelIndex=INVALID_LMODEL_INDEX;
}

//---------------
_incrInterpNgramLM::~_incrInterpNgramLM()
{
  release();
  delete encPtr;
}

//---------------
bool _incrInterpNgramLM::globalStringToWordIndex(const std::string& ht,
                                                 WordIndex& t)const
{
      // Search for code in encPtr
  bool found=encPtr->HighTrg_to_Trg(ht,t);
  if(found)
  {
        // Code found
    return true;
  }
  else
  {
        // Code not found
        // Iterate over the different models
    for(unsigned int i=0;i<modelPtrVec.size();++i)
    {
      found=this->modelPtrVec[i]->existSymbol(ht);
      if(found)
      {
            // Code found, add code to encoder
        t=encPtr->genHTrgCode(ht);
        encPtr->addHTrgCode(ht,t);
        return true;
      }
    }
        // Code not present in the different models
    return false;
  }
}

//---------------
bool _incrInterpNgramLM::globalStrVecToWidxVec(const Vector<std::string>& rq,
                                               Vector<WordIndex>& vu)const
{
  for(unsigned int i=0;i<rq.size();++i)
  {
    WordIndex w;
    bool found=globalStringToWordIndex(rq[i],w);
    if(!found)
      return false;
    else
      vu.push_back(w);
  }
  return true;
}

//---------------
Vector<WordIndex> _incrInterpNgramLM::mapGlobalToLocalWidxVec(unsigned int index,
                                                              const Vector<WordIndex>& global_s)
{
  Vector<WordIndex> widxVec;
  for(unsigned int i=0;i<global_s.size();++i)
  {
    widxVec.push_back(mapGlobalToLocalWidx(index,global_s[i]));
  }
  return widxVec;
}

//---------------
WordIndex _incrInterpNgramLM::mapGlobalToLocalWidx(unsigned int index,
                                                   const WordIndex& global_t)
{
  GlobalToLocalDataMap::iterator iter=gtlDataMapVec[index].find(global_t);
  if(iter!=gtlDataMapVec[index].end())
  {
    return iter->second;
  }
  else
  {
    std::string ht;
    WordIndex t;
    bool found=encPtr->Trg_to_HighTrg(global_t,ht);
    found=modelPtrVec[index]->existSymbol(ht);
    t=modelPtrVec[index]->stringToWordIndex(ht);
    if(found)
    {
      gtlDataMapVec[index][global_t]=t;
    }
    return t;
  }
}

//---------------
void _incrInterpNgramLM::setWeights(const Vector<double>& _weights)
{
  weights=_weights;
  normWeights=obtainNormWeights(weights);
}

//---------------
Vector<double> _incrInterpNgramLM::obtainNormWeights(const Vector<double>& unnormWeights)
{
      // Obtain sum
  double sum=0;
  for(unsigned int i=0;i<unnormWeights.size();++i)
    sum+=unnormWeights[i];

      // Create norm. weights vector
  Vector<double> result;
  for(unsigned int i=0;i<unnormWeights.size();++i)
  {
    result.push_back(unnormWeights[i]/sum);
  }
      // Return result
  return result;  
}

//-----------------------------------------------------------------
