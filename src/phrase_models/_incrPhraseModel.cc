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

/**
 * @file _incrPhraseModel.cc
 * 
 * @brief Definitions file for _incrPhraseModel.h
 */

//--------------- Include files --------------------------------------

#include "_incrPhraseModel.h"

//--------------- Global variables -----------------------------------


//--------------- _incrPhraseModel class method definitions

_incrPhraseModel::_incrPhraseModel(void)
{
}

//-------------------------
void _incrPhraseModel::strAddTableEntry(const std::vector<std::string>& s,
                                        const std::vector<std::string>& t,
                                        PhrasePairInfo inf)
{
  std::vector<WordIndex> wordIndex_t,wordIndex_s;
 
  wordIndex_s=strVectorToSrcIndexVector(s);	
  wordIndex_t=strVectorToTrgIndexVector(t);
 
  addTableEntry(wordIndex_s,wordIndex_t,inf);	
}
//-------------------------
void _incrPhraseModel::addTableEntry(const std::vector<WordIndex>& s,
                                     const std::vector<WordIndex>& t,
                                     PhrasePairInfo inf)
{
      // Revise phrase translation model
  basePhraseTablePtr->addTableEntry(s,t,inf);
}

//-------------------------
void _incrPhraseModel::strIncrCountsOfEntry(const std::vector<std::string>& s,
                                            const std::vector<std::string>& t,
                                            Count count)
{
 std::vector<WordIndex> wordIndex_t,wordIndex_s;
 
 wordIndex_s=strVectorToSrcIndexVector(s);
 wordIndex_t=strVectorToTrgIndexVector(t);
 
 incrCountsOfEntry(wordIndex_s,wordIndex_t,count);	
}

//-------------------------
void _incrPhraseModel::incrCountsOfEntry(const std::vector<WordIndex>& s,
                                         const std::vector<WordIndex>& t,
                                         Count count)
{
 // Revise phrase translation model	
 basePhraseTablePtr->incrCountsOfEntry(s,t,count);	 
}

//-------------------------
Count _incrPhraseModel::cSrcTrg(const std::vector<WordIndex>& s,
                                const std::vector<WordIndex>& t)
{
  return basePhraseTablePtr->cSrcTrg(s,t);
}

//-------------------------
Count _incrPhraseModel::cSrc(const std::vector<WordIndex>& s)
{
  return basePhraseTablePtr->cSrc(s);
}

//-------------------------
Count _incrPhraseModel::cTrg(const std::vector<WordIndex>& t)
{
  return basePhraseTablePtr->cTrg(t);
}

//-------------------------
Count _incrPhraseModel::cHSrcHTrg(const std::vector<std::string>& hs,
                                  const std::vector<std::string>& ht)
{
  std::vector<WordIndex> s;
  std::vector<WordIndex> t;

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
Count _incrPhraseModel::cHSrc(const std::vector<std::string>& hs)
{
  std::vector<WordIndex> s;

      // Generate vector of source WordIndex
  for(unsigned int i=0;i<hs.size();++i)
  {
    if(!existSrcSymbol(hs[i])) return 0;
    else s.push_back(stringToSrcWordIndex(hs[i]));
  }
  return cSrc(s);
}

//-------------------------
Count _incrPhraseModel::cHTrg(const std::vector<std::string>& ht)
{
  std::vector<WordIndex> t;

      // Generate vector of target WordIndex
  for(unsigned int i=0;i<ht.size();++i)
  {
    if(!existTrgSymbol(ht[i])) return 0;
    else t.push_back(stringToTrgWordIndex(ht[i]));
  }
  return cTrg(t);  
}

//-------------------------
PhrasePairInfo _incrPhraseModel::infSrcTrg(const std::vector<WordIndex>& s,
                                           const std::vector<WordIndex>& t,
                                           bool& found)
{
  return basePhraseTablePtr->infSrcTrg(s,t,found);
}

//-------------------------
Prob _incrPhraseModel::pk_tlen(unsigned int tlen,unsigned int k)
{
  Prob p=segLenTable.pk_tlen(tlen,k);

  if((double) p < SEGM_SIZE_PROB_SMOOTH)
    return SEGM_SIZE_PROB_SMOOTH;
  else return p;
}

//-------------------------
LgProb _incrPhraseModel::srcSegmLenLgProb(unsigned int x_k,
                                          unsigned int x_km1,
                                          unsigned int srcLen)
{
  return srcSegmLenTable.srcSegmLenLgProb(x_k,x_km1,srcLen);
}

//-------------------------
LgProb _incrPhraseModel::trgCutsLgProb(int offset)
{
  return trgCutsTable.trgCutsLgProb(offset);
}

//-------------------------
LgProb _incrPhraseModel::trgSegmLenLgProb(unsigned int k,
                                          const SentSegmentation& trgSegm,
                                          unsigned int trgLen,
                                          unsigned int lastSrcSegmLen)
{
  return trgSegmLenTable.trgSegmLenLgProb(k,trgSegm,trgLen,lastSrcSegmLen);
}

//-------------------------
LgProb _incrPhraseModel::logpt_s_(const std::vector<WordIndex>& s,
                                  const std::vector<WordIndex>& t)
{
  LgProb lp=basePhraseTablePtr->logpTrgGivenSrc(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
LgProb _incrPhraseModel::logps_t_(const std::vector<WordIndex>& s,
                                  const std::vector<WordIndex>& t)
{
  LgProb lp=basePhraseTablePtr->logpSrcGivenTrg(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
bool _incrPhraseModel::getTransFor_s_(const std::vector<WordIndex>& s,
                                      _incrPhraseModel::TrgTableNode& trgtn)
{
  return basePhraseTablePtr->getEntriesForSource(s,trgtn);  
}

//-------------------------
bool _incrPhraseModel::getTransFor_t_(const std::vector<WordIndex>& t,
                                      _incrPhraseModel::SrcTableNode& srctn)
{
  return basePhraseTablePtr->getEntriesForTarget(t,srctn);
}

//-------------------------
bool _incrPhraseModel::getNbestTransFor_s_(const std::vector<WordIndex>& s,
                                           NbestTableNode<PhraseTransTableNodeData>& nbt)
{
 return basePhraseTablePtr->getNbestForSrc(s,nbt);
}

//-------------------------	
bool _incrPhraseModel::getNbestTransFor_t_(const std::vector<WordIndex>& t,
                                           NbestTableNode<PhraseTransTableNodeData>& nbt,
                                           int N/*=-1*/) 
{  
  return basePhraseTablePtr->getNbestForTrg(t,nbt,N);
}

//-------------------------
bool _incrPhraseModel::load(const char *prefix)
{
  std::string mainFileName;
  if(fileIsDescriptor(prefix,mainFileName))
  {
    std::string descFileName=prefix;
    std::string absolutizedMainFileName=absolutizeModelFileName(descFileName,mainFileName);
    return load_given_prefix(absolutizedMainFileName.c_str());
  }
  else
  {
    return load_given_prefix(prefix);
  }
}

//-------------------------
bool _incrPhraseModel::load_given_prefix(const char *prefix)
{
  std::string ttablefile;
  std::string seglenfile;
  bool ret;

      // Clear previous tables
  basePhraseTablePtr->clear();
  segLenTable.clear();

      // Load translation table
  ttablefile=prefix;
  ttablefile=ttablefile+".ttable";
  ret=load_ttable(ttablefile.c_str());
  if(ret==THOT_ERROR) return THOT_ERROR;

      // Load segmentation length table
  seglenfile=prefix;
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

  return THOT_OK;
}

//-------------------------
bool _incrPhraseModel::load_ttable(const char *_incrPhraseModelFileName)
{
  AwkInputStream awk;
  
  if(awk.open(_incrPhraseModelFileName)==THOT_ERROR)
  {
    std::cerr<<"Error in ttable file: "<<_incrPhraseModelFileName<<"\n";
    return THOT_ERROR;
  }
  else
  {
    if(awk.getln())
    {
      if(awk.NF==4 && strcmp("****",awk.dollar(1).c_str())==0 &&
         strcmp("cache",awk.dollar(2).c_str())==0 && strcmp("ttable",awk.dollar(3).c_str())==0)
      {
        std::cerr<<"Error in ttable file: "<<_incrPhraseModelFileName<<"\n";
        return THOT_ERROR;
      }
      else
      {
        awk.close();
        return loadPlainTextTTable(_incrPhraseModelFileName);
      }
    }
    else
    {
      awk.close();
      return loadPlainTextTTable(_incrPhraseModelFileName);
    }
  }
}

//-------------------------
bool _incrPhraseModel::loadPlainTextTTable(const char *phraseTTableFileName)
{
 unsigned int i;
 std::vector<std::string> s,t;
 AwkInputStream awk;
 PhrasePairInfo phpinfo; 
 Count count_s_;
 Count count_s_t_;
 
 std::cerr<<"Loading phrase ttable from file "<<phraseTTableFileName<<std::endl;

 if(awk.open(phraseTTableFileName)==THOT_ERROR)
 {
   std::cerr<<"Error in phrase model file: "<<phraseTTableFileName<<std::endl;
   return THOT_ERROR;
 }
 else
 {
   unsigned int numEntry=1; 
   while(awk.getln())
   {
     if(awk.FNR>=1 && awk.NF>1)
     {
           // Read source phrase
       i=1; 
       s.clear();	  
       while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
       {
         s.push_back(awk.dollar(i)); 
         ++i;
       }
           // Read target phrase
       ++i;
       t.clear();
       while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
       {
         t.push_back(awk.dollar(i));			   
         ++i; 
       }
           // Verify entry
       if(i<awk.NF-1 && strcmp("|||",awk.dollar(i).c_str())==0 && !s.empty() && !t.empty())
       {
             // Read count information
         ++i;
         count_s_=atof(awk.dollar(i).c_str());
         ++i;
         count_s_t_=atof(awk.dollar(i).c_str());  

             // Add table entry
         phpinfo.first=count_s_;
         phpinfo.second=count_s_t_;
         strAddTableEntry(s,t,phpinfo);
       } 
       else
       {
         std::cerr<<"Warning: discarding anomalous phrase table entry at line "<<numEntry<<std::endl;
       }
     }
     ++numEntry;
   }
 }
 return THOT_OK;
}

//-------------------------
bool _incrPhraseModel::load_seglentable(const char *segmLengthTableFileName)
{
  return segLenTable.load_seglentable(segmLengthTableFileName);
}

//-------------------------
bool _incrPhraseModel::print(const char *prefix)
{
      // Obtain file prefix
  std::string mainFileName;
  std::string absolutizedMainFileName;
  if(fileIsDescriptor(prefix,mainFileName))
  {
    std::string descFileName=prefix;
    absolutizedMainFileName=absolutizeModelFileName(descFileName,mainFileName);
  }
  else
  {
    absolutizedMainFileName=prefix;
  }

      // Print translation table
  std::string ttableFileName=absolutizedMainFileName;
  ttableFileName+=".ttable";
  bool retVal=printTTable(ttableFileName.c_str());
  if(retVal) return THOT_ERROR;

      // Warning: generation of segmentation length tables is not
      // currently working
  std::string segLenTableFileName=absolutizedMainFileName;
  segLenTableFileName+=".seglentable";
  retVal=printSegmLengthTable(segLenTableFileName.c_str());
  if(retVal) return THOT_ERROR;
  
  return THOT_OK;
}

//-------------------------
bool _incrPhraseModel::printTTable(const char *outputFileName)
{
  FILE *outf;

  outf=fopen(outputFileName,"w");
  if(outf==NULL)
  {
    std::cerr<<"Error while printing phrase model to file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    printTTable(outf);
    return THOT_OK;
  }
}

//-------------------------
bool _incrPhraseModel::printSegmLengthTable(const char *outputFileName)
{
 std::ofstream outF;

 outF.open(outputFileName,std::ios::out);
 if(!outF)
 {
   std::cerr<<"Error while printing segmentation length table."<<std::endl;
   return THOT_ERROR;
 }

 printSegmLengthTable(outF);	
 outF.close();	

 return THOT_OK;
}

//-------------------------
void _incrPhraseModel::printNbestTransTableNode(NbestTableNode<PhraseTransTableNodeData> tTableNode,
                                                std::ostream &outS)
{
 NbestTableNode<PhraseTransTableNodeData>::iterator transTableNodeIter;
 std::vector<WordIndex>::iterator VectorWordIndexIter;
	
  for(transTableNodeIter=tTableNode.begin();transTableNodeIter!=tTableNode.end();++transTableNodeIter)	 
  {
   for(VectorWordIndexIter=transTableNodeIter->second.begin();VectorWordIndexIter!=transTableNodeIter->second.end();++VectorWordIndexIter)
      outS<<wordIndexToTrgString(*VectorWordIndexIter)<<" ";  	 
   outS<<"||| "<< (float) transTableNodeIter->first<<std::endl;
  }	
}

//-------------------------
void _incrPhraseModel::printSegmLengthTable(std::ostream &outS)
{
  return segLenTable.printSegmLengthTable(outS);
}

//-------------------------
size_t _incrPhraseModel::getSrcVocabSize(void)const
{
  return singleWordVocab.getSrcVocabSize();	
}

//-------------------------
bool _incrPhraseModel::loadSrcVocab(const char *srcInputVocabFileName)
{
  return singleWordVocab.loadSrcVocab(srcInputVocabFileName);
}

//-------------------------
bool _incrPhraseModel::loadTrgVocab(const char *trgInputVocabFileName)
{
  return singleWordVocab.loadTrgVocab(trgInputVocabFileName);
}

//-------------------------
WordIndex _incrPhraseModel::stringToSrcWordIndex(std::string s)const
{	
 return singleWordVocab.stringToSrcWordIndex(s);
}

//-------------------------
std::string _incrPhraseModel::wordIndexToSrcString(WordIndex w)const
{
 return singleWordVocab.wordIndexToSrcString(w);
}

//-------------------------
bool _incrPhraseModel::existSrcSymbol(std::string s)const
{
 return singleWordVocab.existSrcSymbol(s);
}

//-------------------------
std::vector<WordIndex> _incrPhraseModel::strVectorToSrcIndexVector(const std::vector<std::string>& s)
{
  std::vector<WordIndex> swVec;
  
  for(unsigned int i=0;i<s.size();++i)
    swVec.push_back(addSrcSymbol(s[i]));
  
  return swVec;
}

//-------------------------
std::vector<std::string> _incrPhraseModel::srcIndexVectorToStrVector(const std::vector<WordIndex>& s)
{
 std::vector<std::string> vStr;
 unsigned int i;

 for(i=0;i<s.size();++i)
    vStr.push_back((wordIndexToSrcString(s[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex _incrPhraseModel::addSrcSymbol(std::string s)
{
 return singleWordVocab.addSrcSymbol(s);
}

//-------------------------
bool _incrPhraseModel::printSrcVocab(const char *outputFileName)
{
 return singleWordVocab.printSrcVocab(outputFileName);
}

//-------------------------
size_t _incrPhraseModel::getTrgVocabSize(void)const
{
 return singleWordVocab.getTrgVocabSize();	
}

//-------------------------
WordIndex _incrPhraseModel::stringToTrgWordIndex(std::string t)const
{
 return singleWordVocab.stringToTrgWordIndex(t);
}

//-------------------------
std::string _incrPhraseModel::wordIndexToTrgString(WordIndex w)const
{
 return singleWordVocab.wordIndexToTrgString(w);
}

//-------------------------
bool _incrPhraseModel::existTrgSymbol(std::string t)const
{
 return singleWordVocab.existTrgSymbol(t);
}

//-------------------------
std::vector<WordIndex> _incrPhraseModel::strVectorToTrgIndexVector(const std::vector<std::string>& t)
{
  std::vector<WordIndex> twVec;
  
  for(unsigned int i=0;i<t.size();++i)
    twVec.push_back(addTrgSymbol(t[i]));
  
  return twVec;
}
//-------------------------
std::vector<std::string> _incrPhraseModel::trgIndexVectorToStrVector(const std::vector<WordIndex>& t)
{
 std::vector<std::string> vStr;
 unsigned int i;

 for(i=0;i<t.size();++i)
    vStr.push_back((wordIndexToTrgString(t[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex _incrPhraseModel::addTrgSymbol(std::string t)
{
 return singleWordVocab.addTrgSymbol(t);
}
//-------------------------
bool _incrPhraseModel::printTrgVocab(const char *outputFileName)
{
 return singleWordVocab.printTrgVocab(outputFileName);
}

//-------------------------
std::vector<std::string> _incrPhraseModel::stringToStringVector(std::string s)
{
 std::vector<std::string> vs;	
 std::string aux;
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
std::vector<std::string> _incrPhraseModel::extractCharItemsToVector(char *ch)const
{
 unsigned int i=0;
 std::string s;	
 std::vector<std::string> v,u;

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
size_t _incrPhraseModel::size(void)
{
  return basePhraseTablePtr->size();
}

//-------------------------
void _incrPhraseModel::clear(void)
{
  singleWordVocab.clear();
  basePhraseTablePtr->clear();
  alignmentExtractor.close();
  segLenTable.clear();
}
//-------------------------
void _incrPhraseModel::clearTempVars(void)
{
}

//-------------------------
_incrPhraseModel::~_incrPhraseModel()
{
}

//-------------------------
