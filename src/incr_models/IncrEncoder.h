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
/* Module: IncrEncoder                                              */
/*                                                                  */
/* Prototype file: IncrEncoder                                      */
/*                                                                  */
/* Description: class to encode high level source and target data.  */
/*                                                                  */
/********************************************************************/

#ifndef _IncrEncoder
#define _IncrEncoder

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrEncoder.h"
#include <string.h>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- IncrEncoder class

template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
class IncrEncoder: public BaseIncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>
{
  public:
  
       // Constructor
   IncrEncoder();

       // Basic functions
   bool HighSrc_to_Src(const HSRCDATA& hs,SRCDATA& s);
       // Given a HSRCDATA object "hs" obtains its corresponding encoded
       // value in "s". Returns true if the encoding was successful
       // ("hs" exists in the vocabulary).  s stores the corresponding
       // code if exists, or a not valid one otherwise
   bool HighTrg_to_Trg(const HTRGDATA& ht,TRGDATA& t);
       // The same for HTRGDATA objects

   bool Src_to_HighSrc(const SRCDATA& s,HSRCDATA& hs);
       // Performs the inverse process (s -> hs)
   bool Trg_to_HighTrg(const TRGDATA& t,HTRGDATA& ht);
       // The same for TRGDATA objects (t -> ht)

   SRCDATA genHSrcCode(const HSRCDATA &hs);
       // Generates a code for a given HSRCDATA object
   TRGDATA genHTrgCode(const HTRGDATA &ht);
       // The same for HTRGDATA objects

   void addHSrcCode(const HSRCDATA &hs,const SRCDATA &s);
       // sets the codification for hs (hs->s)
   void addHTrgCode(const HTRGDATA &ht,const TRGDATA &t);
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
       // to the srcd and trgd variable by means of the C standard
       // function memset. This behaviour may not be adequate if X
       // requires a specific constructor. In such cases, the clear
       // function should be overridden.

       // Destructor
   ~IncrEncoder();
   
  protected:

   bool loadSrc(const char *fileName);
       // Loads source encoding information
   bool loadTrg(const char *fileName);
       // Loads target encoding information

   bool printSrc(const char *fileName);
       // prints source encoding information
   bool printTrg(const char *fileName);
       // prints target encoding information

   std::map<HSRCDATA,SRCDATA> hsrc_to_src;
   std::map<HTRGDATA,TRGDATA> htrg_to_trg;
   SRCDATA srcd;
   TRGDATA trgd;
};

//--------------- Template function definitions

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::IncrEncoder()
{
  clear();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::HighSrc_to_Src(const HSRCDATA& hs,SRCDATA& s)
{
  typename std::map<HSRCDATA,SRCDATA>::iterator iter;

  iter=hsrc_to_src.find(hs);
  if(iter==hsrc_to_src.end())
  {
    s=srcd;
    ++s;
    return false;
  }
  
  s=iter->second;
  return true;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::HighTrg_to_Trg(const HTRGDATA& ht,TRGDATA& t)
{
  typename std::map<HTRGDATA,TRGDATA>::iterator iter;

  iter=htrg_to_trg.find(ht);
  if(iter==htrg_to_trg.end())
  {
    t=trgd;
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
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::Src_to_HighSrc(const SRCDATA& s,HSRCDATA& hs)
{
  typename std::map<HSRCDATA,SRCDATA>::iterator iter;

  for(iter=hsrc_to_src.begin();iter!=hsrc_to_src.end();++iter)
  {
    if(iter->second==s)
    {
      hs=iter->first;
      return true;
    }
  }
  return false;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::Trg_to_HighTrg(const TRGDATA& t,HTRGDATA& ht)
{
  typename std::map<HTRGDATA,TRGDATA>::iterator iter;

  for(iter=htrg_to_trg.begin();iter!=htrg_to_trg.end();++iter)
  {
    if(iter->second==t)
    {
      ht=iter->first;
      return true;
    }
  }
  return false;
  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
SRCDATA IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::genHSrcCode(const HSRCDATA &hs)
{
  ++srcd;
  return srcd;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
TRGDATA IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::genHTrgCode(const HTRGDATA &ht)
{
  ++trgd;
  return trgd;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
void IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::addHSrcCode(const HSRCDATA &hs,const SRCDATA &s)
{
  hsrc_to_src[hs]=s;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
void IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::addHTrgCode(const HTRGDATA &ht,const TRGDATA &t)
{
  htrg_to_trg[ht]=t;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::load(const char *prefixFileName)
{
  std::string srcFileName;
  std::string trgFileName;
  std::string prefix=prefixFileName;
    
  srcFileName=prefix+".src";
  trgFileName=prefix+".trg";

  if(loadSrc(srcFileName.c_str())) return false;
  else
  {
    if(loadTrg(trgFileName.c_str())) return false;
    return true;
  }
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::loadSrc(const char *fileName)
{
  SRCDATA s;
  HSRCDATA hs;
  ifstream srcFile;

  srcFile.open(fileName);
  if(!srcFile)
  {
    cerr<< "Error in source vocabulary file "<<fileName<<endl;
    return false;
  }
  else
  {
    while(srcFile)
    {
      srcFile>>hs>>s;
          //cout<<s<< " ||| " <<t<<" ||| "<<inf<<endl;
      hsrc_to_src[hs]=s;      
    }
    return true;
  }
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::loadTrg(const char *fileName)
{
  TRGDATA t;
  HTRGDATA ht;
  ifstream trgFile;

  trgFile.open(fileName);
  if(!trgFile)
  {
    cerr<< "Error in target vocabulary file "<<fileName<<endl;
    return false;
  }
  else
  {
    while(trgFile)
    {
      trgFile>>ht>>t;
          //cout<<s<< " ||| " <<t<<" ||| "<<inf<<endl;
      htrg_to_trg[ht]=t;      
    }
    return true;
  }  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::print(const char *prefixFileName)
{
  std::string srcFileName;
  std::string trgFileName;
  std::string prefix=prefixFileName;
    
  srcFileName=prefix+".src";
  trgFileName=prefix+".trg";

  if(printSrc(srcFileName.c_str())) return false;
  else
  {
    if(printTrg(trgFileName.c_str())) return false;
    return true;
  }
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::printSrc(const char *fileName)
{
  typename std::map<HSRCDATA,SRCDATA>::iterator iter;
  ofstream srcFile;

  srcFile.open(fileName,ios::out);
  if(!srcFile)
  {
    cerr<< "Error while opening source vocabulary file "<<fileName<<endl;
    return false;
  }
  else
  {
    for(iter=hsrc_to_src.begin();iter!=hsrc_to_src.end();++iter)
    {
      srcFile<<iter->first<<" "<<iter->second<<endl;
    }
    return true;
  }
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
bool IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::printTrg(const char *fileName)
{
  typename std::map<HTRGDATA,TRGDATA>::iterator iter;
  ofstream trgFile;

  trgFile.open(fileName,ios::out);
  if(!trgFile)
  {
    cerr<< "Error while opening target vocabulary file "<<fileName<<endl;
    return false;
  }
  else
  {
    for(iter=htrg_to_trg.begin();iter!=htrg_to_trg.end();++iter)
    {
      trgFile<<iter->first<<" "<<iter->second<<endl;
    }
    
    return true;
  }  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
unsigned int IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::sizeSrc(void)
{
  return hsrc_to_src.size();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
unsigned int IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::sizeTrg(void)
{
  return htrg_to_trg.size();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
void IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::clear(void)
{
  memset(&srcd,0,sizeof(srcd));
  memset(&trgd,0,sizeof(trgd));
  hsrc_to_src.clear();
  htrg_to_trg.clear();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
IncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>::~IncrEncoder()
{
  
}
  
#endif
