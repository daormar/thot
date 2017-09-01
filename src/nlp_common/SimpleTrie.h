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
/* Module: SimpleTrie                                               */
/*                                                                  */
/* Prototype file: SimpleTrie.h                                     */
/*                                                                  */
/* Description: Implements a simple trie data structure             */
/*                                                                  */
/********************************************************************/

#ifndef _SimpleTrie_h
#define _SimpleTrie_h

//--------------- Include files --------------------------------------

#include "_simpleTrie.h"

//--------------- Constants ------------------------------------------


//--------------- User defined types ---------------------------------


//--------------- Classes --------------------------------------------

//--------------- SimpleTrie class

template<class DATA_TYPE> 
class SimpleTrie
{
 public:

      // Constructor
  SimpleTrie(void);

  // Basic functions
  _simpleTrie<DATA_TYPE>* insert(const std::vector<DATA_TYPE>& dataSeq);
      // Inserts a sequence of elements of class key. The last element 
      // of vector keySeq is the first element of the sequence.
  bool erase(const std::vector<DATA_TYPE>& dataSeq);
  bool find(const std::vector<DATA_TYPE>& dataSeq);
  _simpleTrie<DATA_TYPE>* getState(const std::vector<DATA_TYPE>& dataSeq);
  DATA_TYPE& getData(void);
 
  size_t size(void)const;
  unsigned int height(void)const;
  std::vector<unsigned long> branchingFactor(void)const;
  size_t countSparseNodes(void)const;
  void clear(void);
  ~SimpleTrie();
  
      // const_iterator
  class const_iterator;
  friend class const_iterator;
  class const_iterator
    {
      protected:
       const SimpleTrie<DATA_TYPE>* strPtr;
       typename _simpleTrie<DATA_TYPE>::const_iterator _strIter;
            
      public:
       const_iterator(void):strPtr(NULL)
         {
         }
       const_iterator(const SimpleTrie<DATA_TYPE>* _strPtr,
                typename _simpleTrie<DATA_TYPE>::const_iterator __strIter):strPtr(_strPtr)
         {
           _strIter=__strIter;
         }
       bool operator++(void); //prefix
       bool operator++(int);  //postfix
       int operator==(const const_iterator& right);
       int operator!=(const const_iterator& right);
       const typename _simpleTrie<DATA_TYPE>::const_iterator& operator->(void)const;
       std::vector<DATA_TYPE> operator*(void)const;
    };
  
      // const_iterator functions for the trie class
  const_iterator begin(void)const;
  const_iterator end(void)const;

 protected:
  _simpleTrie<DATA_TYPE> _strie;
  size_t trieSize;
          
};

//--------------- Template method definitions


//--------------- SimpleTrie class method definitions

template<class DATA_TYPE>
SimpleTrie<DATA_TYPE>::SimpleTrie(void)
{
  trieSize=0;
}

//---------------
template<class DATA_TYPE>
_simpleTrie<DATA_TYPE>* SimpleTrie<DATA_TYPE>::insert(const std::vector<DATA_TYPE>& dataSeq)
{
  _simpleTrie<DATA_TYPE>* _stptr;

  _stptr=_strie.getState(dataSeq);
  if(_stptr!=NULL) return _stptr;
  else
  {
    ++trieSize;
    return _strie.insert(dataSeq);
  }
}

//---------------
template<class DATA_TYPE>
bool SimpleTrie<DATA_TYPE>::erase(const std::vector<DATA_TYPE>& dataSeq)
{
  bool ret;

  ret=_strie.erase(dataSeq);
  if(ret)
  {
    trieSize=_strie.size();
  }
  return ret;
}

//---------------
template<class DATA_TYPE>
bool SimpleTrie<DATA_TYPE>::find(const std::vector<DATA_TYPE>& dataSeq)
{
  return _strie.erase(dataSeq);
}
//---------------
template<class DATA_TYPE>
_simpleTrie<DATA_TYPE>* SimpleTrie<DATA_TYPE>::getState(const std::vector<DATA_TYPE>& dataSeq)
{
  return _strie.getState(dataSeq);
}

//---------------
template<class DATA_TYPE>
DATA_TYPE& SimpleTrie<DATA_TYPE>::getData(void)
{
  return _strie.getData();
}

//---------------
template<class DATA_TYPE>
size_t SimpleTrie<DATA_TYPE>::size(void)const
{
  return trieSize;
}

//---------------
template<class DATA_TYPE>
unsigned int SimpleTrie<DATA_TYPE>::height(void)const
{
  return _strie.height();
}

//---------------
template<class DATA_TYPE>
std::vector<unsigned long> SimpleTrie<DATA_TYPE>::branchingFactor(void)const
{
  return _strie.branchingFactor();
}

//---------------
template<class DATA_TYPE>
size_t SimpleTrie<DATA_TYPE>::countSparseNodes(void)const
{
  return _strie.countSparseNodes();
}

//---------------
template<class DATA_TYPE>
void SimpleTrie<DATA_TYPE>::clear(void)
{
  trieSize=0;
  _strie.clear();
}

//---------------
template<class DATA_TYPE>
SimpleTrie<DATA_TYPE>::~SimpleTrie()
{
  this->clear();
}
//---------------

// const_iterator functions for the SimpleTrie class
//---------------
template<class DATA_TYPE>
typename SimpleTrie<DATA_TYPE>::const_iterator SimpleTrie<DATA_TYPE>::begin(void)const
{
  typename SimpleTrie<DATA_TYPE>::const_iterator iter(this,_strie.begin());
  return iter;
}

//---------------
template<class DATA_TYPE>
typename SimpleTrie<DATA_TYPE>::const_iterator SimpleTrie<DATA_TYPE>::end(void)const
{
  typename SimpleTrie<DATA_TYPE>::const_iterator iter(this,_strie.end());
  return iter;
}

// const_iterator function definitions
//--------------------------
template<class DATA_TYPE>
bool SimpleTrie<DATA_TYPE>::const_iterator::operator++(void) //prefix
{
  if(strPtr!=NULL)
  {
    ++_strIter;
    if(_strIter==strPtr->_strie.end()) return false;
    else
    {
      return true;
    }
  }
  else return false;
}

//--------------------------
template<class DATA_TYPE>
bool SimpleTrie<DATA_TYPE>::const_iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class DATA_TYPE>
int SimpleTrie<DATA_TYPE>::const_iterator::operator==(const const_iterator& right)
{
 return (strPtr==right.strPtr && _strIter==right._strIter);	
}
//--------------------------
template<class DATA_TYPE>
int SimpleTrie<DATA_TYPE>::const_iterator::operator!=(const const_iterator& right)
{
 return !((*this)==right);
}
//--------------------------
template<class DATA_TYPE>
const typename _simpleTrie<DATA_TYPE>::const_iterator&
SimpleTrie<DATA_TYPE>::const_iterator::operator->(void)const
{
  return (const typename _simpleTrie<DATA_TYPE>::const_iterator&) _strIter;
}
//--------------------------
template<class DATA_TYPE>
std::vector<DATA_TYPE> SimpleTrie<DATA_TYPE>::const_iterator::operator*(void)const
{
  return *_strIter;
}
//--------------------------

#endif
