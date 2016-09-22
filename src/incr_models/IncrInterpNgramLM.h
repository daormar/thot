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

#include "_incrInterpNgramLM.h"
#include "IncrJelMerNgramLM.h"
#include "_incrJelMerNgramLM.h"
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

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

      // Basic function redefinitions
  Prob pTrgGivenSrc(const Vector<WordIndex>& s,const WordIndex& t);

      // Functions to update model weights
  int updateModelWeights(const char *corpusFileName,
                         int verbose=0);

  WordIndex getBosId(bool &found)const;
  WordIndex getEosId(bool &found)const;

  void setNgramOrder(int _ngramOrder);
  unsigned int getNgramOrder(void);

      // Functions to load and print the model
  bool load(const char *fileName);
  bool print(const char *fileName);
      // Prints the interpolated in different files, using "fileName" as
      // prefix

      // Functions to load and print model weights
  bool loadWeights(const char *fileName);
  bool printWeights(const char *fileName);

      // Destructor
  ~IncrInterpNgramLM();
   
 protected:

  Vector<std::string> lmTypeVec;
  Vector<std::string> modelStatusVec;

  int updateModelCombinationWeights(const char *corpusFileName,
                                    int verbose=0);
  int new_dhs_eval(const char *corpusFileName,
                   FILE* tmp_file,
                   double* x,
                   double& obj_func);
  bool loadLmEntries(const char *fileName);
  bool loadLmEntry(std::string lmType,
                   std::string modelFileName,
                   std::string statusStr);
  bool printLmEntries(const char *fileName);
  bool printLm(const char* fileDescName,
               unsigned int entry_index);
  bool printInterModelWeights(const char *fileName);
  bool printIntraModelWeights(const char *fileName);
  std::string obtainFileNameForLmEntry(const std::string fileDescName,
                                       unsigned int entry_index);
  std::string obtainDirNameForLmEntry(const std::string fileDescName,
                                      unsigned int entry_index);
};

//---------------


#endif
