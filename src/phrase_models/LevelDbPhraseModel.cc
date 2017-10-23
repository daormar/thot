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
/* Module: LevelDbPhraseModel                                       */
/*                                                                  */
/* Definitions file: LevelDbPhraseModel.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "LevelDbPhraseModel.h"

//--------------- Global variables -----------------------------------


//--------------- LevelDbPhraseModel class function definitions

//-------------------------
LevelDbPhraseModel::LevelDbPhraseModel(void)
{
}

//-------------------------
void LevelDbPhraseModel::strAddTableEntry(const std::vector<string>& s,
                                          const std::vector<string>& t,
                                          PhrasePairInfo inf)
{
  std::vector<WordIndex> wordIndex_t,wordIndex_s;
 
  wordIndex_s=strVectorToSrcIndexVector(s);	
  wordIndex_t=strVectorToTrgIndexVector(t);
 
  addTableEntry(wordIndex_s,wordIndex_t,inf);
}

//-------------------------
void LevelDbPhraseModel::addTableEntry(const std::vector<WordIndex>& s,
                                       const std::vector<WordIndex>& t,
                                       PhrasePairInfo inf)
{
  levelDbPhraseTable.addTableEntry(s,t,inf);  
}

//-------------------------
void LevelDbPhraseModel::strIncrCountsOfEntry(const std::vector<string>& s,
                                              const std::vector<string>& t,
                                              Count count/*=1*/)
{
  std::vector<WordIndex> wordIndex_t,wordIndex_s;
  
  wordIndex_s=strVectorToSrcIndexVector(s);
  wordIndex_t=strVectorToTrgIndexVector(t);
  
  incrCountsOfEntry(wordIndex_s,wordIndex_t,count);	  
}

//-------------------------
void LevelDbPhraseModel::incrCountsOfEntry(const std::vector<WordIndex>& s,
                                           const std::vector<WordIndex>& t,
                                           Count count/*=1*/)
{
  levelDbPhraseTable.incrCountsOfEntry(s,t,count);	 
}

//-------------------------
Count LevelDbPhraseModel::cSrcTrg(const std::vector<WordIndex>& s,
                                  const std::vector<WordIndex>& t)
{
  bool found;
  return levelDbPhraseTable.getSrcTrgInfo(s, t, found);
}

//-------------------------
Count LevelDbPhraseModel::cSrc(const std::vector<WordIndex>& s)
{
  bool found;
  return levelDbPhraseTable.getSrcInfo(s, found);
}

//-------------------------
Count LevelDbPhraseModel::cTrg(const std::vector<WordIndex>& t)
{
  bool found;
  return levelDbPhraseTable.getTrgInfo(t, found);
}

//-------------------------
Count LevelDbPhraseModel::cHSrcHTrg(const std::vector<std::string>& hs,
                                    const std::vector<std::string>& ht)
{
  std::vector<WordIndex> s;
  std::vector<WordIndex> t;

      // Generate vector of source WordIndex
  for(unsigned int i = 0; i < hs.size(); ++i)
  {
    if(!existSrcSymbol(hs[i]))
      return 0;
    else
      s.push_back(stringToSrcWordIndex(hs[i]));
  }

      // Generate vector of target WordIndex
  for(unsigned int i = 0; i < ht.size(); ++i)
  {
    if(!existTrgSymbol(ht[i]))
      return 0;
    else
      t.push_back(stringToTrgWordIndex(ht[i]));
  }

  return cSrcTrg(s,t);
}

//-------------------------
Count LevelDbPhraseModel::cHSrc(const std::vector<std::string>& hs)
{
  std::vector<WordIndex> s;

      // Generate vector of source WordIndex
  for(unsigned int i = 0; i < hs.size(); ++i)
  {
    if(!existSrcSymbol(hs[i]))
      return 0;
    else
      s.push_back(stringToSrcWordIndex(hs[i]));
  }

  return cSrc(s);
}

//-------------------------
Count LevelDbPhraseModel::cHTrg(const std::vector<std::string>& ht)
{
  std::vector<WordIndex> t;

      // Generate vector of target WordIndex
  for(unsigned int i = 0; i < ht.size(); ++i)
  {
    if(!existTrgSymbol(ht[i]))
      return 0;
    else
      t.push_back(stringToTrgWordIndex(ht[i]));
  }

  return cTrg(t);  
}

//-------------------------
PhrasePairInfo LevelDbPhraseModel::infSrcTrg(const std::vector<WordIndex>& s,
                                             const std::vector<WordIndex>& t,
                                             bool& found)
{
  PhrasePairInfo ppInfo;
  ppInfo.first = levelDbPhraseTable.getSrcInfo(s, found);
  ppInfo.second = levelDbPhraseTable.getSrcTrgInfo(s, t, found);

  return ppInfo;
}

//-------------------------
Prob LevelDbPhraseModel::pk_tlen(unsigned int tlen,
                             unsigned int k)
{
  Prob p = segLenTable.pk_tlen(tlen, k);

  if((double) p < SEGM_SIZE_PROB_SMOOTH)
    return SEGM_SIZE_PROB_SMOOTH;
  else
    return p;
}

//-------------------------
LgProb LevelDbPhraseModel::srcSegmLenLgProb(unsigned int x_k,
                                            unsigned int x_km1,
                                            unsigned int srcLen)
{
  return srcSegmLenTable.srcSegmLenLgProb(x_k, x_km1, srcLen);
}

//-------------------------
LgProb LevelDbPhraseModel::trgCutsLgProb(int offset)
{
  return trgCutsTable.trgCutsLgProb(offset);
}

//-------------------------
LgProb LevelDbPhraseModel::trgSegmLenLgProb(unsigned int k,
                                            const SentSegmentation& trgSegm,
                                            unsigned int trgLen,
                                            unsigned int lastSrcSegmLen)
{
  return trgSegmLenTable.trgSegmLenLgProb(k, trgSegm, trgLen, lastSrcSegmLen);
}

//-------------------------
LgProb LevelDbPhraseModel::logpt_s_(const std::vector<WordIndex>& s,
                                    const std::vector<WordIndex>& t)
{
  LgProb lp = levelDbPhraseTable.logpTrgGivenSrc(s, t);

  if((double) lp < LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
LgProb LevelDbPhraseModel::logps_t_(const std::vector<WordIndex>& s,
                                    const std::vector<WordIndex>& t)
{
  LgProb lp = levelDbPhraseTable.logpSrcGivenTrg(s, t);
  
  if((double) lp < LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
bool LevelDbPhraseModel::getTransFor_s_(const std::vector<WordIndex>& /*s*/,
                                        LevelDbPhraseModel::TrgTableNode& trgtn)
{
  trgtn.clear();
  std::cerr << "Warning: getTransFor_s_() function not implemented for this class" << std::endl;
  return false;
}

//-------------------------
bool LevelDbPhraseModel::getTransFor_t_(const std::vector<WordIndex>& t,
                                        LevelDbPhraseModel::SrcTableNode& srctn)
{
  return levelDbPhraseTable.getEntriesForTarget(t, srctn);
}

//-------------------------
bool LevelDbPhraseModel::getNbestTransFor_s_(const std::vector<WordIndex>& /*s*/,
                                             NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  nbt.clear();
  std::cerr << "Warning: getNbestTransFor_s_() function not implemented for this class" << std::endl;
  return false;
}

//-------------------------	
bool LevelDbPhraseModel::getNbestTransFor_t_(const std::vector<WordIndex>& t,
                                            NbestTableNode<PhraseTransTableNodeData>& nbt,
                                            int N/*=-1*/) 
{  
  return levelDbPhraseTable.getNbestForTrg(t, nbt, N);
}

//-------------------------
bool LevelDbPhraseModel::load(const char *prefix)
{
  std::cerr << "Loading LevelDB phrase model (input: " << prefix << ")" << std::endl;

  std::string mainFileName;
  if(fileIsDescriptor(prefix, mainFileName))
  {
    std::string descFileName = prefix;
    std::string absolutizedMainFileName = absolutizeModelFileName(descFileName, mainFileName);
    int ret = load_given_prefix(absolutizedMainFileName.c_str());
    if(ret == THOT_ERROR)
    {
      std::cerr << "Error while loading phrase model" << std::endl;
      return THOT_ERROR;
    }

    return THOT_OK;
  }
  else
  {
    int ret = load_given_prefix(prefix);
    if(ret == THOT_ERROR)
    {
      std::cerr << "Error while loading phrase model" << std::endl;
      return THOT_ERROR;
    }

    return THOT_OK;
  }
}

//-------------------------
bool LevelDbPhraseModel::load_given_prefix(const char *prefix)
{
  bool ret;

      // Clear previous tables
  levelDbPhraseTable.clear();
  segLenTable.clear();

      // Load source vocabulary
  std::string srcvocabfile = prefix;
  srcvocabfile = srcvocabfile + ".ldb_svcb";
  ret = loadSrcVocab(srcvocabfile.c_str());
  if(ret == THOT_ERROR) return THOT_ERROR;
  
      // Load target vocabulary
  std::string trgvocabfile = prefix;
  trgvocabfile = trgvocabfile + ".ldb_tvcb";
  ret = loadTrgVocab(trgvocabfile.c_str());
  if(ret == THOT_ERROR) return THOT_ERROR;
  
      // Load translation table
  std::string levelPt = prefix;
  levelPt = levelPt + "_ldb_phrdict";
  ret = levelDbPhraseTable.load(levelPt);
  if(ret == THOT_ERROR) return THOT_ERROR;

      // Load segmentation length table
  std::string seglenfile = prefix;
  seglenfile = seglenfile + ".seglentable";
  load_seglentable(seglenfile.c_str());

      // Load source phrase length table
  std::string srcSegmLenFile = prefix;
  srcSegmLenFile = srcSegmLenFile + ".srcsegmlentable";
  srcSegmLenTable.load(srcSegmLenFile.c_str());

      // Load target cuts table
  std::string trgCutsTableFile = prefix;
  trgCutsTableFile = trgCutsTableFile + ".trgcutstable";
  trgCutsTable.load(trgCutsTableFile.c_str());

      // Load target phrase length table
  std::string trgSegmLenFile = prefix;
  trgSegmLenFile = trgSegmLenFile + ".trgsegmlentable";
  trgSegmLenTable.load(trgSegmLenFile.c_str());

      // Store prefix of model files
  prefixOfModelFiles = prefix;

  return THOT_OK;
}

//-------------------------
bool LevelDbPhraseModel::load_seglentable(const char *segmLengthTableFileName)
{
  return segLenTable.load_seglentable(segmLengthTableFileName);
}

//-------------------------
bool LevelDbPhraseModel::print(const char *prefix)
{
      // TO-BE-DONE
  
  std::string prefixStl = prefix;
  if(prefixOfModelFiles == prefixStl)
  {
    return THOT_OK;
  }
  else
  {
    std::cerr << "Warning: print() function not implemented for this model" << std::endl;
    return THOT_ERROR;
  }
}

//-------------------------
size_t LevelDbPhraseModel::getSrcVocabSize(void)const
{
  return singleWordVocab.getSrcVocabSize();	
}

//-------------------------
bool LevelDbPhraseModel::loadSrcVocab(const char *srcInputVocabFileName)
{
  return singleWordVocab.loadSrcVocab(srcInputVocabFileName);
}

//-------------------------
bool LevelDbPhraseModel::loadTrgVocab(const char *trgInputVocabFileName)
{
  return singleWordVocab.loadTrgVocab(trgInputVocabFileName);
}

//-------------------------
WordIndex LevelDbPhraseModel::stringToSrcWordIndex(string s)const
{	
 return singleWordVocab.stringToSrcWordIndex(s);
}

//-------------------------
string LevelDbPhraseModel::wordIndexToSrcString(WordIndex w)const
{
 return singleWordVocab.wordIndexToSrcString(w);
}

//-------------------------
bool LevelDbPhraseModel::existSrcSymbol(string s)const
{
 return singleWordVocab.existSrcSymbol(s);
}

//-------------------------
std::vector<WordIndex> LevelDbPhraseModel::strVectorToSrcIndexVector(const std::vector<string>& s)
{
  std::vector<WordIndex> swVec;
  
  for(unsigned int i = 0; i<s.size(); ++i)
    swVec.push_back(addSrcSymbol(s[i]));
  
  return swVec;
}

//-------------------------
std::vector<string> LevelDbPhraseModel::srcIndexVectorToStrVector(const std::vector<WordIndex>& s)
{
 std::vector<string> vStr;
 unsigned int i;

 for(i = 0; i < s.size(); ++i)
    vStr.push_back((wordIndexToSrcString(s[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex LevelDbPhraseModel::addSrcSymbol(string s)
{
 return singleWordVocab.addSrcSymbol(s);
}

//-------------------------
bool LevelDbPhraseModel::printSrcVocab(const char *outputFileName)
{
 return singleWordVocab.printSrcVocab(outputFileName);
}

//-------------------------
size_t LevelDbPhraseModel::getTrgVocabSize(void)const
{
 return singleWordVocab.getTrgVocabSize();	
}

//-------------------------
WordIndex LevelDbPhraseModel::stringToTrgWordIndex(string t)const
{
 return singleWordVocab.stringToTrgWordIndex(t);
}

//-------------------------
string LevelDbPhraseModel::wordIndexToTrgString(WordIndex w)const
{
 return singleWordVocab.wordIndexToTrgString(w);
}

//-------------------------
bool LevelDbPhraseModel::existTrgSymbol(string t)const
{
 return singleWordVocab.existTrgSymbol(t);
}

//-------------------------
std::vector<WordIndex> LevelDbPhraseModel::strVectorToTrgIndexVector(const std::vector<string>& t)
{
  std::vector<WordIndex> twVec;
  
  for(unsigned int i = 0; i < t.size(); ++i)
    twVec.push_back(addTrgSymbol(t[i]));
  
  return twVec;
}
      
//-------------------------
std::vector<string> LevelDbPhraseModel::trgIndexVectorToStrVector(const std::vector<WordIndex>& t)
{
 std::vector<string> vStr;
 unsigned int i;

 for(i = 0; i < t.size(); ++i)
    vStr.push_back((wordIndexToTrgString(t[i]))); 	 
	
 return vStr;
}
  
//-------------------------
WordIndex LevelDbPhraseModel::addTrgSymbol(string t)
{
 return singleWordVocab.addTrgSymbol(t);
}
//-------------------------
bool LevelDbPhraseModel::printTrgVocab(const char *outputFileName)
{
 return singleWordVocab.printTrgVocab(outputFileName);
}

//-------------------------
size_t LevelDbPhraseModel::size(void)
{
  return levelDbPhraseTable.size();
}

//-------------------------
void LevelDbPhraseModel::clear(void)
{
  singleWordVocab.clear();
  levelDbPhraseTable.clear();
  segLenTable.clear();
  prefixOfModelFiles.clear();
}

//-------------------------
LevelDbPhraseModel::~LevelDbPhraseModel()
{
}

//-------------------------
