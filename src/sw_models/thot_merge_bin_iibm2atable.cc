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
/* Module: thot_merge_bin_iibm2atable.cc                            */
/*                                                                  */
/* Definitions file: thot_merge_bin_iibm2atable.cc                  */
/*                                                                  */
/* Description: Merges counts given in a set of sorted incremental  */
/*              ibm2 alignment tables.                              */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include "options.h"
#include "SwDefs.h"
#include "aSource.h"
#include <MathFuncs.h>

//--------------- Constants ------------------------------------------

#define RECORD_READ     0
#define NO_RECORDS_LEFT 1

//--------------- Type definitions -----------------------------------

struct Entry
{
  aSource asIbm2;
  PositionIndex i;
  float numer;
  float denom;
  unsigned int id;
};

struct SortBySrcAndTrg
{
  bool operator() (const Entry& a,
                   const Entry& b)const
    {
      if(b.asIbm2.j<a.asIbm2.j) return 1;
      if(a.asIbm2.j<b.asIbm2.j) return 0;

      if(b.asIbm2.slen<a.asIbm2.slen) return 1;
      if(a.asIbm2.slen<b.asIbm2.slen) return 0;

      if(b.asIbm2.tlen<a.asIbm2.tlen) return 1;
      if(a.asIbm2.tlen<b.asIbm2.tlen) return 0;

      if(b.i<a.i) return 1;
      if(a.i<b.i) return 0;

      return 0;
    }
};

typedef std::priority_queue<Entry,std::vector<Entry>,SortBySrcAndTrg> MergePrQueue;

typedef std::set<unsigned int> ChunkSet;

//--------------- Function Declarations ------------------------------

int openFiles(void);
void initPrQueue(MergePrQueue& entryPrQueue);
int getNextEntry(MergePrQueue& entryPrQueue,
                 Entry& entry);
void printCounts(aSource firstSrc,
                 const std::vector<WordIndex>& trgWordVec,
                 float lcSrc,
                 const std::vector<float>& lcSrcTrgVec);
void clear();
int readTableRecord(ifstream& inF,
                    aSource& asIbm2,
                    PositionIndex& i,
                    float& numer,
                    float& denom);
int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printDesc(void);

//--------------- Global variables -----------------------------------

std::vector<std::string> fileNameVec;
std::vector<ifstream*> ifstreamPtrVec;
std::vector<bool> eofFlagVec;

//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc,char *argv[])
{
  if(TakeParameters(argc,argv)==THOT_OK)
  {
        // Open files
    int ret=openFiles();
    if(ret==THOT_ERROR)
      return THOT_ERROR;
    
        // Process entries contained in the set of files...

        // Initialize priority queue
    MergePrQueue entryPrQueue;
    initPrQueue(entryPrQueue);
    
        // while loop
    bool end=false;
    bool first_entry=true;
    aSource firstSrc;
    float lcSrc=SMALL_LG_NUM;
    std::vector<PositionIndex> trgPosVec;
    std::vector<float> lcSrcTrgVec;
    ChunkSet chunkSet;
    
    while(!end)
    {
      Entry entry;
      ret=getNextEntry(entryPrQueue,entry);
      if(ret==RECORD_READ)
      {
//        printf("** %d %d %d %d %g %g %g\n",entry.asIbm2.j,entry.asIbm2.slen,entry.asIbm2.tlen,entry.i,entry.numer,entry.denom,exp(entry.numer-entry.denom));

            // Verify if it is the first entry of the table
        if(first_entry==true)
        {
          firstSrc=entry.asIbm2;
          first_entry=false;
          trgPosVec.push_back(entry.i);
          lcSrcTrgVec.push_back(entry.numer);
        }
        else
        {
              // A new source pair has appeared?
          if(!(firstSrc==entry.asIbm2))
          {
                // Print counts
            printCounts(firstSrc,trgPosVec,lcSrc,lcSrcTrgVec);

                // Reset variables
            firstSrc=entry.asIbm2;
            chunkSet.clear();
            trgPosVec.clear();
            lcSrcTrgVec.clear();
            trgPosVec.push_back(entry.i);
            lcSrcTrgVec.push_back(entry.numer);
            lcSrc=SMALL_LG_NUM;
          }
          else
          {
            trgPosVec.push_back(entry.i);
            lcSrcTrgVec.push_back(entry.numer);
          }
        }
        ChunkSet::const_iterator csConstIter=chunkSet.find(entry.id);
        if(csConstIter==chunkSet.end()) 
        {
          chunkSet.insert(entry.id);
          lcSrc=MathFuncs::lns_sumlog(lcSrc,entry.denom);
        }
      }
      else end=true;
    }
        // Print last group of counts
    printCounts(firstSrc,trgPosVec,lcSrc,lcSrcTrgVec);

        // Close files and release pointers
    clear();

    return THOT_OK;
  }
  else return THOT_ERROR;
}

//--------------- openFiles() function
int openFiles(void)
{
  for(unsigned int i=0;i<fileNameVec.size();++i)
  {
        // Create file stream
    ifstream* ifstreamPtr=new ifstream;
    ifstreamPtrVec.push_back(ifstreamPtr);
    ifstreamPtrVec[i]->open(fileNameVec[i].c_str(), ios::in | ios::binary);
    if(! *ifstreamPtrVec[i])
    {
      std::cerr<<"Error in file with incremental lexical table, file "<<fileNameVec[i]<<" does not exist.\n";
      return THOT_ERROR;    
    }
    
        // Create flag for file
    eofFlagVec.push_back(false);
  }
  
  return THOT_OK;
}

//--------------- initPrQueue() function
void initPrQueue(MergePrQueue& entryPrQueue)
{
  for(unsigned int i=0;i<ifstreamPtrVec.size();++i)
  {
    Entry entry;
    int ret=readTableRecord(*ifstreamPtrVec[i],entry.asIbm2,entry.i,entry.numer,entry.denom);
    if(ret==RECORD_READ)
    {
      entry.id=i;
      entryPrQueue.push(entry);
    }
    else
    {
      eofFlagVec[i]=true;
    }
  }
}

//--------------- getNextEntry() function
int getNextEntry(MergePrQueue& entryPrQueue,
                 Entry& entry)
{
      // Check if queue is not empty
  if(!entryPrQueue.empty())
  {
        // Obtain top of the queue and pop it
    entry=entryPrQueue.top();
    entryPrQueue.pop();
        // Push next entry of corresponding file if there exists
    Entry nextEntry;
    int ret=readTableRecord(*ifstreamPtrVec[entry.id],nextEntry.asIbm2,nextEntry.i,nextEntry.numer,nextEntry.denom);
    if(ret==RECORD_READ)
    {
      nextEntry.id=entry.id;
      entryPrQueue.push(nextEntry);
    }
    else
    {
      eofFlagVec[entry.id]=true;
    }
            // Print entry
    return RECORD_READ;
  }
  else
    return NO_RECORDS_LEFT;
}

//--------------- printCounts() function
void printCounts(aSource firstSrc,
                 const std::vector<WordIndex>& trgPosVec,
                 float lcSrc,
                 const std::vector<float>& lcSrcTrgVec)
{
  PositionIndex firstTrg=trgPosVec[0];
  float glcSrcTrg=lcSrcTrgVec[0];
  for(unsigned int n=1;n<trgPosVec.size();++n)
  {
    if(firstTrg==trgPosVec[n])
    {
          // Accumulate count of target phrase for additional chunk
      glcSrcTrg=MathFuncs::lns_sumlog(glcSrcTrg,lcSrcTrgVec[n]);
    }
    else
    {
          // Print count for current target phrase
//      printf("%d %d %d %d %g %g\n",firstSrc.j,firstSrc.slen,firstSrc.tlen,firstTrg,glcSrcTrg,lcSrc);
      cout.write((char*)&firstSrc.j,sizeof(PositionIndex));
      cout.write((char*)&firstSrc.slen,sizeof(PositionIndex));
      cout.write((char*)&firstSrc.tlen,sizeof(PositionIndex));
      cout.write((char*)&firstTrg,sizeof(PositionIndex));
      cout.write((char*)&glcSrcTrg,sizeof(float));
      cout.write((char*)&lcSrc,sizeof(float));            
 
          // Initialize variables for next target phrase
      firstTrg=trgPosVec[n];
      glcSrcTrg=lcSrcTrgVec[n];
    }
  }
      // Print last target phrase
//  printf("%d %d %d %d %g %g\n",firstSrc.j,firstSrc.slen,firstSrc.tlen,firstTrg,glcSrcTrg,lcSrc);
  cout.write((char*)&firstSrc.j,sizeof(PositionIndex));
  cout.write((char*)&firstSrc.slen,sizeof(PositionIndex));
  cout.write((char*)&firstSrc.tlen,sizeof(PositionIndex));
  cout.write((char*)&firstTrg,sizeof(PositionIndex));
  cout.write((char*)&glcSrcTrg,sizeof(float));
  cout.write((char*)&lcSrc,sizeof(float));            
}

//--------------- clear() function
void clear(void)
{
  for(unsigned int i=0;i<ifstreamPtrVec.size();++i)
  {
    ifstreamPtrVec[i]->close();
    delete ifstreamPtrVec[i];
  }
}

//--------------- readTableRecord() function
int readTableRecord(ifstream& inF,
                    aSource& asIbm2,
                    PositionIndex& i,
                    float& numer,
                    float& denom)
{
  if(inF.read((char*)&asIbm2.j,sizeof(PositionIndex)))
  {
    inF.read((char*)&asIbm2.slen,sizeof(PositionIndex));
    inF.read((char*)&asIbm2.tlen,sizeof(PositionIndex));
    inF.read((char*)&i,sizeof(PositionIndex));
    inF.read((char*)&numer,sizeof(float));
    inF.read((char*)&denom,sizeof(float));
    return RECORD_READ;
  }
  else return NO_RECORDS_LEFT;
}

//--------------- TakeParameters function
int TakeParameters(int argc,char *argv[])
{
 int err;

 if(argc==1)
 {
   printDesc();
   return THOT_ERROR;   
 }

     /* Verify --help option */
 err=readOption(argc,argv,"--help");
 if(err!=-1)
 {
   printUsage();
   return THOT_ERROR;
 }

     /* Takes the table file names */
 for(int i=1;i<argc;++i)
 {
   std::string fileName=argv[i];
   fileNameVec.push_back(fileName);
 }

 return THOT_OK;  
}

//--------------- printDesc() function
void printDesc(void)
{
  printf("thot_merge_bin_iibm2atable written by Daniel Ortiz\n");
  printf("A tool to merge the counts of a set of sorted incremental ibm2 alignment tables\n");
  printf("type \"thot_merge_bin_iibm2atable --help\" to get usage information.\n");
}

//--------------- printUsage() function
void printUsage(void)
{
  printf("Usage: thot_merge_bin_iibm2atable <sorted_iibm2atable_1> [<sorted_iibm2atable_2> ...]\n");
  printf("                                  [--help]\n\n");
  printf("--help                     Display this help and exit.\n\n");
}

//--------------------------------
