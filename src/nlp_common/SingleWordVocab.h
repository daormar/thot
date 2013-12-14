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
 
/*********************************************************************/
/*                                                                   */
/* Module: SingleWordVocab                                           */
/*                                                                   */
/* Prototype file: SingleWordVocab.h                                 */
/*                                                                   */
/* Description: Manages a single-word vocabulary.                    */
/*                                                                   */
/*********************************************************************/

#ifndef _SingleWordVocab
#define _SingleWordVocab

//--------------- Include files ---------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "StatModelDefs.h"
#include "ClassDic.h"
#include "awkInputStream.h"
#include <map>
#include <string>

//--------------- Constants -------------------------------------------


//--------------- typedefs --------------------------------------------


//--------------- function declarations -------------------------------


//--------------- Classes ---------------------------------------------

//--------------- SingleWordVocab class

class SingleWordVocab
{
  public:

   typedef std::map<std::string,pair<WordIndex,Count> > StrToIdxVocab;
   typedef std::map<WordIndex,std::string> IdxToStrVocab;

   // Constructor
   SingleWordVocab(void);
   
   // Functions related to the source vocabulary
   StrToIdxVocab getSrcVocab(void)const;
   size_t getSrcVocabSize(void)const; // Returns the source vocabulary size
   WordIndex stringToSrcWordIndex(std::string s)const;
   std::string wordIndexToSrcString(WordIndex w)const;
   bool existSrcSymbol(std::string s)const;
   Vector<WordIndex> strVectorToSrcIndexVector(Vector<std::string> s,
                                               Count numTimes=1);
       //converts a string vector into a source word index vector, this
       //function automatically handles the source vocabulary,
       //increasing and modifying it if necessary
   WordIndex addSrcSymbol(std::string s,Count numTimes=1);
   bool loadSrcVocab(const char *srcInputVocabFileName);
   bool printSrcVocab(const char *outputFileName);
   bool loadGIZASrcVocab(const char *srcInputVocabFileName);
   bool printGIZASrcVocab(const char *outputFileName);
       // Reads source vocabulary from a file in GIZA format

   // Functions related to the target vocabulary
   StrToIdxVocab getTrgVocab(void)const;
   size_t getTrgVocabSize(void)const; // Returns the target vocabulary size
   WordIndex stringToTrgWordIndex(std::string t)const;
   std::string wordIndexToTrgString(WordIndex w)const;
   bool existTrgSymbol(std::string t)const;
   Vector<WordIndex> strVectorToTrgIndexVector(Vector<std::string> t,
                                               Count numTimes=1);
       //converts a string vector into a target word index vector, this
       //function automatically handles the target vocabulary,
       //increasing and modifying it if necessary
   WordIndex addTrgSymbol(std::string t,Count numTimes=1);
   bool loadTrgVocab(const char *trgInputVocabFileName);
   bool printTrgVocab(const char *outputFileName);
   bool loadGIZATrgVocab(const char *trgInputVocabFileName);
   bool printGIZATrgVocab(const char *trgInputVocabFileName);
       // Reads target vocabulary from a file in GIZA format

   // Functions related to classes for source words
   bool loadSrcClassDicFile(char *srcClassDicFileName);
   ClassIndex getClassForSrcWord(WordIndex w);

   // Functions related to classes for target words
   bool loadTrgClassDicFile(char *trgClassDicFileName);
   ClassIndex getClassForTrgWord(WordIndex w);

   // clear() function
   void clear(void);

   // Destructor
   ~SingleWordVocab();

  protected:
   StrToIdxVocab stringToSrcWordIndexMap;
   IdxToStrVocab srcWordIndexMapToString;
   StrToIdxVocab stringToTrgWordIndexMap;
   IdxToStrVocab trgWordIndexMapToString;
      
   ClassDic srcClassDic;
   ClassDic trgClassDic;

   void add_null_word(void);
   void add_unk_word(void);
   void add_unused_word(void);
   void clearDataMembers(void);

};

#endif
