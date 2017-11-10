/*
thot package for statistical machine translation
Copyright (C) 2013-2017 Daniel Ortiz-Mart\'inez, Adam Harasimowicz
 
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
/* Module: IncrLexTable                                             */
/*                                                                  */
/* Prototype file: IncrLexTable.h                                   */
/*                                                                  */
/* Description: Defines the IncrLexTable class.                     */
/*              IncrLexTable class stores an incremental            */
/*              lexical table.                                      */
/*                                                                  */
/********************************************************************/

#ifndef _IncrLexTable_h
#define _IncrLexTable_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <_incrLexTable.h>
#include <ErrorDefs.h>
#include <fstream>
#include <AwkInputStream.h>
#include <StatModelDefs.h>
#include <set>
#include <vector>

#ifdef THOT_DISABLE_SPACE_EFFICIENT_LEXDATA_STRUCTURES

#if __GNUC__>2
#include <ext/hash_map>
using __gnu_cxx::hash_map;
#else
#include <hash_map>
#endif

#else

#include <OrderedVector.h>

#endif

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrLexTable class

class IncrLexTable : public _incrLexTable
{
  public:

       // Constructor and destructor
   IncrLexTable(void);
   ~IncrLexTable(void);

       // Functions to handle lexNumer
   void setLexNumer(WordIndex s,
                    WordIndex t,
                    float f);
   float getLexNumer(WordIndex s,
                     WordIndex t,
                     bool& found);
   
   // Functions to handle lexDenom
   void setLexDenom(WordIndex s,
                    float f);
   float getLexDenom(WordIndex s,
                     bool& found);

   // Function to set lexical numerator and denominator
   void setLexNumDen(WordIndex s,
                     WordIndex t,
                     float num,
                     float den);

   // Functions to get translations for word
   bool getTransForTarget(WordIndex t,
                          std::set<WordIndex>& transSet);

       // load function
   bool load(const char* lexNumDenFile);
   
       // print function
   bool print(const char* lexNumDenFile);

       // clear() function
   void clear(void);

  protected:

       // Lexical model types
#ifdef THOT_DISABLE_SPACE_EFFICIENT_LEXDATA_STRUCTURES
   typedef hash_map<WordIndex,float> LexNumerElem;
#else   
   typedef OrderedVector<WordIndex,float> LexNumerElem;
#endif
   
   typedef std::vector<LexNumerElem> LexNumer;
   typedef std::vector<std::pair<bool,float> >LexDenom;

   LexNumer lexNumer;
   LexDenom lexDenom;

       // load and print auxiliary functions
   bool loadBin(const char* lexNumDenFile);
   bool loadPlainText(const char* lexNumDenFile);
   bool printBin(const char* lexNumDenFile);
   bool printPlainText(const char* lexNumDenFile);
};

#endif
