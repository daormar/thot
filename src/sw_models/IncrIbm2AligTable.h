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
/* Module: IncrIbm2AligTable                                        */
/*                                                                  */
/* Prototype file: IncrIbm2AligTable.h                              */
/*                                                                  */
/* Description: Defines the IncrIbm2AligTable class.                */
/*              IncrIbm2AligTable class stores an incremental       */
/*              IBM 2 alignment table.                              */
/*                                                                  */
/********************************************************************/

#ifndef _IncrIbm2AligTable_h
#define _IncrIbm2AligTable_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <ErrorDefs.h>
#include <fstream>
#include <awkInputStream.h>
#include <StatModelDefs.h>
#include "aSource.h"
#include "aSourceHashF.h"
#include <vector>

#if __GNUC__>2
#include <ext/hash_map>
using __gnu_cxx::hash_map;
#else
#include <hash_map>
#endif

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrIbm2AligTable class

class IncrIbm2AligTable
{
  public:

       // Constructor
   IncrIbm2AligTable(void);   

       // Functions to handle aligNumer
   void setAligNumer(aSource as,
                     PositionIndex i,
                     float f);
   float getAligNumer(aSource as,
                      PositionIndex i,
                      bool& found);
   
   // Functions to handle aligDenom
   void setAligDenom(aSource as,
                     float f);
   float getAligDenom(aSource as,
                      bool& found);

   // Function to set lexical numerator and denominator
   void setAligNumDen(aSource as,
                      PositionIndex i,
                      float num,
                      float den);

       // load function
   bool load(const char* lexNumDenFile);
   
       // print function
   bool print(const char* lexNumDenFile);

       // clear() function
   void clear(void);
   
  protected:

       // Alignment model types
   typedef hash_map<aSource,float,aSourceHashF> AligNumerElem;
   typedef std::vector<AligNumerElem> AligNumer;
   typedef hash_map<aSource,float,aSourceHashF> AligDenom;

   AligNumer aligNumer;
   AligDenom aligDenom;

       // load and print auxiliary functions
   bool loadBin(const char* lexNumDenFile);
   bool loadPlainText(const char* lexNumDenFile);
   bool printBin(const char* lexNumDenFile);
   bool printPlainText(const char* lexNumDenFile);
};

#endif
