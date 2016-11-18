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
/* Module: FilePt                                                   */
/*                                                                  */
/* Definitions file: FilePt.cc                                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "FilePt.h"

//--------------- Function definitions

//-------------------------
FilePt::FilePt(void)
{
  srcTransForTrgFileHandle=NULL;
  srcPhrasesFileHandle=NULL;
  trgPhrasesFileHandle=NULL;
  numDictRecords=0;
}

//-------------------------
bool FilePt::init(const char *fileName)
{
      // clear data structures
  clear();

  cerr<<"Loading phrase table stored in files..."<<endl;
  
      // Open srcinfo file
  std::string srcinfofile=fileName;
  srcinfofile=srcinfofile+".srcphr";
  srcPhrasesFileHandle=fopen(srcinfofile.c_str(),"r");
  if(srcPhrasesFileHandle==NULL)
  {
    cerr<<"Error while opening file with information about source phrases: "<<srcinfofile<<endl;
    return ERROR;
  }

      // Open srccounts file
  std::string srccountsfile=fileName;
  srccountsfile=srccountsfile+".srccounts";
  bool ret=loadSrcCountsFile(srccountsfile);
  if(ret==ERROR)
    return ERROR;
  
        // Open trginfo file
  std::string trginfofile=fileName;
  trginfofile=trginfofile+".trgphr";
  trgPhrasesFileHandle=fopen(trginfofile.c_str(),"r");
  if(trgPhrasesFileHandle==NULL)
  {
    cerr<<"Error while opening file with information about target phrases: "<<trginfofile<<endl;
    return ERROR;
  }

      // Open trgcounts file
  std::string trgcountsfile=fileName;
  trgcountsfile=trgcountsfile+".trgcounts";
  ret=loadTrgCountsFile(trgcountsfile);
  if(ret==ERROR)
    return ERROR;
  
      // Process file with dictionary (source translations for target phrases)
  std::string phrdictfile=fileName;
  phrdictfile=phrdictfile+".phrdict";
  ret=processDictFile(phrdictfile);
  if(ret==ERROR)
    return ERROR;
  
  return OK;
}

//-------------------------
bool FilePt::loadSrcCountsFile(std::string fileName)
{
  awkInputStream awk;
  
  bool ret=awk.open(fileName.c_str());
  if(ret==ERROR)
  {
    cerr<<"Error while opening file with source phrase counts: "<<fileName<<endl;
    return ERROR;
  }
  awk.getln();
  for(unsigned int i=1;i<=awk.NF;i+=3)
  {
    unsigned int idx=atoi(awk.dollar(i).c_str());
    PhrIndex num=atoi(awk.dollar(i+1).c_str());
    while(numRecordsInSrcInfoFile.size() <= idx) 
      numRecordsInSrcInfoFile.push_back(0);
    numRecordsInSrcInfoFile[idx]=num;
  }
  return OK;
}

//-------------------------
bool FilePt::loadTrgCountsFile(std::string fileName)
{
  awkInputStream awk;

  bool ret=awk.open(fileName.c_str());
  if(ret==ERROR)
  {
    cerr<<"Error while opening file with target phrase counts: "<<fileName<<endl;
    return ERROR;
  }
  awk.getln();
  for(unsigned int i=1;i<=awk.NF;i+=3)
  {
    unsigned int idx=atoi(awk.dollar(i).c_str());
    PhrIndex num=atoi(awk.dollar(i+1).c_str());
    while(numRecordsInTrgInfoFile.size() <= idx) 
      numRecordsInTrgInfoFile.push_back(0);
    numRecordsInTrgInfoFile[idx]=num;
  }
  return OK;
}

//-------------------------
bool FilePt::processDictFile(std::string fileName)
{
  srcTransForTrgFileHandle=fopen(fileName.c_str(),"r");
  if(srcTransForTrgFileHandle==NULL)
  {
    cerr<<"Error while opening file with source translations for target phrases: "<<fileName<<endl;
    return ERROR;
  }
      // Obtain dict file size
  fseeko(srcTransForTrgFileHandle,0,SEEK_END); // seek to end of file
  off_t filesize=ftello(srcTransForTrgFileHandle); // get current file pointer
  rewind(srcTransForTrgFileHandle); // seek back to beginning of file and clear error indicator

      // Obtain number of records stored in dictionary
  numDictRecords=(filesize/getDictRecordLen());

  return OK;
}

//-------------------------
size_t FilePt::getDictRecordLen(void)
{
  return (2*sizeof(PhrIndex))+1*sizeof(float);
}

//-------------------------
size_t FilePt::getSrcInfoRecordLen(unsigned int srcphrlen)
{
  return (srcphrlen*sizeof(WordIndex))+1*sizeof(float);
}

//-------------------------
size_t FilePt::getTrgInfoRecordLen(unsigned int trgphrlen)
{
  // return (trgphrlen*sizeof(WordIndex))+1*sizeof(float);
  return (trgphrlen*sizeof(WordIndex))+1*sizeof(float)+1*sizeof(long long);
}

//-------------------------
Count FilePt::getSrcInfo(const Vector<WordIndex>& s,
                         bool& found)
{
  pair<Count,PhrIndex> pcp=getSrcInfoAux(s,found);
  return pcp.first;
}

//-------------------------
pair<Count,PhrIndex>
FilePt::getSrcInfoAux(const Vector<WordIndex>& s,
                      bool& found)
{
  Count c=0.0;
  PhrIndex idx=0;
  
      // Init found variable
  found=false;
  
      // Check if size is too great
  if(s.size()>numRecordsInSrcInfoFile.size())
    return make_pair(c,idx);

      // Initialize recordLen
  off_t recordLen=getSrcInfoRecordLen(s.size());

      // Search phrase s using binary search
  off_t left;
  off_t right;
  off_t offset;
  off_t mid;
  
      // Initialize limits
  obtainLimitsForSrcPhrasesFile(s.size(),offset,left,right);
  
  while(left<=right)
  {
    mid=(off_t)((left+right)/2);
    int err=fseeko(srcPhrasesFileHandle,offset+mid*recordLen,0);
    if(err==-1)
    {
      found=false;
      return make_pair(0.0,idx);
    }
    unsigned int i;
    for(i=0;i<s.size();++i)
    {
      WordIndex word;
      size_t st=fread(&word,sizeof(WordIndex),1,srcPhrasesFileHandle);
      if(s[i]<word)
      {
        right=mid-1;
        break;
      }
      if(s[i]>word)
      {
        left=mid+1;
        break;
      }
    }
    if(i==s.size())
    {
      size_t st=fread(&c,sizeof(float),1,srcPhrasesFileHandle);
      break;
    }
  }
      // Obtain info for source phrase if found
  if(left<=right)
  {
        // Obtain index for source phrase
    idx=mid;
    for(unsigned int i=1;i<=s.size()-1;++i)
    {
      idx+=numRecordsInSrcInfoFile[i];
    }
    ++idx;
    
    found=true;
  }
      // Return count and index
  return make_pair(c,idx);
}

//-------------------------
Count FilePt::getTrgInfo(const Vector<WordIndex>& t,
                         bool& found)
{
  PhrIndex phrIdx;
  Count c;
  long long dictStartRecord;
  found=obtainInfoForTrgPhraseFile(t,phrIdx,c,dictStartRecord);
  return c;
}

//-------------------------
Count FilePt::getSrcTrgInfo(const Vector<WordIndex>& s,
                            const Vector<WordIndex>& t,
                            bool &found)
{
  return getSrcTrgInfoAux1(s,t,found);
  // return getSrcTrgInfoAux2(s,t,found);

}

//-------------------------
Count FilePt::getSrcTrgInfoAux1(const Vector<WordIndex>& s,
                                const Vector<WordIndex>& t,
                                bool &found)
{
      // Declare variable to store result
  Count result;
  
      // Init found variable
  found=false;

      // Obtain index for s
  bool foundInTrgFile;
  PhrIndex sidx=obtainIdxForSrcPhraseFile(s,foundInTrgFile);
  
  if(!foundInTrgFile) return 0;

      // Obtain index for t
  PhrIndex tidx=obtainIdxForTrgPhraseFile(t,foundInTrgFile);

  if(!foundInTrgFile) return 0;

      // Initialize recordLen
  off_t recordLen=getDictRecordLen();

      // Search phrase s using binary search
  off_t left;
  off_t right;
  off_t offset;
  off_t mid;
  
      // Initialize limits
  obtainLimitsForDictFile(offset,left,right);
  
  while(left<=right)
  {
    mid=(off_t)((left+right)/2);
    int err=fseeko(srcTransForTrgFileHandle,offset+mid*recordLen,0);
    if(err==-1)
    {
      found=false;
      return 0;
    }

        // Read target index
    PhrIndex spidx;
    PhrIndex tpidx;
    Count stc;
    size_t st=readDictRecord(spidx,tpidx,stc);
    if(tidx<tpidx)
      right=mid-1;
    if(tidx>tpidx)
      left=mid+1;
    if(tidx==tpidx)
    {
      if(sidx<spidx)
        right=mid-1;
      if(sidx>spidx)
        left=mid+1;
      if(sidx==spidx)
      {
        result=stc;
        break;
      }
    }
  }
      // Retrieve translations for t if found
  if(left<=right)
  {
    found=true;
  }

      // Return joint count
  return result;
}

//-------------------------
Count FilePt::getSrcTrgInfoAux2(const Vector<WordIndex>& s,
                                const Vector<WordIndex>& t,
                                bool &found)
{
      // Obtain all entries for t
  SrcTableNode srctn;
  getEntriesForTarget(t,srctn);
  
      // Search if there exists an entry for s
  Count c;
  found=false;
  SrcTableNode::iterator iter;
  for(iter=srctn.begin();iter!=srctn.end();++iter)
  {
    if(s==iter->first)
    {
      c=iter->second.second;
      found=true;
      break;
    }
  }
  return c;
}

//-------------------------
void FilePt::obtainLimitsForSrcPhrasesFile(unsigned int srcphrlen,
                                           off_t& offset,
                                           off_t& left,
                                           off_t& right)
{
      // Set value of the offset variable
  offset=0;
  for(unsigned int i=1;i<srcphrlen;++i)
  {
    offset=offset+(numRecordsInSrcInfoFile[i]*getSrcInfoRecordLen(i));
  }

      // Initialize left variable
  left=0;

      // Set value of the right variable
  right=numRecordsInSrcInfoFile[srcphrlen]-1;
}

//-------------------------
bool FilePt::getEntriesForTarget(const Vector<WordIndex>& t,
                                 SrcTableNode& srctn,
                                 int N/*=-1*/)
{
  Vector<pair<Vector<WordIndex>,off_t> > offsetOfRecordsVec;
  return getEntriesForTargetAux(t,srctn,offsetOfRecordsVec,N);
}

//-------------------------
bool FilePt::getEntriesForTargetAux(const Vector<WordIndex>& t,
                                    FilePt::SrcTableNode& srctn,
                                    Vector<pair<Vector<WordIndex>,off_t> >& offsetOfRecordsVec,
                                    int N)
{
      // Reset srctn
  srctn.clear();

      // Reset offsetOfRecordsVec
  offsetOfRecordsVec.clear();

      // Obtain information for target phrase t
  PhrIndex tidx;
  Count c;
  long long dictStartRecord;
  bool trg_found=obtainInfoForTrgPhraseFile(t,tidx,c,dictStartRecord);

      // Retrieve translations for t if found
  if(trg_found)
  {
        // Set dictionary file position
    off_t recordLen=getDictRecordLen();
    off_t offset=dictStartRecord*recordLen;
    fseeko(srcTransForTrgFileHandle,offset,0);
    
        // Obtain translation options starting from current dictionary
        // file position
    addTransOptsFromCurrentFilePos(tidx,c,srctn,offsetOfRecordsVec);
  }
      // Return value of the trg_found variable
  return trg_found;
}

//-------------------------
bool FilePt::searchTargetPhraseInDict(PhrIndex tidx)
{
      // Search target phrase using binary search
  off_t left;
  off_t right;
  off_t offset;
  off_t mid;

      // Initialize recordLen
  off_t recordLen=getDictRecordLen();

      // Initialize limits
  obtainLimitsForDictFile(offset,left,right);
  
  while(left<=right)
  {
    mid=(off_t)((left+right)/2);
    int err=fseeko(srcTransForTrgFileHandle,offset+mid*recordLen,0);
    if(err==-1)
    {
      return false;
    }

        // Read target index
    PhrIndex spidx;
    PhrIndex tpidx;
    Count stc;
    size_t st=readDictRecord(spidx,tpidx,stc);
    if(tidx<tpidx)
      right=mid-1;
    if(tidx>tpidx)
      left=mid+1;
    if(tidx==tpidx)
      break;
  }

      // Return true if target phrase was found
  if(left<=right)
    return true;
  else
    return false;
}

//-------------------------
void FilePt::rewindUntilFirstOcurrenceOfTrgPhrase(PhrIndex tidx)
{
      // Rewind until finding first ocurrence of t
  bool end=false;
  while(!end)
  {
        // Position file indicator
    off_t offsetInRecordsForDict=getCurrOffsetInRecordsForDict();
    if(offsetInRecordsForDict<=1)
    {
          // Rewind to start of file
      jumpDictRecords(-1);
          // End while loop
      end=true;
    }
    else
    {
          // Rewind two records
      jumpDictRecords(-2);

          // Read record
      PhrIndex spidx;
      PhrIndex tpidx;
      Count stc;
      size_t st=readDictRecord(spidx,tpidx,stc);
        
          // Find the last ocurrence before the first ocurrence of t
      if(tpidx!=tidx)
        end=true;
    }
  }
}

//-------------------------
void FilePt::addTransOptsFromCurrentFilePos(PhrIndex tidx,
                                            Count trgCount,
                                            FilePt::SrcTableNode& srctn,
                                            Vector<pair<Vector<WordIndex>,off_t> >& offsetOfRecordsVec)
{
      // Obtain translation options
  bool end=false;
  Vector<PhrIndex> spidxVec;
  Vector<PhrasePairInfo> phpinfoVec;
  Vector<off_t> offsetVec;
  
  while(!end)
  {
        // Initialize variables
    PhrIndex spidx;
    PhrIndex tpidx;
    Count jointCount;
    off_t offset=ftello(srcTransForTrgFileHandle);
    size_t st=readDictRecord(spidx,tpidx,jointCount);
    PhrasePairInfo phpinfo;
    phpinfo.first=trgCount;
    phpinfo.second=jointCount;

        // Obtain entry information
    if(st==0) end=true;
    else
    {
      if(tpidx==tidx)
      {
        spidxVec.push_back(spidx);
        phpinfoVec.push_back(phpinfo);
        offsetVec.push_back(offset);
      }
      else
      {
        end=true;
      }
    }
  }

      // Store translation options
  for(unsigned int i=0;i<spidxVec.size();++i)
  {
        // Insert entry in SrcTableNode
    Vector<WordIndex> srcVec;
    Count srcCount;
    bool found;

        // Obtain source vector and its count
    obtainInfoGivenScrPhrIdx(spidxVec[i],
                             srcVec,
                             srcCount,
                             found);
  
        // Complete phpinfo with joint count
    pair<Vector<WordIndex>,PhrasePairInfo> pVecPhinfo;
    pVecPhinfo.first=srcVec;
    pVecPhinfo.second=phpinfoVec[i];
  
        // Add entry to source table node
    srctn.insert(pVecPhinfo);

        // Insert offset in offset vector
    offsetOfRecordsVec.push_back(make_pair(pVecPhinfo.first,offsetVec[i]));
  }
}

//-------------------------
off_t FilePt::getCurrOffsetInRecordsForDict(void)
{
  off_t curr_pos=ftello(srcTransForTrgFileHandle);
  return curr_pos/getDictRecordLen();
}

//-------------------------
bool FilePt::jumpDictRecords(off_t numRecords)
{
  int err=fseeko(srcTransForTrgFileHandle,numRecords*getDictRecordLen(),SEEK_CUR);
  if(err==-1)
  {
    return ERROR;
  }
  else return OK;
}

//-------------------------
size_t FilePt::readDictRecord(PhrIndex& spidx,
                              PhrIndex& tpidx,
                              Count& stc)
{
  size_t st=fread(&tpidx,sizeof(PhrIndex),1,srcTransForTrgFileHandle);
  st+=fread(&spidx,sizeof(PhrIndex),1,srcTransForTrgFileHandle);
  st+=fread(&stc,sizeof(Count),1,srcTransForTrgFileHandle);

  return st;
}

//-------------------------
void FilePt::obtainInfoGivenScrPhrIdx(PhrIndex idx,
                                      Vector<WordIndex>& s,
                                      Count& c,
                                      bool& found)
{
  found=false;
  
  s.clear();

      // Obtain offset
  unsigned int len;
  bool offsetFound;
  off_t offset=obtainOffsetOfSrcPhraseGivenIdx(idx,len,offsetFound);

  if(offsetFound)
  {
        // Set file position indicator
    int err=fseeko(srcPhrasesFileHandle,offset,0);
    if(err==0)
    {      
          // Read source phrase
      for(unsigned int j=0;j<len;++j)
      {
        WordIndex widx;
        size_t st=fread(&widx,sizeof(WordIndex),1,srcPhrasesFileHandle);
        s.push_back(widx);
      }
          // Obtain count
      size_t st=fread(&c,sizeof(float),1,srcPhrasesFileHandle);

          // Set found flag to true
      found=true;
    }
  }
}

//-------------------------
off_t FilePt::obtainOffsetOfSrcPhraseGivenIdx(PhrIndex idx,
                                              unsigned int& len,
                                              bool& found)
{
      // Init output vars
  found=false;
  len=0;

      // Try to obtain offset
  if(!numRecordsInSrcInfoFile.empty())
  {
    PhrIndex phrIndex;
    off_t offset=0;

        // Determine length
    unsigned int i=0;
    phrIndex=numRecordsInSrcInfoFile[i];

    while(phrIndex<idx && i<numRecordsInSrcInfoFile.size())
    {
      ++i;
      phrIndex+=numRecordsInSrcInfoFile[i];
    }
    if(phrIndex>=idx)
    {
          // Set length
      len=i;

          // Obtain phrase order for length "len"
      PhrIndex phrIdxLen=idx-(phrIndex-numRecordsInSrcInfoFile[i]);

          // Set value of the offset variable for phrase lengths lower
          // than the current one
      for(unsigned int j=1;j<len;++j)
      {
        offset=offset+(numRecordsInSrcInfoFile[j]*getSrcInfoRecordLen(j));
      }

          // Finally we sum the offset for the current phrase length
          // NOTE: we subtract 1 to phrIdxLen since indices start at 1
      offset=offset+((phrIdxLen-1)*getSrcInfoRecordLen(len));
    }
        // Set found flag
    found=true;
    
        // Return result
    return offset;
  }
  else return 0;
}

//-------------------------
PhrIndex FilePt::obtainIdxForSrcPhraseFile(const Vector<WordIndex>& s,
                                           bool& found)
{
  pair<Count,PhrIndex> pcp=getSrcInfoAux(s,found);
  return pcp.second;
}

//-------------------------
PhrIndex FilePt::obtainIdxForTrgPhraseFile(const Vector<WordIndex>& t,
                                           bool& found)
{
  PhrIndex phrIdx;
  Count c;
  long long dictStartRecord;
  found=obtainInfoForTrgPhraseFile(t,phrIdx,c,dictStartRecord);
  return phrIdx;
}

//-------------------------
bool FilePt::obtainInfoForTrgPhraseFile(const Vector<WordIndex>& t,
                                        PhrIndex& phrIdx,
                                        Count& count,
                                        long long& dictStartRecord)
{
      // Initialize output variables
  phrIdx=0;
  count=0;
  dictStartRecord=-1;

      // Check if size is too great
  if(t.size()>numRecordsInTrgInfoFile.size())
    return false;

      // Check if t is empty
  if(t.empty())
    return false;

      // Initialize recordLen
  off_t recordLen=getTrgInfoRecordLen(t.size());

      // Search phrase t using binary search
  off_t left;
  off_t right;
  off_t offset;
  off_t mid;

      // Initialize limits
  obtainLimitsForTrgPhrasesFile(t.size(),offset,left,right);

  while(left<=right)
  {
    mid=(off_t)((left+right)/2);
    int err=fseeko(trgPhrasesFileHandle,offset+mid*recordLen,0);
    if(err==-1)
    {
      return false;
    }
    unsigned int i;
    for(i=0;i<t.size();++i)
    {
      WordIndex word;
      size_t st=fread(&word,sizeof(WordIndex),1,trgPhrasesFileHandle);
      if(t[i]<word)
      {
        right=mid-1;
        break;
      }
      if(t[i]>word)
      {
        left=mid+1;
        break;
      }
    }
    if(i==t.size())
    {
      size_t st=fread(&count,sizeof(float),1,trgPhrasesFileHandle);
      st=fread(&dictStartRecord,sizeof(long long),1,trgPhrasesFileHandle);
      break;
    }
  }
      // Obtain index for t if found
  if(left<=right)
  {
        // Obtain index for target phrase
    phrIdx=mid;
    for(unsigned int i=1;i<=t.size()-1;++i)
      phrIdx+=numRecordsInTrgInfoFile[i];
    ++phrIdx;
    return true;
  }
  
  return false;
}

//-------------------------
void FilePt::obtainLimitsForTrgPhrasesFile(unsigned int trgphrlen,
                                           off_t &offset,
                                           off_t &left,
                                           off_t &right)
{
      // Set value of the offset variable
  offset=0;
  for(unsigned int i=1;i<trgphrlen;++i)
  {
    offset=offset+(numRecordsInTrgInfoFile[i]*getTrgInfoRecordLen(i));
  }

      // Initialize left variable
  left=0;

      // Set value of the right variable
  right=numRecordsInTrgInfoFile[trgphrlen]-1;
}

//-------------------------
void FilePt::obtainLimitsForDictFile(off_t &offset,
                                     off_t &left,
                                     off_t &right)
{
  offset=0;
  left=0;
  right=numDictRecords-1;
}

//-------------------------
Prob FilePt::pTrgGivenSrc(const Vector<WordIndex>& s,
                          const Vector<WordIndex>& t)
{
  bool found;
  Count count_s_t_=getSrcTrgInfo(s,t,found);
  if((float)count_s_t_>0)
  {
    Count count_s=getSrcInfo(s,found);
	if((float)count_s>0) 
    {
      return (float) count_s_t_/(float)count_s;
    }
	else
      return PHRASE_PROB_SMOOTH;
  }
  else
    return PHRASE_PROB_SMOOTH;
}

//-------------------------
LgProb FilePt::logpTrgGivenSrc(const Vector<WordIndex>& s,
                               const Vector<WordIndex>& t)
{
  return log((double)pTrgGivenSrc(s,t));
}

//-------------------------
Prob FilePt::pSrcGivenTrg(const Vector<WordIndex>& s,
                          const Vector<WordIndex>& t)
{
  bool found;
  Count count_s_t_=getSrcTrgInfo(s,t,found);
  if((float)count_s_t_>0)
  {
    Count count_t_=getTrgInfo(t,found);
	if((float)count_t_>0) 
    {
//      cerr<<"** "<< count_s_t_<<" "<<count_t_<<endl;
      return (float) count_s_t_/(float)count_t_;
    }
	else
      return PHRASE_PROB_SMOOTH;
  }
  else
    return PHRASE_PROB_SMOOTH;
}

//-------------------------
LgProb FilePt::logpSrcGivenTrg(const Vector<WordIndex>& s,
                               const Vector<WordIndex>& t)
{
  return log((double)pSrcGivenTrg(s,t));  
}

//-------------------------
bool FilePt::getNbestForTrg(const Vector<WordIndex>& t,
                            NbestTableNode<PhraseTransTableNodeData>& nbt,
                            int N)
{
      // Obtain entries for target phrase
  SrcTableNode srctn;
  bool found=getEntriesForTarget(t,srctn);
  if(!found)
    return false;

      // Put entries in NbestTableNode object
  nbt.clear();
  for(SrcTableNode::const_iterator citer=srctn.begin();citer!=srctn.end();++citer)
  {
    LgProb lgProb=log((float)citer->second.second/(float)citer->second.first);
    nbt.insert(lgProb,citer->first);
  }

      // Keep N-best entries
  while(nbt.size()>(unsigned int)N && N>=0)
  {
        // node contains N inverse translations, remove last element
    nbt.removeLastElement();
  }

  return true;
}

//-------------------------
size_t FilePt::size(void)
{
  return numDictRecords;
}

//-------------------------
void FilePt::clear(void)
{
      // Close files
  if(srcPhrasesFileHandle!=NULL) fclose(srcPhrasesFileHandle);
  srcPhrasesFileHandle=NULL;

  if(trgPhrasesFileHandle!=NULL) fclose(trgPhrasesFileHandle);
  trgPhrasesFileHandle=NULL;

  if(srcTransForTrgFileHandle!=NULL) fclose(srcTransForTrgFileHandle);
  srcTransForTrgFileHandle=NULL;
  
      // Clear data structures
  numDictRecords=0;
  numRecordsInSrcInfoFile.clear();
  numRecordsInTrgInfoFile.clear();
}

//-------------------------
FilePt::~FilePt()
{
  clear();
}

//-------------------------
