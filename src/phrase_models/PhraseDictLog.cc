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
/* Module: PhraseDictLog                                            */
/*                                                                  */
/* Definitions file: PhraseDictLog.cc                               */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhraseDictLog.h"

//--------------- Function definitions

//-------------------------
PhraseDictLog::PhraseDictLog(void)
{
}

//-------------------------
LogCount PhraseDictLog::getLogCounts_t_(PhraseCountLogState pls_state,
                                        const std::vector<WordIndex>& t,
                                        bool& found)
{
  PhraseTableNodeLog* ptNodeLogPtr;
  
  ptNodeLogPtr=dictLog.find(t);	
  if(ptNodeLogPtr!=NULL)
  {
    LogCount *lcptr=ptNodeLogPtr->findPtr(pls_state);
    if(lcptr==NULL)
    {
      found=false;
      return SMALL_LG_NUM;
    }
    else
    {
      found=true;
      return *lcptr;
    }
  }
  else
  {
    found=false;
    return SMALL_LG_NUM;
  }
}

//-------------------------
LogCount PhraseDictLog::getLogCount_t(const std::vector<WordIndex>& t)
{
  if(cache_lct_.t_present(t))
  {
    return cache_lct_.lct_();
  }
  else
  {
    PhraseTableNodeLog* ptNodeLogPtr;
    PhraseTableNodeLog::iterator ptNodeLogIter;	
    LogCount lcount_t_;	
  
    ptNodeLogPtr=dictLog.find(t);	

    if(ptNodeLogPtr!=NULL)
    { // phrase t is stored in PhraseMap
      lcount_t_=0;	    
	  
          // obtain lcount_t_
      for(ptNodeLogIter=ptNodeLogPtr->begin();ptNodeLogIter!=ptNodeLogPtr->end();++ptNodeLogIter)
      {
        lcount_t_=MathFuncs::lns_sumlog(lcount_t_,ptNodeLogIter->second);
      }
    }
    else lcount_t_=SMALL_LG_NUM;   // phrase t is not stored in PhraseMap

    cache_lct_.init(t,lcount_t_);
    
    return lcount_t_;
  }
}
//-------------------------
void PhraseDictLog::addSrcTrgPair(PhraseCountLogState pls_state,
                                  const std::vector<WordIndex>& t,
                                  LogCount logCount)
{
  PhraseTableNodeLog* ptNodeLogPtr;
  PhraseTableNodeLog ptNodeLog;
  pair<PhraseCountLogState,LogCount> ppc;
  LogCount *lcptr;
  
  if(cache_lct_.t_present(t))
    cache_lct_.clear();
  
  ptNodeLogPtr=dictLog.find(t);

  if(ptNodeLogPtr!=NULL)
  {
    lcptr=ptNodeLogPtr->findPtr(pls_state);
    if(lcptr!=NULL)
    {// The phrase s was already associated to the target phrase t
      *lcptr=logCount;
    }
	else
    {// The phrase s was not previously associated to the target phrase t
      ppc.first=pls_state;
      ppc.second=logCount;
      ptNodeLogPtr->push(ppc.first,ppc.second);
    }
  }
  else
  { // There is not any source phrase associated to the target phrase t
    ppc.first=pls_state;
    ppc.second=logCount;
    ptNodeLog.push(ppc.first,ppc.second);
    dictLog.insert(t,ptNodeLog);
  }
}
//-------------------------
void PhraseDictLog::incrSrcTrgLogCount(PhraseCountLogState pls_state,
                                       const std::vector<WordIndex>& t,
                                       LogCount logCount)
{
  PhraseTableNodeLog* ptNodeLogPtr;
  PhraseTableNodeLog ptNodeLog;
  pair<PhraseCountLogState,LogCount> ppc;
  LogCount *lcptr;

  if(cache_lct_.t_present(t))
    cache_lct_.clear();

  ptNodeLogPtr=dictLog.find(t);	

  if(ptNodeLogPtr!=NULL)
  {
    lcptr=ptNodeLogPtr->findPtr(pls_state);
    if(lcptr!=NULL)
    {// The phrase s was already associated to the target phrase t
      *lcptr=MathFuncs::lns_sumlog(*lcptr,logCount);
    }
	else
    {// The phrase s was not previously associated to the target phrase t
      ppc.first=pls_state;
      ppc.second=logCount;
      ptNodeLogPtr->push(ppc.first,ppc.second);
    }
  }
  else
  { // There is not any source phrase associated to the target phrase t
    ppc.first=pls_state;
    ppc.second=logCount;
    ptNodeLog.push(ppc.first,ppc.second);
    dictLog.insert(t,ptNodeLog);
  }
}

//-------------------------
std::map<std::vector<WordIndex>,PhrasePairInfo> PhraseDictLog::getEntriesFor_s(PhraseCountLogState pls_state)
{
  std::map<std::vector<WordIndex>,PhrasePairInfo> map_vecw_phpinfo;
  DictLog::const_iterator phraseTableIter;
  PhraseTableNodeLog::const_iterator ptNodeLogIter;
  pair<std::vector<WordIndex>,PhrasePairInfo> vecw_phpinfo;
	
  for(phraseTableIter=dictLog.begin();phraseTableIter!=dictLog.end();++phraseTableIter)
  {
    for(ptNodeLogIter=phraseTableIter->second.begin();ptNodeLogIter!=phraseTableIter->second.end();++ptNodeLogIter)
    {
      if(ptNodeLogIter->first==pls_state)
      {
        vecw_phpinfo.first=phraseTableIter->first;
        vecw_phpinfo.second.first=(float)pls_state->getData();
        vecw_phpinfo.second.second=(float)ptNodeLogIter->second;
        map_vecw_phpinfo.insert(vecw_phpinfo);
        break;
      }
    }
  }
 
  return map_vecw_phpinfo;
}

//-------------------------
NbestTableNode<PhraseTransTableNodeData> PhraseDictLog::getTranslationsFor_s_(PhraseCountLogState pls_state)
{
  NbestTableNode<PhraseTransTableNodeData> transTableNode; 
  DictLog::const_iterator phraseTableIter;
  PhraseTableNodeLog::const_iterator ptNodeLogIter;
  pair<LgProb,std::vector<WordIndex> > lgProbVecPair;
	
  for(phraseTableIter=dictLog.begin();phraseTableIter!=dictLog.end();++phraseTableIter)
  {
    for(ptNodeLogIter=phraseTableIter->second.begin();ptNodeLogIter!=phraseTableIter->second.end();++ptNodeLogIter)
    {
      if(ptNodeLogIter->first==pls_state)
      {
        lgProbVecPair.first=(float)ptNodeLogIter->second-(float)pls_state->getData();
        lgProbVecPair.second=phraseTableIter->first;	 
        transTableNode.insert(lgProbVecPair);
        break;
      }
    }
  }
 
  return transTableNode;
}

//-------------------------
PhraseTableNodeLog* PhraseDictLog::getTranslationsFor_t_(const std::vector<WordIndex>& t)
{
  return dictLog.find(t);
}
//-------------------------
pair<bool,PhraseTableNodeLog*> PhraseDictLog::nodeForTrgHasOneTransOrMore(const std::vector<WordIndex>& t)
{
  pair<bool,PhraseTableNodeLog*> p_bool_ptnode;
  
  p_bool_ptnode.second=dictLog.find(t);
  if(p_bool_ptnode.second!=NULL)
  { // phrase t is stored in PhraseMap
    if(p_bool_ptnode.second->size()>0)
    {// The node has at least one translation
      p_bool_ptnode.first=true;
      return p_bool_ptnode;
    }
    else
    {
      p_bool_ptnode.first=false;
      return p_bool_ptnode;
    }
  }
  else
  {
    p_bool_ptnode.first=false;
    return p_bool_ptnode;
  }
}

//-------------------------
void PhraseDictLog::addTrgIfNotExist(const std::vector<WordIndex>& t)
{
  PhraseTableNodeLog* ptNodeLogPtr;
  PhraseTableNodeLog ptNodeLog;
  
  ptNodeLogPtr=dictLog.find(t);	
  if(ptNodeLogPtr==NULL)
  { // phrase t is not stored in PhraseMap
    dictLog.insert(t,ptNodeLog);
  }
}
//-------------------------
size_t PhraseDictLog::size(void)
{
 return dictLog.size();	
}

//-------------------------
void PhraseDictLog::clear(void)
{
 dictLog.clear();
 cache_lct_.clear();
}

//-------------------------
PhraseDictLog::const_iterator PhraseDictLog::begin(void)const
{
  PhraseDictLog::const_iterator iter(this,dictLog.begin());
  return iter;
}
//-------------------------
PhraseDictLog::const_iterator PhraseDictLog::end(void)const
{
  PhraseDictLog::const_iterator iter(this,dictLog.end());
  return iter;
}

// const_iterator function definitions
//--------------------------
bool PhraseDictLog::const_iterator::operator++(void)
{
  if(phdPtr!=NULL)
  {
    ++trIter;
    if(trIter==phdPtr->dictLog.end()) return false;
    else
    {
      return true;
    }
  }
  else return false;
}
//--------------------------
bool PhraseDictLog::const_iterator::operator++(int)
{
  return operator++();
}
//--------------------------
int PhraseDictLog::const_iterator::operator==(const const_iterator& right)
{
  return (phdPtr==right.phdPtr && trIter==right.trIter);	
}
//--------------------------
int PhraseDictLog::const_iterator::operator!=(const const_iterator& right)
{
  return !((*this)==right);
}
//--------------------------
const DictLog::const_iterator& PhraseDictLog::const_iterator::operator->(void)const
{
  return trIter;
}

//-------------------------
