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
/* Module: WordPredictor                                            */
/*                                                                  */
/* Prototypes file: WordPredictor.h                                 */
/*                                                                  */
/* Description: Declares the WordPredictor class,                   */
/*              this class tries to predict the ending of           */
/*              incomplete words, and is intended to be used in a   */
/*              CAT scenario                                        */
/*                                                                  */
/********************************************************************/

/**
 * @file WordPredictor.h
 * 
 * @brief Defines the WordPredictor class, this class tries to predict
 * the ending of incomplete words, and is intended to be used in a CAT
 * scenario
 */

#ifndef _WordPredictor_h
#define _WordPredictor_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include "Count.h"
#include "ErrorDefs.h"
#include "awkInputStream.h"
#include "Trie.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- WordPredictor template class

/**
 * @brief the WordPredictor class tries to predict the ending of
 * incomplete words, and is intended to be used in a CAT scenario
 */

class WordPredictor
{
 public:

  typedef std::map<Count,std::string,std::greater<Count> > SuffixList;
    
      // Constructor
  WordPredictor();

      // Load file with word prediction information
  bool load(const char *fileName);

      // Add a new sentence to the word predictor
  void addSentence(std::vector<std::string> strVec);

      // Get set of possible suffixes for a string
  void getSuffixList(std::string input,SuffixList &out);

      // Get the suffix with highest count for given string
  std::pair<Count,std::string> getBestSuffix(std::string input);
  
  void clear(void);
  
      // Destructor
  ~WordPredictor();
  
 protected:
  
  Trie<char,Count> charTrie;
  unsigned int numSentsToRetain;
  std::vector<std::vector<std::string> > strVecVec;
  
  bool loadFileWithSents(const char *fileName);
  bool loadFileWithAdditionalInfo(const char *fileName);
  void addSentenceAux(std::vector<std::string> strVec);

};
#endif
