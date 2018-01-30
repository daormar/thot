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
 * @file vecx_x_incr_enc.h
 * 
 * @brief class to encode high level source and target data.
 */

#ifndef _vecx_x_incr_enc
#define _vecx_x_incr_enc

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrEncoder.h"
#include <string.h>
#include <map>
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- vecx_x_incr_enc class

template<class HX,class X>
class vecx_x_incr_enc: public BaseIncrEncoder<std::vector<HX>,HX,std::vector<X>,X>
{
  public:

       // Constructor
   vecx_x_incr_enc();

       // Basic functions
   bool HighSrc_to_Src(const std::vector<HX>& hs,std::vector<X>& s);
       // Given a HSRCDATA object "hs" obtains its corresponding encoded
       // value in "s". Returns true if the encoding was successful
       // ("hs" exists in the vocabulary).  s stores the corresponding
       // code if exists, or a not valid one otherwise
   bool HighTrg_to_Trg(const HX& ht,X& t);
       // The same for HX objects
   bool Src_to_HighSrc(const std::vector<X>& s,std::vector<HX>& hs);
       // Performs the inverse process (s -> hs)
   bool Trg_to_HighTrg(const X& t,HX& ht);
       // The same for X objects (t -> ht)

   std::vector<X> genHSrcCode(const std::vector<HX> &hs);
       // Generates a code for a given std::vector<HX> object
   X genHTrgCode(const HX &ht);
       // The same for HX objects

   void addHSrcCode(const std::vector<HX> &hs,const std::vector<X> &s);
       // sets the codification for hs (hs->s)
   void addHTrgCode(const HX &ht,const X &t);
       // sets the codifcation for ht (ht->t)

       // Functions to load and print the model
   bool load(const char *prefixFileName);
       // Loads encoding information given a prefix file name
   bool print(const char *prefixFileName);
       // Prints encoding information

       // size and clear functions
   virtual unsigned int sizeSrc(void);
   virtual unsigned int sizeTrg(void);
   virtual void clear(void);
       // The clear function is called from the constructor. It is a
       // function of a great importance since it gives an initial value
       // to x_object by means of the C standard function memset. This
       // behaviour may not be adequate if X requires a specific
       // constructor. In such cases, the clear function should be
       // overridden.

       // Destructor
   ~vecx_x_incr_enc();
   
  protected:

   bool printSrc(const char *fileName);
       // prints source encoding information
   bool printTrg(const char *fileName);
       // prints target encoding information

   std::map<HX,X> hx_to_x;
   std::map<X,HX> x_to_hx;
   X x_object;
};

//--------------- Template function definitions

//---------------
template<class HX,class X>
vecx_x_incr_enc<HX,X>::vecx_x_incr_enc()
{
  clear();
}

//---------------
template<class HX,class X>
bool vecx_x_incr_enc<HX,X>::HighSrc_to_Src(const std::vector<HX>& hs,std::vector<X>& s)
{
  typename std::map<HX,X>::iterator iter;
  unsigned int i;
  bool retval=true;
  
  s.clear();
  for(i=0;i<hs.size();++i)
  {
    iter=hx_to_x.find(hs[i]);
    if(iter==hx_to_x.end())
    {
      X x_obj=x_object;
      ++x_obj;
      retval=false;
      s.push_back(x_obj);
    }
    else
    {
      s.push_back(iter->second);
    }
  }
  return retval;
}

//---------------
template<class HX,class X>
bool vecx_x_incr_enc<HX,X>::HighTrg_to_Trg(const HX& ht,X& t)
{
  typename std::map<HX,X>::iterator iter;

  iter=hx_to_x.find(ht);
  if(iter==hx_to_x.end())
  {
    t=x_object;
    ++t;
    return false;
  }
  else
  {
    t=iter->second;
    return true;
  }
}

//---------------
template<class HX,class X>
bool vecx_x_incr_enc<HX,X>::Src_to_HighSrc(const std::vector<X>& s,std::vector<HX>& hs)
{
  typename std::map<X,HX>::iterator iter;
  unsigned int i;

  hs.clear();
  for(i=0;i<s.size();++i)
  {
    iter=this->x_to_hx.find(s[i]);
    if(iter==this->x_to_hx.end())
    {
      return false;
    }
    else
    {
      hs.push_back(iter->second);
    }
  }
  return true;
}

//---------------
template<class HX,class X>
bool vecx_x_incr_enc<HX,X>::Trg_to_HighTrg(const X& t,HX& ht)
{
  typename std::map<X,HX>::iterator iter;

  iter=this->x_to_hx.find(t);
  if(iter==this->x_to_hx.end())
  {
    return false;
  }
  else
  {
    ht=iter->second;
    return true;
  }  
}

//---------------
template<class HX,class X>
std::vector<X> vecx_x_incr_enc<HX,X>::genHSrcCode(const std::vector<HX> &hs)
{
  typename std::map<HX,X>::iterator iter;
  std::vector<X> vecx;
  unsigned int i;

  for(i=0;i<hs.size();++i)
  {
    iter=hx_to_x.find(hs[i]);
    if(iter==hx_to_x.end())
    {
      ++x_object;
      vecx.push_back(x_object);
    }
    else
    {
      vecx.push_back(iter->second);
    }
  }
  return vecx;
}

//---------------
template<class HX,class X>
X vecx_x_incr_enc<HX,X>::genHTrgCode(const HX &ht)
{
  typename std::map<HX,X>::iterator iter;

  iter=hx_to_x.find(ht);
  if(iter==hx_to_x.end())
  {
    ++x_object;
    return x_object;
  }
  else
  {
    return iter->second;
  }
}

//---------------
template<class HX,class X>
void vecx_x_incr_enc<HX,X>::addHSrcCode(const std::vector<HX> &hs,const std::vector<X> &s)
{
  unsigned int i;

  if(hs.size()==s.size())
  {
    for(i=0;i<hs.size();++i)
    {
      this->hx_to_x[hs[i]]=s[i];
      this->x_to_hx[s[i]]=hs[i];
    }
  }
}

//---------------
template<class HX,class X>
void vecx_x_incr_enc<HX,X>::addHTrgCode(const HX &ht,const X &t)
{
  this->hx_to_x[ht]=t;
  this->x_to_hx[t]=ht;
}

//---------------
template<class HX,class X>
bool vecx_x_incr_enc<HX,X>::load(const char *prefixFileName)
{
  X x;
  HX hx;
  std::ifstream ifile;

  ifile.open(prefixFileName);
  if(!ifile)
  {
    std::cerr<< "Error in target vocabulary file "<<prefixFileName<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    while(ifile)
    {
      ifile>>hx>>x;
          //cout<<s<< " ||| " <<t<<" ||| "<<inf<<std::endl;
      this->hx_to_x[hx]=x;
      this->x_to_hx[x]=hx;
    }
    return THOT_OK;
  }  
}

//---------------
template<class HX,class X>
bool vecx_x_incr_enc<HX,X>::print(const char *prefixFileName)
{
  typename std::map<HX,X>::iterator iter;
  std::ofstream ofile;

  ofile.open(prefixFileName,std::ios::out);
  if(!ofile)
  {
    std::cerr<< "Error while opening target vocabulary file "<<prefixFileName<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    for(iter=hx_to_x.begin();iter!=hx_to_x.end();++iter)
    {
      ofile<<iter->first<<" "<<iter->second<<std::endl;
    }
    
    return THOT_OK;
  }  
}

//---------------
template<class HX,class X>
unsigned int vecx_x_incr_enc<HX,X>::sizeSrc(void)
{
  return hx_to_x.size();
}

//---------------
template<class HX,class X>
unsigned int vecx_x_incr_enc<HX,X>::sizeTrg(void)
{
  return hx_to_x.size();
}

//---------------
template<class HX,class X>
void vecx_x_incr_enc<HX,X>::clear(void)
{
  memset(&x_object,0,sizeof(x_object));
  hx_to_x.clear();
  x_to_hx.clear();
}

//---------------
template<class HX,class X>
vecx_x_incr_enc<HX,X>::~vecx_x_incr_enc()
{
  
}
  
#endif
