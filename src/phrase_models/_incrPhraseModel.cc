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
/* Module: _incrPhraseModel                                         */
/*                                                                  */
/* Definitions file: _incrPhraseModel.cc                            */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "_incrPhraseModel.h"

//--------------- Global variables -----------------------------------


//--------------- _incrPhraseModel class method definitions

_incrPhraseModel::_incrPhraseModel(void)
{
 logFileName="";
}

//-------------------------
void _incrPhraseModel::strAddTableEntry(const Vector<string>& s,
                                        const Vector<string>& t,
                                        PhrasePairInfo inf)
{
  Vector<WordIndex> wordIndex_t,wordIndex_s;
 
  wordIndex_s=strVectorToSrcIndexVector(s,inf.first.get_c_s());	
  wordIndex_t=strVectorToTrgIndexVector(t,inf.second.get_c_st());
 
  addTableEntry(wordIndex_s,wordIndex_t,inf);	
}
//-------------------------
void _incrPhraseModel::addTableEntry(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t,
                                     PhrasePairInfo inf)
{
      // Revise phrase translation model
  basePhraseTablePtr->addTableEntry(s,t,inf);
}

//-------------------------
void _incrPhraseModel::strIncrCountsOfEntry(const Vector<string>& s,
                                            const Vector<string>& t,
                                            Count count)
{
 Vector<WordIndex> wordIndex_t,wordIndex_s;
 
 wordIndex_s=strVectorToSrcIndexVector(s,count);	
 wordIndex_t=strVectorToTrgIndexVector(t,count);
 
 incrCountsOfEntry(wordIndex_s,wordIndex_t,count);	
}

//-------------------------
void _incrPhraseModel::incrCountsOfEntry(const Vector<WordIndex>& s,
                                         const Vector<WordIndex>& t,
                                         Count count)
{
 // Revise phrase translation model	
 basePhraseTablePtr->incrCountsOfEntry(s,t,count);	 
}

//-------------------------
Count _incrPhraseModel::cSrcTrg(const Vector<WordIndex>& s,
                                const Vector<WordIndex>& t)
{
  return basePhraseTablePtr->cSrcTrg(s,t);
}

//-------------------------
Count _incrPhraseModel::cSrc(const Vector<WordIndex>& s)
{
  return basePhraseTablePtr->cSrc(s);
}

//-------------------------
Count _incrPhraseModel::cTrg(const Vector<WordIndex>& t)
{
  return basePhraseTablePtr->cTrg(t);
}

//-------------------------
Count _incrPhraseModel::cHSrcHTrg(const Vector<std::string>& hs,
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
Count _incrPhraseModel::cHSrc(const Vector<std::string>& hs)
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
Count _incrPhraseModel::cHTrg(const Vector<std::string>& ht)
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
PhrasePairInfo _incrPhraseModel::infSrcTrg(const Vector<WordIndex>& s,
                                           const Vector<WordIndex>& t,
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
LgProb _incrPhraseModel::logpt_s_(const Vector<WordIndex>& s,
                                  const Vector<WordIndex>& t)
{
  LgProb lp=basePhraseTablePtr->logpTrgGivenSrc(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
LgProb _incrPhraseModel::logps_t_(const Vector<WordIndex>& s,
                                  const Vector<WordIndex>& t)
{
  LgProb lp=basePhraseTablePtr->logpSrcGivenTrg(s,t);
  if((double)lp<LOG_PHRASE_PROB_SMOOTH)
    return LOG_PHRASE_PROB_SMOOTH;
  else
    return lp;
}

//-------------------------
bool _incrPhraseModel::getTransFor_s_(const Vector<WordIndex>& s,
                                      _incrPhraseModel::TrgTableNode& trgtn)
{
  return basePhraseTablePtr->getEntriesForSource(s,trgtn);  
}

//-------------------------
bool _incrPhraseModel::getTransFor_t_(const Vector<WordIndex>& t,
                                      _incrPhraseModel::SrcTableNode& srctn)
{
  return basePhraseTablePtr->getEntriesForTarget(t,srctn);
}

//-------------------------
bool _incrPhraseModel::getNbestTransFor_s_(const Vector<WordIndex>& s,
                                           NbestTableNode<PhraseTransTableNodeData>& nbt)
{
 return basePhraseTablePtr->getNbestForSrc(s,nbt);
}

//-------------------------	
bool _incrPhraseModel::getNbestTransFor_t_(const Vector<WordIndex>& t,
                                           NbestTableNode<PhraseTransTableNodeData>& nbt,
                                           int N/*=-1*/) 
{  
  NbestTableNode<PhraseTransTableNodeData> nbestTransTableNode;
  bool b;
      
  b=basePhraseTablePtr->getNbestForTrg(t,nbt,N);	
    
  return b;
}

//-------------------------
bool _incrPhraseModel::load(const char *prefix)
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
  if(ret==ERROR) return ERROR;

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

  return OK;
}

//-------------------------
bool _incrPhraseModel::load_ttable(const char *_incrPhraseModelFileName)
{
  awkInputStream awk;
  
  if(awk.open(_incrPhraseModelFileName)==ERROR)
  {
    cerr<<"Error in ttable file: "<<_incrPhraseModelFileName<<"\n";
    return ERROR;
  }
  else
  {
    if(awk.getline())
    {
      if(awk.NF==4 && strcmp("****",awk.dollar(1).c_str())==0 &&
         strcmp("cache",awk.dollar(2).c_str())==0 && strcmp("ttable",awk.dollar(3).c_str())==0)
      {
        cerr<<"Error in ttable file: "<<_incrPhraseModelFileName<<"\n";
        return ERROR;
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
 Vector<string> s,t;
 awkInputStream awk;
 PhrasePairInfo phpinfo; 
 Count count_s_;
 Count count_s_t_;
 
 cerr<<"Loading phrase ttable from file "<<phraseTTableFileName<<endl;

 if(logFileOpen()) logF<<"Loading phrase ttable from file "<<phraseTTableFileName<<endl;	

 if(awk.open(phraseTTableFileName)==ERROR)
 {
   cerr<<"Error in WBA-Phrase Model file: "<<phraseTTableFileName<<endl;
   return ERROR;
 }
 else
 {   
  while(awk.getline())
  {
    if(awk.FNR>=1 && awk.NF>1)
    {
      i=1; 
      s.clear();	  
      while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
      {
        s.push_back(awk.dollar(i)); 
        ++i;
      }
      ++i;
      t.clear();
      while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
      {
        t.push_back(awk.dollar(i));			   
        ++i; 
      }
      if(i<awk.NF-1 && strcmp("|||",awk.dollar(i).c_str())==0)
      {
        ++i;
        count_s_=atof(awk.dollar(i).c_str());
        ++i;
        count_s_t_=atof(awk.dollar(i).c_str());  

        phpinfo.first=count_s_;
        phpinfo.second=count_s_t_;
        strAddTableEntry(s,t,phpinfo);  
      } 
      else
      {
        cerr<<"Error in WBA-Phrase Model file: "<<phraseTTableFileName<<endl;
        cerr<<"(Note: ensure your model was generated using the -pc option)"<<endl;
        return ERROR;
      }
    }
  }
 }
 return OK;
}

//-------------------------
bool _incrPhraseModel::load_seglentable(const char *segmLengthTableFileName)
{
  if(logFileOpen()) logF<<"Loading segmentation length table from file "<<segmLengthTableFileName<<endl;
  return segLenTable.load_seglentable(segmLengthTableFileName);
}
//-------------------------
bool _incrPhraseModel::print(const char *prefix)
{
  char ttableFileName[1024];
  char segLenTableFileName[1024];
  bool retVal;
  
  sprintf(ttableFileName,"%s.ttable",prefix);
  retVal=printTTable(ttableFileName);
  if(retVal) return false;
  
  sprintf(segLenTableFileName,"%s.seglentable",prefix);
  return printSegmLengthTable(segLenTableFileName);
}

//-------------------------
bool _incrPhraseModel::printTTable(const char *outputFileName)
{
# ifdef _GLIBCXX_USE_LFS
  ofstream outF;

  outF.open(outputFileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing phrase model to file."<<endl;
    return ERROR;
  }
  else
  {
    printTTable(outF);
    return OK;
  }
# else
  FILE *outf;

  outf=fopen(outputFileName,"w");
  if(outf==NULL)
  {
    cerr<<"Error while printing phrase model to file."<<endl;
    return ERROR;
  }
  else
  {
    printTTable(outf);
    return OK;
  }
# endif
}

//-------------------------
bool _incrPhraseModel::printTTablePharaoh_v1(const char *outputFileName)
{
# ifdef _GLIBCXX_USE_LFS
  ofstream outF;

  outF.open(outputFileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing phrase model in Pharaoh v1 format to file."<<endl;
    return ERROR;
  }
  else
  {
    printTTablePharaoh_v1(outF);		
    outF.close();	
    return OK;
  }
# else
  FILE *outf;

  outf=fopen(outputFileName,"w");
  if(outf==NULL)
  {
    cerr<<"Error while printing phrase model in Pharaoh v1 format to file."<<endl;
    return ERROR;
  }
  else
  {
    printTTablePharaoh_v1(outf);
    return OK;
  } 
# endif
}
//-------------------------
bool _incrPhraseModel::printTTableMoses(const char *outputFileName)
{
# ifdef _GLIBCXX_USE_LFS
  ofstream outF;

  outF.open(outputFileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing phrase model in Pharaoh v123 format to file."<<endl;
    return ERROR;
  }
  else
  {
    printTTableMoses(outF);		
    outF.close();	
    return OK;
  }
# else
  FILE *outf;

  outf=fopen(outputFileName,"w");
  if(outf==NULL)
  {
    cerr<<"Error while printing phrase model in Pharaoh v123 format to file."<<endl;
    return ERROR;
  }
  else
  {
    printTTableMoses(outf);
    return OK;
  }
# endif
}

//-------------------------
bool _incrPhraseModel::printTTableMoses_id(const char *outputFileName)
{
# ifdef _GLIBCXX_USE_LFS
  ofstream outF;

  outF.open(outputFileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing phrase model in Pharaoh v123 format to file."<<endl;
    return ERROR;
  }
  else
  {
    printTTableMoses_id(outF);		
    outF.close();	
    return OK;
  }
# else
  FILE *outf;

  outf=fopen(outputFileName,"w");
  if(outf==NULL)
  {
    cerr<<"Error while printing phrase model in Pharaoh v123 format to file."<<endl;
    return ERROR;
  }
  else
  {
    printTTableMoses_id(outf);
    return OK;
  }
# endif
}

//-------------------------
bool _incrPhraseModel::printInverseTTable(const char *outputFileName,
                                          int outputFormat/*=PHARAOH_V1_OUTPUT*/)
{
# ifdef _GLIBCXX_USE_LFS
  ofstream outF;
	
  outF.open(outputFileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing inverse phrase model."<<endl;
    return ERROR;
  }
  else
  {
    printInverseTTable(outF,outputFormat);
    outF.close();
    return OK;	 
  }
# else
  FILE *outf;

  outf=fopen(outputFileName,"w");
  if(outf==NULL)
  {
    cerr<<"Error while printing inverse phrase model."<<endl;
    return ERROR;
  }
  else
  {
    printInverseTTable(outf,outputFormat);
    return OK;
  }  
# endif
}

//-------------------------
bool _incrPhraseModel::printSegmLengthTable(const char *outputFileName)
{
 ofstream outF;

 outF.open(outputFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while printing segmentation length table."<<endl;
   return 1;
 }

 printSegmLengthTable(outF);	
 outF.close();	

 return 0;
}

# ifdef _GLIBCXX_USE_LFS
//-------------------------
void _incrPhraseModel::printTTable(ostream &outS)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
  
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;
      
        // Set float precision.
    outS.setf( ios::fixed, ios::floatfield );
    outS.precision(8);
    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          outS<<wordIndexToSrcString(*vectorWordIndexIter)<<" "; 
        outS<<"|||"; 
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          outS<<" "<<wordIndexToTrgString(*vectorWordIndexIter);
        outS<<" ||| "<<srctnIter->second.first.get_c_s()<<" "<<srctnIter->second.second.get_c_st()<<endl;
      }
    }
  }
}

//-------------------------
void _incrPhraseModel::printTTablePharaoh_v1(ostream &outS)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
     
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;

        // Set float precision.
    outS.setf( ios::fixed, ios::floatfield );
    outS.precision(8);
    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          outS<<wordIndexToSrcString(*vectorWordIndexIter)<<" "; 
        outS<<"|||"; 
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          outS<<" "<<wordIndexToTrgString(*vectorWordIndexIter);
        outS<<" ||| "<<(float)srctnIter->second.second.get_c_st()/(float)srctnIter->second.first.get_c_s()<<endl;
      }
    }
  }
}
//-------------------------
void _incrPhraseModel::printTTableMoses(ostream &outS)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
  
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;
      
        // Set float precision.
    outS.setf( ios::fixed, ios::floatfield );
    outS.precision(8);
 
    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          outS<<wordIndexToTrgString(*vectorWordIndexIter)<<" ";
        outS<<"||| ";
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          outS<<wordIndexToSrcString(*vectorWordIndexIter)<<" "; 
        outS<<"||| "<<(float)srctnIter->second.second.get_c_st()/(float)srctnIter->second.first.get_c_s()<<endl;
      }
    }
  }
}
//-------------------------
void _incrPhraseModel::printTTableMoses_id(ostream &outS)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
  
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;
      
        // Set float precision.
    outS.setf( ios::fixed, ios::floatfield );
    outS.precision(8);

    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      Count count_t_=ptPtr->cTrg(phraseTIter->first);
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          outS<<wordIndexToTrgString(*vectorWordIndexIter)<<" ";
        outS<<"||| ";
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          outS<<wordIndexToSrcString(*vectorWordIndexIter)<<" "; 
        outS<<"||| "<<(float)srctnIter->second.second.get_c_st()/(float)srctnIter->second.first.get_c_s()<<" ";
        outS<<(float)srctnIter->second.second.get_c_st()/(float)count_t_<<endl;
      }
    }
  }
}

//-------------------------
void _incrPhraseModel::printInverseTTable(ostream &outS,
                                          int outputFormat/*=PHARAOH_V1_OUTPUT*/)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
  
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;

        // Set float precision.
    outS.setf( ios::fixed, ios::floatfield );
    outS.precision(8);

    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);
      Count count_t_=ptPtr->cTrg(phraseTIter->first);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          outS<<wordIndexToTrgString(*vectorWordIndexIter).c_str()<<" ";
        outS<<"|||";
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          outS<<" "<<wordIndexToSrcString(*vectorWordIndexIter).c_str();
        outS<<" ||| "<<(float)srctnIter->second.second.get_c_st()/(float)count_t_;
        if(outputFormat==THOT_COUNT_OUTPUT)
          outS<<" "<<(float)srctnIter->second.second.get_c_st();
        outS<<endl;
      }
    }
  }
}
#endif
//-------------------------
void _incrPhraseModel::printTTable(FILE* file)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);

  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;
      
    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          fprintf(file,"%s ",wordIndexToSrcString(*vectorWordIndexIter).c_str());
        fprintf(file,"|||"); 
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          fprintf(file," %s",wordIndexToTrgString(*vectorWordIndexIter).c_str());
        fprintf(file," ||| %.8f %.8f\n",(float)srctnIter->second.first.get_c_s(),(float)srctnIter->second.second.get_c_st());
      }
    }
  }
}

//-------------------------
void _incrPhraseModel::printTTablePharaoh_v1(FILE* file)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
     
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;

    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          fprintf(file,"%s ",wordIndexToSrcString(*vectorWordIndexIter).c_str());
        fprintf(file,"|||"); 
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          fprintf(file," %s",wordIndexToTrgString(*vectorWordIndexIter).c_str());
        fprintf(file," ||| %.8f\n",(float)srctnIter->second.second.get_c_st()/(float)srctnIter->second.first.get_c_s());
      }
    }
  }
}

//-------------------------
void _incrPhraseModel::printTTableMoses(FILE *file)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
  
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;

    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          fprintf(file,"%s ",wordIndexToTrgString(*vectorWordIndexIter).c_str());
        fprintf(file,"|||");
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          fprintf(file," %s",wordIndexToSrcString(*vectorWordIndexIter).c_str());
        fprintf(file," ||| %.8f\n",(float)srctnIter->second.second.get_c_st()/(float)srctnIter->second.first.get_c_s());
      }
    }
  }
}
//-------------------------
void _incrPhraseModel::printTTableMoses_id(FILE *file)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
  
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;

    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);
      Count count_t_=ptPtr->cTrg(phraseTIter->first);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          fprintf(file,"%s ",wordIndexToTrgString(*vectorWordIndexIter).c_str());
        fprintf(file,"|||");
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          fprintf(file," %s",wordIndexToSrcString(*vectorWordIndexIter).c_str());
        fprintf(file," ||| %.8f ",(float)srctnIter->second.second.get_c_st()/(float)srctnIter->second.first.get_c_s());
        fprintf(file,"%.8f\n",(float)srctnIter->second.second.get_c_st()/(float)count_t_);
      }
    }
  }
}

//-------------------------
void _incrPhraseModel::printInverseTTable(FILE* file,
                                          int outputFormat/*=PHARAOH_V1_OUTPUT*/)
{
  PhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<PhraseTable*>(basePhraseTablePtr);
  
  if(ptPtr) // C++ RTTI
  {
    PhraseTable::const_iterator phraseTIter;

    for(phraseTIter=ptPtr->begin();phraseTIter!=ptPtr->end();++phraseTIter)
    {
      PhraseTable::SrcTableNode srctn;
      PhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);
      Count count_t_=ptPtr->cTrg(phraseTIter->first);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        Vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          fprintf(file,"%s ",wordIndexToTrgString(*vectorWordIndexIter).c_str());
        fprintf(file,"|||");
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          fprintf(file," %s",wordIndexToSrcString(*vectorWordIndexIter).c_str());
        fprintf(file," ||| %.8f",(float)srctnIter->second.second.get_c_st()/(float)count_t_);
        if(outputFormat==THOT_COUNT_OUTPUT)
          fprintf(file," %.8f",(float)srctnIter->second.second.get_c_st());
        fprintf(file,"\n");
      }
    }
  }
}

//-------------------------
void _incrPhraseModel::printNbestTransTableNode(NbestTableNode<PhraseTransTableNodeData> tTableNode,
                                                ostream &outS)
{
 NbestTableNode<PhraseTransTableNodeData>::iterator transTableNodeIter;
 Vector<WordIndex>::iterator VectorWordIndexIter;
	
  for(transTableNodeIter=tTableNode.begin();transTableNodeIter!=tTableNode.end();++transTableNodeIter)	 
  {
   for(VectorWordIndexIter=transTableNodeIter->second.begin();VectorWordIndexIter!=transTableNodeIter->second.end();++VectorWordIndexIter)
      outS<<wordIndexToTrgString(*VectorWordIndexIter)<<" ";  	 
   outS<<"||| "<< (float) transTableNodeIter->first<<endl;
  }	
}

//-------------------------
void _incrPhraseModel::printSegmLengthTable(ostream &outS)
{
  return segLenTable.printSegmLengthTable(outS);
}

//-------------------------
size_t _incrPhraseModel::getSrcVocabSize(void)const
{
  return singleWordVocab.getSrcVocabSize();	
}

//-------------------------
void _incrPhraseModel::printSentAligInAachenFormat(const Vector<unsigned int> &alig)
{
 unsigned int j;
	
 for(j=0;j<alig.size();++j)	
 {
  if(alig[j]!=0) cout<<"S "<<j<<" "<<alig[j]-1<<endl;
 }
}

//-------------------------
void _incrPhraseModel::printSentAligInAachenFormat(const WordAligMatrix &alig)
{
 unsigned int j,i;
	
 for(j=0;j<alig.get_J();++j)	
 {
  for(i=0;i<alig.get_I();++i)
	  if(alig.getValue(i,j)!=0) cout<<"S "<<j<<" "<<i<<endl;
 }
}

//-------------------------
void _incrPhraseModel::printPhraseAligInAachenFormat(unsigned int srcPhraseBeg,
                                                     unsigned int srcPhraseEnd,
                                                     unsigned int I,
                                                     Bitset<MAX_SENTENCE_LENGTH> coverage)
{
 unsigned int j_,i_;
 
 for(j_=srcPhraseBeg;j_<=srcPhraseEnd;++j_)
 {
  for(i_=1;i_<I;++i_)
  {
   if(coverage.test(i_)==1) cout<<"P "<<j_<<" "<<i_-1<<endl;	 
  }
 }
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
WordIndex _incrPhraseModel::stringToSrcWordIndex(string s)const
{	
 return singleWordVocab.stringToSrcWordIndex(s);
}

//-------------------------
string _incrPhraseModel::wordIndexToSrcString(WordIndex w)const
{
 return singleWordVocab.wordIndexToSrcString(w);
}

//-------------------------
bool _incrPhraseModel::existSrcSymbol(string s)const
{
 return singleWordVocab.existSrcSymbol(s);
}

//-------------------------
Vector<WordIndex> _incrPhraseModel::strVectorToSrcIndexVector(const Vector<string>& s,
                                                              Count numTimes/*=1*/)
{
  Vector<WordIndex> swVec;
  
  for(unsigned int i=0;i<s.size();++i)
    swVec.push_back(addSrcSymbol(s[i],numTimes));
  
  return swVec;
}

//-------------------------
Vector<string> _incrPhraseModel::srcIndexVectorToStrVector(const Vector<WordIndex>& s)
{
 Vector<string> vStr;
 unsigned int i;

 for(i=0;i<s.size();++i)
    vStr.push_back((wordIndexToSrcString(s[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex _incrPhraseModel::addSrcSymbol(string s,
                                         Count numTimes/*=1*/)
{
 return singleWordVocab.addSrcSymbol(s,numTimes);
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
WordIndex _incrPhraseModel::stringToTrgWordIndex(string t)const
{
 return singleWordVocab.stringToTrgWordIndex(t);
}

//-------------------------
string _incrPhraseModel::wordIndexToTrgString(WordIndex w)const
{
 return singleWordVocab.wordIndexToTrgString(w);
}

//-------------------------
bool _incrPhraseModel::existTrgSymbol(string t)const
{
 return singleWordVocab.existTrgSymbol(t);
}

//-------------------------
Vector<WordIndex> _incrPhraseModel::strVectorToTrgIndexVector(const Vector<string>& t,
                                                              Count numTimes/*=1*/)
{
  Vector<WordIndex> twVec;
  
  for(unsigned int i=0;i<t.size();++i)
    twVec.push_back(addTrgSymbol(t[i],numTimes));
  
  return twVec;
}
//-------------------------
Vector<string> _incrPhraseModel::trgIndexVectorToStrVector(const Vector<WordIndex>& t)
{
 Vector<string> vStr;
 unsigned int i;

 for(i=0;i<t.size();++i)
    vStr.push_back((wordIndexToTrgString(t[i]))); 	 
	
 return vStr;
}
//-------------------------
WordIndex _incrPhraseModel::addTrgSymbol(string t,
                                         Count numTimes/*=1*/)
{
 return singleWordVocab.addTrgSymbol(t,numTimes);
}
//-------------------------
bool _incrPhraseModel::printTrgVocab(const char *outputFileName)
{
 return singleWordVocab.printTrgVocab(outputFileName);
}

//-------------------------
bool _incrPhraseModel::createLogFile(char *_logFileName)
{
 logF.open(_logFileName,ios::out);
 
 if(!logF)
 {
   cerr<<"Error while creating log file."<<endl; 
   return 1;
 }
 else
 {
   logFileName=_logFileName;
   return 0;	 
 }
}

//-------------------------
bool _incrPhraseModel::addToLogFile(char *s)
{
 if(!logFileOpen()) return 0;
 else
 {
   logF<<s;
   return 1;		
 }
}

//-------------------------
bool _incrPhraseModel::logFileOpen(void)
{
 if(logFileName=="") return 0;
 else return 1;	 
}

//-------------------------
void _incrPhraseModel::closeLogFile(void)
{
 logFileName="";	
 logF.close();	
}

//-------------------------
Vector<string> _incrPhraseModel::stringToStringVector(string s)
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
Vector<string> _incrPhraseModel::extractCharItemsToVector(char *ch)const
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
  if(logFileOpen()) closeLogFile();
}

//-------------------------
