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
/* Module: TrieVecs                                                 */
/*                                                                  */
/* Prototype file: TrieVecs.h                                       */
/*                                                                  */
/* Description: Implements the trie data structure using ordered    */
/*              vectors                                             */
/*                                                                  */
/********************************************************************/

#ifndef _TrieVecs_h
#define _TrieVecs_h

//--------------- Include files --------------------------------------

#include <float.h>
#include "OrderedVector.h"
#include <iostream>
#include <iomanip>
#include <utility>

//--------------- Constants ------------------------------------------


//--------------- User defined types ---------------------------------

//--------------- Classes --------------------------------------------

//--------------- TrieVecs class

template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION=less<KEY> >
class TrieVecs 
{
 public:

   typedef OrderedVector<KEY,TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>,KEY_SORT_CRITERION> Children;

     // Constructor
   TrieVecs(void);

   // Basic functions
   TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>* insert(const std::vector<KEY>& keySeq,
                                                      const DATA_TYPE& d);
     // Inserts a sequence of elements of class key. The last element 
     // of vector keySeq is the first element of the sequence.
   DATA_TYPE* find(const std::vector<KEY>& keySeq);

   size_t size(void)const;
   unsigned int height(void)const;
   std::vector<unsigned long> branchingFactor(void)const;
   size_t countSparseNodes(void)const;
   void clear(void);

   ~TrieVecs();

     // const_iterator
   class const_iterator;
   friend class const_iterator;
   class const_iterator
   {
     protected:
      std::vector<const TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>* > vecTrieVecsPtr;
      std::vector<typename Children::const_iterator> childrenIterVec;
      pair<std::vector<KEY>,DATA_TYPE> vecKeyDataPair;
           
     public:
      const_iterator(void)
        {
          TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>* tptrn=NULL;
          vecTrieVecsPtr.push_back(tptrn);
        }
      const_iterator(const TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>* trieptr)
        {          
          if(trieptr!=NULL)
          {
            vecTrieVecsPtr.push_back(trieptr);
            childrenIterVec.push_back(trieptr->children.begin());
            while((*vecTrieVecsPtr.back()).children.size()>0)
            {// reach the deepest child
              childrenIterVec.push_back((*vecTrieVecsPtr.back()).children.begin());
              vecTrieVecsPtr.push_back(&childrenIterVec.back()->second);
              vecKeyDataPair.first.push_back(childrenIterVec.back()->first);
            }
            vecKeyDataPair.second=(*vecTrieVecsPtr.back()).data;
            childrenIterVec.push_back((*vecTrieVecsPtr.back()).children.begin());
          }
          else
          {
            vecTrieVecsPtr.push_back(NULL);
          }
        }
      bool operator++(void); //prefix
      bool operator++(int);  //postfix
      int operator==(const const_iterator& right);
      int operator!=(const const_iterator& right);
      const pair<std::vector<KEY>,DATA_TYPE>* operator->(void)const;
      pair<std::vector<KEY>,DATA_TYPE> operator*(void)const;
   };

     // const_iterator functions for the TrieVecs class
   const_iterator begin(void)const;
   const_iterator end(void)const;

 private:
   DATA_TYPE data;
   Children children;
   void height(unsigned int i,unsigned int& r)const;
   void branchingFactor(unsigned int i,std::vector<unsigned long>& r)const;
   void countSparseNodes(size_t& sp)const; 
};

//--------------- Template method definitions


//--------------- TrieVecs class method definitions

template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::TrieVecs(void)
{	
}

//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>*
TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::insert(const std::vector<KEY>& keySeq,
                                                   const DATA_TYPE& d)
{
  unsigned int i;
  TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION> *childrenPos;
  TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION> *t;  
  KEY k;

  if(keySeq.size()==0) return NULL;

  t=this;
 
  for(i=0;i<keySeq.size();++i)
  {
    k=keySeq[i];
    childrenPos=t->children.findPtr(k);
    
    if(childrenPos==NULL)
    {
      pair<KEY,TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION> > p;
      p.first=k;
      t=t->children.push(p.first,p.second);
    }	
    else
    {
      t=childrenPos;
    }
  }
  t->data=d;
  return t;
}

//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
DATA_TYPE* TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::find(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION> *childrenPos;
  TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION> *t;	

  if(keySeq.size()==0) return NULL;
 
  t=this;
 
  for(i=0;i<keySeq.size();++i)
  {
    childrenPos=t->children.findPtr(keySeq[i]);	
    if(childrenPos==NULL)
    {
      return NULL;
    }
    else 
    {
      t=childrenPos;
    }	
  }
  return ((DATA_TYPE*)&(t->data));
}

//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
size_t TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::size(void)const
{
  typename Children::const_iterator childrenPos;
  size_t s;

  if (children.size()!=0)
  {
    s=1;
    for(childrenPos=children.begin();childrenPos!=children.end();++childrenPos)
    {
      s=s+childrenPos->second.size();
    }
    return s;
  }  
  else
  {
    return 1;
  }
}

//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
unsigned int TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::height(void)const
{
  unsigned int i=0;	
  height(0,i);
  return i;	
}
//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
void TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::height(unsigned int i,unsigned int& r)const
{
  typename Children::const_iterator childrenPos;
 
  if(i>r) r=i;
  if (children.size()!=0)
  {
    for(childrenPos=children.begin();childrenPos!=children.end();++childrenPos)
      childrenPos->second.height(i+1,r);
  }  	 	
}
//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
std::vector<unsigned long> TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::branchingFactor(void)const
{
  std::vector<unsigned long> result;
  unsigned long z=0;
	
  result.insert(result.begin(),height(),z);	
  branchingFactor(0,result);
  return result;
}
//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
void TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::branchingFactor(unsigned int i,
                                                                 std::vector<unsigned long>& r)const
{
  typename Children::const_iterator childrenPos;
	
  if (children.size()!=0)
  {
    r[i]+=children.size();
    for(childrenPos=children.begin();childrenPos!=children.end();++childrenPos)
      childrenPos->second.branchingFactor(i+1,r);
  }  	
}
//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
size_t TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::countSparseNodes(void)const
{
  size_t sp=0;
  
  countSparseNodes(sp,true);
  return sp;
}
//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
void TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::countSparseNodes(size_t& sp)const
{
  typename Children::const_iterator childrenPos;
	
  if (children.size()!=0)
  {
    if(children.size()==1) ++sp;
    for(childrenPos=children.begin();childrenPos!=children.end();++childrenPos)
      childrenPos->second.countSparseNodes(sp,false);
  }  	
}
//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
void TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::clear(void)
{
  typename Children::iterator childrenPos;
	
  if (children.size()!=0)
  {
    for(childrenPos=children.begin();childrenPos!=children.end();++childrenPos)
    {
      childrenPos->second.clear();
    }
    children.clear();
  }  
}

//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::~TrieVecs()
{
  this->clear();
}
//---------------

// const_iterator functions for the trie class
//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
typename TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator
TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::begin(void)const
{
  typename TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator iter(this);
  return iter;
}

//---------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
typename TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator
TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::end(void)const
{
  typename TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator iter(NULL);
	
  return iter;
}

// const_iterator function definitions
//--------------------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
bool TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator::operator++(void) //prefix
{
  if(vecTrieVecsPtr.size()==1 && vecTrieVecsPtr[0]==NULL)
  {// end reached or const_iterator not initialized
    return false;    
  }
  else
  {
    ++childrenIterVec.back();
    if(childrenIterVec.back()==(*vecTrieVecsPtr.back()).children.end())
    {// trie node has not alternative nodes
      vecTrieVecsPtr.pop_back();
      childrenIterVec.pop_back();
      if(vecKeyDataPair.first.size()>0) vecKeyDataPair.first.pop_back();
      if(vecTrieVecsPtr.size()==0)
      {
        vecTrieVecsPtr.push_back(NULL);
        childrenIterVec.clear();
        return false;
      }
      else
      {
        vecKeyDataPair.second=(*vecTrieVecsPtr.back()).data;
        ++childrenIterVec.back();
      }
    }
    
    if(childrenIterVec.back()!=(*vecTrieVecsPtr.back()).children.end())
    {// trie node has alternative nodes
      const TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>* tptr=&childrenIterVec.back()->second;
      vecTrieVecsPtr.push_back(tptr);
      vecKeyDataPair.first.push_back(childrenIterVec.back()->first);

      while((*vecTrieVecsPtr.back()).children.size()>0)
      {// reach the deepest child
        childrenIterVec.push_back((*vecTrieVecsPtr.back()).children.begin());
        vecTrieVecsPtr.push_back(&childrenIterVec.back()->second);
        vecKeyDataPair.first.push_back(childrenIterVec.back()->first);
      }
      vecKeyDataPair.second=(*vecTrieVecsPtr.back()).data;
      childrenIterVec.push_back((*vecTrieVecsPtr.back()).children.begin());
    }
    return true;
  }
}
//--------------------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
bool TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
int TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator::operator==(const const_iterator& right)
{
 return (vecTrieVecsPtr==right.vecTrieVecsPtr);	
}
//--------------------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
int TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator::operator!=(const const_iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
const pair<std::vector<KEY>,DATA_TYPE>*
TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator::operator->(void)const
{
  if(vecTrieVecsPtr.size()==1 && vecTrieVecsPtr[0]==NULL)
  {
    return NULL;
  }
  else  return (pair<std::vector<KEY>,DATA_TYPE>*) &(vecKeyDataPair);
}
//--------------------------
template<class KEY,class DATA_TYPE,class KEY_SORT_CRITERION>
pair<std::vector<KEY>,DATA_TYPE> TrieVecs<KEY,DATA_TYPE,KEY_SORT_CRITERION>::const_iterator::operator*(void)const
{
  return vecKeyDataPair;
}
//--------------------------

#endif

