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
/* Definitions file: ClassDic.cc                                     */
/*                                                                   */
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "ClassDic.h"

//--------------- Global variables ------------------------------------

//--------------- Function declarations 

//--------------- Constants

//--------------- Classes ---------------------------------------------

//-------------------------
void ClassDic::addEntry(WordIndex w,ClassIndex c)
{
 wordToClassMap[w]=c;
}
//-------------------------
ClassIndex ClassDic::getClassForWord(WordIndex w)
{
 std::map<WordIndex,ClassIndex>::iterator wordToClassMapIter;

 wordToClassMapIter=wordToClassMap.find(w); 
 if(wordToClassMapIter!=wordToClassMap.end()) 
     return wordToClassMapIter->second;
 else return -1;
}
//-------------------------
Vector<WordIndex> ClassDic::getWordsOfClass(ClassIndex c)
{
 std::map<WordIndex,ClassIndex>::iterator wordToClassMapIter;
 Vector<WordIndex> wordVec;    
    
 for(wordToClassMapIter=wordToClassMap.begin();wordToClassMapIter!=wordToClassMap.end();++wordToClassMapIter)   
     if(wordToClassMapIter->second==c) wordVec.push_back(wordToClassMapIter->first);
    
 return wordVec;
}
//--------------------------
void ClassDic::clear(void)
{
 wordToClassMap.clear();   
}
//--------------------------
