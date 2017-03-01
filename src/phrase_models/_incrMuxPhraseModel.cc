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
/* Module: _incrMuxPhraseModel                                      */
/*                                                                  */
/* Definitions file: _incrMuxPhraseModel.cc                         */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "_incrMuxPhraseModel.h"


//--------------- Global variables -----------------------------------


//--------------- _incrMuxPhraseModel class method definitions

_incrMuxPhraseModel::_incrMuxPhraseModel(void)
{
  modelIndex=INVALID_MUX_PMODEL_INDEX;
  swVocPtr=new SingleWordVocab;
}

//-------------------------
void _incrMuxPhraseModel::strAddTableEntry(const Vector<string>& s,
                                           const Vector<string>& t,
                                           PhrasePairInfo inf)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    ipmPtr->strAddTableEntry(s,t,inf);
  else
    cerr<<"Warning: strAddTableEntry() function not implemented!"<<endl;
}

//-------------------------
void _incrMuxPhraseModel::addTableEntry(const Vector<WordIndex>& s,
                                        const Vector<WordIndex>& t,
                                        PhrasePairInfo inf)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    ipmPtr->addTableEntry(s,t,inf);
  else
    cerr<<"Warning: addTableEntry() function not implemented!"<<endl;
}

//-------------------------
void _incrMuxPhraseModel::strIncrCountsOfEntry(const Vector<string>& s,
                                               const Vector<string>& t,
                                               Count count)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    ipmPtr->strIncrCountsOfEntry(s,t,count);
  else
    cerr<<"Warning: strIncrCountsOfEntry() function not implemented!"<<endl;
}

//-------------------------
void _incrMuxPhraseModel::incrCountsOfEntry(const Vector<WordIndex>& s,
                                            const Vector<WordIndex>& t,
                                            Count count)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    ipmPtr->incrCountsOfEntry(s,t,count);
  else
    cerr<<"Warning: incrCountsOfEntry() function not implemented!"<<endl;
}

//-------------------------
Count _incrMuxPhraseModel::cSrcTrg(const Vector<WordIndex>& s,
                                   const Vector<WordIndex>& t)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    return ipmPtr->cSrcTrg(s,t);
  else
  {
    cerr<<"Warning: cSrcTrg() function not implemented!"<<endl;
    return 0;
  }
}

//-------------------------
Count _incrMuxPhraseModel::cSrc(const Vector<WordIndex>& s)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    return ipmPtr->cSrc(s);
  else
  {
    cerr<<"Warning: cSrc() function not implemented!"<<endl;
    return 0;
  }
}

//-------------------------
Count _incrMuxPhraseModel::cTrg(const Vector<WordIndex>& t)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    return ipmPtr->cTrg(t);
  else
  {
    cerr<<"Warning: cTrg() function not implemented!"<<endl;
    return 0;
  }
}

//-------------------------
Count _incrMuxPhraseModel::cHSrcHTrg(const Vector<std::string>& hs,
                                     const Vector<std::string>& ht)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    return ipmPtr->cHSrcHTrg(hs,ht);
  else
  {
    cerr<<"Warning: cHSrcHTrg() function not implemented!"<<endl;
    return 0;
  }
}

//-------------------------
Count _incrMuxPhraseModel::cHSrc(const Vector<std::string>& hs)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    return ipmPtr->cHSrc(hs);
  else
  {
    cerr<<"Warning: cHSrc() function not implemented!"<<endl;
    return 0;
  }
}

//-------------------------
Count _incrMuxPhraseModel::cHTrg(const Vector<std::string>& ht)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    return ipmPtr->cHTrg(ht);
  else
  {
    cerr<<"Warning: cHTrg() function not implemented!"<<endl;
    return 0;
  }
}

//-------------------------
PhrasePairInfo _incrMuxPhraseModel::infSrcTrg(const Vector<WordIndex>& s,
                                              const Vector<WordIndex>& t,
                                              bool& found)
{
  BaseIncrPhraseModel* ipmPtr=dynamic_cast<BaseIncrPhraseModel* > (modelPtrVec[modelIndex]);
  if(ipmPtr)
    return ipmPtr->infSrcTrg(s,t,found);
  else
  {
    cerr<<"Warning: infSrcTrg() function not implemented!"<<endl;
    PhrasePairInfo phpinfo;
    return phpinfo;
  }
}

//-------------------------
Prob _incrMuxPhraseModel::pk_tlen(unsigned int tlen,
                                  unsigned int k)
{
  return modelPtrVec[modelIndex]->pk_tlen(tlen,k);
}

//-------------------------
LgProb _incrMuxPhraseModel::srcSegmLenLgProb(unsigned int x_k,
                                             unsigned int x_km1,
                                             unsigned int srcLen)
{
  return modelPtrVec[modelIndex]->srcSegmLenLgProb(x_k,x_km1,srcLen);
}

//-------------------------
LgProb _incrMuxPhraseModel::idxSrcSegmLenLgProb(int idx,
                                                unsigned int x_k,
                                                unsigned int x_km1,
                                                unsigned int srcLen)
{
  return modelPtrVec[idx]->srcSegmLenLgProb(x_k,x_km1,srcLen);
}

//-------------------------
LgProb _incrMuxPhraseModel::trgCutsLgProb(int offset)
{
  return modelPtrVec[modelIndex]->trgCutsLgProb(offset);
}

//-------------------------
LgProb _incrMuxPhraseModel::idxTrgCutsLgProb(int idx,
                                             int offset)
{
  return modelPtrVec[idx]->trgCutsLgProb(offset);
}

//-------------------------
LgProb _incrMuxPhraseModel::trgSegmLenLgProb(unsigned int k,
                                             const SentSegmentation& trgSegm,
                                             unsigned int trgLen,
                                             unsigned int lastSrcSegmLen)
{
  return modelPtrVec[modelIndex]->trgSegmLenLgProb(k,trgSegm,trgLen,lastSrcSegmLen);
}

//-------------------------
LgProb _incrMuxPhraseModel::idxTrgSegmLenLgProb(int idx,
                                                unsigned int k,
                                                const SentSegmentation& trgSegm,
                                                unsigned int trgLen,
                                                unsigned int lastSrcSegmLen)
{
  return modelPtrVec[idx]->trgSegmLenLgProb(k,trgSegm,trgLen,lastSrcSegmLen);
}

//-------------------------
LgProb _incrMuxPhraseModel::logpt_s_(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t)
{
  return modelPtrVec[modelIndex]->logpt_s_(srcMapGlobalToLocalWidxVec(modelIndex,s),
                                           trgMapGlobalToLocalWidxVec(modelIndex,t));
}

//-------------------------
LgProb _incrMuxPhraseModel::idxLogpt_s_(int idx,
                                        const Vector<WordIndex>& s,
                                        const Vector<WordIndex>& t)
{
  return modelPtrVec[idx]->logpt_s_(srcMapGlobalToLocalWidxVec(idx,s),
                                    trgMapGlobalToLocalWidxVec(idx,t));
}

//-------------------------
LgProb _incrMuxPhraseModel::logps_t_(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t)
{
  return modelPtrVec[modelIndex]->logps_t_(srcMapGlobalToLocalWidxVec(modelIndex,s),
                                           trgMapGlobalToLocalWidxVec(modelIndex,t));
}

//-------------------------
LgProb _incrMuxPhraseModel::idxLogps_t_(int idx,
                                        const Vector<WordIndex>& s,
                                        const Vector<WordIndex>& t)
{
  return modelPtrVec[idx]->logps_t_(srcMapGlobalToLocalWidxVec(idx,s),
                                    trgMapGlobalToLocalWidxVec(idx,t));
}

//-------------------------
bool _incrMuxPhraseModel::getTransFor_s_(const Vector<WordIndex>& s,
                                         _incrMuxPhraseModel::TrgTableNode& trgtn)
{
      // Obtain translations for local model
  _incrMuxPhraseModel::TrgTableNode localTrgtn;
  bool ret=modelPtrVec[modelIndex]->getTransFor_s_(srcMapGlobalToLocalWidxVec(modelIndex,s),
                                                   localTrgtn);
      // Map local word indices to global vocabulary
  trgtn.clear();
  _incrMuxPhraseModel::TrgTableNode::iterator iter;
  for(iter=localTrgtn.begin();iter!=localTrgtn.end();++iter)
  {
    Vector<WordIndex> widxVec=trgMapLocalToGlobalWidxVec(modelIndex,iter->first);
    trgtn.insert(std::make_pair(widxVec,iter->second));
  }
  return ret;
}

//-------------------------
bool _incrMuxPhraseModel::getTransFor_t_(const Vector<WordIndex>& t,
                                         _incrMuxPhraseModel::SrcTableNode& srctn)
{
      // Obtain translations for local model
  _incrMuxPhraseModel::SrcTableNode localSrctn;
  bool ret=modelPtrVec[modelIndex]->getTransFor_t_(trgMapGlobalToLocalWidxVec(modelIndex,t),
                                                   localSrctn);
      // Map local word indices to global vocabulary
  srctn.clear();
  for(_incrMuxPhraseModel::SrcTableNode::iterator iter=localSrctn.begin();iter!=localSrctn.end();++iter)
  {
    Vector<WordIndex> widxVec=srcMapLocalToGlobalWidxVec(modelIndex,iter->first);
    srctn.insert(std::make_pair(widxVec,iter->second));
  }
  return ret;
}

//-------------------------
bool _incrMuxPhraseModel::getTransVecFor_t_(const Vector<WordIndex>& t,
                                            Vector<SrcTableNode>& srctnVec)
{
  bool ret=true;
  srctnVec.clear();
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
        // Obtain translations for local model
    _incrMuxPhraseModel::SrcTableNode localSrctn;
    bool localRet=modelPtrVec[i]->getTransFor_t_(trgMapGlobalToLocalWidxVec(i,t),
                                                 localSrctn);
        // Map local word indices to global vocabulary
    SrcTableNode srctn;
    for(_incrMuxPhraseModel::SrcTableNode::iterator iter=localSrctn.begin();iter!=localSrctn.end();++iter)
    {
      Vector<WordIndex> widxVec=srcMapLocalToGlobalWidxVec(modelIndex,iter->first);
      srctn.insert(std::make_pair(widxVec,iter->second));
    }
    srctnVec.push_back(srctn);

    if(!localRet)
      ret=false;
  }
  return ret;
}

//-------------------------
bool _incrMuxPhraseModel::getNbestTransFor_s_(const Vector<WordIndex>& s,
                                              NbestTableNode<PhraseTransTableNodeData>& nbt)
{
      // Obtain translations for local model
  NbestTableNode<PhraseTransTableNodeData> localNbt;
  bool ret=modelPtrVec[modelIndex]->getNbestTransFor_s_(srcMapGlobalToLocalWidxVec(modelIndex,s),
                                                        localNbt);
      // Map local word indices to global vocabulary
  nbt.clear();
  NbestTableNode<PhraseTransTableNodeData>::iterator iter;
  for(iter=localNbt.begin();iter!=localNbt.end();++iter)
  { 
    Vector<WordIndex> widxVec=trgMapLocalToGlobalWidxVec(modelIndex,iter->second);
    nbt.insert(iter->first,widxVec);
  }
  
  return ret;
}

//-------------------------	
bool _incrMuxPhraseModel::getNbestTransFor_t_(const Vector<WordIndex>& t,
                                              NbestTableNode<PhraseTransTableNodeData>& nbt,
                                              int N/*=-1*/) 
{  
      // Obtain translations for local model
  NbestTableNode<PhraseTransTableNodeData> localNbt;
  bool ret=modelPtrVec[modelIndex]->getNbestTransFor_t_(trgMapGlobalToLocalWidxVec(modelIndex,t),
                                                        localNbt);
      // Map local word indices to global vocabulary
  nbt.clear();
  NbestTableNode<PhraseTransTableNodeData>::iterator iter;
  for(iter=localNbt.begin();iter!=localNbt.end();++iter)
  { 
    Vector<WordIndex> widxVec=srcMapLocalToGlobalWidxVec(modelIndex,iter->second);
    nbt.insert(iter->first,widxVec);
  }

  while(nbt.size()>(unsigned int)N && N>=0)
  {
        // node contains more than N elements, remove last one
    nbt.removeLastElement();
  }

  return ret;
}

//-------------------------
size_t _incrMuxPhraseModel::getSrcVocabSize(void)const
{
  return swVocPtr->getSrcVocabSize();	
}

//-------------------------
bool _incrMuxPhraseModel::loadSrcVocab(const char *srcInputVocabFileName)
{
  return swVocPtr->loadSrcVocab(srcInputVocabFileName);
}

//-------------------------
bool _incrMuxPhraseModel::loadTrgVocab(const char *trgInputVocabFileName)
{
  return swVocPtr->loadTrgVocab(trgInputVocabFileName);
}

//-------------------------
WordIndex _incrMuxPhraseModel::stringToSrcWordIndex(string s)const
{
  WordIndex w;
  
  bool found=srcGlobalStringToWordIndex(s,w);
  if(!found) w=UNK_WORD;
    
  return w;
}

//-------------------------
string _incrMuxPhraseModel::wordIndexToSrcString(WordIndex w)const
{
  return swVocPtr->wordIndexToSrcString(w);
}

//-------------------------
bool _incrMuxPhraseModel::existSrcSymbol(string s)const
{
  return existSrcGlobalString(s);  
}

//-------------------------
Vector<WordIndex> _incrMuxPhraseModel::strVectorToSrcIndexVector(const Vector<string>& s,
                                                                 Count numTimes/*=1*/)
{
  Vector<WordIndex> swVec;
  
  for(unsigned int i=0;i<s.size();++i)
    swVec.push_back(addSrcSymbol(s[i],numTimes));
  
  return swVec;
}

//-------------------------
Vector<string> _incrMuxPhraseModel::srcIndexVectorToStrVector(const Vector<WordIndex>& s)
{
  Vector<string> vStr;
  unsigned int i;
  
  for(i=0;i<s.size();++i)
    vStr.push_back((wordIndexToSrcString(s[i]))); 	 
	
  return vStr;
}

//-------------------------
WordIndex _incrMuxPhraseModel::addSrcSymbol(string s,
                                            Count numTimes/*=1*/)
{
  return swVocPtr->addSrcSymbol(s,numTimes);
}

//-------------------------
bool _incrMuxPhraseModel::printSrcVocab(const char* /*outputFileName*/)
{
  cerr<<"Warning: printSrcVocab() function not implemented!"<<endl;
  return ERROR;
}

//-------------------------
size_t _incrMuxPhraseModel::getTrgVocabSize(void)const
{
  return swVocPtr->getTrgVocabSize();	
}

//-------------------------
WordIndex _incrMuxPhraseModel::stringToTrgWordIndex(string t)const
{
  WordIndex w;
  
  bool found=trgGlobalStringToWordIndex(t,w);
  if(!found) w=UNK_WORD;
    
  return w;
}

//-------------------------
string _incrMuxPhraseModel::wordIndexToTrgString(WordIndex w)const
{
  return swVocPtr->wordIndexToTrgString(w);
}

//-------------------------
bool _incrMuxPhraseModel::existTrgSymbol(string t)const
{
  return existTrgGlobalString(t);
}

//-------------------------
Vector<WordIndex> _incrMuxPhraseModel::strVectorToTrgIndexVector(const Vector<string>& t,
                                                                 Count numTimes/*=1*/)
{
  Vector<WordIndex> twVec;
  
  for(unsigned int i=0;i<t.size();++i)
    twVec.push_back(addTrgSymbol(t[i],numTimes));
  
  return twVec;
}

//-------------------------
Vector<string> _incrMuxPhraseModel::trgIndexVectorToStrVector(const Vector<WordIndex>& t)
{
  Vector<string> vStr;
  unsigned int i;

  for(i=0;i<t.size();++i)
    vStr.push_back((wordIndexToTrgString(t[i]))); 	 
  
  return vStr;
}

//-------------------------
WordIndex _incrMuxPhraseModel::addTrgSymbol(string t,
                                            Count numTimes/*=1*/)
{
  return swVocPtr->addTrgSymbol(t,numTimes);
}

//-------------------------
bool _incrMuxPhraseModel::printTrgVocab(const char* /*outputFileName*/)
{
  cerr<<"Warning: printTrgVocab() function not implemented!"<<endl;
  return ERROR;
}

//-------------------------
bool _incrMuxPhraseModel::existSrcGlobalString(const std::string& str)const
{
      // Search for index in vocabulary
  bool found=swVocPtr->existSrcSymbol(str);
  if(found)
    return true;
  else
  {
        // Index not found
        // Iterate over the different models
    for(unsigned int i=0;i<modelPtrVec.size();++i)
    {
      found=this->modelPtrVec[i]->existSrcSymbol(str);
      if(found)
        return true;
    }
        // Index not present in the different models
    return false;
  }
}

//-------------------------
bool _incrMuxPhraseModel::srcGlobalStrVecToWidxVec(const Vector<std::string>& rq,
                                                   Vector<WordIndex>& vu)const
{
  for(unsigned int i=0;i<rq.size();++i)
  {
    WordIndex w;
    bool found=srcGlobalStringToWordIndex(rq[i],w);
    if(!found)
      return false;
    else
      vu.push_back(w);
  }
  return true;  
}

//-------------------------
bool _incrMuxPhraseModel::srcGlobalStringToWordIndex(const std::string& str,
                                                     WordIndex& w)const
{
      // Search for index in vocabulary
  bool found=swVocPtr->existSrcSymbol(str);
  if(found)
  {
        // Index found
    w=swVocPtr->stringToSrcWordIndex(str);
    return true;
  }
  else
  {
        // Index not found
        // Iterate over the different models
    for(unsigned int i=0;i<modelPtrVec.size();++i)
    {
      found=this->modelPtrVec[i]->existSrcSymbol(str);
      if(found)
      {
            // Index found, add index to encoder
        w=swVocPtr->addSrcSymbol(str);
        return true;
      }
    }
        // Index not present in the different models
    return false;
  }
}

//-------------------------
bool _incrMuxPhraseModel::existTrgGlobalString(const std::string& str)const
{
      // Search for index in vocabulary
  bool found=swVocPtr->existTrgSymbol(str);
  if(found)
    return true;
  else
  {
        // Index not found
        // Iterate over the different models
    for(unsigned int i=0;i<modelPtrVec.size();++i)
    {
      found=this->modelPtrVec[i]->existTrgSymbol(str);
      if(found)
        return true;
    }
        // Index not present in the different models
    return false;
  }
}

//-------------------------
bool _incrMuxPhraseModel::trgGlobalStrVecToWidxVec(const Vector<std::string>& rq,
                                                   Vector<WordIndex>& vu)const
{
  for(unsigned int i=0;i<rq.size();++i)
  {
    WordIndex w;
    bool found=trgGlobalStringToWordIndex(rq[i],w);
    if(!found)
      return false;
    else
      vu.push_back(w);
  }
  return true;    
}

//-------------------------
bool _incrMuxPhraseModel::trgGlobalStringToWordIndex(const std::string& str,
                                                     WordIndex& w)const
{
      // Search for index in vocabulary
  bool found=swVocPtr->existTrgSymbol(str);
  if(found)
  {
        // Index found
    w=swVocPtr->stringToTrgWordIndex(str);
    return true;
  }
  else
  {
        // Index not found
        // Iterate over the different models
    for(unsigned int i=0;i<modelPtrVec.size();++i)
    {
      found=this->modelPtrVec[i]->existTrgSymbol(str);
      if(found)
      {
            // Index found, add index to encoder
        w=swVocPtr->addTrgSymbol(str);
        return true;
      }
    }
        // Index not present in the different models
    return false;
  }  
}

//-------------------------
Vector<WordIndex> _incrMuxPhraseModel::srcMapGlobalToLocalWidxVec(unsigned int index,
                                                                  const Vector<WordIndex>& widxVec)
{
  Vector<WordIndex> result;
  for(unsigned int i=0;i<widxVec.size();++i)
  {
    result.push_back(srcMapGlobalToLocalWidx(index,widxVec[i]));
  }
  return result;
}


//-------------------------
WordIndex _incrMuxPhraseModel::srcMapGlobalToLocalWidx(unsigned int index,
                                                       const WordIndex& widx)
{
  GlobalToLocalDataMap::iterator iter=srcGtlDataMapVec[index].find(widx);
  if(iter!=srcGtlDataMapVec[index].end())
  {
    return iter->second;
  }
  else
  {
    WordIndex w;
    std::string str=swVocPtr->wordIndexToSrcString(widx);
    bool found=modelPtrVec[index]->existSrcSymbol(str);
    w=modelPtrVec[index]->stringToSrcWordIndex(str);
    if(found)
    {
      srcGtlDataMapVec[index][widx]=w;
    }
    return w;
  }
}

//-------------------------
Vector<WordIndex> _incrMuxPhraseModel::srcMapLocalToGlobalWidxVec(unsigned int /*index*/,
                                                                  const Vector<WordIndex>& widxVec)
{
      // Obtain string vector
  Vector<std::string> strVec;
  for(unsigned int i=0;i<widxVec.size();++i)
  {
    strVec.push_back(modelPtrVec[modelIndex]->wordIndexToSrcString(widxVec[i]));
  }

      // Obtain global word index vector
  Vector<WordIndex> localWidxVec;
  srcGlobalStrVecToWidxVec(strVec,localWidxVec);
  return localWidxVec;
}

//-------------------------
Vector<WordIndex> _incrMuxPhraseModel::trgMapGlobalToLocalWidxVec(unsigned int index,
                                                                  const Vector<WordIndex>& widxVec)
{
  Vector<WordIndex> result;
  for(unsigned int i=0;i<widxVec.size();++i)
  {
    result.push_back(trgMapGlobalToLocalWidx(index,widxVec[i]));
  }
  return result;
}

//-------------------------
WordIndex _incrMuxPhraseModel::trgMapGlobalToLocalWidx(unsigned int index,
                                                       const WordIndex& widx)
{
  GlobalToLocalDataMap::iterator iter=trgGtlDataMapVec[index].find(widx);
  if(iter!=trgGtlDataMapVec[index].end())
  {
    return iter->second;
  }
  else
  {
    WordIndex w;
    std::string str=swVocPtr->wordIndexToTrgString(widx);
    bool found=modelPtrVec[index]->existTrgSymbol(str);
    w=modelPtrVec[index]->stringToTrgWordIndex(str);
    if(found)
    {
      trgGtlDataMapVec[index][widx]=w;
    }
    return w;
  }
}

//-------------------------
Vector<WordIndex> _incrMuxPhraseModel::trgMapLocalToGlobalWidxVec(unsigned int /*index*/,
                                                                  const Vector<WordIndex>& widxVec)
{
      // Obtain string vector
  Vector<std::string> strVec;
  for(unsigned int i=0;i<widxVec.size();++i)
  {
    strVec.push_back(modelPtrVec[modelIndex]->wordIndexToTrgString(widxVec[i]));
  }

      // Obtain global word index vector
  Vector<WordIndex> localWidxVec;
  trgGlobalStrVecToWidxVec(strVec,localWidxVec);
  return localWidxVec;
}

//-------------------------
Vector<string> _incrMuxPhraseModel::stringToStringVector(string s)
{
 Vector<string> vs;	
 string aux;
 unsigned int i=0;	
 bool end=false;
	
 while(!end)
 {
  aux="";	 
  while(s[i]!=' ' && s[i]!='	' && i<s.size()) 
  {
    aux+=s[i];
    ++i;		   
  }
  if(aux!="") vs.push_back(aux);	   
  while((s[i]==' ' || s[i]=='	') && i<s.size()) ++i;	 
  if(i>=s.size()) end=true;	  
 }
 
 return vs;	
}

//-------------------------
Vector<string> _incrMuxPhraseModel::extractCharItemsToVector(char *ch)const
{
 unsigned int i=0;
 string s;	
 Vector<string> v,u;

 while(ch[i]!=0)
 {
   s=""; 
   while(ch[i]==' ' && ch[i]!=0) {++i;}	  
   while(ch[i]!=' ' && ch[i]!=0) {s=s+ch[i]; ++i;}
   if(s!="") v.push_back(s);  
 }	  
	 
 for(i=0;i<v.size();++i)
 {
   u.push_back(v[v.size()-1-i]);
 }
 return u;
}

//-------------------------
size_t _incrMuxPhraseModel::size(void)
{
  size_t s=0;
  
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    s+=modelPtrVec[i]->size();
  }
  return s;
}

//-------------------------
int _incrMuxPhraseModel::getNumModels(void)
{
  return modelPtrVec.size();
}

//-------------------------
int _incrMuxPhraseModel::getModelIndex(void)
{
  return modelIndex;
}

//-------------------------
void _incrMuxPhraseModel::setModelIndex(int idx)
{
  if(idx<(int) modelPtrVec.size())
    modelIndex=idx;
}

//-------------------------
void _incrMuxPhraseModel::resetModelIndex(void)
{
  if(modelIndex!=INVALID_MUX_PMODEL_INDEX && modelPtrVec.size()>=1)
    modelIndex=0;
}

//-------------------------
_incrMuxPhraseModel::~_incrMuxPhraseModel()
{
  delete swVocPtr;
}

//-------------------------
