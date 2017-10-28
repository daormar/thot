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
/* Module: Bitset                                                   */
/*                                                                  */
/* Prototypes file: Bitset.h                                        */
/*                                                                  */
/* Description: Defines a Bitset class similar to the one offered   */
/*              by the STL.                                         */    
/*                                                                  */
/********************************************************************/

#ifndef _Bitset_h
#define _Bitset_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#ifdef THOT_DONT_USE_MY_BITSET
# include <bitset>
# define Bitset bitset

template<size_t N>
bool operator < (const Bitset<N> &left,const Bitset<N> &right);

template<size_t N>
bool operator > (const Bitset<N> &left,const Bitset<N> &right);

//-------------------------
template<size_t N>
bool operator< (const Bitset<N> &left,const Bitset<N> &right)
{
  for(unsigned int i=0;i<N;++i)	
  {
    if(left.test(i)<right.test(i)) return 1;
    if(right.test(i)<left.test(i)) return 0;
  }
  return 0;
}

//-------------------------
template<size_t N>
bool operator> (const Bitset<N> &left,const Bitset<N> &right)
{
  for(unsigned int i=0;i<N;++i)	
  {
    if(left.test(i)>right.test(i)) return 1;
    if(right.test(i)>left.test(i)) return 0;
  }
  return 0;
}

#else

#include <limits.h>
#include <iostream>
#include <iomanip>
#include <fstream>

//--------------- Constants ------------------------------------------

#define NUM_BYTES_INT sizeof(unsigned int)
#define NUM_BITS_INT 8*NUM_BYTES_INT
#define NUM_BYTES_LONG sizeof(unsigned long)
#define NUM_INTS_PER_LONG sizeof(unsigned long)/sizeof(unsigned int)
#define NUM_WORDS(__n) ((__n) < 1 ? 1 :(__n-1+(NUM_BITS_INT))/(NUM_BITS_INT))

//--------------- Function declarations ------------------------------
template<size_t N> class Bitset;

template<size_t N>
std::ostream& operator << (std::ostream &outS,const Bitset<N> &bs);

template<size_t N>
bool operator < (const Bitset<N> &left,const Bitset<N> &right);

template<size_t N>
bool operator > (const Bitset<N> &left,const Bitset<N> &right);

//--------------- Classes --------------------------------------------

//--------------- Bitset template class

template<size_t N>
class Bitset
{
 public:
  Bitset(void);
  Bitset(unsigned int ui);
  Bitset(unsigned long ul);
  Bitset(unsigned int *uiptr);
  bool operator== (const Bitset<N> &right)const;
  bool operator!= (const Bitset<N> &right)const;
  friend bool operator < <N> (const Bitset<N> &left,const Bitset<N> &right);
  friend bool operator > <N> (const Bitset<N> &left,const Bitset<N> &right);
  Bitset<N>& reset(void);
  Bitset<N>& set(void);
  Bitset<N>& reset(size_t n);
  Bitset<N>& set(size_t n, int val = 1);
  bool test(size_t n) const;
  Bitset<N>& operator=(const Bitset<N> &right);
  Bitset<N>& operator+(const Bitset<N> &right);
  Bitset<N>& operator&=(const Bitset<N> &right);
  Bitset<N>& operator|=(const Bitset<N> &right);
  Bitset<N>& operator^=(const Bitset<N> &right);
  Bitset<N>& operator>>=(size_t pos);  
  Bitset<N> operator>>(size_t pos) const;
  //Bitset<N>& operator<<=(size_t pos);  
  //Bitset<N> operator<<(size_t pos) const;
  void operator++(void);
  size_t count(unsigned int J=N)const;
  unsigned int to_uint(void)const;
  unsigned long to_ulong(void)const;
  friend std::ostream& operator << <N> (std::ostream &outS,const Bitset<N> &bs);
 
 private:   
  unsigned int words[NUM_WORDS(N)];
};

//--------------- Bitset template class method definitions

template<size_t N>
Bitset<N>::Bitset(void)
{
 reset();
}

//---------------------------------------
template<size_t N>
Bitset<N>::Bitset(unsigned int ui)
{
 reset();
 words[0]=ui; 
}

//---------------------------------------
template<size_t N>
Bitset<N>::Bitset(unsigned long ul)
{
  if(sizeof(unsigned int)==sizeof(unsigned long))
  {
    reset();
    words[0]=(unsigned int ) ul;
  }
  else
  {
    unsigned int n=NUM_BITS_INT;
    reset();
    for(unsigned int i=0;i<NUM_INTS_PER_LONG;++i)
    {
      words[i]=(unsigned int) ul;
      ul=ul>>n;
    }
  }
}

//---------------------------------------
template<size_t N>
Bitset<N>::Bitset(unsigned int *uiptr)
{
  for(unsigned int i=0;i<NUM_WORDS(N);++i)
  {
    words[i]=uiptr[i];
  }
}

//---------------------------------------
template<size_t N>
bool Bitset<N>::operator== (const Bitset<N> &right)const
{
 unsigned int i;
    
 for(i=0;i<NUM_WORDS(N);++i)
 {
   if(words[i]!=right.words[i]) return false;
 }
 return true;
}

//---------------------------------------
template<size_t N>
bool Bitset<N>::operator!= (const Bitset<N> &right)const
{
 unsigned int i;
    
 for(i=0;i<NUM_WORDS(N);++i)
 {
   if(words[i]!=right.words[i]) return true;
 }
 return false;
}

//---------------------------------------
template<size_t N>
Bitset<N>& Bitset<N>::reset(void)
{
  unsigned int i;
    
  for(i=0;i<NUM_WORDS(N);++i){words[i]=0;}
 
  return *this;   
}

//---------------------------------------
template<size_t N>
Bitset<N>& Bitset<N>::set(void)
{
  unsigned int i;
    
  for(i=0;i<NUM_WORDS(N);++i)
  {
    words[i]=UINT_MAX;
  }
 
  return *this;
}

//---------------------------------------
template<size_t N>
Bitset<N>& Bitset<N>::reset(size_t n)
{
  unsigned int aux,i; 
 
  i=NUM_WORDS(n+1)-1; 
  n=n-(i*NUM_BITS_INT);
  aux=1;
  aux=aux<<n;
  aux=~aux;
  words[i]=words[i] & aux;    
  return *this;
}

//---------------------------------------
template<size_t N> 
Bitset<N>& Bitset<N>::set(size_t n, int val)
{
  unsigned int i;
 
  if(val==0) reset(n);
  else
  {     
    i=NUM_WORDS(n+1)-1;   
    n=n-(i*NUM_BITS_INT);
  
    words[i]=words[i] | (1<<n);    
  }

  return *this;
}

//---------------------------------------
template<size_t N> 
bool Bitset<N>::test(size_t n) const
{
  unsigned int i;
    
  i=NUM_WORDS(n+1)-1;   
     
  return (words[i]>>(n-(i*NUM_BITS_INT))) & 1;
}

//---------------------------------------
template<size_t N> 
Bitset<N>& Bitset<N>::operator=(const Bitset<N> &right)
{
  unsigned int i;
    
  for(i=0;i<NUM_WORDS(N);++i)
  {
    words[i]=right.words[i];
  }

  return *this;   
}

//---------------------------------------
template<size_t N> 
Bitset<N>& Bitset<N>::operator+(const Bitset<N> &right)
{ 
 unsigned int i,lastbit,n=1;   
    
 n=n<<((NUM_BITS_INT)-1);   
 for(i=0;i<NUM_WORDS(N);++i)
 {
  lastbit=words[i]&n;
  words[i]=words[i]+right.words[i];   
  if(lastbit==n && (words[i]&n)==0 && i<NUM_WORDS(N)-1)
    ++words[i+1];     
 }     

 return *this;   
}

//---------------------------------------
template<size_t N> 
Bitset<N>& Bitset<N>::operator&=(const Bitset<N> &right)
{
 unsigned int i;
    
 for(i=0;i<NUM_WORDS(N);++i)
 {
   words[i]=words[i]& right.words[i];
 }

 return *this;   
}
//---------------------------------------
template<size_t N> 
Bitset<N>& Bitset<N>::operator|=(const Bitset<N> &right)
{
 unsigned int i;
    
 for(i=0;i<NUM_WORDS(N);++i)
 {
   words[i]=words[i] | right.words[i];
 }
 
 return *this;   
}

//---------------------------------------
template<size_t N> 
Bitset<N>& Bitset<N>::operator^=(const Bitset<N> &right)
{
 unsigned int i;
    
 for(i=0;i<NUM_WORDS(N);++i)
 {
   words[i]=words[i] ^ right.words[i];
 }
 
 return *this;   
}

//---------------------------------------
template<size_t N>
Bitset<N>& Bitset<N>::operator>>=(size_t pos)
{
 size_t i;
 unsigned int j;
 unsigned int w=0,aux;

 j=NUM_WORDS(pos+1)-1;   
 pos=pos-(j*NUM_BITS_INT);

 for(i=0;i<pos;++i)
 {
   w=w<<1; w=w|1;
 }
    
 for(i=0;i<NUM_WORDS(N);++i)
 {
   if(pos<(NUM_BITS_INT) && i+j<(NUM_WORDS(N)))
   {
     words[i]=words[i+j]>>pos;
     if(i+j+1<(NUM_WORDS(N)))
     {
       aux=words[i+j+1]&w;
       aux=aux<<((8*NUM_BYTES_INT)-pos);
       words[i]=words[i]|aux;      
     }
   }
   else words[i]=0;      
 }
 
 return *this;   
}

//---------------------------------------
template<size_t N>
Bitset<N> Bitset<N>::operator>>(size_t pos) const
{
 Bitset<N> b;
 
 b=*this;
 return b>>=pos;
}

//---------------------------------------
template<size_t N>
void Bitset<N>::operator++(void)
{
 unsigned int i,lastbit,n=1;   
    
 n=n<<((NUM_BITS_INT)-1);   
 for(i=0;i<NUM_WORDS(N);++i)
 {
  lastbit=words[i]&n;
  ++words[i];   
  if(!(lastbit==n && (words[i]&n)==0))
    break;     
 }     
}

//---------------------------------------
template<size_t N>
size_t Bitset<N>::count(unsigned int J)const
{
 unsigned int i,j,w=1;
 size_t c;

 c=0;   
 i=0; j=0;   
 while(j<J)
 {
  if((words[i]&w)!=0) ++c;
  w=w<<1;
  if(w==0) 
  {
    w=1;  
    ++i;   
  }
  ++j;  
 } 
 return c;
}

//---------------------------------------
template<size_t N>
unsigned int Bitset<N>::to_uint(void)const
{
 return words[0];  
}

//---------------------------------------
template<size_t N>
unsigned long Bitset<N>::to_ulong(void)const
{
  if(sizeof(unsigned int)==sizeof(unsigned long))
  {
    return (unsigned long) words[0];
  }
  else
  {
    unsigned long result=0;
    unsigned int n=NUM_BITS_INT;
    
    for(unsigned int i=NUM_INTS_PER_LONG;i>0;--i)
    {
      result=result<<n;
      result+=words[i-1];
    }  
    return result;
  }
}

//-------------------------
template<size_t N>
bool operator< (const Bitset<N> &left,const Bitset<N> &right)
{
 unsigned int i;
    
 for(i=NUM_WORDS(N);i>0;--i)
 {
   if(left.words[i-1]<right.words[i-1]) return true;
   if(right.words[i-1]<left.words[i-1]) return false;
 }
 return false;
}

//-------------------------
template<size_t N>
bool operator> (const Bitset<N> &left,const Bitset<N> &right)
{
 unsigned int i;
    
 for(i=NUM_WORDS(N);i>0;--i)
 {
   if(left.words[i-1]<right.words[i-1]) return false;
   if(right.words[i-1]<left.words[i-1]) return true;
 }
 return false;
}

//-------------------------
template<size_t N>
std::ostream& operator << (std::ostream &outS,const Bitset<N> &bs)
{
  unsigned int i,j,n=1,w;
  unsigned int first_one=0;
  
  n=n<<((8*NUM_BYTES_INT)-1);
   
  for(i=NUM_WORDS(N);i>0;--i)
  {
    w=bs.words[i-1];
    for(j=0;j<NUM_BITS_INT;++j)
    {      
      if((n&w)==0)
      {
        if(first_one!=0)
          outS<<0;
      }
      else
      {
        if(!first_one) first_one=1;
        outS<<1;
      }
      w=w<<1;       
    }
  }
  if(first_one==0) outS<<0;
  return outS;	
}

#endif

#endif
