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
/* Module: Trie                                                     */
/*                                                                  */
/* Prototype file: Trie.h                                           */
/*                                                                  */
/* Description: Implements the Trie data structure                  */
/*                                                                  */
/********************************************************************/

#ifndef _Trie_h
#define _Trie_h

//--------------- Include files --------------------------------------

#include <float.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>

//--------------- Constants ------------------------------------------


//--------------- User defined types ---------------------------------


//--------------- Classes --------------------------------------------

//--------------- Trie class

template<class KEY,class DATA_TYPE>
class Trie 
{
 public:

      // Constructor
  Trie(void);
      // Copy constructor
  Trie(const Trie<KEY,DATA_TYPE>& t);
      // Assignment operator
  Trie<KEY,DATA_TYPE>& operator=(const Trie<KEY,DATA_TYPE>& t);

  // Basic functions
  Trie<KEY,DATA_TYPE>* insert(const std::vector<KEY>& keySeq,const DATA_TYPE& d);
      // Inserts a sequence of elements of class key. The last element 
      // of vector keySeq is the first element of the sequence.
  bool erase(const std::vector<KEY>& keySeq);
  DATA_TYPE* find(const std::vector<KEY>& keySeq);
  Trie<KEY,DATA_TYPE>* getState(const std::vector<KEY>& keySeq);
  std::vector<DATA_TYPE*> findV(const std::vector<KEY>& keySeq);
  DATA_TYPE&  operator[](const std::vector<KEY>& keySeq);
  const DATA_TYPE&  operator[](const std::vector<KEY>& keySeq) const;
  DATA_TYPE& getData(void);
	
  size_t size(void)const;
  unsigned int height(void)const;
  std::vector<unsigned long> branchingFactor(void)const;
  size_t countSparseNodes(void)const;
  void clear(void);
  ~Trie();

      // const_iterator
  class const_iterator;
  friend class const_iterator;
  class const_iterator
    {
      protected:
       std::vector<const Trie<KEY,DATA_TYPE>* > vecTriePtr;
       std::pair<std::vector<KEY>,DATA_TYPE> vecKeyDataPair;
           
      public:
       const_iterator(void)
         {
           Trie<KEY,DATA_TYPE>* tptrn=NULL;
           
           vecTriePtr.push_back(tptrn);
         }
       const_iterator(const Trie<KEY,DATA_TYPE>* trieptr)
         {
           vecTriePtr.push_back(trieptr);
           
           if(trieptr!=NULL)
           { 
             if(trieptr!=trieptr->children)
             {
               vecKeyDataPair.first.push_back(trieptr->key);
               vecKeyDataPair.second=trieptr->data;
               
               while((*vecTriePtr[vecTriePtr.size()-1]).children!=NULL)
               {// reach the deepest child
                 vecTriePtr.push_back((*vecTriePtr[vecTriePtr.size()-1]).children);
                 vecKeyDataPair.first.push_back((*vecTriePtr[vecTriePtr.size()-1]).key);
               }
               vecKeyDataPair.second=(*vecTriePtr[vecTriePtr.size()-1]).data;
             }
             else vecTriePtr[0]=NULL;
           }
         }
       bool operator++(void); //prefix
       bool operator++(int);  //postfix
       int operator==(const const_iterator& right);
       int operator!=(const const_iterator& right);
       const std::pair<std::vector<KEY>,DATA_TYPE>* operator->(void)const;
       std::pair<std::vector<KEY>,DATA_TYPE> operator*(void)const;
    };
 
      // const_iterator functions for the trie class
  const_iterator begin(void)const;
  const_iterator end(void)const;

 protected:
  KEY key;
  DATA_TYPE data;
  Trie<KEY,DATA_TYPE>* next;
  Trie<KEY,DATA_TYPE>* children;
  
  void height(unsigned int i,unsigned int& r)const;
  void branchingFactor(unsigned int i,std::vector<unsigned long>& r)const;
  void countSparseNodes(size_t& sp,bool isFirst)const;		      
};

//--------------- Template method definitions


//--------------- Trie class method definitions

template<class KEY,class DATA_TYPE>
Trie<KEY,DATA_TYPE>::Trie(void)
{
  next=NULL;
  children=this;
}

//---------------
template<class KEY,class DATA_TYPE>
Trie<KEY,DATA_TYPE>::Trie(const Trie<KEY,DATA_TYPE>& t)
{
  if(&t!=t.children)
  {
        // The node is initialized
    this->data=t.data;
      
    if(t.children!=NULL) // node has children
    {
      this->children=new Trie<KEY,DATA_TYPE>;
      *this->children=*t.children;
    }
    else this->children=NULL;
      
    if(t.next!=NULL) // next is not NULL
    {
      this->next=new Trie<KEY,DATA_TYPE>;
      *this->next=*t.next;
    }
    else this->next=NULL;
  }
  else
  {
    next=NULL;
    children=this;
  }
}

//---------------
template<class KEY,class DATA_TYPE>
Trie<KEY,DATA_TYPE>&
Trie<KEY,DATA_TYPE>::operator=(const Trie<KEY,DATA_TYPE>& t)
{
  if(&t!=this)
  {
    clear();
    if(&t!=t.children)
    {
          // The node is initialized
      this->data=t.data;
      
      if(t.children!=NULL) // node has children
      {
        this->children=new Trie<KEY,DATA_TYPE>;
        *this->children=*t.children;
      }
      else this->children=NULL;
      
      if(t.next!=NULL) // next is not NULL
      {
        this->next=new Trie<KEY,DATA_TYPE>;
        *this->next=*t.next;
      }
      else this->next=NULL;
    }
    else
    {
      next=NULL;
      children=this;
    }    
  }
  return *this;
}
  
//---------------
template<class KEY,class DATA_TYPE>
Trie<KEY,DATA_TYPE>* Trie<KEY,DATA_TYPE>::insert(const std::vector<KEY>& keySeq,const DATA_TYPE& d)
{
  unsigned int i;
  Trie<KEY,DATA_TYPE> *t,*newt;
  KEY k;

  t=this;
 
  for(i=0;i<keySeq.size();++i) // for each position of the sequence...
  {
    k=keySeq[i];
    while(t->next!=NULL && t->key!=k) // search the key 'k'
    {
      t=t->next; 
    }
    if(t!=t->children && t->key==k) // key 'k' was found?
    {
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          newt=new Trie<KEY,DATA_TYPE>;
          t->children=newt;
          t=newt;
        }
      }
      else t->data=d;
    }
    else
    { // 'k' was not found
      if(t->children==t) // t is uninitialized?
      {
        t->key=k;
        t->children=NULL;
      }
      else
      { // t is initialized, add new key to 'next' list
        newt=new Trie<KEY,DATA_TYPE>;
        t->next=newt;
        t=newt;
        t->key=k;
        t->children=NULL;
      }
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        newt=new Trie<KEY,DATA_TYPE>;
        t->children=newt;
        t=newt;
      }
      else
      { // end of seq. reached
        t->data=d;
      }
    }
  }
  return t;
}

//---------------
template<class KEY,class DATA_TYPE>
bool Trie<KEY,DATA_TYPE>::erase(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  Trie<KEY,DATA_TYPE> *t,tprev;
  KEY k;

  t=this;
 
  for(i=0;i<keySeq.size();++i) // for each position of the sequence...
  { 
    k=keySeq[i];
    while(t->next!=NULL && t->key!=k) // search the key 'k'
    {
      tprev=t;
      t=t->next; 
    }
    if(t!=t->children && t->key==k) // key 'k' was found?
    {
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          return false;
        }
      }
      else // end of seq. reached
      {
        tprev->next=t->next;
        t->clear();
        delete t;
      }
    }
    else
    { // 'k' was not found
      return false;
    }
  }
  return true;
}

//---------------
template<class KEY,class DATA_TYPE>
DATA_TYPE* Trie<KEY,DATA_TYPE>::find(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  Trie<KEY,DATA_TYPE> *t;
  KEY k;

  t=this;
 
  for(i=0;i<keySeq.size();++i) // for each position of the sequence...
  { 
    k=keySeq[i];
    while(t->next!=NULL && t->key!=k) // search the key 'k'
    {
      t=t->next; 
    }
    if(t!=t->children && t->key==k) // key 'k' was found?
    {
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          return NULL;
        }
      }
    }
    else
    { // 'k' was not found
      return NULL;
    }
  }
  return ((DATA_TYPE*)&(t->data));  
}
//---------------
template<class KEY,class DATA_TYPE>
Trie<KEY,DATA_TYPE>* Trie<KEY,DATA_TYPE>::getState(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  Trie<KEY,DATA_TYPE> *t;
  KEY k;

  t=this;
 
  for(i=0;i<keySeq.size();++i) // for each position of the sequence...
  { 
    k=keySeq[i];
    while(t->next!=NULL && t->key!=k) // search the key 'k'
    {
      t=t->next; 
    }
    if(t!=t->children && t->key==k) // key 'k' was found?
    {
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          return NULL;
        }
      }
    }
    else
    { // 'k' was not found
      return NULL;
    }
  }
  return t;  
}
//---------------
template<class KEY,class DATA_TYPE>
std::vector<DATA_TYPE*> Trie<KEY,DATA_TYPE>::findV(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  const Trie<KEY,DATA_TYPE> *t;
  KEY k;
  std::vector<DATA_TYPE*> result;
  
  t=this;
 
  for(i=0;i<keySeq.size();++i) // for each position of the sequence...
  { 
    k=keySeq[i];
    while(t->next!=NULL && t->key!=k) // search the key 'k'
    {
      t=t->next; 
    }
    if(t!=t->children && t->key==k) // key 'k' was found?
    {
      result.push_back((DATA_TYPE*)&(t->data));
      
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          return result;
        }
      }
    }
    else
    { // 'k' was not found
      return result;
    }
  }
  return result;
}
//---------------
template<class KEY,class DATA_TYPE>
DATA_TYPE& Trie<KEY,DATA_TYPE>::operator[](const std::vector<KEY>& keySeq)
{
  unsigned int i;
  Trie<KEY,DATA_TYPE> *t,*newt;
  KEY k;

  t=this;
 
  for(i=0;i<keySeq.size();++i) // for each position of the sequence...
  { 
    k=keySeq[i];
    while(t->next!=NULL && t->key!=k) // search the key 'k'
    {
      t=t->next; 
    }
    if(t!=t->children && t->key==k) // key 'k' was found?
    {
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          newt=new Trie<KEY,DATA_TYPE>;
          t->children=newt;
          t=newt;
        }
      }
      else return t->data;
    }
    else
    { // 'k' was not found
      if(t->children==t) // t is uninitialized?
      {
        t->key=k;
        t->children=NULL;
      }
      else
      { // t is initialized, add new key to 'next' list
        newt=new Trie<KEY,DATA_TYPE>;
        t->next=newt;
        t=newt;
        t->key=k;
        t->children=NULL;
      }
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        newt=new Trie<KEY,DATA_TYPE>;
        t->children=newt;
        t=newt;
      }
      else
      { // end of seq. reached
        return t->data;
      }
    }
  }
  return t->data;
}
//---------------
template<class KEY,class DATA_TYPE>
const DATA_TYPE& Trie<KEY,DATA_TYPE>::operator[](const std::vector<KEY>& keySeq) const 
{
  unsigned int i;
  const Trie<KEY,DATA_TYPE> *t,*newt;
  KEY k;

  t=this;
 
  for(i=0;i<keySeq.size();++i) // for each position of the sequence...
  { 
    k=keySeq[i];
    while(t->next!=NULL && t->key!=k) // search the key 'k'
    {
      t=t->next; 
    }
    if(t!=t->children && t->key==k) // key 'k' was found?
    {
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          return NULL;
        }
      }
    }
    else
    { // 'k' was not found
      return NULL;
    }
  }
  return ((DATA_TYPE*)&(t->data));  
}
//---------------
template<class KEY,class DATA_TYPE>
DATA_TYPE& Trie<KEY,DATA_TYPE>::getData(void)
{
  return this->data;
}

//---------------
template<class KEY,class DATA_TYPE>
size_t Trie<KEY,DATA_TYPE>::size(void)const
{
  size_t s;

  if(this!=this->children) // the node is initialized?
  {
    s=1;
    if(this->children!=NULL) // node has children
    {
      s+=this->children->size();
    }
    if(this->next!=NULL) // next is not NULL
    {
      s+=this->next->size();
    }
    return s;  
  }
      //node not initialized
  else return 0;
  
}

//---------------
template<class KEY,class DATA_TYPE>
unsigned int Trie<KEY,DATA_TYPE>::height(void)const
{
  unsigned int i=1;	
  if(this!=this->children)
  {
    height(1,i);
    return i;
  }
  else return 0;
}
//---------------
template<class KEY,class DATA_TYPE>
void Trie<KEY,DATA_TYPE>::height(unsigned int i,unsigned int& r)const
{
  if(i>r) r=i;
  if(this!=this->children) // the node is initialized?
  {
    if(this->children!=NULL) // node has children
    {
      this->children->height(i+1,r);
    }
    if(this->next!=NULL) // next is not NULL
    {
      this->next->height(i,r);
    }
  }
}
//---------------
template<class KEY,class DATA_TYPE>
std::vector<unsigned long> Trie<KEY,DATA_TYPE>::branchingFactor(void)const
{
  std::vector<unsigned long> result;
  unsigned long z=0,h;

  h=height();
  for(z=0;z<h;++z) result.push_back(0);
  branchingFactor(0,result);
  return result;
}
//---------------
template<class KEY,class DATA_TYPE>
void Trie<KEY,DATA_TYPE>::branchingFactor(unsigned int i,std::vector<unsigned long>& r)const
{
  if(this!=this->children) // the node is initialized?
  {
    r[i]+=1;
    if(this->children!=NULL) // node has children
    {
      this->children->branchingFactor(i+1,r);
    }
    if(this->next!=NULL) // next is not NULL
    {
      this->next->branchingFactor(i,r);
    }
  }
}
//---------------
template<class KEY,class DATA_TYPE>
size_t Trie<KEY,DATA_TYPE>::countSparseNodes(void)const
{
  size_t sp=0;
  
  countSparseNodes(sp,true);
  return sp;
}
//---------------
template<class KEY,class DATA_TYPE>
void Trie<KEY,DATA_TYPE>::countSparseNodes(size_t& sp,bool isFirst)const
{
  if(this!=this->children) // the node is initialized?
  {
    if(this->children!=NULL) // node has children
    {
      this->children->countSparseNodes(sp,true);
    }
    if(this->next!=NULL) // next is not NULL
    {
      this->next->countSparseNodes(sp,false);
    }
    else
    {
      if(isFirst) ++sp;
    }
  }
}
//---------------
template<class KEY,class DATA_TYPE>
void Trie<KEY,DATA_TYPE>::clear(void)
{
  if(this!=this->children) // the node is initialized?
  {
    if(this->children!=NULL) // node has children
    {
      this->children->clear();
      delete this->children;
    }
    this->children=this;
    
    if(this->next!=NULL) // next is not NULL
    {
      this->next->clear();
      delete this->next;
      this->next=NULL;
    }
  }
}

//---------------
template<class KEY,class DATA_TYPE>
Trie<KEY,DATA_TYPE>::~Trie()
{
  this->clear();
}
//---------------

// Const_Iterator functions for the trie class
//---------------
template<class KEY,class DATA_TYPE>
typename Trie<KEY,DATA_TYPE>::const_iterator Trie<KEY,DATA_TYPE>::begin(void)const
{
  typename Trie<KEY,DATA_TYPE>::const_iterator iter(this);
  return iter;
}

//---------------
template<class KEY,class DATA_TYPE>
typename Trie<KEY,DATA_TYPE>::const_iterator Trie<KEY,DATA_TYPE>::end(void)const
{
  typename Trie<KEY,DATA_TYPE>::const_iterator iter(NULL);
	
  return iter;
}

// const_iterator function definitions
//--------------------------
template<class KEY,class DATA_TYPE>
bool Trie<KEY,DATA_TYPE>::const_iterator::operator++(void) //prefix
{
  Trie<KEY,DATA_TYPE>* tptr;

  if(vecTriePtr.size()==1 && vecTriePtr[0]==NULL)
  {// end reached or const_iterator not initialized
    return false;    
  }
  else
  {    
    if((*vecTriePtr[vecTriePtr.size()-1]).next==NULL)
    {// trie node has not alternative nodes
      vecTriePtr.pop_back();
      vecKeyDataPair.first.pop_back();
      if(vecTriePtr.size()==0)
      {
        vecTriePtr.push_back(NULL);
        return false;
      }
      else vecKeyDataPair.second=(*vecTriePtr[vecTriePtr.size()-1]).data;
    }
    else
    {// trie node has alternative nodes
      tptr=(*vecTriePtr[vecTriePtr.size()-1]).next;
      vecTriePtr.pop_back();
      vecTriePtr.push_back(tptr);
      vecKeyDataPair.first.pop_back();
      vecKeyDataPair.first.push_back(tptr->key);

      while((*vecTriePtr[vecTriePtr.size()-1]).children!=NULL)
      {// reach the deepest child
        vecTriePtr.push_back((*vecTriePtr[vecTriePtr.size()-1]).children);
        vecKeyDataPair.first.push_back((*vecTriePtr[vecTriePtr.size()-1]).key);
      }
      vecKeyDataPair.second=(*vecTriePtr[vecTriePtr.size()-1]).data;
    }
    return true;
  }
}
//--------------------------
template<class KEY,class DATA_TYPE>
bool Trie<KEY,DATA_TYPE>::const_iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class KEY,class DATA_TYPE>
int Trie<KEY,DATA_TYPE>::const_iterator::operator==(const const_iterator& right)
{
 return (vecTriePtr==right.vecTriePtr);	
}
//--------------------------
template<class KEY,class DATA_TYPE>
int Trie<KEY,DATA_TYPE>::const_iterator::operator!=(const const_iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class KEY,class DATA_TYPE>
const std::pair<std::vector<KEY>,DATA_TYPE>*
Trie<KEY,DATA_TYPE>::const_iterator::operator->(void)const
{
  if(vecTriePtr.size()==1 && vecTriePtr[0]==NULL)
  {
    return NULL;
  }
  else  return (std::pair<std::vector<KEY>,DATA_TYPE>*) &(vecKeyDataPair);
}
//--------------------------
template<class KEY,class DATA_TYPE>
std::pair<std::vector<KEY>,DATA_TYPE> Trie<KEY,DATA_TYPE>::const_iterator::operator*(void)const
{
  return vecKeyDataPair;
}
//--------------------------

#endif

