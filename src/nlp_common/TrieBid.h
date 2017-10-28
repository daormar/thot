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
/* Module: TrieBid                                                  */
/*                                                                  */
/* Prototype file: TrieBid.h                                        */
/*                                                                  */
/* Description: Implements a  bidirectional trie                    */
/*                                                                  */
/********************************************************************/

#ifndef _TrieBid_h
#define _TrieBid_h

//--------------- Include files --------------------------------------

#include <float.h>
#include <iostream>
#include <iomanip>
#include <utility>
#include <vector>

//--------------- Constants ------------------------------------------


//--------------- User defined types ---------------------------------


//--------------- Classes --------------------------------------------

//--------------- TrieBid class

template<class KEY,class DATA_TYPE>
class TrieBid
{
 public:

      // Constructor
  TrieBid(void);
      // Copy constructor
  TrieBid(const TrieBid<KEY,DATA_TYPE>& t);
      // Assignment operator
  TrieBid<KEY,DATA_TYPE>& operator=(const TrieBid<KEY,DATA_TYPE>& tb);

  // Basic functions
  TrieBid<KEY,DATA_TYPE>* insert(const std::vector<KEY>& keySeq,const DATA_TYPE& d);
      // Inserts a sequence of elements of class key. The last element 
      // of vector keySeq is the first element of the sequence.
  bool erase(const std::vector<KEY>& keySeq);
  DATA_TYPE* find(const std::vector<KEY>& keySeq);
  TrieBid<KEY,DATA_TYPE>* getState(const std::vector<KEY>& keySeq);
  std::vector<DATA_TYPE*> findV(const std::vector<KEY>& keySeq);
  DATA_TYPE&  operator[](const std::vector<KEY>& keySeq);
  const DATA_TYPE& operator[](const std::vector<KEY>& keySeq) const;
  DATA_TYPE& getData(void);
  size_t size(void)const;
  unsigned int height(void)const;
  std::vector<unsigned long> branchingFactor(void)const;
  size_t countSparseNodes(void)const;
  void getKeySeq(std::vector<KEY>& keySeq);
  void clear(void);
  ~TrieBid();

      // const_iterator (TrieBid objects cannot be modified by means of this const_iterator)
  class const_iterator;
  friend class const_iterator;
  class const_iterator
    {
      protected:
       std::vector<const TrieBid<KEY,DATA_TYPE>* > vecTriePtr;
       std::pair<std::vector<KEY>,DATA_TYPE> vecKeyDataPair;
           
      public:
       const_iterator(void)
         {
           vecTriePtr.push_back(NULL);
         }
       const_iterator(const TrieBid<KEY,DATA_TYPE>* trieptr)
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

 private:
  KEY key;
  DATA_TYPE data;
  TrieBid<KEY,DATA_TYPE>* next;
  TrieBid<KEY,DATA_TYPE>* children;
  TrieBid<KEY,DATA_TYPE>* father;
  
  void height(unsigned int i,unsigned int& r)const;
  void branchingFactor(unsigned int i,std::vector<unsigned long>& r)const;
  void countSparseNodes(size_t& sp,bool isFirst)const;

};

//--------------- Template method definitions


//--------------- TrieBid class method definitions

template<class KEY,class DATA_TYPE>
TrieBid<KEY,DATA_TYPE>::TrieBid(void)
{
  next=NULL;
  father=NULL;
  children=this;
}

//---------------
template<class KEY,class DATA_TYPE>
TrieBid<KEY,DATA_TYPE>::TrieBid(const TrieBid<KEY,DATA_TYPE>& tb)
{
  if(&tb!=tb.children)
  {
        // The node is initialized
    this->data=tb.data;
      
    if(tb.children!=NULL) // node has children
    {
      this->children=new TrieBid<KEY,DATA_TYPE>;
      this->children->father=this;
      *this->children=*tb.children;
    }
    else this->children=NULL;
      
    if(tb.next!=NULL) // next is not NULL
    {
      this->next=new TrieBid<KEY,DATA_TYPE>;
      *this->next=*tb.next;
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
TrieBid<KEY,DATA_TYPE>&
TrieBid<KEY,DATA_TYPE>::operator=(const TrieBid<KEY,DATA_TYPE>& tb)
{
  if(&tb!=this)
  {
    clear();
    if(&tb!=tb.children)
    {
          // The node is initialized
      this->data=tb.data;
      
      if(tb.children!=NULL) // node has children
      {
        this->children=new TrieBid<KEY,DATA_TYPE>;
        this->children->father=this;
        *this->children=*tb.children;
      }
      else this->children=NULL;
      
      if(tb.next!=NULL) // next is not NULL
      {
        this->next=new TrieBid<KEY,DATA_TYPE>;
        *this->next=*tb.next;
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
TrieBid<KEY,DATA_TYPE>* TrieBid<KEY,DATA_TYPE>::insert(const std::vector<KEY>& keySeq,const DATA_TYPE& d)
{
  unsigned int i;
  TrieBid<KEY,DATA_TYPE> *t,*newt;
  KEY k;

  if(keySeq.size()==0) return NULL;
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
          newt=new TrieBid<KEY,DATA_TYPE>;
          newt->father=t;
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
        newt=new TrieBid<KEY,DATA_TYPE>;
        t->next=newt;
        newt->father=t->father;
        t=newt;
        t->key=k;
        t->children=NULL;
      }
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        newt=new TrieBid<KEY,DATA_TYPE>;
        newt->father=t;
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
bool TrieBid<KEY,DATA_TYPE>::erase(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  TrieBid<KEY,DATA_TYPE> *t,tprev;
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
DATA_TYPE* TrieBid<KEY,DATA_TYPE>::find(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  TrieBid<KEY,DATA_TYPE> *t;
  KEY k;

  if(keySeq.size()==0) return NULL;
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
TrieBid<KEY,DATA_TYPE>* TrieBid<KEY,DATA_TYPE>::getState(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  TrieBid<KEY,DATA_TYPE> *t;
  KEY k;

  if(keySeq.size()==0) return NULL;
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
std::vector<DATA_TYPE*> TrieBid<KEY,DATA_TYPE>::findV(const std::vector<KEY>& keySeq)
{
  unsigned int i;
  TrieBid<KEY,DATA_TYPE> *t;
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
DATA_TYPE& TrieBid<KEY,DATA_TYPE>::operator[](const std::vector<KEY>& keySeq)
{
  unsigned int i;
  TrieBid<KEY,DATA_TYPE> *t,*newt;
  KEY k;

  if(keySeq.size()==0) return ((DATA_TYPE*)&(t->data));
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
          newt=new TrieBid<KEY,DATA_TYPE>;
          newt->father=t;
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
        newt=new TrieBid<KEY,DATA_TYPE>;
        t->next=newt;
        newt->father=t->father;
        t=newt;
        t->key=k;
        t->children=NULL;
      }
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        newt=new TrieBid<KEY,DATA_TYPE>;
        newt->father=t;
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
const DATA_TYPE& TrieBid<KEY,DATA_TYPE>::operator[](const std::vector<KEY>& keySeq) const 
{
  unsigned int i;
  const TrieBid<KEY,DATA_TYPE> *t,*newt;
  KEY k;

  if(keySeq.size()==0) return ((DATA_TYPE*)&(t->data));
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
DATA_TYPE& TrieBid<KEY,DATA_TYPE>::getData(void)
{
  return this->data;
}

//---------------
template<class KEY,class DATA_TYPE>
size_t TrieBid<KEY,DATA_TYPE>::size(void)const
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
unsigned int TrieBid<KEY,DATA_TYPE>::height(void)const
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
void TrieBid<KEY,DATA_TYPE>::height(unsigned int i,unsigned int& r)const
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
std::vector<unsigned long> TrieBid<KEY,DATA_TYPE>::branchingFactor(void)const
{
 std::vector<unsigned long> result;
 unsigned long z=0;
	
 result.insert(result.begin(),height(),z);	
 branchingFactor(0,result);
 return result;
}
//---------------
template<class KEY,class DATA_TYPE>
void TrieBid<KEY,DATA_TYPE>::branchingFactor(unsigned int i,std::vector<unsigned long>& r)const
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
size_t TrieBid<KEY,DATA_TYPE>::countSparseNodes(void)const
{
  size_t sp=0;
  
  countSparseNodes(sp,true);
  return sp;
}
//---------------
template<class KEY,class DATA_TYPE>
void TrieBid<KEY,DATA_TYPE>::countSparseNodes(size_t& sp,bool isFirst)const
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
void TrieBid<KEY,DATA_TYPE>::getKeySeq(std::vector<KEY>& keySeq)
{
  TrieBid<KEY,DATA_TYPE> *t;

  t=this;
  keySeq.clear();
  if(t->children!=t) keySeq.push_back(this->key);
  
  while(t->father!=NULL && t->children!=t)
  {
    t=t->father;
    keySeq.push_back(t->key);
  }
}

//---------------
template<class KEY,class DATA_TYPE>
void TrieBid<KEY,DATA_TYPE>::clear(void)
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
    this->father=NULL;
  }
}

//---------------
template<class KEY,class DATA_TYPE>
TrieBid<KEY,DATA_TYPE>::~TrieBid()
{
  this->clear();
}
//---------------

// const_iterator functions for the trie class
//---------------
template<class KEY,class DATA_TYPE>
typename TrieBid<KEY,DATA_TYPE>::const_iterator TrieBid<KEY,DATA_TYPE>::begin(void)const
{
  typename TrieBid<KEY,DATA_TYPE>::const_iterator iter(this);
  return iter;
}

//---------------
template<class KEY,class DATA_TYPE>
typename TrieBid<KEY,DATA_TYPE>::const_iterator TrieBid<KEY,DATA_TYPE>::end(void)const
{
  typename TrieBid<KEY,DATA_TYPE>::const_iterator iter(NULL);
	
  return iter;
}

// const_iterator function definitions
//--------------------------
template<class KEY,class DATA_TYPE>
bool TrieBid<KEY,DATA_TYPE>::const_iterator::operator++(void) //prefix
{
  TrieBid<KEY,DATA_TYPE>* tptr;

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
bool TrieBid<KEY,DATA_TYPE>::const_iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class KEY,class DATA_TYPE>
int TrieBid<KEY,DATA_TYPE>::const_iterator::operator==(const const_iterator& right)
{
 return (vecTriePtr==right.vecTriePtr);	
}
//--------------------------
template<class KEY,class DATA_TYPE>
int TrieBid<KEY,DATA_TYPE>::const_iterator::operator!=(const const_iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class KEY,class DATA_TYPE>
const std::pair<std::vector<KEY>,DATA_TYPE>* TrieBid<KEY,DATA_TYPE>::const_iterator::operator->(void)const
{
  if(vecTriePtr.size()==1 && vecTriePtr[0]==NULL)
  {
    return NULL;
  }
  else  return (const std::pair<std::vector<KEY>,DATA_TYPE>*) &(vecKeyDataPair);
}
//--------------------------
template<class KEY,class DATA_TYPE>
std::pair<std::vector<KEY>,DATA_TYPE> TrieBid<KEY,DATA_TYPE>::const_iterator::operator*(void)const
{
  return vecKeyDataPair;
}
//--------------------------

#endif
