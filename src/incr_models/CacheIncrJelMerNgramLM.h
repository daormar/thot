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
/* Module: CacheIncrJelMerNgramLM                                   */
/*                                                                  */
/* Prototype file: CacheIncrJelMerNgramLM.h                         */
/*                                                                  */
/* Description: Class to manage encoded incremental                 */
/*              Jelinek-Mercer ngram language                       */
/*              models p(x|Vector<x>).                              */
/*                                                                  */
/********************************************************************/

#ifndef _CacheIncrJelMerNgramLM
#define _CacheIncrJelMerNgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "CacheLmIncrCpTable.h"
#include "_incrJelMerNgramLM.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- CacheIncrJelMerNgramLM class

class CacheIncrJelMerNgramLM: public _incrJelMerNgramLM<Count,Count>
{
 public:

  typedef _incrJelMerNgramLM<Count,Count>::SrcTableNode SrcTableNode;
  typedef _incrJelMerNgramLM<Count,Count>::TrgTableNode TrgTableNode;

      // Constructor
  CacheIncrJelMerNgramLM():_incrJelMerNgramLM<Count,Count>()
    {
          // Set new pointer to table
      this->tablePtr=new CacheLmIncrCpTable;
    }

      // Functions to load and print the model
  bool load(const char *fileName);
  bool print(const char *fileName);

      // clearTempVars() function
  void clearTempVars(void);

      // Destructor
  ~CacheIncrJelMerNgramLM();
   
 protected:
  
  bool loadCacheTable(const char *fileName);
  bool loadSecLevelTable(const char *fileName);
  bool printMainFile(const char *fileName);
  bool printSecLevelTable(const char *fileName);
  ostream& printSecLevelTable(ostream &outS);
};

//---------------

#endif
