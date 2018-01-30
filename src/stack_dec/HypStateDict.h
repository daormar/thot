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
 
/**
 * @file HypStateDict.h
 *
 * @brief Declares the HypStateDict class. This class implements a state
 * dictionary for being used in stack decoding.
 */

#ifndef _HypStateDict_h
#define _HypStateDict_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "HypStateDictData.h"
#include "ErrorDefs.h"
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- HypStateDict template class

/**
 * @brief The HypStateDict class implements a dictionary of states for
 * being used in stack decoding.
 */

template<class HYPOTHESIS_REC> 
class HypStateDict
{
 public:

  typedef typename HYPOTHESIS_REC::HypState HypState;
  typedef std::map<HypState,HypStateDictData > HypStateDictDataMap;

      // iterator
  class iterator;
  friend class iterator;
  class iterator
  {
   protected:
    HypStateDict<HYPOTHESIS_REC>* hypstatedictPtr;
    typename HypStateDictDataMap::iterator hsddIter;
   public:
    iterator(void){hypstatedictPtr=NULL;}
    iterator(HypStateDict<HYPOTHESIS_REC>* hypstatedict,
             typename HypStateDictDataMap::iterator iter):hypstatedictPtr(hypstatedict)
      {
        hsddIter=iter;
      }  
    bool operator++(void); //prefix
    bool operator++(int);  //postfix
    int operator==(const iterator& right); 
    int operator!=(const iterator& right); 
    typename HypStateDictDataMap::iterator&
      operator->(void);
    std::pair<HypState,HypStateDictData >
      operator*(void)const;
  };
 
      // HypStateDict iterator-related functions
  iterator begin(void);
  iterator end(void);

      // Constructor
  HypStateDict(void);

      // Basic functions
  iterator createDictEntry(const HYPOTHESIS_REC& hyp);
  iterator find(const HypState& hypstate);

      // size() function
  size_t size(void);
      // clear() function
  void clear(void);

 protected:

  HypStateDictDataMap hypStateDictDataMap;
};

//--------------- HypStateDict template class function definitions

template<class HYPOTHESIS_REC> 
HypStateDict<HYPOTHESIS_REC>::HypStateDict(void)
{
}

//---------------------------------------
template<class HYPOTHESIS_REC>
typename HypStateDict<HYPOTHESIS_REC>::iterator
HypStateDict<HYPOTHESIS_REC>::createDictEntry(const HYPOTHESIS_REC& hyp)
{
  HypState hypState=hyp.getHypState();
    
  typename HypStateDictDataMap::iterator hsddIter=hypStateDictDataMap.find(hypState);
  if(hsddIter==hypStateDictDataMap.end())
  {
        // HypState not present in the dictionary, create index and set
        // score
    HypStateDictData hypStateDictData;
    hypStateDictData.hypStateIndex=hypStateDictDataMap.size();
    hypStateDictData.coverage=hyp.getKey();
    hypStateDictData.score=hyp.getScore();
    
    hsddIter=hypStateDictDataMap.insert(std::make_pair(hypState,hypStateDictData)).first;
  }
  else
  {
        // Hypstate present in the dictionary, update score
    hsddIter->second.score=hyp.getScore();
  }

      // Return iterator
  typename HypStateDict<HYPOTHESIS_REC>::iterator ret(this,hsddIter);
  return ret;

}

//---------------------------------------
template<class HYPOTHESIS_REC> 
typename HypStateDict<HYPOTHESIS_REC>::iterator
HypStateDict<HYPOTHESIS_REC>::find(const HypState& hypstate)
{
  typename HypStateDictDataMap::iterator hsddIter;
  
  hsddIter=hypStateDictDataMap.find(hypstate);
  typename HypStateDict<HYPOTHESIS_REC>::iterator ret(this,hsddIter);
  return ret;
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
size_t HypStateDict<HYPOTHESIS_REC>::size(void)
{
  return hypStateDictDataMap.size();  
}

//---------------------------------------
template<class HYPOTHESIS_REC> 
void HypStateDict<HYPOTHESIS_REC>::clear(void)
{
  hypStateDictDataMap.clear();
}

//--------------------------
template<class HYPOTHESIS_REC>
typename HypStateDict<HYPOTHESIS_REC>::iterator HypStateDict<HYPOTHESIS_REC>::begin(void)
{
 typename HypStateDict<HYPOTHESIS_REC>::iterator iter(this,hypStateDictDataMap.begin());
	
 return iter;
}
//--------------------------
template<class HYPOTHESIS_REC>
typename HypStateDict<HYPOTHESIS_REC>::iterator HypStateDict<HYPOTHESIS_REC>::end(void)
{
 typename HypStateDict<HYPOTHESIS_REC>::iterator iter(this,hypStateDictDataMap.end());
	
 return iter;
}

// Iterator function definitions
//--------------------------
template<class HYPOTHESIS_REC>
bool HypStateDict<HYPOTHESIS_REC>::iterator::operator++(void) //prefix
{
 if(hypstatedictPtr!=NULL)
 {
  ++hsddIter;
  if(hsddIter==hypstatedictPtr->hypStateDictDataMap.end()) return false;
  else return true;	 
 }
 else return false;
}
//--------------------------
template<class HYPOTHESIS_REC>
bool HypStateDict<HYPOTHESIS_REC>::iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class HYPOTHESIS_REC>
int HypStateDict<HYPOTHESIS_REC>::iterator::operator==(const iterator& right)
{
 return (hypstatedictPtr==right.hypstatedictPtr && hsddIter==right.hsddIter);	
}
//--------------------------
template<class HYPOTHESIS_REC>
int HypStateDict<HYPOTHESIS_REC>::iterator::operator!=(const iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class HYPOTHESIS_REC>
typename HypStateDict<HYPOTHESIS_REC>::HypStateDictDataMap::iterator&
HypStateDict<HYPOTHESIS_REC>::iterator::operator->(void)
{
  return hsddIter;
}

//--------------------------
template<class HYPOTHESIS_REC>
std::pair<typename HypStateDict<HYPOTHESIS_REC>::HypState,HypStateDictData >
HypStateDict<HYPOTHESIS_REC>::iterator::operator*(void)const
{
   return *hsddIter;
}

#endif
