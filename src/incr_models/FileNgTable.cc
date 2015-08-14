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
/* Module: FileNgTable                                              */
/*                                                                  */
/* Definitions file: FileNgTable.cc                                 */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "FileNgTable.h"

//--------------- Function definitions

//-------------------------
FileNgTable::FileNgTable(void)
{
  ngFileHandle=NULL;
  histsFileHandle=NULL;
  numNgRecords=0;
}

//-------------------------
bool FileNgTable::init(const char *fileName)
{
  awkInputStream awk;

      // clear data structures
  numRecordsInHistInfoFile.clear();

  cerr<<"Initializing file n-gram table..."<<endl;
  
      // Open histinfo file
  std::string histinfofile=fileName;
  histinfofile=histinfofile+".histinfo";
  histsFileHandle=fopen(histinfofile.c_str(),"r");
  if(histsFileHandle==NULL)
  {
    cerr<<"Error while opening file with information about n-gram histories: "<<histinfofile<<endl;
    return ERROR;
  }

      // Open histrecords file
  std::string histrecordsfile=fileName;
  histrecordsfile=histrecordsfile+".histrecords";
  bool ret=awk.open(histrecordsfile.c_str());
  if(ret==ERROR)
  {
    cerr<<"Error while opening file with information of records about source phrases: "<<histrecordsfile<<endl;
    return ERROR;
  }
  awk.getln();
  for(unsigned int i=1;i<=awk.NF;i+=3)
  {
    unsigned int idx=atoi(awk.dollar(i).c_str());
    NgIdx num=atoi(awk.dollar(i+1).c_str());
    while(numRecordsInHistInfoFile.size() <= idx) 
      numRecordsInHistInfoFile.push_back(0);
    numRecordsInHistInfoFile[idx]=num;
  }
  
      // Read file with ngram counts (words given lm histories)
  std::string ngcountsfile=fileName;
  ngcountsfile=ngcountsfile+".ngcounts";
  ngFileHandle=fopen(ngcountsfile.c_str(),"r");
  if(ngFileHandle==NULL)
  {
    cerr<<"Error while opening file with n-gram counts: "<<ngcountsfile<<endl;
    return ERROR;
  }
      // Obtain dict file size
  fseeko(ngFileHandle,0,SEEK_END); // seek to end of file
  off_t filesize=ftello(ngFileHandle); // get current file pointer
  rewind(ngFileHandle); // seek back to beginning of file and clear error indicator

      // Obtain number of records stored in dictionary
  numNgRecords=(filesize/getNgRecordLen());
  
  return OK;
}

//-------------------------
size_t FileNgTable::getNgRecordLen(void)
{
  return 1*sizeof(NgIdx)+1*sizeof(WordIndex)+1*sizeof(float);
}

//-------------------------
size_t FileNgTable::getHistInfoRecordLen(unsigned int histlen)
{
  return histlen*sizeof(WordIndex)+1*sizeof(float)+2*sizeof(off_t);
}

//-------------------------
Count FileNgTable::getHistInfo(const Vector<WordIndex>& hist,
                               bool& found)
{
      // Obtain src info
  FileHistInfo fileHistInfo=getHistInfoAux(hist,found);
  return fileHistInfo.count;
}

//-------------------------
FileHistInfo
FileNgTable::getHistInfoAux(const Vector<WordIndex>& hist,
                            bool& found)
{
  FileHistInfo fileHistInfo;
  
      // Init found variable
  found=false;
  
      // Check if size is too great
  if(hist.size()>numRecordsInHistInfoFile.size())
    return fileHistInfo;

      // Initialize recordLen
  off_t recordLen=getHistInfoRecordLen(hist.size());

      // Search phrase s using binary search
  off_t left;
  off_t right;
  off_t offset;
  off_t mid;
  
      // Initialize limits
  obtainLimitsForHistFile(hist.size(),offset,left,right);
  
  while(left<=right)
  {
    mid=(off_t)((left+right)/2);
    int err=fseeko(histsFileHandle,offset+mid*recordLen,0);
    if(err==-1)
    {
      found=false;
      return fileHistInfo;
    }
    unsigned int i;
    for(i=0;i<hist.size();++i)
    {
      unsigned int word;
      size_t st=fread(&word,sizeof(WordIndex),1,histsFileHandle);
      if(hist[i]<word)
      {
        right=mid-1;
        break;
      }
      if(hist[i]>word)
      {
        left=mid+1;
        break;
      }
    }
    if(i==hist.size())
    {
      size_t st=fread(&fileHistInfo.count,sizeof(float),1,histsFileHandle);
      st=fread(&fileHistInfo.ngcountPtr,sizeof(off_t),1,histsFileHandle);
      break;
    }
  }
      // Obtain info for hist if found
  if(left<=right)
  {
        // Obtain index for hist
    fileHistInfo.ngIdx=mid;
    for(unsigned int i=1;i<=hist.size()-1;++i)
    {
      fileHistInfo.ngIdx+=numRecordsInHistInfoFile[i];
    }
    ++fileHistInfo.ngIdx;
    
    found=true;
  }
      // Return count and index
  return fileHistInfo;
}

//-------------------------
Count FileNgTable::getNgInfo(const Vector<WordIndex>& hist,
                             const WordIndex w,
                             bool &found)
{
      // Obtain ngram trg info
  Count jcount=getNgInfoAux1(hist,w,found);
//  Count jcount=getNgInfoAux2(hist,w,found);
      // Return result
  return jcount;
}

//-------------------------
Count FileNgTable::getNgInfoAux1(const Vector<WordIndex>& hist,
                                 WordIndex w,
                                 bool &found)
{
      // Declare variable to store result
  Count result;
  
      // Init found variable
  found=false;

      // Obtain index for hist
  bool foundInHistFile;
  NgIdx hngidx=obtainIdxForHistFile(hist,foundInHistFile);
  
  if(!foundInHistFile) return 0;

      // Initialize recordLen
  off_t recordLen=getNgRecordLen();

      // Search phrase s using binary search
  off_t left;
  off_t right;
  off_t offset;
  off_t mid;
  
      // Initialize limits
      // REVISE
  obtainLimitsForNgFile(hngidx,offset,left,right);
  
  while(left<=right)
  {
    mid=(off_t)((left+right)/2);
    int err=fseeko(ngFileHandle,offset+mid*recordLen,0);
    if(err==-1)
    {
      found=false;
      return 0;
    }

        // Read target index
    NgIdx histngidx;
    WordIndex word;
    Count stc;
    size_t st=readNgRecord(histngidx,word,stc);
    if(hngidx<histngidx)
      right=mid-1;
    if(hngidx>histngidx)
      left=mid+1;
    if(hngidx==histngidx)
    {
      if(w<word)
        right=mid-1;
      if(w>word)
        left=mid+1;
      if(w==word)
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
Count FileNgTable::getNgInfoAux2(const Vector<WordIndex>& hist,
                                 WordIndex w,
                                 bool &found)
{
      // REVISE

      // Obtain all entries for hist
  WordTableNode wtnode;
  getEntriesForHist(hist,wtnode);
  
      // Search if there exists an entry for w
  Count c;
  found=false;
  WordTableNode::iterator iter;
  for(iter=wtnode.begin();iter!=wtnode.end();++iter)
  {
    if(w==iter->first)
    {
      c=iter->second.second;
      found=true;
      break;
    }
  }
  return c;
}

//-------------------------
void FileNgTable::obtainLimitsForHistFile(unsigned int histlen,
                                          off_t& offset,
                                          off_t& left,
                                          off_t& right)
{
      // Set value of the offset variable
  offset=0;
  for(unsigned int i=1;i<histlen;++i)
  {
    offset=offset+(numRecordsInHistInfoFile[i]*getHistInfoRecordLen(i));
  }

      // Initialize left variable
  left=0;

      // Set value of the right variable
  right=numRecordsInHistInfoFile[histlen]-1;
}

//-------------------------
bool FileNgTable::getEntriesForHist(const Vector<WordIndex>& hist,
                                    WordTableNode& wtnode,
                                    int N/*=-1*/)
{
  Vector<pair<WordIndex,off_t> > offsetOfRecordsVec;
  return getEntriesForHistAux1(hist,wtnode,offsetOfRecordsVec,N);
}

//-------------------------
bool FileNgTable::getEntriesForHistAux1(const Vector<WordIndex>& hist,
                                        FileNgTable::WordTableNode& wtnode,
                                        Vector<pair<WordIndex,off_t> >& offsetOfRecordsVec,
                                        int N)
{
      // Reset wtnode
  wtnode.clear();

      // Reset offsetOfRecordsVec
  offsetOfRecordsVec.clear();
  
      // Obtain info for hist
  bool foundInHistFile;
  FileHistInfo fileHistInfo=getHistInfoAux(hist,foundInHistFile);
  NgIdx hngidx=fileHistInfo.ngIdx;

  if(!foundInHistFile) return false;

      // Initialize recordLen
  off_t recordLen=getNgRecordLen();

      // Seek file position
  int err=fseeko(ngFileHandle,(fileHistInfo.ngcountPtr-1)*recordLen,0);
  if(err==-1)
  {
    return false;
  }
    
      // Add translation options
  bool end=false;
  while(!end)
  {
    NgIdx histngidx;
    WordIndex word;
    NgInfo nginfo;
    off_t offset=ftello(ngFileHandle);
    nginfo.first=fileHistInfo.count;
    size_t st=readNgRecord(histngidx,word,nginfo.second);
    if(st==0) end=true;
    else
    {
      if(histngidx==hngidx)
      {
            // Insert entry in HistTableNode
        pair<WordIndex,NgInfo> pWordNgInfo;
          
            // Complete nginfo with joint count
        pWordNgInfo.first=word;
        pWordNgInfo.second=nginfo;
        wtnode.insert(pWordNgInfo);
        
            // Insert offset in offset vector
        offsetOfRecordsVec.push_back(make_pair(pWordNgInfo.first,offset));
      }
      else
      {
        end=true;
      }
    }
  }

      // Return value of the hist_found variable
  return true;
}

//-------------------------
bool FileNgTable::getEntriesForHistAux2(const Vector<WordIndex>& hist,
                                        FileNgTable::WordTableNode& wtnode,
                                        Vector<pair<WordIndex,off_t> >& offsetOfRecordsVec,
                                        int N)
{
      // Reset wtnode
  wtnode.clear();

      // Reset offsetOfRecordsVec
  offsetOfRecordsVec.clear();
  
      // Obtain info for hist
  bool foundInHistFile;
  FileHistInfo fileHistInfo=getHistInfoAux(hist,foundInHistFile);
  NgIdx hngidx=fileHistInfo.ngIdx;

  if(!foundInHistFile) return false;

      // Initialize recordLen
  off_t recordLen=getNgRecordLen();

      // Search for hist idx using binary search
  off_t left;
  off_t right;
  off_t offset;
  off_t mid;
  
      // Initialize limits
  obtainLimitsForNgFile(hngidx,offset,left,right);
  
  while(left<=right)
  {
    mid=(off_t)((left+right)/2);
    int err=fseeko(ngFileHandle,offset+mid*recordLen,0);
    if(err==-1)
    {
      return false;
    }

        // Read target index
    NgIdx histngidx;
    WordIndex word;
    Count stc;
    size_t st=readNgRecord(histngidx,word,stc);
    if(hngidx<histngidx)
      right=mid-1;
    if(hngidx>histngidx)
      left=mid+1;
    if(hngidx==histngidx)
      break;
  }
      // Retrieve words for hist if found
  if(left<=right)
  {
        // Rewind until finding first ocurrence of hist
    bool end=false;
    while(!end)
    {
          // Position file indicator
      off_t offsetInRecordsForDict=getCurrOffsetInRecordsForNg();
      if(offsetInRecordsForDict<=1)
      {
            // Rewind to start of file
        jumpNgRecords(-1);
            // End while loop
        end=true;
      }
      else
      {
            // Rewind two records
        jumpNgRecords(-2);

            // Read record
        NgIdx histngidx;
        WordIndex word;
        Count stc;
        size_t st=readNgRecord(histngidx,word,stc);
        
            // Find the last ocurrence before the first ocurrence of t
        if(histngidx!=hngidx)
          end=true;
      }
    }
        
        // Add translation options
    end=false;
    while(!end)
    {
      NgIdx histngidx;
      WordIndex word;
      NgInfo nginfo;
      off_t offset=ftello(ngFileHandle);
      nginfo.first=fileHistInfo.count;
      size_t st=readNgRecord(histngidx,word,nginfo.second);
      if(st==0) end=true;
      else
      {
        if(histngidx==hngidx)
        {
              // Insert entry in HistTableNode
          pair<WordIndex,NgInfo> pWordNgInfo;
          
              // Complete nginfo with joint count
          pWordNgInfo.first=word;
          pWordNgInfo.second=nginfo;
          wtnode.insert(pWordNgInfo);

              // Insert offset in offset vector
          offsetOfRecordsVec.push_back(make_pair(pWordNgInfo.first,offset));
        }
        else
        {
          end=true;
        }
      }
    }
    return true;
  }
  else return false;
}

//-------------------------
off_t FileNgTable::getCurrOffsetInRecordsForNg(void)
{
  off_t curr_pos=ftello(ngFileHandle);
  return curr_pos/getNgRecordLen();
}

//-------------------------
bool FileNgTable::jumpNgRecords(off_t numRecords)
{
  int err=fseeko(ngFileHandle,numRecords*getNgRecordLen(),SEEK_CUR);
  if(err==-1)
  {
    return ERROR;
  }
  else return OK;
}

//-------------------------
size_t FileNgTable::readNgRecord(NgIdx& hngidx,
                                 WordIndex& w,
                                 Count& stc)
{
  size_t st=fread(&hngidx,sizeof(NgIdx),1,ngFileHandle);
  st+=fread(&w,sizeof(WordIndex),1,ngFileHandle);
  st+=fread(&stc,sizeof(Count),1,ngFileHandle);

  return st;
}

//-------------------------
void FileNgTable::obtainInfoGivenHistIdx(NgIdx idx,
                                         Vector<WordIndex>& hist,
                                         Count& c,
                                         bool& found)
{
  found=false;
  
  hist.clear();

      // Obtain offset
  unsigned int len;
  bool offsetFound;
  off_t offset=obtainOffsetOfHistGivenIdx(idx,len,offsetFound);

  if(offsetFound)
  {
        // Set file position indicator
    int err=fseeko(histsFileHandle,offset,0);
    if(err==0)
    {      
          // Read source phrase
      for(unsigned int j=0;j<len;++j)
      {
        WordIndex widx;
        size_t st=fread(&widx,sizeof(WordIndex),1,histsFileHandle);
        hist.push_back(widx);
      }
          // Obtain count
      size_t st=fread(&c,sizeof(float),1,histsFileHandle);

          // Set found flag to true
      found=true;
    }
  }
}

//-------------------------
off_t FileNgTable::obtainOffsetOfHistGivenIdx(NgIdx idx,
                                              unsigned int& len,
                                              bool& found)
{
      // Init output vars
  found=false;
  len=0;

      // Try to obtain offset
  if(!numRecordsInHistInfoFile.empty())
  {
    NgIdx ngramIndex;
    off_t offset=0;

        // Determine length
    unsigned int i=0;
    ngramIndex=numRecordsInHistInfoFile[i];

    while(ngramIndex<idx && i<numRecordsInHistInfoFile.size())
    {
      ++i;
      ngramIndex+=numRecordsInHistInfoFile[i];
    }
    if(ngramIndex>=idx)
    {
          // Set length
      len=i;

          // Obtain hist order for length "len"
      NgIdx ngIdxLen=idx-(ngramIndex-numRecordsInHistInfoFile[i]);

          // Set value of the offset variable for hist lengths lower
          // than the current one
      for(unsigned int j=1;j<len;++j)
      {
        offset=offset+(numRecordsInHistInfoFile[j]*getHistInfoRecordLen(j));
      }

          // Finally we sum the offset for the current hist length
          // NOTE: we subtract 1 to ngIdxLen since indices start at 1
      offset=offset+((ngIdxLen-1)*getHistInfoRecordLen(len));
    }
        // Set found flag
    found=true;
    
        // Return result
    return offset;
  }
  else return 0;
}
//-------------------------
NgIdx FileNgTable::obtainIdxForHistFile(const Vector<WordIndex>& hist,
                                        bool& found)
{
      // Obtain hist idx
  FileHistInfo fileHistInfo=getHistInfoAux(hist,found);
      // Return result
  return fileHistInfo.ngIdx;
}

//-------------------------
void FileNgTable::obtainLimitsForNgFile(NgIdx hngidx,
                                        off_t &offset,
                                        off_t &left,
                                        off_t &right)
{
  offset=0;
  left=0;
  right=numNgRecords-1;
}

//-------------------------
Count FileNgTable::getSrcInfo(const Vector<WordIndex>& srcVec,
                              bool& found)
{
  return getHistInfo(srcVec,found);

}

//-------------------------
Count FileNgTable::getSrcTrgInfo(const Vector<WordIndex>& srcVec,
                                 const WordIndex t,
                                 bool &found)
{
  return getNgInfo(srcVec,t,found);
}

//-------------------------
bool FileNgTable::getEntriesForSrc(const Vector<WordIndex>& srcVec,
                                   WordTableNode& wtnode,
                                   int N/*=-1*/)
{
  return getEntriesForHist(srcVec,wtnode,N);
}

//-------------------------
size_t FileNgTable::size(void)
{
  return numNgRecords;
}

//-------------------------
void FileNgTable::clear(void)
{
      // Close files
  if(histsFileHandle!=NULL) fclose(histsFileHandle);
  histsFileHandle=NULL;

  if(ngFileHandle!=NULL) fclose(ngFileHandle);
  ngFileHandle=NULL;
  
      // Clear data structures
  numNgRecords=0;
  numRecordsInHistInfoFile.clear();
}

//-------------------------
void FileNgTable::clearTempVars(void)
{

}

//-------------------------
FileNgTable::~FileNgTable()
{
  clear();
}

//-------------------------
