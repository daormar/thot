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
   outS<< vocabIter->second.first <<" "<< vocabIter->first << " " <<vocabIter->second.second <<endl;
 }
 
 return outS;
}

//-------------------------
SingleWordVocab::SingleWordVocab(void)
{
      // Add the null word to both the source and the target vocabulary
  add_null_word();
  
      // Add the unknown word to both the source and the target
      // vocabulary
  add_unk_word();

      // Add the unused word to both the source and the target
      // vocabulary
  add_unused_word();
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
 StrToIdxVocab::const_iterator srcVocabIter;
	
 srcVocabIter=stringToSrcWordIndexMap.find(s);
 if(srcVocabIter!=stringToSrcWordIndexMap.end())
 {
   return srcVocabIter->second.first;
 }
 else return UNK_WORD;
}

//-------------------------
std::string SingleWordVocab::wordIndexToSrcString(WordIndex w)const
{
 IdxToStrVocab::const_iterator srcVocabIter;
	
 srcVocabIter=srcWordIndexMapToString.find(w);
 if(srcVocabIter!=srcWordIndexMapToString.end())
 {
   return srcVocabIter->second;
 }
 else return UNK_WORD_STR;
}

//-------------------------
bool SingleWordVocab::existSrcSymbol(std::string s)const
{
 StrToIdxVocab::const_iterator srcVocabIter;
	
 srcVocabIter=stringToSrcWordIndexMap.find(s);
 if(srcVocabIter!=stringToSrcWordIndexMap.end())
 {
   return 1;
 }
 else return 0;
}

//-------------------------
Vector<WordIndex> SingleWordVocab::strVectorToSrcIndexVector(Vector<std::string> s,
                                                             Count numTimes/*=1*/)
{
 unsigned int i;
 Vector<WordIndex> wordIndex_s;
 WordIndex wordIndex;	
	
 for(i=0;i<s.size();++i)
 {
   wordIndex=addSrcSymbol(s[i],numTimes);
   wordIndex_s.push_back(wordIndex);  
 }
 return wordIndex_s;
}

//-------------------------
WordIndex SingleWordVocab::addSrcSymbol(std::string s,
                                        Count numTimes/*=1*/)
{
 WordIndex wordIndex;	
 StrToIdxVocab::const_iterator srcVocabIter;
	
 srcVocabIter=stringToSrcWordIndexMap.find(s);
 if(srcVocabIter!=stringToSrcWordIndexMap.end()) 
 {
   stringToSrcWordIndexMap[s].second+=numTimes;
   return srcVocabIter->second.first;
 }
 else
 {
   wordIndex=stringToSrcWordIndexMap.size();	
   stringToSrcWordIndexMap[s].first=wordIndex;
   stringToSrcWordIndexMap[s].second=numTimes;	 
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

 if(awk.open(srcInputVocabFileName)==ERROR)
 {
   cerr<<"Error in GIZA source vocabulary, file "<<srcInputVocabFileName<<" does not exist.\n";
   return ERROR;
 }  
 else
 {
   cerr<<"Reading source vocabulary from: "<<srcInputVocabFileName<<endl;

   pair<WordIndex,Count> vocEntry;

       // Read file
   while(awk.getln())
   {
     if(awk.NF>1)
     {
       if(awk.NF==3)
       {
         vocEntry.first=atoi(awk.dollar(1).c_str());
         vocEntry.second=atof(awk.dollar(3).c_str());
         stringToSrcWordIndexMap[awk.dollar(2)]=vocEntry;
         srcWordIndexMapToString[atoi(awk.dollar(1).c_str())]=awk.dollar(2);   
       }
       else
       {
         cerr<<"Error in GIZA source vocabulary file\n";
         return ERROR;
       }
     } 
   }
   awk.close();
   
   return OK;
 } 
}

//-------------------------
bool SingleWordVocab::printGIZASrcVocab(const char *outputFileName)
{
 ofstream outF;
	
 outF.open(outputFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while printing source vocabulary."<<endl;
   return ERROR;
 }
 outF<<stringToSrcWordIndexMap;
 outF.close();
 return OK;
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
   return trgVocabIter->second.first;
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
Vector<WordIndex> SingleWordVocab::strVectorToTrgIndexVector(Vector<std::string> t,
                                                             Count numTimes/*=1*/)
{
 unsigned int i;
 Vector<WordIndex> wordIndex_t;
 WordIndex wordIndex;	
	
 for(i=0;i<t.size();++i)
 {
   wordIndex=addTrgSymbol(t[i],numTimes);
   wordIndex_t.push_back(wordIndex);  
 }
 return wordIndex_t;	
}
//-------------------------
WordIndex SingleWordVocab::addTrgSymbol(std::string t,
                                        Count numTimes/*=1*/)
{
 WordIndex wordIndex;	
 StrToIdxVocab::const_iterator trgVocabIter;
	
 trgVocabIter=stringToTrgWordIndexMap.find(t);
 if(trgVocabIter!=stringToTrgWordIndexMap.end()) 
 {
   stringToTrgWordIndexMap[t].second+=numTimes;
   return trgVocabIter->second.first;
 }
 else
 {
  wordIndex=stringToTrgWordIndexMap.size();	
  stringToTrgWordIndexMap[t].first=wordIndex;
  stringToTrgWordIndexMap[t].second=numTimes;	 
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

 if(awk.open(trgInputVocabFileName)==ERROR)
 {
   cerr<<"Error in GIZA target vocabulary, file "<<trgInputVocabFileName<<" does not exist.\n";
   return ERROR;
 }  
 else
 {
   cerr<<"Reading target vocabulary from: "<<trgInputVocabFileName<<endl;

   pair<WordIndex,Count> vocEntry;

   while(awk.getln())
   {
     if(awk.NF>1)
     {
       if(awk.NF==3)
       {
         vocEntry.first=atoi(awk.dollar(1).c_str());
         vocEntry.second=atof(awk.dollar(3).c_str());
         stringToTrgWordIndexMap[awk.dollar(2)]=vocEntry;
         trgWordIndexMapToString[atoi(awk.dollar(1).c_str())]=awk.dollar(2);    
       }
       else
       {
         cerr<<"Error in GIZA target vocabulary file\n";
         return 1;
       }
     }
   }
   awk.close();

   return OK;
 } 
}

//-------------------------
bool SingleWordVocab::printGIZATrgVocab(const char *outputFileName)
{
 ofstream outF;
	
 outF.open(outputFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while printing target vocabulary."<<endl;
   return ERROR;
 }
 outF<<stringToTrgWordIndexMap;
 outF.close();
 return OK;
}

//-------------------------
bool SingleWordVocab::loadSrcClassDicFile(char *srcClassDicFileName)
{
 awkInputStream awk;
 
 srcClassDic.clear();
 if(awk.open(srcClassDicFileName)==ERROR)
 {
   cerr<<"Error while reading source class dictionary file, file "<<srcClassDicFileName<<" does not exist.\n";
   return ERROR;
 }  
 else
 {
   cerr<<"Reading source class dictionary from: "<<srcClassDicFileName<<endl;
   awk.FS='	';
   srcClassDic.addEntry(0,0);   
   while(awk.getln())
   {
     if(awk.NF>1)
     {
       if(awk.NF==2)
       {
         srcClassDic.addEntry(stringToSrcWordIndex(awk.dollar(1)),atoi(awk.dollar(2).c_str()));
       }
       else
       {
         cerr<<"Error in source class dictionary file\n";
         return ERROR;
       }
     }
   }
 }
 
 return OK;
}
//-------------------------
ClassIndex SingleWordVocab::getClassForSrcWord(WordIndex w)
{
 return srcClassDic.getClassForWord(w);
}
//-------------------------
bool SingleWordVocab::loadTrgClassDicFile(char *trgClassDicFileName)
{
 awkInputStream awk;
 
 trgClassDic.clear();
 if(awk.open(trgClassDicFileName)==ERROR)
 {
   cerr<<"Error while reading target class dictionary file, file "<<trgClassDicFileName<<" does not exist.\n";
   return ERROR;
 }  
 else
 {
   cerr<<"Reading target class dictionary from: "<<trgClassDicFileName<<endl;
   awk.FS='	';
   while(awk.getln())
   {
     if(awk.NF>1)
     {
       if(awk.NF==2)
       {
         trgClassDic.addEntry(stringToTrgWordIndex(awk.dollar(1)),atoi(awk.dollar(2).c_str()));
       }
       else
       {
         cerr<<"Error in target class dictionary file\n";
         return ERROR;
       }
     }
   }
 }
 
 return OK;
}

//-------------------------
ClassIndex SingleWordVocab::getClassForTrgWord(WordIndex w)
{
 return trgClassDic.getClassForWord(w);
}

//-------------------------
void SingleWordVocab::clear(void)
{
      // Clear data members
 clearDataMembers();

     // Add the null word to both the source and the target vocabulary
 add_null_word();

     // Add the unknown word to both the source and the target
     // vocabulary
 add_unk_word();

     // Add the unused word to both the source and the target
     // vocabulary
 add_unused_word();
}

//-------------------------
void SingleWordVocab::add_null_word(void)
{
  pair<WordIndex,Count> vocEntry;

      // Add the null word to the source vocabulary
  vocEntry.first=NULL_WORD;
  vocEntry.second=0;
  stringToSrcWordIndexMap[NULL_WORD_STR]=vocEntry;
  srcWordIndexMapToString[vocEntry.first]=NULL_WORD_STR;

      // Add the null word to the target vocabulary
  vocEntry.first=NULL_WORD;
  vocEntry.second=0;
  stringToTrgWordIndexMap[NULL_WORD_STR]=vocEntry;
  trgWordIndexMapToString[vocEntry.first]=NULL_WORD_STR;
}

//-------------------------
void SingleWordVocab::add_unk_word(void)
{
  pair<WordIndex,Count> vocEntry;

      // Add the null word to the source vocabulary
  vocEntry.first=UNK_WORD;
  vocEntry.second=0;
  stringToSrcWordIndexMap[UNK_WORD_STR]=vocEntry;
  srcWordIndexMapToString[vocEntry.first]=UNK_WORD_STR;

      // Add the null word to the target vocabulary
  vocEntry.first=UNK_WORD;
  vocEntry.second=0;
  stringToTrgWordIndexMap[UNK_WORD_STR]=vocEntry;
  trgWordIndexMapToString[vocEntry.first]=UNK_WORD_STR;
}

//-------------------------
void SingleWordVocab::add_unused_word(void)
{
  pair<WordIndex,Count> vocEntry;

      // Add the null word to the source vocabulary
  vocEntry.first=UNUSED_WORD;
  vocEntry.second=0;
  stringToSrcWordIndexMap[UNUSED_WORD_STR]=vocEntry;
  srcWordIndexMapToString[vocEntry.first]=UNUSED_WORD_STR;

      // Add the null word to the target vocabulary
  vocEntry.first=UNUSED_WORD;
  vocEntry.second=0;
  stringToTrgWordIndexMap[UNUSED_WORD_STR]=vocEntry;
  trgWordIndexMapToString[vocEntry.first]=UNUSED_WORD_STR;
}

//-------------------------
void SingleWordVocab::clearDataMembers(void)
{
  stringToSrcWordIndexMap.clear();
  stringToTrgWordIndexMap.clear();
  srcWordIndexMapToString.clear();
  trgWordIndexMapToString.clear();
  srcClassDic.clear();
  trgClassDic.clear();
}

//-------------------------
SingleWordVocab::~SingleWordVocab()
{
}
