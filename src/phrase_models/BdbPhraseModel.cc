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
/* Module: BdbPhraseModel                                           */
/*                                                                  */
/* Definitions file: BdbPhraseModel.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "BdbPhraseModel.h"

//--------------- Global variables -----------------------------------


//--------------- BdbPhraseModel class function definitions

//-------------------------
BdbPhraseModel::BdbPhraseModel(void)
{
}

//-------------------------
Count BdbPhraseModel::cSrcTrg(const Vector<WordIndex>& s,
                      const Vector<WordIndex>& t)
{
  bool found;
  return bdbPhraseTable.getSrcTrgInfo(s,t,found);
}

//-------------------------
Count BdbPhraseModel::cSrc(const Vector<WordIndex>& s)
{
  bool found;
  return bdbPhraseTable.getSrcInfo(s,found);
}

//-------------------------
Count BdbPhraseModel::cTrg(const Vector<WordIndex>& t)
{
  bool found;
  return bdbPhraseTable.getTrgInfo(t,found);
}

//-------------------------
Count BdbPhraseModel::cHSrcHTrg(const Vector<std::string>& hs,
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
Count BdbPhraseModel::cHSrc(const Vector<std::string>& hs)
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
Count BdbPhraseModel::cHTrg(const Vector<std::string>& ht)
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
PhrasePairInfo BdbPhraseModel::infSrcTrg(const Vector<WordIndex>& s,
                                         const Vector<WordIndex>& t,
                                         bool& found)
{
  PhrasePairInfo ppInfo;
  ppInfo.first=bdbPhraseTable.getSrcInfo(s,found);
  ppInfo.second=bdbPhraseTable.getSrcTrgInfo(s,t,found);
  return ppInfo;
}

//-------------------------
Prob BdbPhraseModel::pk_tlen(unsigned int tlen,
                             unsigned int k)
{
  Prob p=segLenTable.pk_tlen(tlen,k);

  if((double) p < SEGM_SIZE_PROB_SMOOTH)
    return SEGM_SIZE_PROB_SMOOTH;
  else return p;
}

//-------------------------
LgProb BdbPhraseModel::srcSegmLenLgProb(unsigned int x_k,
                                        unsigned int x_km1,
                                        unsigned int srcLen)
{
  return srcSegmLenTable.srcSegmLenLgProb(x_k,x_km1,srcLen);
}

//-------------------------
LgProb BdbPhraseModel::trgCutsLgProb(int offset)
{
  return trgCutsTable.trgCutsLgProb(offset);
}

//-------------------------
LgProb BdbPhraseModel::trgSegmLenLgProb(unsigned int k,
                                        const SentSegmentation& trgSegm,
                                        unsigned int trgLen,
                                        unsigned int lastSrcSegmLen)
{
  return trgSegmLenTable.trgSegmLenLgProb(k,trgSegm,trgLen,lastSrcSegmLen);
}

//-------------------------
LgProb BdbPhraseModel::logpt_s_(const Vector<WordIndex>& s,
                                const Vector<WordIndex>& t)
{
  LgProb lp=bdbPhraseTable.logpTrgGivenSrc(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
LgProb BdbPhraseModel::logps_t_(const Vector<WordIndex>& s,
                                const Vector<WordIndex>& t)
{
  LgProb lp=bdbPhraseTable.logpSrcGivenTrg(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
bool BdbPhraseModel::getTransFor_s_(const Vector<WordIndex>& /*s*/,
                                    BdbPhraseModel::TrgTableNode& trgtn)
{
  trgtn.clear();
  cerr<<"Warning: getTransFor_s_() function not implemented for this class"<<endl;
  return false;
}

//-------------------------
bool BdbPhraseModel::getTransFor_t_(const Vector<WordIndex>& t,
                                    BdbPhraseModel::SrcTableNode& srctn)
{
  return bdbPhraseTable.getEntriesForTarget(t,srctn);
}

//-------------------------
bool BdbPhraseModel::getNbestTransFor_s_(const Vector<WordIndex>& /*s*/,
                                         NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  nbt.clear();
  cerr<<"Warning: getNbestTransFor_s_() function not implemented for this class"<<endl;
  return false;
}

//-------------------------	
bool BdbPhraseModel::getNbestTransFor_t_(const Vector<WordIndex>& t,
                                         NbestTableNode<PhraseTransTableNodeData>& nbt,
                                         int N/*=-1*/) 
{  
  return bdbPhraseTable.getNbestForTrg(t,nbt,N);
}

//-------------------------
bool BdbPhraseModel::load(const char *prefix)
{
  cerr<<"Loading phrase model (input: "<<prefix<<")"<<endl;

  std::string mainFileName;
  if(fileIsDescriptor(prefix,mainFileName))
  {
    std::string descFileName=prefix;
    std::string absolutizedMainFileName=absolutizeModelFileName(descFileName,mainFileName);
    int ret=load_given_prefix(absolutizedMainFileName.c_str());
    if(ret==ERROR)
    {
      cerr<<"Error while loading phrase model"<<endl;
      return ERROR;
    }

    return OK;
  }
  else
  {
    int ret=load_given_prefix(prefix);
    if(ret==ERROR)
    {
      cerr<<"Error while loading phrase model"<<endl;
      return ERROR;
    }

    return OK;
  }
}

//-------------------------
bool BdbPhraseModel::load_given_prefix(const char *prefix)
{
  bool ret;

      // Clear previous tables
  bdbPhraseTable.clear();
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
  ret=bdbPhraseTable.init(prefix);
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
bool BdbPhraseModel::load_seglentable(const char *segmLengthTableFileName)
{
  return segLenTable.load_seglentable(segmLengthTableFileName);
}
//-------------------------
bool BdbPhraseModel::print(const char *prefix)
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
size_t BdbPhraseModel::getSrcVocabSize(void)const
{
  return singleWordVocab.getSrcVocabSize();	
}

//-------------------------
bool BdbPhraseModel::loadSrcVocab(const char *srcInputVocabFileName)
{
  return singleWordVocab.loadSrcVocab(srcInputVocabFileName);
}

//-------------------------
bool BdbPhraseModel::loadTrgVocab(const char *trgInputVocabFileName)
{
  return singleWordVocab.loadTrgVocab(trgInputVocabFileName);
}

//-------------------------
WordIndex BdbPhraseModel::stringToSrcWordIndex(string s)const
{	
 return singleWordVocab.stringToSrcWordIndex(s);
}

//-------------------------
string BdbPhraseModel::wordIndexToSrcString(WordIndex w)const
{
 return singleWordVocab.wordIndexToSrcString(w);
}

//-------------------------
bool BdbPhraseModel::existSrcSymbol(string s)const
{
 return singleWordVocab.existSrcSymbol(s);
}

//-------------------------
Vector<WordIndex> BdbPhraseModel::strVectorToSrcIndexVector(const Vector<string>& s,
                                                            Count numTimes/*=1*/)
{
  Vector<WordIndex> swVec;
  
  for(unsigned int i=0;i<s.size();++i)
    swVec.push_back(addSrcSymbol(s[i],numTimes));
  
  return swVec;
}

//-------------------------
Vector<string> BdbPhraseModel::srcIndexVectorToStrVector(const Vector<WordIndex>& s)
{
 Vector<string> vStr;
 unsigned int i;

 for(i=0;i<s.size();++i)
    vStr.push_back((wordIndexToSrcString(s[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex BdbPhraseModel::addSrcSymbol(string s,
                               Count numTimes/*=1*/)
{
 return singleWordVocab.addSrcSymbol(s,numTimes);
}

//-------------------------
bool BdbPhraseModel::printSrcVocab(const char *outputFileName)
{
 return singleWordVocab.printSrcVocab(outputFileName);
}

//-------------------------
size_t BdbPhraseModel::getTrgVocabSize(void)const
{
 return singleWordVocab.getTrgVocabSize();	
}

//-------------------------
WordIndex BdbPhraseModel::stringToTrgWordIndex(string t)const
{
 return singleWordVocab.stringToTrgWordIndex(t);
}

//-------------------------
string BdbPhraseModel::wordIndexToTrgString(WordIndex w)const
{
 return singleWordVocab.wordIndexToTrgString(w);
}

//-------------------------
bool BdbPhraseModel::existTrgSymbol(string t)const
{
 return singleWordVocab.existTrgSymbol(t);
}

//-------------------------
Vector<WordIndex> BdbPhraseModel::strVectorToTrgIndexVector(const Vector<string>& t,
                                                            Count numTimes/*=1*/)
{
  Vector<WordIndex> twVec;
  
  for(unsigned int i=0;i<t.size();++i)
    twVec.push_back(addTrgSymbol(t[i],numTimes));
  
  return twVec;
}
//-------------------------
Vector<string> BdbPhraseModel::trgIndexVectorToStrVector(const Vector<WordIndex>& t)
{
 Vector<string> vStr;
 unsigned int i;

 for(i=0;i<t.size();++i)
    vStr.push_back((wordIndexToTrgString(t[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex BdbPhraseModel::addTrgSymbol(string t,
                                       Count numTimes/*=1*/)
{
 return singleWordVocab.addTrgSymbol(t,numTimes);
}
//-------------------------
bool BdbPhraseModel::printTrgVocab(const char *outputFileName)
{
 return singleWordVocab.printTrgVocab(outputFileName);
}

//-------------------------
size_t BdbPhraseModel::size(void)
{
  return bdbPhraseTable.size();
}

//-------------------------
void BdbPhraseModel::clear(void)
{
  singleWordVocab.clear();
  bdbPhraseTable.clear();
  segLenTable.clear();
  prefixOfModelFiles.clear();
}

//-------------------------
BdbPhraseModel::~BdbPhraseModel()
{
}

//-------------------------
