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
/* Module: myVector                                                 */
/*                                                                  */
/* Prototype file: myVector.h                                       */
/*                                                                  */
/* Description: Declares the Vector class which is similar to the   */
/*              one provided by the STL.                            */
/*                                                                  */
/********************************************************************/

/**
 * @file myVector.h
 * @brief Defines the Vector class which is similar to
 * the one provided by the STL.
 */

#ifndef _myVector_h
#define _myVector_h

//--------------- Include files --------------------------------------

#ifndef USE_MY_STL_VECTOR

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <vector>
//#include <bits/stl_alloc.h>

#define Vector vector

using namespace std;

#endif

#ifdef USE_MY_STL_VECTOR
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

//--------------- Constants ------------------------------------------

#define RESIZE_FACTOR 2
#define INIT_CAPACITY 1

//--------------- typedefs -------------------------------------------

template<class T> class VectorBase;
	
//--------------- function declarations ------------------------------
template<class T>
ostream& operator << (ostream &outS,const VectorBase<T> &vect);

//--------------- Classes --------------------------------------------

//--------------- VectorBase class
/**
 * @brief base class for the Vector class.
 */
template<class T>
class VectorBase
{
 public:
	VectorBase(void);
	VectorBase(unsigned int s,T &t);
    VectorBase(const VectorBase<T>& v); // Copy-constructor
	void push_back(T &t);
	void pop_back(void);
	void reserve(unsigned int newCapacity);
	T& operator[](unsigned int x);
	const T& operator[](unsigned int x)const;
	VectorBase<T>& operator=(const VectorBase<T>& v);
    bool operator==(const VectorBase<T>& v)const;
    bool operator<(const VectorBase<T>& v)const;
	unsigned int size(void)const;
	unsigned int capacity(void)const;
	void clear(void);
	~VectorBase();
	
    // iterator
   class iterator;
   friend class iterator;
   class iterator
   {
    protected:
	   VectorBase<T>* vecPtr;
       unsigned int index;
	   
    public:
	   iterator(void){vecPtr=NULL; index=0;}
	   iterator(VectorBase<T>* vecptr,unsigned int i):vecPtr(vecptr)
       {index=i;
       }  
	   bool operator++(void); //prefix
       bool operator++(int);  //postfix
	   int operator==(const iterator& right); 
	   int operator!=(const iterator& right); 
	   T* operator->(void);
       T operator*(void);
   };
 
   // const_iterator
   class const_iterator;
   friend class const_iterator;
   class const_iterator: public VectorBase<T>::iterator
   {
    protected:
	   const VectorBase<T>* vecPtr;
       unsigned int index;
	   
    public:
	   const_iterator(void){vecPtr=NULL; index=0;}
	   const_iterator(const VectorBase<T>* vecptr,unsigned int i):vecPtr(vecptr)
       {
         index=i;
       }  
	   bool operator++(void); //prefix
       bool operator++(int);  //postfix
	   int operator==(const const_iterator& right); 
	   int operator!=(const const_iterator& right); 
	   const T* operator->(void)const;
       T operator*(void)const;
   };
   
   // ConditionalProbTable iterator-related functions
   iterator begin(void);
   iterator end(void);
   const_iterator begin(void)const;
   const_iterator end(void)const;
    
   friend ostream& operator<< <T> (ostream &outS,const VectorBase<T> &VectorBase);
	
 protected:
	unsigned int sizeOfVector;
	unsigned int capacityOfVector;
	T* container;
	
};

//--------------- Vector class
/**
 * @brief The Vector class is an alternative implementation of the
 * vector class provided by the STL.
 * @param T class of the objects
 * stored by the vector.
 */
template<class T>
class Vector: public VectorBase<T>
{ 

};
//--------------- Vector<unsigned int> class

class Vector<unsigned int>: public VectorBase<unsigned int>
{
 public:
    
   void push_back(unsigned int i){VectorBase<unsigned int>::push_back(i);} 
   void push_back(int i){VectorBase<unsigned int>::push_back((unsigned int) i);} 
    
};
//--------------- Vector<std::string> class

class Vector<std::string>: public VectorBase<std::string>
{
 public:
    
   void push_back(std::string str){VectorBase<std::string>::push_back(str);} 
   void push_back(char *c)
     {
       std::string str(c);
       VectorBase<std::string>::push_back(str);
     } 
};

//--------------- Template method definitions

//--------------- VectorBase template class method definitions

//-------------------------
template<class T>
VectorBase<T>::VectorBase(void)
{
 sizeOfVector=0;
 capacityOfVector=0;   
 container=NULL;   
 reserve(INIT_CAPACITY);   
}

//-------------------------
template<class T>
VectorBase<T>::VectorBase(unsigned int s,T &t)
{unsigned int i;
	
 sizeOfVector=s;
 capacityOfVector=0;   
 container=NULL;
 reserve(s);   
	
 for(i=0;i<s;++i) container[i]=t;
}
//-------------------------
template<class T>
VectorBase<T>::VectorBase(const VectorBase<T>& v)
{	
 sizeOfVector=0;
 capacityOfVector=0;   
 container=NULL;      
 *this=v;
}
//-------------------------
template<class T>
void VectorBase<T>::push_back(T &t)
{
 if(sizeOfVector+1<=capacityOfVector)
 {
  container[sizeOfVector]=t;
  ++sizeOfVector;
 }
 else
 {
  reserve((unsigned int) (capacityOfVector*RESIZE_FACTOR));	  
   
  container[sizeOfVector]=t;
  ++sizeOfVector; 
 }
}

//-------------------------
template<class T>
void VectorBase<T>::pop_back(void)
{
 unsigned int i;

 if(sizeOfVector>0)
 {
  //container[sizeOfVector-1].~T();
  --sizeOfVector; 
  if(sizeOfVector<= (unsigned int)(capacityOfVector)/RESIZE_FACTOR)
  {
   reserve((unsigned int) (capacityOfVector)/RESIZE_FACTOR);	  	 
  }
 }
}

//-------------------------
template<class T>
void VectorBase<T>::reserve(unsigned int newCapacity)
{
 unsigned int i;   
   
 if(newCapacity==0)
 {// Delete vector
  for(i=0;i<capacityOfVector;++i)
  {
    container[i].~T();
  }
  sizeOfVector=0;  
  capacityOfVector=0;  
  if(container!=NULL) 
  {
   free(container);  
   container=NULL;  
  }
 }
 else
 {// If the capacity of the vector decreases, 
  // call the destructor for the objects that will be eliminated
  for(i=newCapacity;i<capacityOfVector;++i)
      {container[i].~T();
      }
  // Reallocate Vector memory      
  container=(T*)realloc(container,sizeof(T)*newCapacity);	
  if(!container) 
    {printf("Out of memory\n");
     exit(1);
    }
  else 
  {// Call constructor for the new objects
   for(i=capacityOfVector;i<newCapacity;++i)
      {new(container+i) T;
      }
   // Set the new capacity   
   capacityOfVector=newCapacity;
  }
 }
 
 /*T* cont_aux;  
 unsigned int i;   
    
 if(newCapacity==0)
 {
  sizeOfVector=0;
  delete []container;   
 }
 else
 {   
  cont_aux=new T[newCapacity];  
  if(!cont_aux) 
    {printf("Out of memory\n");
     exit(1);
    }
  else 
  {
   for(i=0;i<sizeOfVector;++i) cont_aux[i]=container[i];   
   if(container!=NULL) delete []container;
   container=cont_aux;   
   capacityOfVector=newCapacity;
  }
 }*/

}

//-------------------------
template<class T>
T& VectorBase<T>::operator[](unsigned int x)
{
 return container[x];	
}

//-------------------------
template<class T>
const T& VectorBase<T>::operator[](unsigned int x)const
{
 return container[x];	
}

//-------------------------
template<class T>
VectorBase<T>& VectorBase<T>::operator=(const VectorBase<T>& v)
{
 unsigned int i;
	
 if(&v!=this)
 {
  sizeOfVector=v.size();
  if(capacityOfVector!=v.capacity())
  {
    capacityOfVector=0;
    reserve(v.capacity());
  }
  for(i=0;i<sizeOfVector;++i)	 
   {
     container[i]=v[i];
   }
 }
 return *this;

}
//-------------------------
template<class T>
bool VectorBase<T>::operator==(const VectorBase<T>& v)const
{
 unsigned int i;
	
 if(sizeOfVector!=v.size()) return false;
 if(capacityOfVector!=v.capacity()) return false;
 for(i=0;i<sizeOfVector;++i)	 
  {
    if(container[i]!=v[i]) return false;
  }    
 return true;
}
//-------------------------
template<class T>
bool VectorBase<T>::operator<(const VectorBase<T>& v)const
{
 unsigned int i;
					  
 if(v.size()<size()) return 0; if(size()<v.size()) return 1;	  
 for(i=0;i<size();++i)
 {
   if(v[i]<(*this)[i]) return 0; if((*this)[i]<v[i]) return 1;
 }
 return 0;
}

//-------------------------
template<class T>
unsigned int VectorBase<T>::size(void)const
{
 return sizeOfVector;
}

//-------------------------
template<class T>
unsigned int VectorBase<T>::capacity(void)const
{
 return capacityOfVector;
}

//-------------------------
template<class T>
void VectorBase<T>::clear(void)
{    
 sizeOfVector=0;  
 reserve(INIT_CAPACITY);	
}
//-------------------------
template<class T>
ostream& operator << (ostream &outS,const VectorBase<T> &vect)
{
 unsigned int i;
	
 for(i=0;i<vect.size();++i)
 {
  outS<<vect[i]<<endl;
 }
 return outS;
}

//-------------------------
template<class T>
VectorBase<T>::~VectorBase()
{
 reserve(0);
}

//--------------------------
template<class T>
typename VectorBase<T>::iterator VectorBase<T>::begin(void)
{
 typename VectorBase<T>::iterator iter(this,0);
	
 return iter;
}
//--------------------------
template<class T>
typename VectorBase<T>::iterator VectorBase<T>::end(void)
{
 typename VectorBase<T>::iterator iter(this,size());
	
 return iter;
}
//--------------------------
template<class T>
typename VectorBase<T>::const_iterator VectorBase<T>::begin(void)const
{
 typename VectorBase<T>::const_iterator citer(this,0);
	
 return citer;
}
//--------------------------
template<class T>
typename VectorBase<T>::const_iterator VectorBase<T>::end(void)const
{
 typename VectorBase<T>::const_iterator citer(this,size());
	
 return citer;
}
//--------------------------

// iterator function definitions
//--------------------------
template<class T>
bool VectorBase<T>::iterator::operator++(void) //prefix
{
 if(vecPtr!=NULL)
 {
  ++index;
  if(index>=vecPtr->size())
  {
    index=vecPtr->size();
    return false;
  }
  else return true;	 
 }
 else return false;
}
//--------------------------
template<class T>
bool VectorBase<T>::iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class T>
int VectorBase<T>::iterator::operator==(const iterator& right)
{
 return (vecPtr==right.vecPtr && index==right.index);	
}
//--------------------------
template<class T>
int VectorBase<T>::iterator::operator!=(const iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class T>
T* VectorBase<T>::iterator::operator->(void)const
{
 if(vecPtr!=NULL)
 {return (T*) &((*vecPtr)[index]);
 }
 else return NULL;
}
//--------------------------
template<class T>
T VectorBase<T>::iterator::operator*(void)const
{
 return ((*vecPtr)[index]);
}
//--------------------------

// const_iterator function definitions
//--------------------------
template<class T>
bool VectorBase<T>::const_iterator::operator++(void) //prefix
{
 if(vecPtr!=NULL)
 {
  ++index;
  if(index>=vecPtr->size())
  {
    index=vecPtr->size();
    return false;
  }
  else return true;	 
 }
 else return false;
}
//--------------------------
template<class T>
bool VectorBase<T>::const_iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class T>
int VectorBase<T>::const_iterator::operator==(const const_iterator& right)
{
 return (vecPtr==right.vecPtr && index==right.index);	
}
//--------------------------
template<class T>
int VectorBase<T>::const_iterator::operator!=(const const_iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class T>
const T* VectorBase<T>::const_iterator::operator->(void)
{
 if(vecPtr!=NULL)
 {
   return (T*) &((*vecPtr)[index]);
 }
 else return NULL;
}
//--------------------------
template<class T>
T VectorBase<T>::const_iterator::operator*(void)
{
 return ((*vecPtr)[index]);
}
//--------------------------

#endif
#endif
