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
/* Module: DaTriePhraseModel                                        */
/*                                                                  */
/* Definitions file: DaTriePhraseModel.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "DaTriePhraseModel.h"

//--------------- Global variables -----------------------------------


//--------------- DaTriePhraseModel class function definitions

//-------------------------
DaTriePhraseModel::DaTriePhraseModel(void)
{
}

//-------------------------
Count DaTriePhraseModel::cSrcTrg(const Vector<WordIndex>& s,
                      const Vector<WordIndex>& t)
{
  bool found;
  return daTriePhraseTable.getSrcTrgInfo(s,t,found);
}

//-------------------------
Count DaTriePhraseModel::cSrc(const Vector<WordIndex>& s)
{
  bool found;
  return daTriePhraseTable.getSrcInfo(s,found);
}

//-------------------------
Count DaTriePhraseModel::cTrg(const Vector<WordIndex>& t)
{
  bool found;
  return daTriePhraseTable.getTrgInfo(t,found);
}

//-------------------------
Count DaTriePhraseModel::cHSrcHTrg(const Vector<std::string>& hs,
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
Count DaTriePhraseModel::cHSrc(const Vector<std::string>& hs)
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
Count DaTriePhraseModel::cHTrg(const Vector<std::string>& ht)
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
PhrasePairInfo DaTriePhraseModel::infSrcTrg(const Vector<WordIndex>& s,
                                         const Vector<WordIndex>& t,
                                         bool& found)
{
  PhrasePairInfo ppInfo;
  ppInfo.first=daTriePhraseTable.getSrcInfo(s,found);
  ppInfo.second=daTriePhraseTable.getSrcTrgInfo(s,t,found);
  return ppInfo;
}

//-------------------------
Prob DaTriePhraseModel::pk_tlen(unsigned int tlen,
                             unsigned int k)
{
  Prob p=segLenTable.pk_tlen(tlen,k);

  if((double) p < SEGM_SIZE_PROB_SMOOTH)
    return SEGM_SIZE_PROB_SMOOTH;
  else return p;
}

//-------------------------
LgProb DaTriePhraseModel::srcSegmLenLgProb(unsigned int x_k,
                                        unsigned int x_km1,
                                        unsigned int srcLen)
{
  return srcSegmLenTable.srcSegmLenLgProb(x_k,x_km1,srcLen);
}

//-------------------------
LgProb DaTriePhraseModel::trgCutsLgProb(int offset)
{
  return trgCutsTable.trgCutsLgProb(offset);
}

//-------------------------
LgProb DaTriePhraseModel::trgSegmLenLgProb(unsigned int k,
                                        const SentSegmentation& trgSegm,
                                        unsigned int trgLen,
                                        unsigned int lastSrcSegmLen)
{
  return trgSegmLenTable.trgSegmLenLgProb(k,trgSegm,trgLen,lastSrcSegmLen);
}

//-------------------------
LgProb DaTriePhraseModel::logpt_s_(const Vector<WordIndex>& s,
                                const Vector<WordIndex>& t)
{
  LgProb lp=daTriePhraseTable.logpTrgGivenSrc(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
LgProb DaTriePhraseModel::logps_t_(const Vector<WordIndex>& s,
                                const Vector<WordIndex>& t)
{
  LgProb lp=daTriePhraseTable.logpSrcGivenTrg(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
bool DaTriePhraseModel::getTransFor_s_(const Vector<WordIndex>& /*s*/,
                                    DaTriePhraseModel::TrgTableNode& trgtn)
{
  trgtn.clear();
  cerr<<"Warning: getTransFor_s_() function not implemented for this class"<<endl;
  return false;
}

//-------------------------
bool DaTriePhraseModel::getTransFor_t_(const Vector<WordIndex>& t,
                                    DaTriePhraseModel::SrcTableNode& srctn)
{
  return daTriePhraseTable.getEntriesForTarget(t,srctn);
}

//-------------------------
bool DaTriePhraseModel::getNbestTransFor_s_(const Vector<WordIndex>& /*s*/,
                                         NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  nbt.clear();
  cerr<<"Warning: getNbestTransFor_s_() function not implemented for this class"<<endl;
  return false;
}

//-------------------------	
bool DaTriePhraseModel::getNbestTransFor_t_(const Vector<WordIndex>& t,
                                         NbestTableNode<PhraseTransTableNodeData>& nbt,
                                         int N/*=-1*/) 
{  
  return daTriePhraseTable.getNbestForTrg(t,nbt,N);
}

//-------------------------
bool DaTriePhraseModel::load(const char *prefix)
{
  bool ret;

      // Clear previous tables
  daTriePhraseTable.clear();
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
  ret=daTriePhraseTable.load(prefix);
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
bool DaTriePhraseModel::load_seglentable(const char *segmLengthTableFileName)
{
  return segLenTable.load_seglentable(segmLengthTableFileName);
}
//-------------------------
bool DaTriePhraseModel::print(const char *prefix)
{
  std::string prefixStl=prefix;
  if(prefixOfModelFiles==prefixStl)
  {
    return OK;
  }
  else
  {
    cerr<<"Warning: print() function not implemented for this model"<<endl;
    return ERROR;
  }
}

//-------------------------
size_t DaTriePhraseModel::getSrcVocabSize(void)const
{
  return singleWordVocab.getSrcVocabSize();	
}

//-------------------------
bool DaTriePhraseModel::loadSrcVocab(const char *srcInputVocabFileName)
{
  return singleWordVocab.loadSrcVocab(srcInputVocabFileName);
}

//-------------------------
bool DaTriePhraseModel::loadTrgVocab(const char *trgInputVocabFileName)
{
  return singleWordVocab.loadTrgVocab(trgInputVocabFileName);
}

//-------------------------
WordIndex DaTriePhraseModel::stringToSrcWordIndex(string s)const
{	
 return singleWordVocab.stringToSrcWordIndex(s);
}

//-------------------------
string DaTriePhraseModel::wordIndexToSrcString(WordIndex w)const
{
 return singleWordVocab.wordIndexToSrcString(w);
}

//-------------------------
bool DaTriePhraseModel::existSrcSymbol(string s)const
{
 return singleWordVocab.existSrcSymbol(s);
}

//-------------------------
Vector<WordIndex> DaTriePhraseModel::strVectorToSrcIndexVector(const Vector<string>& s,
                                                            Count numTimes/*=1*/)
{
  Vector<WordIndex> swVec;
  
  for(unsigned int i=0;i<s.size();++i)
    swVec.push_back(addSrcSymbol(s[i],numTimes));
  
  return swVec;
}

//-------------------------
Vector<string> DaTriePhraseModel::srcIndexVectorToStrVector(const Vector<WordIndex>& s)
{
 Vector<string> vStr;
 unsigned int i;

 for(i=0;i<s.size();++i)
    vStr.push_back((wordIndexToSrcString(s[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex DaTriePhraseModel::addSrcSymbol(string s,
                               Count numTimes/*=1*/)
{
 return singleWordVocab.addSrcSymbol(s,numTimes);
}

//-------------------------
bool DaTriePhraseModel::printSrcVocab(const char *outputFileName)
{
 return singleWordVocab.printSrcVocab(outputFileName);
}

//-------------------------
size_t DaTriePhraseModel::getTrgVocabSize(void)const
{
 return singleWordVocab.getTrgVocabSize();	
}

//-------------------------
WordIndex DaTriePhraseModel::stringToTrgWordIndex(string t)const
{
 return singleWordVocab.stringToTrgWordIndex(t);
}

//-------------------------
string DaTriePhraseModel::wordIndexToTrgString(WordIndex w)const
{
 return singleWordVocab.wordIndexToTrgString(w);
}

//-------------------------
bool DaTriePhraseModel::existTrgSymbol(string t)const
{
 return singleWordVocab.existTrgSymbol(t);
}

//-------------------------
Vector<WordIndex> DaTriePhraseModel::strVectorToTrgIndexVector(const Vector<string>& t,
                                                            Count numTimes/*=1*/)
{
  Vector<WordIndex> twVec;
  
  for(unsigned int i=0;i<t.size();++i)
    twVec.push_back(addTrgSymbol(t[i],numTimes));
  
  return twVec;
}
//-------------------------
Vector<string> DaTriePhraseModel::trgIndexVectorToStrVector(const Vector<WordIndex>& t)
{
 Vector<string> vStr;
 unsigned int i;

 for(i=0;i<t.size();++i)
    vStr.push_back((wordIndexToTrgString(t[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex DaTriePhraseModel::addTrgSymbol(string t,
                                       Count numTimes/*=1*/)
{
 return singleWordVocab.addTrgSymbol(t,numTimes);
}
//-------------------------
bool DaTriePhraseModel::printTrgVocab(const char *outputFileName)
{
 return singleWordVocab.printTrgVocab(outputFileName);
}

//-------------------------
Vector<string> DaTriePhraseModel::stringToStringVector(string s)
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
Vector<string> DaTriePhraseModel::extractCharItemsToVector(char *ch)const
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
size_t DaTriePhraseModel::size(void)
{
  return daTriePhraseTable.size();
}

//-------------------------
void DaTriePhraseModel::clear(void)
{
  singleWordVocab.clear();
  daTriePhraseTable.clear();
  segLenTable.clear();
  prefixOfModelFiles.clear();
}

//-------------------------
DaTriePhraseModel::~DaTriePhraseModel()
{
}

//-------------------------
