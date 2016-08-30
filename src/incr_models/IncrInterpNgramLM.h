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
/* Module: IncrInterpNgramLM                                        */
/*                                                                  */
/* Prototype file: IncrInterpNgramLM.h                              */
/*                                                                  */
/* Description: Class to manage incremental encoded interpolated    */
/*              ngram language models p(x|Vector<x>).               */
/*                                                                  */
/********************************************************************/

#ifndef _IncrInterpNgramLM
#define _IncrInterpNgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <sstream>
#include "_incrInterpNgramLM.h"
#include "IncrJelMerNgramLM.h"
#include "CacheIncrJelMerNgramLM.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- IncrInterpNgramLM class

class IncrInterpNgramLM: public _incrInterpNgramLM<Count,Count>
{
 public:

  typedef _incrInterpNgramLM<Count,Count>::SrcTableNode SrcTableNode;
  typedef _incrInterpNgramLM<Count,Count>::TrgTableNode TrgTableNode;

      // Constructor
  IncrInterpNgramLM():_incrInterpNgramLM<Count,Count>()
  {
  }

      // Functions to load and print the model
  bool load(const char *fileName);
  bool print(const char *fileName);
      // Prints the interpolated in different files, using "fileName" as
      // prefix

      // basic vecx_x_iecpm function redefinitions
  Prob pTrgGivenSrc(const Vector<WordIndex>& s,const WordIndex& t);

  WordIndex getBosId(bool &found)const;
  WordIndex getEosId(bool &found)const;

  void setNgramOrder(int _ngramOrder);
  unsigned int getNgramOrder(void);

      // Destructor
  ~IncrInterpNgramLM();
   
 protected:

  Vector<std::string> lmTypeVec;
  Vector<std::string> modelFileNameVec;

  bool loadLmEntries(const char *fileName);
  bool loadLmEntry(std::string lmType,
                   std::string modelFileName,
                   std::string statusStr);
  bool loadWeights(const char *fileName);
  bool printLmEntries(const char *fileName);
  bool printWeights(const char *fileName);
};

//---------------


#endif
