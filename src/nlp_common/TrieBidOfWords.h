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
/* Module: TrieBidOfWords                                           */
/*                                                                  */
/* Prototype file: TrieBidOfWords.h                                 */
/*                                                                  */
/* Description: Specialized bidirectional trie for being used in    */
/*              language technologies.                              */
/*                                                                  */
/********************************************************************/

#ifndef _TrieBidOfWords_h
#define _TrieBidOfWords_h

//--------------- Include files --------------------------------------

#include "TrieBid.h"
#include "WordIndex.h"

//--------------- Constants ------------------------------------------

#define MAX_NUM_OF_WORDS 262144

//--------------- User defined types ---------------------------------


//--------------- Classes --------------------------------------------

//--------------- Trie class

template<class DATA_TYPE>
class TrieBidOfWords 
{
 public:

      // Constructor
  TrieBidOfWords(void);
      // Copy constructor
  TrieBidOfWords(const TrieBidOfWords<DATA_TYPE>& tbw);
      // Assignment operator
  TrieBidOfWords<DATA_TYPE>& operator=(const TrieBidOfWords<DATA_TYPE>& tbw);

  // Basic functions
  TrieBid<WordIndex,DATA_TYPE>* insert(const Vector<WordIndex>& keySeq,
                                       const DATA_TYPE& d);
      // Inserts a sequence of elements of class key. The last element 
      // of vector keySeq is the first element of the sequence.
  bool erase(const Vector<WordIndex>& keySeq);
  DATA_TYPE* find(const Vector<WordIndex>& keySeq);
  TrieBid<WordIndex,DATA_TYPE>* getState(const Vector<WordIndex>& keySeq);
	
  size_t size(void)const;
  unsigned int height(void)const;
  Vector<unsigned long> branchingFactor(void)const;
  size_t countSparseNodes(void)const;
  void clear(void);
  ~TrieBidOfWords();

      // const_iterator
  class const_iterator;
  friend class const_iterator;
  class const_iterator
    {
      protected:
       unsigned int i;
       const TrieBidOfWords* tptr;
       typename TrieBid<WordIndex,DATA_TYPE>::const_iterator trieIter;
       
      public:
       const_iterator(void):tptr(NULL)
         {
           i=0;
         }
       const_iterator(const TrieBidOfWords* trieptr):tptr(trieptr)
         {
           i=0;
           if(tptr!=NULL)
           {
             while(i<tptr->vecTriePtr.size() && tptr->vecTriePtr[i]==NULL) ++i;
             if(i<tptr->vecTriePtr.size())
             {
               trieIter=tptr->vecTriePtr[i]->begin();
             }
           }
         }
       bool operator++(void); //prefix
       bool operator++(int);  //postfix
       int operator==(const const_iterator& right);
       int operator!=(const const_iterator& right);
       const typename TrieBid<WordIndex,DATA_TYPE>::const_iterator& operator->(void)const;
       pair<Vector<WordIndex>,DATA_TYPE> operator*(void)const;
    };

      // const_iterator functions for the trie class
  const_iterator begin(void)const;
  const_iterator end(void)const;

 protected:
  Vector<TrieBid<WordIndex,DATA_TYPE>* > vecTriePtr;      		      
};

//--------------- Template method definitions


//--------------- TrieBidOfWords class method definitions

template<class DATA_TYPE>
TrieBidOfWords<DATA_TYPE>::TrieBidOfWords(void)
{
  this->clear();
}

//---------------
template<class DATA_TYPE>
TrieBidOfWords<DATA_TYPE>::TrieBidOfWords(const TrieBidOfWords<DATA_TYPE>& tbw)
{
  this->vecTriePtr.insert(this->vecTriePtr.begin(),tbw.vecTriePtr.size(),NULL);

  for(unsigned int i=0;i<tbw.vecTriePtr.size();++i)
  {
    if(tbw.vecTriePtr[i]!=NULL)
    {
      this->vecTriePtr[i]=new TrieBid<WordIndex,DATA_TYPE>;
      *this->vecTriePtr[i]=*tbw.vecTriePtr[i];
    }
  }
}

//---------------
template<class DATA_TYPE>
TrieBidOfWords<DATA_TYPE>&
TrieBidOfWords<DATA_TYPE>::operator=(const TrieBidOfWords<DATA_TYPE>& tbw)
{
  if(&tbw!=this)
  {
    clear();

    this->vecTriePtr.insert(this->vecTriePtr.begin(),tbw.vecTriePtr.size(),NULL);

    for(unsigned int i=0;i<tbw.vecTriePtr.size();++i)
    {
      if(tbw.vecTriePtr[i]!=NULL)
      {
        this->vecTriePtr[i]=new TrieBid<WordIndex,DATA_TYPE>;
        *this->vecTriePtr[i]=*tbw.vecTriePtr[i];
      }
    }
  }
  return *this;
}

//---------------
template<class DATA_TYPE>
TrieBid<WordIndex,DATA_TYPE>* TrieBidOfWords<DATA_TYPE>::insert(const Vector<WordIndex>& keySeq,const DATA_TYPE& d)
{
  TrieBid<WordIndex,DATA_TYPE>* tptrn=NULL;

  if(keySeq.size()==0) return NULL;
  else
  {
    while(vecTriePtr.size()<=keySeq[0])
    {    
      vecTriePtr.push_back(tptrn);
    } 
    
    if(vecTriePtr[keySeq[0]]==NULL)
    {
      vecTriePtr[keySeq[0]]=new TrieBid<WordIndex,DATA_TYPE>;
    }
    return vecTriePtr[keySeq[0]]->insert(keySeq,d);
  }
}

//---------------
template<class DATA_TYPE>
bool TrieBidOfWords<DATA_TYPE>::erase(const Vector<WordIndex>& keySeq)
{
  if(keySeq.size()==0) return false;
  else
  {
    if(keySeq[0]<vecTriePtr.size())
    {
      if(vecTriePtr[keySeq[0]]==NULL) return false;
      else
      {
        vecTriePtr[keySeq[0]]->clear();
        delete vecTriePtr[keySeq[0]];
        vecTriePtr[keySeq[0]]=NULL;
      }
    }
    else return NULL;
  }
}

//---------------
template<class DATA_TYPE>
DATA_TYPE* TrieBidOfWords<DATA_TYPE>::find(const Vector<WordIndex>& keySeq)
{
  if(keySeq.size()==0) return NULL;
  else
  {
    if(keySeq[0]<vecTriePtr.size())
    {
      if(vecTriePtr[keySeq[0]]==NULL) return NULL;
      else
      {
        return vecTriePtr[keySeq[0]]->find(keySeq);
      }
    }
    else return NULL;
  }
}
//---------------
template<class DATA_TYPE>
TrieBid<WordIndex,DATA_TYPE>* TrieBidOfWords<DATA_TYPE>::getState(const Vector<WordIndex>& keySeq)
{
  if(keySeq.size()==0) return NULL;
  else
  {
    if(keySeq[0]<vecTriePtr.size())
    {
      if(vecTriePtr[keySeq[0]]==NULL) return NULL;
      else
      {
        return vecTriePtr[keySeq[0]]->getState(keySeq);
      }
    }
    else return NULL;
  }  
}

//---------------
template<class DATA_TYPE>
size_t TrieBidOfWords<DATA_TYPE>::size(void)const
{
  size_t s=0;
  unsigned int i;

  for(i=0;i<vecTriePtr.size();++i)
  {
    if(vecTriePtr[i]!=NULL)
    {
      s+=vecTriePtr[i]->size();
    }
  }
  return s;  
}

//---------------
template<class DATA_TYPE>
unsigned int TrieBidOfWords<DATA_TYPE>::height(void)const
{
  unsigned int h,aux;
  unsigned int i;

  h=0;
  for(i=0;i<vecTriePtr.size();++i)
  {
    if(vecTriePtr[i]!=NULL)
    {
      aux=vecTriePtr[i]->height();
      if(aux>h) h=aux;
    }
  }
  return h;  
}
//---------------
template<class DATA_TYPE>
Vector<unsigned long> TrieBidOfWords<DATA_TYPE>::branchingFactor(void)const
{
  Vector<unsigned long> result,aux;
  unsigned long i,j;

  for(i=0;i<vecTriePtr.size();++i)
  {
    if(vecTriePtr[i]!=NULL)
    {
      aux=vecTriePtr[i]->branchingFactor();
      for(j=0;j<aux.size();++j)
      {
        if(j<result.size()) result[j]+=aux[j];
        else result.push_back(aux[j]);
      }
    }
  }
  return result;  
}
//---------------
template<class DATA_TYPE>
size_t TrieBidOfWords<DATA_TYPE>::countSparseNodes(void)const
{
  size_t result=0;
  unsigned long i;

  for(i=0;i<vecTriePtr.size();++i)
  {
    if(vecTriePtr[i]!=NULL)
    {
      result+=vecTriePtr[i]->countSparseNodes();
    }
  }
  return result;  
}
//---------------
template<class DATA_TYPE>
void TrieBidOfWords<DATA_TYPE>::clear(void)
{
  unsigned int i;

  for(i=0;i<vecTriePtr.size();++i)
  {
    if(vecTriePtr[i]!=NULL)
    {
      vecTriePtr[i]->clear();
      delete vecTriePtr[i];
    }
  }
  vecTriePtr.clear();
}

//---------------
template<class DATA_TYPE>
TrieBidOfWords<DATA_TYPE>::~TrieBidOfWords()
{
  this->clear();
}
//---------------


// const_iterator functions for the TrieBidOfWords class
//---------------
template<class DATA_TYPE>
typename TrieBidOfWords<DATA_TYPE>::const_iterator TrieBidOfWords<DATA_TYPE>::begin(void)const 
{
  typename TrieBidOfWords<DATA_TYPE>::const_iterator iter(this);
	
  return iter;
}

//---------------
template<class DATA_TYPE>
typename TrieBidOfWords<DATA_TYPE>::const_iterator TrieBidOfWords<DATA_TYPE>::end(void)const
{
  typename TrieBidOfWords<DATA_TYPE>::const_iterator iter(NULL);
	
  return iter;
}

// const_iterator function definitions
//--------------------------
template<class DATA_TYPE>
bool TrieBidOfWords<DATA_TYPE>::const_iterator::operator++(void) //prefix
{
  if(tptr!=NULL)
  {
    ++trieIter;
    if(trieIter==tptr->vecTriePtr[i]->end())
    {
      ++i;
      while(i<tptr->vecTriePtr.size() && tptr->vecTriePtr[i]==NULL) ++i;
      if(i<tptr->vecTriePtr.size())
      {
        trieIter=tptr->vecTriePtr[i]->begin();
        return true;
      }
      else
      {
        i=0;
        tptr=NULL;
        return false;
      }
    }
    else
    {
      return true;
    }
  }
  else return false;  
}
//--------------------------
template<class DATA_TYPE>
bool TrieBidOfWords<DATA_TYPE>::const_iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class DATA_TYPE>
int TrieBidOfWords<DATA_TYPE>::const_iterator::operator==(const const_iterator& right)
{
 return (tptr==right.tptr && i==right.i && trieIter==right.trieIter);	
}
//--------------------------
template<class DATA_TYPE>
int TrieBidOfWords<DATA_TYPE>::const_iterator::operator!=(const const_iterator& right)
{
 return !((*this)==right);	
}

//--------------------------
template<class DATA_TYPE>
const typename TrieBid<WordIndex,DATA_TYPE>::const_iterator&
TrieBidOfWords<DATA_TYPE>::const_iterator::operator->(void)const
{
  return trieIter;
}

//--------------------------
template<class DATA_TYPE>
pair<Vector<WordIndex>,DATA_TYPE> TrieBidOfWords<DATA_TYPE>::const_iterator::operator*(void)const
{
  return *trieIter;
}
//--------------------------

#endif
