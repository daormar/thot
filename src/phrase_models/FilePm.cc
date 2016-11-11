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
/* Module: FilePm                                                   */
/*                                                                  */
/* Definitions file: FilePm.cc                                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "FilePm.h"

//--------------- Global variables -----------------------------------


//--------------- FilePm class function definitions

//-------------------------
FilePm::FilePm(void)
{
}

//-------------------------
Count FilePm::cSrcTrg(const Vector<WordIndex>& s,
                      const Vector<WordIndex>& t)
{
  bool found;
  return filePt.getSrcTrgInfo(s,t,found);
}

//-------------------------
Count FilePm::cSrc(const Vector<WordIndex>& s)
{
  bool found;
  return filePt.getSrcInfo(s,found);
}

//-------------------------
Count FilePm::cTrg(const Vector<WordIndex>& t)
{
  bool found;
  return filePt.getTrgInfo(t,found);
}

//-------------------------
Count FilePm::cHSrcHTrg(const Vector<std::string>& hs,
                        const Vector<std::string>& ht)
{
  Vector<WordIndex> s;
  Vector<WordIndex> t;

      // Generate vector of source WordIndex
  for(unsigned int i=0;i<hs.size();++i)
  {
    if(!existSrcSymbol(hs[i])) return 0;
    else s.push_back(stringToSrcWordIndex(hs[i]));
  }

      // Generate vector of target WordIndex
  for(unsigned int i=0;i<ht.size();++i)
  {
    if(!existTrgSymbol(ht[i])) return 0;
    else t.push_back(stringToTrgWordIndex(ht[i]));
  }

  return cSrcTrg(s,t);
}

//-------------------------
Count FilePm::cHSrc(const Vector<std::string>& hs)
{
  Vector<WordIndex> s;

      // Generate vector of source WordIndex
  for(unsigned int i=0;i<hs.size();++i)
  {
    if(!existSrcSymbol(hs[i])) return 0;
    else s.push_back(stringToSrcWordIndex(hs[i]));
  }
  return cSrc(s);
}

//-------------------------
Count FilePm::cHTrg(const Vector<std::string>& ht)
{
  Vector<WordIndex> t;

      // Generate vector of target WordIndex
  for(unsigned int i=0;i<ht.size();++i)
  {
    if(!existTrgSymbol(ht[i])) return 0;
    else t.push_back(stringToTrgWordIndex(ht[i]));
  }
  return cTrg(t);  
}

//-------------------------
PhrasePairInfo FilePm::infSrcTrg(const Vector<WordIndex>& s,
                                 const Vector<WordIndex>& t,
                                 bool& found)
{
  PhrasePairInfo ppInfo;
  ppInfo.first=filePt.getSrcInfo(s,found);
  ppInfo.second=filePt.getSrcTrgInfo(s,t,found);
  return ppInfo;
}

//-------------------------
Prob FilePm::pk_tlen(unsigned int tlen,unsigned int k)
{
  Prob p=segLenTable.pk_tlen(tlen,k);

  if((double) p < SEGM_SIZE_PROB_SMOOTH)
    return SEGM_SIZE_PROB_SMOOTH;
  else return p;
}

//-------------------------
LgProb FilePm::srcSegmLenLgProb(unsigned int x_k,
                                unsigned int x_km1,
                                unsigned int srcLen)
{
  return srcSegmLenTable.srcSegmLenLgProb(x_k,x_km1,srcLen);
}

//-------------------------
LgProb FilePm::trgCutsLgProb(int offset)
{
  return trgCutsTable.trgCutsLgProb(offset);
}

//-------------------------
LgProb FilePm::trgSegmLenLgProb(unsigned int k,
                                const SentSegmentation& trgSegm,
                                unsigned int trgLen,
                                unsigned int lastSrcSegmLen)
{
  return trgSegmLenTable.trgSegmLenLgProb(k,trgSegm,trgLen,lastSrcSegmLen);
}

//-------------------------
LgProb FilePm::logpt_s_(const Vector<WordIndex>& s,
                        const Vector<WordIndex>& t)
{
  LgProb lp=filePt.logpTrgGivenSrc(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
LgProb FilePm::logps_t_(const Vector<WordIndex>& s,
                        const Vector<WordIndex>& t)
{
  LgProb lp=filePt.logpSrcGivenTrg(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
bool FilePm::getTransFor_s_(const Vector<WordIndex>& s,
                            FilePm::TrgTableNode& trgtn)
{
  trgtn.clear();
  cerr<<"Warning: getTransFor_s_() function not implemented for this class"<<endl;
  return false;
}

//-------------------------
bool FilePm::getTransFor_t_(const Vector<WordIndex>& t,
                            FilePm::SrcTableNode& srctn)
{
  return filePt.getEntriesForTarget(t,srctn);
}

//-------------------------
bool FilePm::getNbestTransFor_s_(const Vector<WordIndex>& s,
                                 NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  nbt.clear();
  cerr<<"Warning: getNbestTransFor_s_() function not implemented for this class"<<endl;
  return false;
}

//-------------------------	
bool FilePm::getNbestTransFor_t_(const Vector<WordIndex>& t,
                                 NbestTableNode<PhraseTransTableNodeData>& nbt,
                                 int N/*=-1*/) 
{  
  return filePt.getNbestForTrg(t,nbt,N);
}

//-------------------------
bool FilePm::load(const char *prefix)
{
  bool ret;

      // Clear previous tables
  filePt.clear();
  segLenTable.clear();

      // Load source vocabulary
  std::string srcvocabfile=prefix;
  srcvocabfile=srcvocabfile+".srcvoc";
  ret=loadSrcVocab(srcvocabfile.c_str());
  if(ret==ERROR) return ERROR;
  
      // Load target vocabulary
  std::string trgvocabfile=prefix;
  trgvocabfile=trgvocabfile+".trgvoc";
  ret=loadTrgVocab(trgvocabfile.c_str());
  if(ret==ERROR) return ERROR;
  
      // Load translation table
  ret=filePt.init(prefix);
  if(ret==ERROR) return ERROR;

      // Load segmentation length table
  std::string seglenfile=prefix;
  seglenfile=seglenfile+".seglentable";
  load_seglentable(seglenfile.c_str());

      // Load source phrase length table
  std::string srcSegmLenFile=prefix;
  srcSegmLenFile=srcSegmLenFile+".srcsegmlentable";
  srcSegmLenTable.load(srcSegmLenFile.c_str());

      // Load target cuts table
  std::string trgCutsTableFile=prefix;
  trgCutsTableFile=trgCutsTableFile+".trgcutstable";
  trgCutsTable.load(trgCutsTableFile.c_str());

      // Load target phrase length table
  std::string trgSegmLenFile=prefix;
  trgSegmLenFile=trgSegmLenFile+".trgsegmlentable";
  trgSegmLenTable.load(trgSegmLenFile.c_str());

      // Store prefix of model files
  prefixOfModelFiles=prefix;

  return OK;
}

//-------------------------
bool FilePm::load_seglentable(const char *segmLengthTableFileName)
{
  return segLenTable.load_seglentable(segmLengthTableFileName);
}
//-------------------------
bool FilePm::print(const char *prefix)
{
  std::string prefixStl=prefix;
  if(prefixOfModelFiles==prefixStl)
  {
    return true;
  }
  else
  {
    cerr<<"Warning: print() function not implemented for this model"<<endl;
    return false;
  }
}

//-------------------------
size_t FilePm::getSrcVocabSize(void)const
{
  return singleWordVocab.getSrcVocabSize();	
}

//-------------------------
bool FilePm::loadSrcVocab(const char *srcInputVocabFileName)
{
  return singleWordVocab.loadSrcVocab(srcInputVocabFileName);
}

//-------------------------
bool FilePm::loadTrgVocab(const char *trgInputVocabFileName)
{
  return singleWordVocab.loadTrgVocab(trgInputVocabFileName);
}

//-------------------------
WordIndex FilePm::stringToSrcWordIndex(string s)const
{	
 return singleWordVocab.stringToSrcWordIndex(s);
}

//-------------------------
string FilePm::wordIndexToSrcString(WordIndex w)const
{
 return singleWordVocab.wordIndexToSrcString(w);
}

//-------------------------
bool FilePm::existSrcSymbol(string s)const
{
 return singleWordVocab.existSrcSymbol(s);
}

//-------------------------
Vector<WordIndex> FilePm::strVectorToSrcIndexVector(const Vector<string>& s,
                                                    Count numTimes/*=1*/)
{
  Vector<WordIndex> swVec;
  
  for(unsigned int i=0;i<s.size();++i)
    swVec.push_back(addSrcSymbol(s[i],numTimes));
  
  return swVec;
}

//-------------------------
Vector<string> FilePm::srcIndexVectorToStrVector(const Vector<WordIndex>& s)
{
 Vector<string> vStr;
 unsigned int i;

 for(i=0;i<s.size();++i)
    vStr.push_back((wordIndexToSrcString(s[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex FilePm::addSrcSymbol(string s,
                               Count numTimes/*=1*/)
{
 return singleWordVocab.addSrcSymbol(s,numTimes);
}

//-------------------------
bool FilePm::printSrcVocab(const char *outputFileName)
{
 return singleWordVocab.printSrcVocab(outputFileName);
}

//-------------------------
size_t FilePm::getTrgVocabSize(void)const
{
 return singleWordVocab.getTrgVocabSize();	
}

//-------------------------
WordIndex FilePm::stringToTrgWordIndex(string t)const
{
 return singleWordVocab.stringToTrgWordIndex(t);
}

//-------------------------
string FilePm::wordIndexToTrgString(WordIndex w)const
{
 return singleWordVocab.wordIndexToTrgString(w);
}

//-------------------------
bool FilePm::existTrgSymbol(string t)const
{
 return singleWordVocab.existTrgSymbol(t);
}

//-------------------------
Vector<WordIndex> FilePm::strVectorToTrgIndexVector(const Vector<string>& t,
                                                    Count numTimes/*=1*/)
{
  Vector<WordIndex> twVec;
  
  for(unsigned int i=0;i<t.size();++i)
    twVec.push_back(addTrgSymbol(t[i],numTimes));
  
  return twVec;
}
//-------------------------
Vector<string> FilePm::trgIndexVectorToStrVector(const Vector<WordIndex>& t)
{
 Vector<string> vStr;
 unsigned int i;

 for(i=0;i<t.size();++i)
    vStr.push_back((wordIndexToTrgString(t[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex FilePm::addTrgSymbol(string t,
                               Count numTimes/*=1*/)
{
 return singleWordVocab.addTrgSymbol(t,numTimes);
}
//-------------------------
bool FilePm::printTrgVocab(const char *outputFileName)
{
 return singleWordVocab.printTrgVocab(outputFileName);
}

//-------------------------
Vector<string> FilePm::stringToStringVector(string s)
{
 Vector<string> vs;	
 string aux;
 unsigned int i=0;	
 bool end=false;
	
 while(!end)
 {
  aux="";	 
  while(s[i]!=' ' && s[i]!='	' && i<s.size()) 
  {
    aux+=s[i];
    ++i;		   
  }
  if(aux!="") vs.push_back(aux);	   
  while((s[i]==' ' || s[i]=='	') && i<s.size()) ++i;	 
  if(i>=s.size()) end=true;	  
 }
 
 return vs;	
}

//-------------------------
Vector<string> FilePm::extractCharItemsToVector(char *ch)const
{
 unsigned int i=0;
 string s;	
 Vector<string> v,u;

 while(ch[i]!=0)
 {
   s=""; 
   while(ch[i]==' ' && ch[i]!=0) {++i;}	  
   while(ch[i]!=' ' && ch[i]!=0) {s=s+ch[i]; ++i;}
   if(s!="") v.push_back(s);  
 }	  
	 
 for(i=0;i<v.size();++i)
 {
   u.push_back(v[v.size()-1-i]);
 }
 return u;
}

//-------------------------
size_t FilePm::size(void)
{
  return filePt.size();
}

//-------------------------
void FilePm::clear(void)
{
  singleWordVocab.clear();
  filePt.clear();
  segLenTable.clear();
  prefixOfModelFiles.clear();
}

//-------------------------
FilePm::~FilePm()
{
}

//-------------------------
