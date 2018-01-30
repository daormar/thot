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
 * @file OrderedVector.h
 * 
 * @brief Implements an ordered vector that allows to search elements
 * with logarithmic cost.
 */

#ifndef _OrderedVector_h
#define _OrderedVector_h

//--------------- Include files --------------------------------------

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

	
//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- OrderedVector class
/**
 * @brief The OrderedVector class implements an ordered vector that
 * allows to search elements with logarithmic cost.
 */

template<class KEY,class DATA,class KEY_ORDER_REL=std::less<KEY> >
class OrderedVector
{
 public:

     // Forward declaration of class iterator
    class iterator;
 
	OrderedVector(void);
    OrderedVector(const OrderedVector<KEY,DATA,KEY_ORDER_REL>& ov);
     // Copy-constructor
    OrderedVector<KEY,DATA,KEY_ORDER_REL>&
     operator=(const OrderedVector<KEY,DATA,KEY_ORDER_REL>& ov);
	DATA* push(const KEY& k,const DATA &d);
    DATA* insert(const KEY& k,const DATA &d);
	void pop(void);
    const std::pair<KEY,DATA>& top(void);
    DATA* findPtr(const KEY& k);
    iterator find(const KEY& k);
    DATA& operator[](const KEY& k);
    bool empty(void)const;
	size_t size(void)const;
	void clear(void);
	~OrderedVector();

    class PairKeyDataOrderRel
    {
      public:
        KEY_ORDER_REL kOrderRel;
        bool operator() (const std::pair<KEY,DATA>& a,
                         const std::pair<KEY,DATA>& b)
        {
          return kOrderRel(a.first,b.first);
        }
    };
    // Constant iterator
    class const_iterator
    {
     protected:
      std::pair<KEY,DATA>* currVecPtr;
      std::pair<KEY,DATA>* endVecPtr;
	   
     public:
      const_iterator(void){currVecPtr=NULL; endVecPtr=NULL;}
      const_iterator(std::pair<KEY,DATA>* _currVecPtr,
                     std::pair<KEY,DATA>* _endVecPtr):currVecPtr(_currVecPtr),endVecPtr(_endVecPtr)
        {
        }  
      bool operator++(void); //prefix
      bool operator++(int);  //postfix
      int operator==(const const_iterator& right); 
      int operator!=(const const_iterator& right); 
      const std::pair<const KEY,DATA>* operator->(void)const;
      std::pair<KEY,DATA> operator*(void)const;
    };

   // Constant iterator functions for the OrderedVector class
   const_iterator begin(void)const;
   const_iterator end(void)const;

     // iterator
   class iterator: public const_iterator
   {
    protected:
	   
    public:
     iterator(void):const_iterator()
       {
       }
     iterator(std::pair<KEY,DATA>* _currVecPtr,
              std::pair<KEY,DATA>* _endVecPtr):const_iterator(_currVecPtr,_endVecPtr)
       {
       }  
     bool operator++(void); //prefix
     bool operator++(int);  //postfix
     std::pair<const KEY,DATA>* operator->(void);
     std::pair<KEY,DATA> operator*(void)const;
    };

   // Iterator functions for the OrderedVector class
   iterator begin(void);
   iterator end(void);

 protected:

   std::pair<KEY,DATA> *vec;
 
   size_t sizeVec;

   void alloc(size_t newSize);
   bool findIndex(const KEY& k,size_t &index)const;
};

//--------------- Template method definitions

//--------------- OrderedVector template class method definitions

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
OrderedVector<KEY,DATA,KEY_ORDER_REL>::OrderedVector(void)
{
  vec=NULL;
  sizeVec=0;
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
OrderedVector<KEY,DATA,KEY_ORDER_REL>::OrderedVector(const OrderedVector<KEY,DATA,KEY_ORDER_REL>& ov)
{
  vec=NULL;
  sizeVec=0;
  alloc(ov.size());
  for(unsigned int i=0;i<ov.size();++i)
  {
    vec[i]=ov.vec[i];
  }
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
OrderedVector<KEY,DATA,KEY_ORDER_REL>&
OrderedVector<KEY,DATA,KEY_ORDER_REL>::operator=(const OrderedVector<KEY,DATA,KEY_ORDER_REL>& ov)
{
  if(&ov!=this)
  {
    clear();
    alloc(ov.size());
    for(unsigned int i=0;i<ov.size();++i)
    {
      vec[i]=ov.vec[i];
    }
  }
  return *this;
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
void OrderedVector<KEY,DATA,KEY_ORDER_REL>::alloc(size_t newSize)
{
  size_t i;   
  if(newSize==0)
  {// Delete vector
    if(vec!=NULL)
    {
      for(i=0;i<sizeVec;++i)
      {
        using std::pair;
        vec[i].~pair<KEY,DATA>();
      }
      sizeVec=0;  
      free(vec);  
      vec=NULL;
    }
  }
  else
  {
        // If the capacity of the vector decreases, call the destructor
        // for the objects that will be eliminated
    for(i=newSize;i<sizeVec;++i)
    {
      using std::pair;
      vec[i].~pair<KEY,DATA>();
    }
        // Reallocate Vector memory      
    vec=(std::pair<KEY,DATA>*)realloc(vec,sizeof(std::pair<KEY,DATA>)*newSize);	
    if(!vec) 
    {
      printf("Out of memory\n");
      exit(1);
    }
    else 
    {// Call constructor for the new objects      
      for(i=sizeVec;i<newSize;++i)
      {
        new(vec+i) std::pair<KEY,DATA>;
      }
          // Set the new capacity   
      sizeVec=newSize;
    }
  }
}

//-------------------------

template<class KEY,class DATA,class KEY_ORDER_REL>
DATA* OrderedVector<KEY,DATA,KEY_ORDER_REL>::push(const KEY& k,
                                                  const DATA& d)
{
  std::pair<KEY,DATA> pkd;
  bool found;
  size_t index;
  
  pkd.first=k;
  pkd.second=d;

  if(size()==0)
  {
    alloc(1);
    vec[0]=pkd;
    return (DATA*) &vec[0].second;
  }
  else
  {
    found=findIndex(k,index);
    if(found)
    {
      vec[index].second=d;
      return (DATA*) &vec[index].second;
    }
    else
    {
      alloc(size()+1);
      memmove((std::pair<KEY,DATA> *)&vec[index+1],(std::pair<KEY,DATA> *)&vec[index],(size()-index-1)*sizeof(std::pair<KEY,DATA>));
      memset((std::pair<KEY,DATA> *)&vec[index],0,sizeof(std::pair<KEY,DATA>));
      new(vec+index) std::pair<KEY,DATA>;
          // The vec[index] object is invalid, since it has been copied
          // to vec[index+1], create a new object in this address.
      vec[index]=pkd;
      return (DATA*) &vec[index].second;
   }
  }
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
DATA* OrderedVector<KEY,DATA,KEY_ORDER_REL>::insert(const KEY& k,
                                                    const DATA& d)
{
  return push(k,d);
}
//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
void OrderedVector<KEY,DATA,KEY_ORDER_REL>::pop(void)
{
  if(size()>0)  
  {
    alloc(size()-1);
  }
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
const std::pair<KEY,DATA>& OrderedVector<KEY,DATA,KEY_ORDER_REL>::top(void)
{
  return (const std::pair<KEY,DATA>&) vec[size()-1];
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
DATA* OrderedVector<KEY,DATA,KEY_ORDER_REL>::findPtr(const KEY& k)
{
  bool found;
  size_t index;

  found=findIndex(k,index);
  if(found)
    return (DATA*) &vec[index].second;
  else
    return NULL;
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator
OrderedVector<KEY,DATA,KEY_ORDER_REL>::find(const KEY& k)
{
  bool found;
  size_t index;

  found=findIndex(k,index);
  if(found)
  {
    typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator iter(vec+index,vec+sizeVec);	
    return iter;
  }
  else
    return end();
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
DATA& OrderedVector<KEY,DATA,KEY_ORDER_REL>::operator[](const KEY& k)
{
  bool found;
  size_t index;

  found=findIndex(k,index);
  if(found)
    return vec[index].second;
  else
  {
    DATA d;
    return *(insert(k,d));
  }
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
bool OrderedVector<KEY,DATA,KEY_ORDER_REL>::findIndex(const KEY& k,
                                                      size_t &index)const
{
  if(size()>0)
  {
    KEY_ORDER_REL kOrderRel;
    size_t left,right,mid;

    left=0;
    right=size();
    while(left<right)
    {
      mid=((left+right)/2);
    
      if(kOrderRel(k,vec[mid].first)) right=mid;
      else
      {
        if(kOrderRel(vec[mid].first,k)) left=mid+1;
        else
        {
          index=mid;
          return true;
        }
      }
    }
    if(left==size()) index=left;
    else
    {
      if(kOrderRel(k,vec[left].first)) index=left;
      else index=left+1;
    }
    return false;
  }
  else
  {
    index=0;
    return false;
  }
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
bool OrderedVector<KEY,DATA,KEY_ORDER_REL>::empty(void)const
{
  if (this->size()==0) return true;
  else return false;
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
size_t OrderedVector<KEY,DATA,KEY_ORDER_REL>::size(void)const
{
  return sizeVec;
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
void OrderedVector<KEY,DATA,KEY_ORDER_REL>::clear(void)
{
  alloc(0);
}

//-------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
OrderedVector<KEY,DATA,KEY_ORDER_REL>::~OrderedVector()
{
  clear();
}

//// Iterator functions for the OrderedVector class
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator
OrderedVector<KEY,DATA,KEY_ORDER_REL>::begin(void)const
{
  typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator iter(vec,vec+sizeVec);
  return iter;
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator
OrderedVector<KEY,DATA,KEY_ORDER_REL>::end(void)const
{
  typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator iter(vec+sizeVec,vec+sizeVec);
  return iter;
}

//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator
OrderedVector<KEY,DATA,KEY_ORDER_REL>::begin(void)
{
  typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator iter(vec,vec+sizeVec);	
  return iter;
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator
OrderedVector<KEY,DATA,KEY_ORDER_REL>::end(void)
{
  typename OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator iter(vec+sizeVec,vec+sizeVec);
  return iter;
}

// const_iterator function definitions
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
bool OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator::operator++(void)
{
  if(currVecPtr!=NULL)
  {
    if(currVecPtr==endVecPtr)
    {
      return false;
    }
    else
    {
      ++currVecPtr;
      if(currVecPtr==endVecPtr)
        return false;
      else return true;
    }
  }
  else return false;
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
bool OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator::operator++(int) 
{
 return operator++();
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
int OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator::operator==(const const_iterator& right)
{
 return (currVecPtr==right.currVecPtr && endVecPtr==right.endVecPtr);	
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
int OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator::operator!=(const const_iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
const std::pair<const KEY,DATA>*
OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator::operator->(void)const
{
  return (std::pair<const KEY,DATA>*) currVecPtr;
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
std::pair<KEY,DATA> OrderedVector<KEY,DATA,KEY_ORDER_REL>::const_iterator::operator*(void)const
{
 return *currVecPtr;
}
//--------------------------

// iterator function definitions
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
bool OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator::operator++(void)
{
  if(this->currVecPtr!=NULL)
  {
    if(this->currVecPtr==this->endVecPtr)
    {
      return false;
    }
    else
    {
      ++this->currVecPtr;
      if(this->currVecPtr==this->endVecPtr)
        return false;
      else return true;
    }
  }
  else return false;
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
bool OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator::operator++(int) 
{
 return operator++();
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
std::pair<const KEY,DATA>*
OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator::operator->(void)
{
  return (std::pair<const KEY,DATA>*) this->currVecPtr;
}
//--------------------------
template<class KEY,class DATA,class KEY_ORDER_REL>
std::pair<KEY,DATA> OrderedVector<KEY,DATA,KEY_ORDER_REL>::iterator::operator*(void)const
{
 return *this->currVecPtr;
}
//--------------------------

#endif
