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
/* Module: PhraseDict                                               */
/*                                                                  */
/* Definitions file: PhraseDict.cc                                  */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhraseDict.h"

//--------------- Function definitions

//-------------------------
PhraseDict::PhraseDict(void)
{
}

//-------------------------
Count PhraseDict::getCounts_t_(PhraseCountState ps_state,
                               const std::vector<WordIndex>& t,
                               bool& found)
{
  PhraseTableNode* phraseTableNodePtr;
  
  phraseTableNodePtr=dict.find(t);	
  if(phraseTableNodePtr!=NULL)
  {
    Count *cptr=phraseTableNodePtr->findPtr(ps_state);
    if(cptr==NULL)
    {
      found=false;
      return 0;
    }
    else
    {
      found=true;
      return *cptr;
    }
  }
  else
  {
    found=false;
    return 0;
  }
}
//-------------------------
Count PhraseDict::getCount_t(const std::vector<WordIndex>& t)
{
  if(cache_ct_.t_present(t))
  {
    return cache_ct_.ct_();
  }
  else
  {
    PhraseTableNode* phraseTableNodePtr;
    PhraseTableNode::iterator phraseTableNodeIter;	
    Count count_t_;	
  
    phraseTableNodePtr=dict.find(t);	

    if(phraseTableNodePtr!=NULL)
    { // phrase t is stored in PhraseMap
      count_t_=0;	    
	  
          // obtain count_t_
      for(phraseTableNodeIter=phraseTableNodePtr->begin();phraseTableNodeIter!=phraseTableNodePtr->end();++phraseTableNodeIter)
      {
        count_t_+=phraseTableNodeIter->second.get_c_st();
      }
    }
    else count_t_=0;   // phrase t is not stored in PhraseMap

    cache_ct_.init(t,count_t_);
    
    return count_t_;
  }
}
//-------------------------
void PhraseDict::addSrcTrgPair(PhraseCountState ps_state,
                               const std::vector<WordIndex>& t,
                               Count count)
{
  PhraseTableNode* phraseTableNodePtr;
  PhraseTableNode phraseTableNode;
  pair<PhraseCountState,Count> ppc;
  Count *cptr;
  
  if(cache_ct_.t_present(t))
    cache_ct_.clear();
  
  phraseTableNodePtr=dict.find(t);

  if(phraseTableNodePtr!=NULL)
  {
    cptr=phraseTableNodePtr->findPtr(ps_state);
    if(cptr!=NULL)
    {// The phrase s was already associated to the target phrase t
      *cptr=count;
    }
	else
    {// The phrase s was not previously associated to the target phrase t
      ppc.first=ps_state;
      ppc.second=count;
      phraseTableNodePtr->push(ppc.first,ppc.second);
    }
  }
  else
  { // There is not any source phrase associated to the target phrase t
    ppc.first=ps_state;
    ppc.second=count;
    phraseTableNode.push(ppc.first,ppc.second);
    dict.insert(t,phraseTableNode);
  }
}
//-------------------------
void PhraseDict::incrSrcTrgCount(PhraseCountState ps_state,
                                 const std::vector<WordIndex>& t,
                                 Count count)
{
  PhraseTableNode* phraseTableNodePtr;
  PhraseTableNode phraseTableNode;
  pair<PhraseCountState,Count> ppc;
  Count *cptr;

  if(cache_ct_.t_present(t))
    cache_ct_.clear();

  phraseTableNodePtr=dict.find(t);	

  if(phraseTableNodePtr!=NULL)
  {
    cptr=phraseTableNodePtr->findPtr(ps_state);
    if(cptr!=NULL)
    {// The phrase s was already associated to the target phrase t
      cptr->incr_count(count);
    }
	else
    {// The phrase s was not previously associated to the target phrase t
      ppc.first=ps_state;
      ppc.second=count;
      phraseTableNodePtr->push(ppc.first,ppc.second);
    }
  }
  else
  { // There is not any source phrase associated to the target phrase t
    ppc.first=ps_state;
    ppc.second=count;
    phraseTableNode.push(ppc.first,ppc.second);
    dict.insert(t,phraseTableNode);
  }
}

//-------------------------
std::map<std::vector<WordIndex>,PhrasePairInfo> PhraseDict::getEntriesFor_s(PhraseCountState ps_state)
{
  std::map<std::vector<WordIndex>,PhrasePairInfo> map_vecw_phpinfo;
  Dict::const_iterator phraseTableIter;
  PhraseTableNode::const_iterator phraseTableNodeIter;
  pair<std::vector<WordIndex>,PhrasePairInfo> vecw_phpinfo;
	
  for(phraseTableIter=dict.begin();phraseTableIter!=dict.end();++phraseTableIter)
  {
    for(phraseTableNodeIter=phraseTableIter->second.begin();phraseTableNodeIter!=phraseTableIter->second.end();++phraseTableNodeIter)
    {
      if(phraseTableNodeIter->first==ps_state)
      {
        vecw_phpinfo.first=phraseTableIter->first;
        vecw_phpinfo.second.first=ps_state->getData();
        vecw_phpinfo.second.second=phraseTableNodeIter->second;
        map_vecw_phpinfo.insert(vecw_phpinfo);
        break;
      }
    }
  }
 
  return map_vecw_phpinfo;
}

//-------------------------
NbestTableNode<PhraseTransTableNodeData> PhraseDict::getTranslationsFor_s_(PhraseCountState ps_state)
{
  NbestTableNode<PhraseTransTableNodeData> transTableNode; 
  Dict::const_iterator phraseTableIter;
  PhraseTableNode::const_iterator phraseTableNodeIter;
  pair<LgProb,std::vector<WordIndex> > lgProbVecPair;
	
  for(phraseTableIter=dict.begin();phraseTableIter!=dict.end();++phraseTableIter)
  {
    for(phraseTableNodeIter=phraseTableIter->second.begin();phraseTableNodeIter!=phraseTableIter->second.end();++phraseTableNodeIter)
    {
      if(phraseTableNodeIter->first==ps_state)
      {
        lgProbVecPair.first=log((float)phraseTableNodeIter->second.get_c_st()/(float)ps_state->getData());
        lgProbVecPair.second=phraseTableIter->first;	 
        transTableNode.insert(lgProbVecPair);
        break;
      }
    }
  }
 
  return transTableNode;
}

//-------------------------
PhraseTableNode* PhraseDict::getTranslationsFor_t_(const std::vector<WordIndex>& t)
{
  return dict.find(t);
}
//-------------------------
pair<bool,PhraseTableNode*> PhraseDict::nodeForTrgHasOneTransOrMore(const std::vector<WordIndex>& t)
{
  pair<bool,PhraseTableNode*> p_bool_ptnode;
  
  p_bool_ptnode.second=dict.find(t);
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
void PhraseDict::addTrgIfNotExist(const std::vector<WordIndex>& t)
{
  PhraseTableNode* phraseTableNodePtr;
  PhraseTableNode phraseTableNode;
  
  phraseTableNodePtr=dict.find(t);	
  if(phraseTableNodePtr==NULL)
  { // phrase t is not stored in PhraseMap
    dict.insert(t,phraseTableNode);
  }
}
//-------------------------
size_t PhraseDict::size(void)
{
 return dict.size();	
}

//-------------------------
void PhraseDict::clear(void)
{
 dict.clear();
 cache_ct_.clear();
}

//-------------------------
PhraseDict::const_iterator PhraseDict::begin(void)const
{
  PhraseDict::const_iterator iter(this,dict.begin());
  return iter;
}
//-------------------------
PhraseDict::const_iterator PhraseDict::end(void)const
{
  PhraseDict::const_iterator iter(this,dict.end());
  return iter;
}

// const_iterator function definitions
//--------------------------
bool PhraseDict::const_iterator::operator++(void)
{
  if(phdPtr!=NULL)
  {
    ++trIter;
    if(trIter==phdPtr->dict.end()) return false;
    else
    {
      return true;
    }
  }
  else return false;
}
//--------------------------
bool PhraseDict::const_iterator::operator++(int)
{
  return operator++();
}
//--------------------------
int PhraseDict::const_iterator::operator==(const const_iterator& right)
{
  return (phdPtr==right.phdPtr && trIter==right.trIter);	
}
//--------------------------
int PhraseDict::const_iterator::operator!=(const const_iterator& right)
{
  return !((*this)==right);
}
//--------------------------
const Dict::const_iterator&
PhraseDict::const_iterator::operator->(void)const
{
  return trIter;
}

//-------------------------
