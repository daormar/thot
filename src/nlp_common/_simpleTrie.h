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
/* Module: _simpleTrie                                              */
/*                                                                  */
/* Prototype file: _simpleTrie.h                                    */
/*                                                                  */
/* Description: Implements a simple trie data structure.            */
/*                                                                  */
/* NOTE: The recursive function "size()" of this class is very      */
/*       slow, this could be a problem in some applications.        */
/*       The SimpleTrie class solves this problem.                  */
/********************************************************************/

#ifndef __simpleTrie_h
#define __simpleTrie_h

//--------------- Include files --------------------------------------

#include <float.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>

//--------------- Constants ------------------------------------------


//--------------- User defined types ---------------------------------


//--------------- Classes --------------------------------------------

//--------------- _simpleTrie class

template<class DATA_TYPE> 
class _simpleTrie 
{
 public:
      // Constructor
  _simpleTrie(void);
      // Copy constructor
  _simpleTrie(const _simpleTrie<DATA_TYPE>& st);
      // Assignment operator
  _simpleTrie<DATA_TYPE>& operator=(const _simpleTrie<DATA_TYPE>& _st);

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
  ~_simpleTrie();
  
      // const_iterator
  class const_iterator;
  friend class const_iterator;
  class const_iterator
    {
      protected:
       std::vector<const _simpleTrie<DATA_TYPE>* > vecTriePtr;
       std::vector<DATA_TYPE> vecData;
      
       bool iterate(void);
            
      public:
       const_iterator(void)
         {
           _simpleTrie<DATA_TYPE>* tptrn=NULL;
           
           vecTriePtr.clear();
           vecTriePtr.push_back(tptrn);
         }
       const_iterator(const _simpleTrie<DATA_TYPE>* trieptr)
         {              
           vecTriePtr.clear();
           vecTriePtr.push_back(trieptr);
           
           if(trieptr!=NULL)
           { 
             if(trieptr!=trieptr->children)
             {
               vecData.push_back(trieptr->data);
               
               while((*vecTriePtr[vecTriePtr.size()-1]).children!=NULL)
               {// reach the deepest child
                 vecTriePtr.push_back((*vecTriePtr[vecTriePtr.size()-1]).children);
                 vecData.push_back((*vecTriePtr[vecTriePtr.size()-1]).data);
               }
               if((*vecTriePtr[vecTriePtr.size()-1]).endOfSeq==false) ++(*this);
             }
             else vecTriePtr[0]=NULL;
           }
         }
       bool operator++(void); //prefix
       bool operator++(int);  //postfix
       int operator==(const const_iterator& right);
       int operator!=(const const_iterator& right);
       const std::vector<DATA_TYPE>* operator->(void)const;
       std::vector<DATA_TYPE> operator*(void)const;
    };
 
      // const_iterator functions for the trie class
  const_iterator begin(void)const;
  const_iterator end(void)const;

 protected:
  DATA_TYPE data;
  bool endOfSeq;
  _simpleTrie<DATA_TYPE>* next;
  _simpleTrie<DATA_TYPE>* children;
  
  void height(unsigned int i,unsigned int& r)const;
  void branchingFactor(unsigned int i,std::vector<unsigned long>& r)const;
  void countSparseNodes(size_t& sp,bool isFirst)const;
		      
};

//--------------- Template method definitions


//--------------- _simpleTrie class method definitions

template<class DATA_TYPE>
_simpleTrie<DATA_TYPE>::_simpleTrie(void)
{
  next=NULL;
  children=this;
}

//---------------
template<class DATA_TYPE>
_simpleTrie<DATA_TYPE>::_simpleTrie(const _simpleTrie<DATA_TYPE>& _st)
{
  if(&_st!=_st.children)
  {
        // The node is initialized
    this->data=_st.data;
    this->endOfSeq=_st.endOfSeq;
      
    if(_st.children!=NULL) // node has children
    {
      this->children=new _simpleTrie<DATA_TYPE>;
      *this->children=*_st.children;
    }
    else this->children=NULL;
      
    if(_st.next!=NULL) // next is not NULL
    {
      this->next=new _simpleTrie<DATA_TYPE>;
      *this->next=*_st.next;
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
template<class DATA_TYPE>
_simpleTrie<DATA_TYPE>&
_simpleTrie<DATA_TYPE>::operator=(const _simpleTrie<DATA_TYPE>& _st)
{
  if(&_st!=this)
  {
    clear();
    if(&_st!=_st.children)
    {
          // The node is initialized
      this->data=_st.data;
      this->endOfSeq=_st.endOfSeq;
      
      if(_st.children!=NULL) // node has children
      {
        this->children=new _simpleTrie<DATA_TYPE>;
        *this->children=*_st.children;
      }
      else this->children=NULL;
      
      if(_st.next!=NULL) // next is not NULL
      {
        this->next=new _simpleTrie<DATA_TYPE>;
        *this->next=*_st.next;
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
template<class DATA_TYPE>
_simpleTrie<DATA_TYPE>* _simpleTrie<DATA_TYPE>::insert(const std::vector<DATA_TYPE>& dataSeq)
{
  unsigned int i;
  _simpleTrie<DATA_TYPE> *t,*newt;
  DATA_TYPE d;

  if(dataSeq.size()==0) return NULL;
  t=this;
 
  for(i=0;i<dataSeq.size();++i) // for each position of the sequence...
  {
    d=dataSeq[i];
    while(t->next!=NULL && t->data!=d) // search the data 'd'
    {
      t=t->next; 
    }
    if(t!=t->children && t->data==d) // data 'd' was found?
    {
      if(i<dataSeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          newt=new _simpleTrie<DATA_TYPE>;
          t->children=newt;
          t=newt;
        }
      }
      else t->endOfSeq=true;
    }
    else
    { // 'd' was not found
      if(t->children==t) // t is uninitialized?
      {
        t->data=d;
        t->children=NULL;
        t->endOfSeq=false;
      }
      else
      { // t is initialized, add new key to 'next' list
        newt=new _simpleTrie<DATA_TYPE>;
        t->next=newt;
        t=newt;
        t->data=d;
        t->children=NULL;
        t->endOfSeq=false;
      }
      if(i<dataSeq.size()-1) // end of seq. not reached?
      {
        newt=new _simpleTrie<DATA_TYPE>;
        t->children=newt;
        t=newt;
      }
      else
      { // end of seq. reached
        t->endOfSeq=true;
      }
    }
  }
  return t;
}

//---------------
template<class DATA_TYPE>
bool _simpleTrie<DATA_TYPE>::erase(const std::vector<DATA_TYPE>& dataSeq)
{
  unsigned int i;
  _simpleTrie<DATA_TYPE> *t,tprev;
  DATA_TYPE d;

  if(dataSeq.size()==0) return false;
  t=this;
 
  for(i=0;i<dataSeq.size();++i) // for each position of the sequence...
  { 
    d=dataSeq[i];
    while(t->next!=NULL && t->data!=d) // search the key 'k'
    {
      tprev=t;
      t=t->next; 
    }
    if(t->data==d && t!=t->children) // 'd' was found?
    {
      if(i<dataSeq.size()-1) // end of seq. not reached?
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
    { // 'd' was not found
      return false;
    }
  }
  return true;
}

//---------------
template<class DATA_TYPE>
bool _simpleTrie<DATA_TYPE>::find(const std::vector<DATA_TYPE>& dataSeq)
{
  unsigned int i;
  _simpleTrie<DATA_TYPE> *t;
  DATA_TYPE d;

  if(dataSeq.size()==0) return false;
  t=this;
 
  for(i=0;i<dataSeq.size();++i) // for each position of the sequence...
  { 
    d=dataSeq[i];
    while(t->next!=NULL && t->data!=d) // search the key 'd'
    {
      t=t->next; 
    }
    if(t!=t->children && t->data==d) // 'd' was found?
    {
      if(i<dataSeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          return false;
        }
      }
    }
    else
    { // 'd' was not found
      return false;
    }
  }
  return true;  
}
//---------------
template<class DATA_TYPE>
_simpleTrie<DATA_TYPE>* _simpleTrie<DATA_TYPE>::getState(const std::vector<DATA_TYPE>& dataSeq)
{
  unsigned int i;
  _simpleTrie<DATA_TYPE> *t;
  DATA_TYPE d;

  if(dataSeq.size()==0) return NULL;
  
  t=this;
 
  for(i=0;i<dataSeq.size();++i) // for each position of the sequence...
  { 
    d=dataSeq[i];
    while(t->next!=NULL && t->data!=d) // search the key 'd'
    {
      t=t->next; 
    }
    if(t!=t->children && t->data==d) // 'd' was found?
    {
      if(i<dataSeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          return NULL;
        }
      }
    }
    else
    { // 'd' was not found
      return NULL;
    }
  }
  return t;  
}
//---------------
template<class DATA_TYPE>
DATA_TYPE& _simpleTrie<DATA_TYPE>::getData(void)
{
  return this->data;
}

//---------------
template<class DATA_TYPE>
size_t _simpleTrie<DATA_TYPE>::size(void)const
{
  size_t s;

  if(this!=this->children) // the node is initialized?
  {
    if(endOfSeq==true) s=1;
    else s=0;
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
template<class DATA_TYPE>
unsigned int _simpleTrie<DATA_TYPE>::height(void)const
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
template<class DATA_TYPE>
void _simpleTrie<DATA_TYPE>::height(unsigned int i,unsigned int& r)const
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
template<class DATA_TYPE>
std::vector<unsigned long> _simpleTrie<DATA_TYPE>::branchingFactor(void)const
{
  std::vector<unsigned long> result;
  unsigned long z=0,h;

  h=height();
  for(z=0;z<h;++z) result.push_back(0);
  branchingFactor(0,result);
  return result;
}
//---------------
template<class DATA_TYPE>
void _simpleTrie<DATA_TYPE>::branchingFactor(unsigned int i,std::vector<unsigned long>& r)const
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
template<class DATA_TYPE>
size_t _simpleTrie<DATA_TYPE>::countSparseNodes(void)const
{
  size_t sp=0;
  
  countSparseNodes(sp,true);
  return sp;
}
//---------------
template<class DATA_TYPE>
void _simpleTrie<DATA_TYPE>::countSparseNodes(size_t& sp,bool isFirst)const
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
template<class DATA_TYPE>
void _simpleTrie<DATA_TYPE>::clear(void)
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
template<class DATA_TYPE>
_simpleTrie<DATA_TYPE>::~_simpleTrie()
{
  this->clear();
}
//---------------

// const_iterator functions for the _simpleTrie class
//---------------
template<class DATA_TYPE>
typename _simpleTrie<DATA_TYPE>::const_iterator _simpleTrie<DATA_TYPE>::begin(void)const
{
  typename _simpleTrie<DATA_TYPE>::const_iterator iter(this);

  return iter;
}

//---------------
template<class DATA_TYPE>
typename _simpleTrie<DATA_TYPE>::const_iterator _simpleTrie<DATA_TYPE>::end(void)const
{
  typename _simpleTrie<DATA_TYPE>::const_iterator iter(NULL);
	
  return iter;
}

// const_iterator function definitions
//--------------------------
template<class DATA_TYPE>
bool _simpleTrie<DATA_TYPE>::const_iterator::operator++(void) //prefix
{
  bool end=false;
  bool itOk;
  
  while(!end)
  {
    itOk=this->iterate();
    if(!itOk) end=true;
    else
    {
      if((*vecTriePtr[vecTriePtr.size()-1]).endOfSeq) end=true;
    }
  }
  if(!itOk) return false;
  else return true;
}

//--------------------------
template<class DATA_TYPE>
bool _simpleTrie<DATA_TYPE>::const_iterator::iterate(void) //prefix
{
  _simpleTrie<DATA_TYPE>* tptr;

  if(vecTriePtr.size()==1 && vecTriePtr[0]==NULL)
  {// end reached or const_iterator not initialized
    return false;    
  }
  else
  {    
    if((*vecTriePtr[vecTriePtr.size()-1]).next==NULL)
    {// trie node has not alternative nodes
      vecTriePtr.pop_back();
      vecData.pop_back();
      if(vecTriePtr.size()==0)
      {
        vecTriePtr.push_back(NULL);
        return false;
      }
    }
    else
    {// trie node has alternative nodes
      tptr=(*vecTriePtr[vecTriePtr.size()-1]).next;
      vecTriePtr.pop_back();
      vecTriePtr.push_back(tptr);
      vecData.pop_back();
      vecData.push_back(tptr->data);

      while((*vecTriePtr[vecTriePtr.size()-1]).children!=NULL)
      {// reach the deepest child
        vecTriePtr.push_back((*vecTriePtr[vecTriePtr.size()-1]).children);
        vecData.push_back((*vecTriePtr[vecTriePtr.size()-1]).data);
      }
    }
    return true;
  }
}
//--------------------------
template<class DATA_TYPE>
bool _simpleTrie<DATA_TYPE>::const_iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class DATA_TYPE>
int _simpleTrie<DATA_TYPE>::const_iterator::operator==(const const_iterator& right)
{
 return (vecTriePtr==right.vecTriePtr);	
}
//--------------------------
template<class DATA_TYPE>
int _simpleTrie<DATA_TYPE>::const_iterator::operator!=(const const_iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class DATA_TYPE>
const std::vector<DATA_TYPE>*
_simpleTrie<DATA_TYPE>::const_iterator::operator->(void)const
{
  if(vecTriePtr.size()==1 && vecTriePtr[0]==NULL)
  {
    return NULL;
  }
  else  return (const std::vector<DATA_TYPE>*) &(vecData);
}
//--------------------------
template<class DATA_TYPE>
std::vector<DATA_TYPE> _simpleTrie<DATA_TYPE>::const_iterator::operator*(void)const
{
  return vecData;
}
//--------------------------

#endif

