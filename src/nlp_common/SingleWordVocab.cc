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
/* Definitions file: SingleWordVocab.cc                              */
/*                                                                   */
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "SingleWordVocab.h"

//--------------- Function definitions

//-------------------------
ostream& operator << (ostream &outS,
                      SingleWordVocab::StrToIdxVocab const &vocab)
{
 SingleWordVocab::StrToIdxVocab::const_iterator vocabIter;
 
 for(vocabIter=vocab.begin();vocabIter!=vocab.end();++vocabIter)
 {
   // outS<< vocabIter->second.first <<" "<< vocabIter->first << " " <<vocabIter->second.second <<std::endl;
   outS<< vocabIter->second <<" "<< vocabIter->first <<std::endl;
 }
 
 return outS;
}

//-------------------------
SingleWordVocab::SingleWordVocab(void)
{
  clear();
}

//-------------------------
bool SingleWordVocab::loadSrcVocab(const char *srcInputVocabFileName)
{
  return loadGIZASrcVocab(srcInputVocabFileName);
}

//-------------------------	
SingleWordVocab::StrToIdxVocab SingleWordVocab::getSrcVocab(void)const
{
 return stringToSrcWordIndexMap;	
}

//-------------------------
size_t SingleWordVocab::getSrcVocabSize(void)const
{
 return stringToSrcWordIndexMap.size();	
}
//-------------------------
WordIndex SingleWordVocab::stringToSrcWordIndex(std::string s)const
{
 StrToIdxVocab::const_iterator strToIdxVocabIter;
	
 strToIdxVocabIter=stringToSrcWordIndexMap.find(s);
 if(strToIdxVocabIter!=stringToSrcWordIndexMap.end())
 {
   return strToIdxVocabIter->second;
 }
 else return UNK_WORD;
}

//-------------------------
std::string SingleWordVocab::wordIndexToSrcString(WordIndex w)const
{
 IdxToStrVocab::const_iterator idxToStrVocabIter;
	
 idxToStrVocabIter=srcWordIndexMapToString.find(w);
 if(idxToStrVocabIter!=srcWordIndexMapToString.end())
 {
   return idxToStrVocabIter->second;
 }
 else return UNK_WORD_STR;
}

//-------------------------
bool SingleWordVocab::existSrcSymbol(std::string s)const
{
 StrToIdxVocab::const_iterator strToIdxVocabIter;
	
 strToIdxVocabIter=stringToSrcWordIndexMap.find(s);
 if(strToIdxVocabIter!=stringToSrcWordIndexMap.end())
 {
   return 1;
 }
 else return 0;
}

//-------------------------
std::vector<WordIndex> SingleWordVocab::strVectorToSrcIndexVector(std::vector<std::string> s)
{
 unsigned int i;
 std::vector<WordIndex> wordIndex_s;
 WordIndex wordIndex;	
	
 for(i=0;i<s.size();++i)
 {
   wordIndex=addSrcSymbol(s[i]);
   wordIndex_s.push_back(wordIndex);  
 }
 return wordIndex_s;
}

//-------------------------
WordIndex SingleWordVocab::addSrcSymbol(std::string s)
{
 WordIndex wordIndex;	
 StrToIdxVocab::const_iterator strToIdxVocabIter;
	
 strToIdxVocabIter=stringToSrcWordIndexMap.find(s);
 if(strToIdxVocabIter!=stringToSrcWordIndexMap.end()) 
 {
   return strToIdxVocabIter->second;
 }
 else
 {
   wordIndex=stringToSrcWordIndexMap.size();	
   stringToSrcWordIndexMap[s]=wordIndex;
   srcWordIndexMapToString[wordIndex]=s; 
 }
 return wordIndex;
}
//-------------------------
bool SingleWordVocab::printSrcVocab(const char *outputFileName)
{
  return printGIZASrcVocab(outputFileName);
}
//-------------------------
bool SingleWordVocab::loadGIZASrcVocab(const char *srcInputVocabFileName)
{
 awkInputStream awk;

 if(awk.open(srcInputVocabFileName)==THOT_ERROR)
 {
   std::cerr<<"Error in GIZA source vocabulary, file "<<srcInputVocabFileName<<" does not exist.\n";
   return THOT_ERROR;
 }  
 else
 {
   std::cerr<<"Reading source vocabulary from: "<<srcInputVocabFileName<<std::endl;

   clearSrcVocab();
   
       // Read file
   while(awk.getln())
   {
     if(awk.NF>1)
     {
       if(awk.NF==2 || awk.NF==3)
       {
         stringToSrcWordIndexMap[awk.dollar(2)]=atoi(awk.dollar(1).c_str());
         srcWordIndexMapToString[atoi(awk.dollar(1).c_str())]=awk.dollar(2);   
       }
       else
       {
         std::cerr<<"Error in GIZA source vocabulary file\n";
         return THOT_ERROR;
       }
     }
   }
   awk.close();
   
   return THOT_OK;
 } 
}

//-------------------------
bool SingleWordVocab::printGIZASrcVocab(const char *outputFileName)
{
 ofstream outF;
	
 outF.open(outputFileName,ios::out);
 if(!outF)
 {
   std::cerr<<"Error while printing source vocabulary."<<std::endl;
   return THOT_ERROR;
 }
 outF<<stringToSrcWordIndexMap;
 outF.close();
 return THOT_OK;
}

//-------------------------
bool SingleWordVocab::loadTrgVocab(const char *trgInputVocabFileName)
{
  return loadGIZATrgVocab(trgInputVocabFileName);
}

//-------------------------
SingleWordVocab::StrToIdxVocab SingleWordVocab::getTrgVocab(void)const
{
 return stringToTrgWordIndexMap;	
}

//-------------------------
size_t SingleWordVocab::getTrgVocabSize(void)const
{
 return stringToTrgWordIndexMap.size();	
}

//-------------------------
WordIndex SingleWordVocab::stringToTrgWordIndex(std::string t)const
{
 StrToIdxVocab::const_iterator trgVocabIter;
	
 trgVocabIter=stringToTrgWordIndexMap.find(t);
 if(trgVocabIter!=stringToTrgWordIndexMap.end())
 {
   return trgVocabIter->second;
 }
 else return UNK_WORD;
}

//-------------------------
std::string SingleWordVocab::wordIndexToTrgString(WordIndex w)const
{
  IdxToStrVocab::const_iterator trgVocabIter;
	
  trgVocabIter=trgWordIndexMapToString.find(w);
  if(trgVocabIter!=trgWordIndexMapToString.end())
  {
	return trgVocabIter->second;
  }
  else return UNK_WORD_STR;
}

//-------------------------
bool SingleWordVocab::existTrgSymbol(std::string t)const
{
 StrToIdxVocab::const_iterator trgVocabIter;
	
 trgVocabIter=stringToTrgWordIndexMap.find(t);
 if(trgVocabIter!=stringToTrgWordIndexMap.end())
 {
   return 1;
 }
 else return 0;
}

//-------------------------
std::vector<WordIndex> SingleWordVocab::strVectorToTrgIndexVector(std::vector<std::string> t)
{
 unsigned int i;
 std::vector<WordIndex> wordIndex_t;
 WordIndex wordIndex;	
	
 for(i=0;i<t.size();++i)
 {
   wordIndex=addTrgSymbol(t[i]);
   wordIndex_t.push_back(wordIndex);  
 }
 return wordIndex_t;	
}
//-------------------------
WordIndex SingleWordVocab::addTrgSymbol(std::string t)
{
 WordIndex wordIndex;	
 StrToIdxVocab::const_iterator trgVocabIter;
	
 trgVocabIter=stringToTrgWordIndexMap.find(t);
 if(trgVocabIter!=stringToTrgWordIndexMap.end()) 
 {
   return trgVocabIter->second;
 }
 else
 {
  wordIndex=stringToTrgWordIndexMap.size();	
  stringToTrgWordIndexMap[t]=wordIndex;
  trgWordIndexMapToString[wordIndex]=t; 
 }
 return wordIndex;
}
//-------------------------
bool SingleWordVocab::printTrgVocab(const char *outputFileName)
{
  return printGIZATrgVocab(outputFileName);
}

//-------------------------
bool SingleWordVocab::loadGIZATrgVocab(const char *trgInputVocabFileName)
{
 awkInputStream awk;

 if(awk.open(trgInputVocabFileName)==THOT_ERROR)
 {
   std::cerr<<"Error in GIZA target vocabulary, file "<<trgInputVocabFileName<<" does not exist.\n";
   return THOT_ERROR;
 }  
 else
 {
   std::cerr<<"Reading target vocabulary from: "<<trgInputVocabFileName<<std::endl;

   clearTrgVocab();
   
   while(awk.getln())
   {
     if(awk.NF>1)
     {
       if(awk.NF==2 || awk.NF==3)
       {
         stringToTrgWordIndexMap[awk.dollar(2)]=atoi(awk.dollar(1).c_str());
         trgWordIndexMapToString[atoi(awk.dollar(1).c_str())]=awk.dollar(2);    
       }
       else
       {
         std::cerr<<"Error in GIZA target vocabulary file\n";
         return 1;
       }
     }
   }
   awk.close();

   return THOT_OK;
 } 
}

//-------------------------
bool SingleWordVocab::printGIZATrgVocab(const char *outputFileName)
{
 ofstream outF;
	
 outF.open(outputFileName,ios::out);
 if(!outF)
 {
   std::cerr<<"Error while printing target vocabulary."<<std::endl;
   return THOT_ERROR;
 }
 outF<<stringToTrgWordIndexMap;
 outF.close();
 return THOT_OK;
}

//-------------------------
void SingleWordVocab::clearSrcVocab(void)
{
  stringToSrcWordIndexMap.clear();
  srcWordIndexMapToString.clear();

  add_null_word_to_srcvoc();
  add_unk_word_to_srcvoc();
  add_unused_word_to_srcvoc();
}

//-------------------------
void SingleWordVocab::clearTrgVocab(void)
{
  stringToTrgWordIndexMap.clear();
  trgWordIndexMapToString.clear();

  add_null_word_to_trgvoc();
  add_unk_word_to_trgvoc();
  add_unused_word_to_trgvoc();
}

//-------------------------
void SingleWordVocab::clear(void)
{
  clearSrcVocab();
  clearTrgVocab();
}

//-------------------------
void SingleWordVocab::add_null_word_to_srcvoc(void)
{
  stringToSrcWordIndexMap[NULL_WORD_STR]=NULL_WORD;
  srcWordIndexMapToString[NULL_WORD]=NULL_WORD_STR;
}

//-------------------------
void SingleWordVocab::add_null_word_to_trgvoc(void)
{
  stringToTrgWordIndexMap[NULL_WORD_STR]=NULL_WORD;
  trgWordIndexMapToString[NULL_WORD]=NULL_WORD_STR;
}

//-------------------------
void SingleWordVocab::add_unk_word_to_srcvoc(void)
{
  stringToSrcWordIndexMap[UNK_WORD_STR]=UNK_WORD;
  srcWordIndexMapToString[UNK_WORD]=UNK_WORD_STR;
}

//-------------------------
void SingleWordVocab::add_unk_word_to_trgvoc(void)
{
  stringToTrgWordIndexMap[UNK_WORD_STR]=UNK_WORD;
  trgWordIndexMapToString[UNK_WORD]=UNK_WORD_STR;
}

//-------------------------
void SingleWordVocab::add_unused_word_to_srcvoc(void)
{
  stringToSrcWordIndexMap[UNUSED_WORD_STR]=UNUSED_WORD;
  srcWordIndexMapToString[UNUSED_WORD]=UNUSED_WORD_STR;
}

//-------------------------
void SingleWordVocab::add_unused_word_to_trgvoc(void)
{
  stringToTrgWordIndexMap[UNUSED_WORD_STR]=UNUSED_WORD;
  trgWordIndexMapToString[UNUSED_WORD]=UNUSED_WORD_STR;
}

//-------------------------
SingleWordVocab::~SingleWordVocab()
{
}
