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
/* Module: ClassDic                                                  */
/*                                                                   */
/* Prototype file: ClassDic                                          */
/*                                                                   */
/* Description: Manages a word-index to class dictionary.            */
/*                                                                   */
/*********************************************************************/

#ifndef _ClassDic
#define _ClassDic

//--------------- Include files ---------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "StatModelDefs.h"
#include <map>
#include <vector>
#include <string>

//--------------- Constants -------------------------------------------


//--------------- typedefs --------------------------------------------


//--------------- function declarations -------------------------------


//--------------- Classes ---------------------------------------------

//--------------- ClassDic class

class ClassDic
{
 public:

   void addEntry(WordIndex w,ClassIndex c); 
   ClassIndex getClassForWord(WordIndex w);
       // Returns the class for a given word. If the word does not exist
       // the function returns -1
   std::vector<WordIndex> getWordsOfClass(ClassIndex c);
  
   void clear(void); 
 
 private:
   std::map<WordIndex,ClassIndex> wordToClassMap;
};

#endif
