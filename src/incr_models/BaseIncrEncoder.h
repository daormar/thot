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
/* Module: BaseIncrEncoder                                          */
/*                                                                  */
/* Prototype file: BaseIncrEncoder                                  */
/*                                                                  */
/* Description: Abstract class to encode high level source and      */
/*              target data.                                        */
/*                                                                  */
/********************************************************************/

#ifndef _BaseIncrEncoder
#define _BaseIncrEncoder

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <ErrorDefs.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseIncrEncoder class

template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA>
class BaseIncrEncoder
{
  public:

      // Basic functions
   virtual bool HighSrc_to_Src(const HSRCDATA& hs,SRCDATA& s)=0;
       // Given a HSRCDATA object "hs" obtains its corresponding
       // encoded value in "s". Returns true if the encoding
       // was successful ("hs" exists in the vocabulary).
   virtual bool HighTrg_to_Trg(const HTRGDATA& ht,TRGDATA& t)=0;
       // The same for HTRGDATA objects

   virtual bool Src_to_HighSrc(const SRCDATA& s,HSRCDATA& hs)=0;
       // Performs the inverse process (s -> hs)
   virtual bool Trg_to_HighTrg(const TRGDATA& t,HTRGDATA& ht)=0;
       // The same for TRGDATA objects (t -> ht)

   virtual SRCDATA genHSrcCode(const HSRCDATA &hs)=0;
       // Generates a code for a given HSRCDATA object
   virtual TRGDATA genHTrgCode(const HTRGDATA &ht)=0;
       // The same for HTRGDATA objects

   virtual void addHSrcCode(const HSRCDATA &hs,const SRCDATA &s)=0;
       // sets the codification for hs (hs->s)
   virtual void addHTrgCode(const HTRGDATA &ht,const TRGDATA &t)=0;
       // sets the codifcation for ht (ht->t)

       // Functions to load and print the model
   virtual bool load(const char *prefixFileName)=0;
       // Load encoding information given a prefix file name
   virtual bool print(const char *prefixFileName)=0;
       // Prints encoding information

       // size and clear functions
   virtual unsigned int sizeSrc(void)=0;
   virtual unsigned int sizeTrg(void)=0;
   virtual void clear(void)=0;
   
   virtual ~BaseIncrEncoder(){};
};

#endif
