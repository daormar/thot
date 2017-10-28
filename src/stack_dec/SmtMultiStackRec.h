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
/* Module: SmtMultiStackRec                                         */
/*                                                                  */
/* Prototypes file: SmtMultiStackRec.h                              */
/*                                                                  */
/* Description: The SmtMultiStackRec class implements a multiple    */
/*              stack with hypothesis recombination to              */
/*              be used in stack decoding.                          */
/*                                                                  */
/********************************************************************/

/**
 * @file SmtMultiStackRec.h
 * 
 * @brief The SmtMultiStackRec class implements a multiple stack with
 * hypothesis recombination to be used in stack decoding.
 */

#ifndef _SmtMultiStackRec_h
#define _SmtMultiStackRec_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <_smtMultiStack.h>
#include "HypStateDict.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- SmtMultiStackRec template class

/**
 * @brief Multiple stack with hypothesis recombination for statistical
 * machine translation.
 */

template<class HYPOTHESIS_REC> 
class SmtMultiStackRec: public _smtMultiStack<HYPOTHESIS_REC>
{
 public:

  typedef typename _smtMultiStack<HYPOTHESIS_REC>::EqClassFunc EqClassFunc;
  typedef typename _smtMultiStack<HYPOTHESIS_REC>::EqClassType EqClassType;
  typedef typename _smtMultiStack<HYPOTHESIS_REC>::EqClassTypeHashF EqClassTypeHashF;
  typedef typename _smtMultiStack<HYPOTHESIS_REC>::MultiContainer MultiContainer;
  typedef typename _smtMultiStack<HYPOTHESIS_REC>::SortedStacksMap SortedStacksMap;
  typedef std::map<HypStateIndex,typename SmtStack<HYPOTHESIS_REC>::iterator> RecInfoMap;

      // iterator
  class iterator;
  friend class iterator;
  class iterator
  {
   protected:
    SmtMultiStackRec<HYPOTHESIS_REC>* smtmstackrecPtr;
    typename MultiContainer::iterator mcIter;
   public:
    iterator(void){smtmstackrecPtr=NULL;}
    iterator(SmtMultiStackRec<HYPOTHESIS_REC>* smtmstackrec,
             typename MultiContainer::iterator iter):smtmstackrecPtr(smtmstackrec)
      {
        mcIter=iter;
      }  
    bool operator++(void); //prefix
    bool operator++(int);  //postfix
    int operator==(const iterator& right); 
    int operator!=(const iterator& right); 
    typename MultiContainer::iterator&
      operator->(void);
    std::pair<EqClassType,SmtStack<HYPOTHESIS_REC> >
      operator*(void)const;

    friend void SmtMultiStackRec<HYPOTHESIS_REC>::remove(SmtMultiStackRec<HYPOTHESIS_REC>::iterator iter);
  };
 
      // iterator-related functions
  iterator begin(void);
  iterator end(void);

      // basic functionality
  typename SmtMultiStackRec<HYPOTHESIS_REC>::iterator pushIter(const HYPOTHESIS_REC& hyp);
  bool push(const HYPOTHESIS_REC& hyp);
  HYPOTHESIS_REC pop(void);
  void remove(SmtMultiStackRec<HYPOTHESIS_REC>::iterator iter);
  void removeLast(void);
  void clear(void);

      // set pointer to HypStateDict object
  void setHypStateDictPtr(HypStateDict<HYPOTHESIS_REC>* _hypStateDictPtr);

 protected:

  HypStateDict<HYPOTHESIS_REC>* hypStateDictPtr;
  RecInfoMap recInfoMap;

      // auxiliary functions
  bool pushOnSmtStack(typename MultiContainer::iterator pos,
                      const HYPOTHESIS_REC& hyp);
  bool pushOnSmtStackIdx(typename MultiContainer::iterator pos,
                         const HYPOTHESIS_REC& hyp,
                         HypStateIndex hypStateIndex);
  void eraseRecInfo(const typename HYPOTHESIS_REC::HypState& hypState);
};

//--------------- SmtMultiStackRec template class function definitions

//---------------------------------------
template<class HYPOTHESIS_REC> 
typename SmtMultiStackRec<HYPOTHESIS_REC>::iterator
SmtMultiStackRec<HYPOTHESIS_REC>::pushIter(const HYPOTHESIS_REC& hyp)
{
  EqClassType key;
  typename MultiContainer::iterator pos;	
  size_t prev_size;
  
  key=hyp.getEqClass();
  pos=this->multiContainer.find(key);
  if(pos==this->multiContainer.end())
  {
        // key not found, create new sub-stack
    SmtStack<HYPOTHESIS_REC> smtStack;
    smtStack.setMaxStackSize(this->maxStackSize);
    pos=this->multiContainer.insert(std::make_pair(key,smtStack)).first;
    this->sortedStacksMap.insert(std::make_pair(key,pos));
  }
  prev_size=pos->second.size();
    
  if(pushOnSmtStack(pos,hyp))
  {
    if(prev_size==0)
      this->sortedStacksMap.insert(std::make_pair(key,pos));
    typename SmtMultiStackRec<HYPOTHESIS_REC>::iterator ret(this,pos);
    return ret;
  }
  else
  {
        // The stack pointed by pos may be empty if its maximum stack
        // size is zero. The stack may also be empty due to hypotheses
        // recombination
    if(pos->second.size()==0)
      this->sortedStacksMap.erase(pos->first);
    return end();
  }
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
bool SmtMultiStackRec<HYPOTHESIS_REC>::push(const HYPOTHESIS_REC& hyp)
{
  iterator smtmsiter;

  smtmsiter=pushIter(hyp);
  if(smtmsiter==end()) return false;
  else return true;
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
HYPOTHESIS_REC SmtMultiStackRec<HYPOTHESIS_REC>::pop(void)
{
  HYPOTHESIS_REC result,aux;
  typename SortedStacksMap::iterator sortedStacksMapIter;
  typename MultiContainer::iterator pos;	
  typename MultiContainer::iterator posBest;	
  Score bestScore;

  sortedStacksMapIter=this->sortedStacksMap.begin();
  pos=sortedStacksMapIter->second;
  posBest=pos;
  result=pos->second.top();
  bestScore=result.getScore();

  if(!this->breadthFirst) 
  {
        // For each non-empty stack stored in the map (except the first
        // one)...
    for(;sortedStacksMapIter!=this->sortedStacksMap.end();++sortedStacksMapIter) 
    {
      pos=sortedStacksMapIter->second;
      aux=pos->second.top(); 
      if(aux.getScore()>bestScore)
      {
        result=aux;
        bestScore=aux.getScore();
        posBest=pos;
      }
    }
  }
      // erase recombination info
  eraseRecInfo(posBest->second.top().getHypState());
      // pop hypothesis
  posBest->second.pop();
  if(posBest->second.size()==0)
    this->sortedStacksMap.erase(posBest->first);

  return result;       
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
void SmtMultiStackRec<HYPOTHESIS_REC>::remove(SmtMultiStackRec<HYPOTHESIS_REC>::iterator iter)
{
  if(iter.smtmstackrecPtr==this)
  {
        // erase recombination info
    eraseRecInfo((*iter.mcIter).getHypState());
        // remove hypothesis
    iter.mcIter->second.remove(iter.mcIter);
    if(iter.mcIter->second.size()==0)
      this->sortedStacksMap.erase(iter.mcIter->first);
  }
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
void SmtMultiStackRec<HYPOTHESIS_REC>::removeLast(void)
{
  if(this->breadthFirst) 
  {
        // breadth-first mode
    typename SortedStacksMap::iterator sortedStacksMapIter;
    typename MultiContainer::iterator pos;	
    typename MultiContainer::iterator posWorst;	

    sortedStacksMapIter=this->sortedStacksMap.end();
    --sortedStacksMapIter;
    pos=sortedStacksMapIter->second;
    posWorst=pos;
    eraseRecInfo(posWorst->second.last().getHypState());
    posWorst->second.removeLast();
    if(posWorst->second.size()==0)
      this->sortedStacksMap.erase(posWorst->first);
  }
  else
  {
        // best-first mode
    typename SortedStacksMap::iterator sortedStacksMapIter;
    typename MultiContainer::iterator pos;	
    typename MultiContainer::iterator posWorst;	
    Score worstScore;
  
    sortedStacksMapIter=this->sortedStacksMap.begin();
    pos=sortedStacksMapIter->second;
    posWorst=pos;
    worstScore=pos->second.last().getScore();
    
        // For each non-empty stack stored in the map (except the first
        // one)...
    for(;sortedStacksMapIter!=this->sortedStacksMap.end();++sortedStacksMapIter) 
    {
      if(pos->second.last().getScore()>worstScore)
      {
        worstScore=pos->second.last().getScore();
        posWorst=pos;
      }
    }
        // erase recombination info
    eraseRecInfo(posWorst->second.last().getHypState());
        // remove last hypothesis
    posWorst->second.removeLast();
    if(posWorst->second.size()==0)
      this->sortedStacksMap.erase(posWorst->first);
  }
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
void SmtMultiStackRec<HYPOTHESIS_REC>::setHypStateDictPtr(HypStateDict<HYPOTHESIS_REC>* _hypStateDictPtr)
{
  hypStateDictPtr=_hypStateDictPtr;
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
void SmtMultiStackRec<HYPOTHESIS_REC>::clear(void)
{
  _smtMultiStack<HYPOTHESIS_REC>::clear();
  recInfoMap.clear();
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
bool SmtMultiStackRec<HYPOTHESIS_REC>::pushOnSmtStack(typename MultiContainer::iterator pos,
                                                      const HYPOTHESIS_REC& hyp)
{
  typename HypStateDict<HYPOTHESIS_REC>::iterator hypStateDictIter;
  typename HYPOTHESIS_REC::HypState hypState;

      // find hypothesis state in the hypothesis state dictionary
  hypState=hyp.getHypState();
  hypStateDictIter=hypStateDictPtr->find(hypState);
  if(hypStateDictIter!=hypStateDictPtr->end() &&
     hypStateDictIter->second.score>hyp.getScore())
  {
        // hypothesis state exists with higher score than the score of
        // hyp
#  ifdef THOT_STATS
    ++this->discardedPushOpsDueToRec;
#  endif

    return false;
  }
  else
  {
    HypStateIndex hypStateIndex;
    
    if(hypStateDictIter==hypStateDictPtr->end())
    {
          // create entry in hypothesis state dictionary
      hypStateDictIter=hypStateDictPtr->createDictEntry(hyp);
    }
    else
    {
          // update score in the hypothesis state dictionary
      hypStateDictIter->second.score=hyp.getScore();
    }
        // retrieve hypStateIndex
    hypStateIndex=hypStateDictIter->second.hypStateIndex;

        // push given hyp and its index
    return pushOnSmtStackIdx(pos,hyp,hypStateIndex);
  }
}

//---------------------------------------
template<class HYPOTHESIS_REC>
bool SmtMultiStackRec<HYPOTHESIS_REC>::pushOnSmtStackIdx(typename MultiContainer::iterator pos,
                                                         const HYPOTHESIS_REC& hyp,
                                                         HypStateIndex hypStateIndex)
{
  typename RecInfoMap::iterator recInfoMapIter;
  typename SmtStack<HYPOTHESIS_REC>::iterator smtStackIter;

      // retrieve pointer to hypothesis in recInfoMap
  recInfoMapIter=recInfoMap.find(hypStateIndex);
  if(recInfoMapIter!=recInfoMap.end())
  {
        // remove hypothesis with lower score (recInfoMap entry is
        // also removed)
    pos->second.remove(recInfoMapIter->second);
    recInfoMapIter=recInfoMap.end();
  }

      // Keep last hypothesis of the container, and the size of the
      // container before the insertion
  HYPOTHESIS_REC lastHyp;
  if(!pos->second.empty()) lastHyp=pos->second.last();
  size_t prev_stack_size=pos->second.size();

      // insert hypothesis into the stack
  smtStackIter=pos->second.pushIter(hyp);
  if(smtStackIter!=pos->second.end())
  {
        // If hyp was inserted, update pointer to hypothesis in
        // recInfoMap
    if(recInfoMapIter!=recInfoMap.end())
    {
      recInfoMapIter->second=smtStackIter;
    }
    else
    {
      recInfoMap[hypStateIndex]=smtStackIter;
    }
        // If stack was pruned due to its size, delete the
        // corresponding entry in recInfoMap
    if(prev_stack_size==pos->second.size())
    {
#     ifdef THOT_STATS
        ++this->discardedPushOpsDueToSize;
#     endif
      HypStateIndex hypStateIndex=hypStateDictPtr->find(lastHyp.getHypState())->second.hypStateIndex;

      recInfoMap.erase(hypStateIndex);
    }
    return true;
  }
  else
  {
        // hyp was not inserted because the maximum stack size has been
        // reached
#   ifdef THOT_STATS
      ++this->discardedPushOpsDueToSize;
#   endif
    return false;
  }
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
void SmtMultiStackRec<HYPOTHESIS_REC>::eraseRecInfo(const typename HYPOTHESIS_REC::HypState& hypState)
{
  HypStateIndex hypStateIndex;
  hypStateIndex=hypStateDictPtr->find(hypState)->second.hypStateIndex;
  recInfoMap.erase(hypStateIndex);
}

//--------------------------
template<class HYPOTHESIS_REC>
typename SmtMultiStackRec<HYPOTHESIS_REC>::iterator
SmtMultiStackRec<HYPOTHESIS_REC>::begin(void)
{
 typename SmtMultiStackRec<HYPOTHESIS_REC>::iterator iter(this,this->multiContainer.begin());
	
 return iter;
}
//--------------------------
template<class HYPOTHESIS_REC>
typename SmtMultiStackRec<HYPOTHESIS_REC>::iterator
SmtMultiStackRec<HYPOTHESIS_REC>::end(void)
{
 typename SmtMultiStackRec<HYPOTHESIS_REC>::iterator iter(this,this->multiContainer.end());
	
 return iter;
}

// Iterator function definitions
//--------------------------
template<class HYPOTHESIS_REC>
bool SmtMultiStackRec<HYPOTHESIS_REC>::iterator::operator++(void) //prefix
{
 if(smtmstackrecPtr!=NULL)
 {
   ++mcIter;
   if(mcIter==smtmstackrecPtr->multiContainer.end()) return false;
   else
   {
     return true;
   }
 }
 else return false;
}
//--------------------------
template<class HYPOTHESIS_REC>
bool SmtMultiStackRec<HYPOTHESIS_REC>::iterator::operator++(int)  //postfix
{
 return operator++();
}

//--------------------------
template<class HYPOTHESIS_REC>
int SmtMultiStackRec<HYPOTHESIS_REC>::iterator::operator==(const iterator& right)
{
  if(smtmstackrecPtr==right.smtmstackrecPtr && mcIter==right.mcIter) return true;
  else return false;
}

//--------------------------
template<class HYPOTHESIS_REC>
int SmtMultiStackRec<HYPOTHESIS_REC>::iterator::operator!=(const iterator& right)
{
 return !((*this)==right);	
}

//--------------------------
template<class HYPOTHESIS_REC>
typename SmtMultiStackRec<HYPOTHESIS_REC>::MultiContainer::iterator&
SmtMultiStackRec<HYPOTHESIS_REC>::iterator::operator->(void)
{
  return mcIter;
}

//--------------------------
template<class HYPOTHESIS_REC>
std::pair<typename SmtMultiStackRec<HYPOTHESIS_REC>::EqClassType,SmtStack<HYPOTHESIS_REC> >
SmtMultiStackRec<HYPOTHESIS_REC>::iterator::operator*(void)const
{
   return *mcIter;
}

#endif
