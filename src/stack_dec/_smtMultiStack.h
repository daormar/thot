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
/* Module: _smtMultiStack                                           */
/*                                                                  */
/* Prototypes file: _smtMultiStack.h                                */
/*                                                                  */
/* Description: The _smtMultiStack class is a predecessor class for */
/*              implementing a multiple stack to be used            */
/*              in stack decoding.                                  */
/*                                                                  */
/********************************************************************/

/**
 * @file _smtMultiStack.h
 * 
 * @brief The _smtMultiStack class is a predecessor class for
 * implementing a multiple stack to be used in stack decoding.
 */

#ifndef __smtMultiStack_h
#define __smtMultiStack_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <float.h>
#include <utility>
#include <Score.h>
#include <BaseSmtMultiStack.h>
#include <SmtStack.h>

#include <map>
#if __GNUC__>2
#include <ext/hash_map>
using __gnu_cxx::hash_map;
#else
#include <hash_map>
#endif

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- _smtMultiStack template class

/**
 * @brief Predecessor class for implementing a multiple stack to be used
 * in stack decoding.
 */

template<class HYPOTHESIS> 
class _smtMultiStack: public BaseSmtMultiStack<HYPOTHESIS>
{
 public:

  typedef typename HYPOTHESIS::EqClassFunc EqClassFunc;
  typedef typename EqClassFunc::EqClassType EqClassType;
  typedef typename EqClassFunc::EqClassTypeHashF EqClassTypeHashF;  
  typedef hash_map<EqClassType,SmtStack<HYPOTHESIS>,EqClassTypeHashF> MultiContainer;
  typedef map<EqClassType,typename MultiContainer::iterator,less<EqClassType> > SortedStacksMap;

      // constructor
  _smtMultiStack(void);

      // stack size related functions
  void setMaxStackSize(unsigned int _maxStackSize);
  unsigned int getMaxStackSize(void);

      // basic functionality
  HYPOTHESIS top(void);
  HYPOTHESIS last(void);
  void set_bf(bool _breadthFirst);
  bool empty(void);
  size_t size(void);
  void clear(void);

#ifdef THOT_STATS
  unsigned int numDiscardedPushOpsDueToSize(void);
#endif

 protected:
  
  unsigned int maxStackSize;
  MultiContainer multiContainer;
  SortedStacksMap sortedStacksMap;
  bool breadthFirst;
};

//--------------- _smtMultiStack template class function definitions


//---------------------------------------
template<class HYPOTHESIS> 
_smtMultiStack<HYPOTHESIS>::_smtMultiStack(void)
{
  maxStackSize=64;
  breadthFirst=false;
# ifdef THOT_STATS
  this->discardedPushOpsDueToSize=0;
  this->discardedPushOpsDueToRec=0;
# endif

}

//---------------------------------------
template<class HYPOTHESIS> 
void _smtMultiStack<HYPOTHESIS>::setMaxStackSize(unsigned int _maxStackSize)
{
  typename MultiContainer::iterator pos;	

  maxStackSize=_maxStackSize;

      // For each stack stored in the map...
  for(pos=multiContainer.begin();pos!=multiContainer.end();++pos) 
  {
    pos->second.setMaxStackSize(maxStackSize);
  }
}

//---------------------------------------
template<class HYPOTHESIS> 
unsigned int _smtMultiStack<HYPOTHESIS>::getMaxStackSize(void)
{
  return maxStackSize;
}

//---------------------------------------
template<class HYPOTHESIS> 
HYPOTHESIS _smtMultiStack<HYPOTHESIS>::top(void)
{
  HYPOTHESIS result,aux;
  typename SortedStacksMap::iterator sortedStacksMapIter;
  typename MultiContainer::iterator pos;	
  typename MultiContainer::iterator posBest;	
  Score bestScore;

  sortedStacksMapIter=sortedStacksMap.begin();
  pos=sortedStacksMapIter->second;
  posBest=pos;
  result=pos->second.top();
  if(!breadthFirst) 
  {
    bestScore=aux.getScore();
  
        // For each non-empty stack stored in the map (except the first
        // one)...
    for(;sortedStacksMapIter!=sortedStacksMap.end();++sortedStacksMapIter) 
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
  return result;
}

//---------------------------------------
template<class HYPOTHESIS> 
HYPOTHESIS _smtMultiStack<HYPOTHESIS>::last(void)
{
  if(breadthFirst) 
  {
    typename SortedStacksMap::iterator sortedStacksMapIter;
    typename MultiContainer::iterator pos;	

    sortedStacksMapIter=sortedStacksMap.end();
    --sortedStacksMapIter;
    pos=sortedStacksMapIter->second;

    return pos->second.last();
  }
  else
  {
    HYPOTHESIS result,aux;
    typename SortedStacksMap::iterator sortedStacksMapIter;
    typename MultiContainer::iterator pos;	
    typename MultiContainer::iterator posWorst;	
    Score worstScore;

    sortedStacksMapIter=sortedStacksMap.begin();
    pos=sortedStacksMapIter->second;
    posWorst=pos;
    result=pos->second.last();
    worstScore=aux.getScore();
    
        // For each non-empty stack stored in the map (except the first
        // one)...
    for(;sortedStacksMapIter!=sortedStacksMap.end();++sortedStacksMapIter) 
    {
      pos=sortedStacksMapIter->second;
      aux=pos->second.last(); 
      if(aux.getScore()>worstScore)
      {
        result=aux;
        worstScore=aux.getScore();
        posWorst=pos;
      }
    }
    return result;
  }
}

//---------------------------------------
template<class HYPOTHESIS> 
void _smtMultiStack<HYPOTHESIS>::set_bf(bool _breadthFirst)
{
  breadthFirst=_breadthFirst;
}

//---------------------------------------
template<class HYPOTHESIS> 
bool _smtMultiStack<HYPOTHESIS>::empty(void)
{
  if(sortedStacksMap.size()==0) return true;
  else return false;
}

//---------------------------------------
template<class HYPOTHESIS> 
size_t _smtMultiStack<HYPOTHESIS>::size(void)
{
  return sortedStacksMap.size();  
}

//---------------------------------------
template<class HYPOTHESIS> 
void _smtMultiStack<HYPOTHESIS>::clear(void)
{
  multiContainer.clear();
  sortedStacksMap.clear();
# ifdef THOT_STATS
  this->discardedPushOpsDueToSize=0;
  this->discardedPushOpsDueToRec=0;
# endif
}

#endif
