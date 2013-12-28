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
/* Module: CacheIncrJelMerNgramLM                                   */
/*                                                                  */
/* Definitions file: CacheIncrJelMerNgramLM.cc                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "CacheIncrJelMerNgramLM.h"

//--------------- Global variables -----------------------------------

//--------------- Function declarations 

//--------------- Constants


//--------------- Classes --------------------------------------------


//-------------------------
bool CacheIncrJelMerNgramLM::load(const char *fileName)
{
      // Load weights
  std::string fileNameW=fileName;
  fileNameW=fileNameW+".weights";
  bool retval=loadWeights(fileNameW.c_str());
  if(retval==ERROR) return ERROR;

  
      // Load cache table
  awkInputStream awk;

  tablePtr->clear();
  modelFileName=fileName;
  
  if(awk.open(fileName)==ERROR)
  {
    cerr<<"Error in table file: "<<fileName<<"\n";
    return ERROR;
  }
  else
  {
    if(awk.getline())
    {
      if(awk.NF==3 && strcmp("****",awk.dollar(1).c_str())==0 &&
         strcmp("cache",awk.dollar(2).c_str())==0 && strcmp("lm",awk.dollar(3).c_str())==0)
      {
        awk.close();
        return loadCacheTable(fileName);
      }
      else
      {
        awk.close();
        cerr<<"Error in table file: "<<fileName<<"\n";
        return ERROR;
      }
    }
    else
    {
      awk.close();
      cerr<<"Error in table file: "<<fileName<<"\n";
      return ERROR;
    }
  }
}

//-------------------------
bool CacheIncrJelMerNgramLM::loadCacheTable(const char *fileName)
{
  std::string second_level;
  
  cerr<<"Loading cache table from: "<<fileName<<endl;

  clear();

  CacheLmIncrCpTable* cptPtr=dynamic_cast<CacheLmIncrCpTable*>(tablePtr);
  
      // Load vocabulary
  std::string lmvoc=fileName;
  lmvoc=lmvoc+".voc";
  cerr<<"Loading vocabulary from: "<<lmvoc<<endl;
  bool ret=loadVocab(lmvoc.c_str());
  if(ret==ERROR) return ERROR;
  
      // Load second level cache table
  second_level=fileName;
  second_level=second_level+".sec_level";
  cerr<<"Searching second level table file ("<<second_level<<")... ";
  ret=loadSecLevelTable(second_level.c_str());
  if(ret==ERROR)
  {
    cerr<<"ERROR"<<endl;
    return ERROR;
  }
  cerr<<"OK"<<endl;

      // Init first level table
  ret=cptPtr->init(fileName);
  if(ret==ERROR)
  {
    cerr<<"Error in cache table\n";
    return ERROR;
  }

  return OK;
}

//-------------------------
bool CacheIncrJelMerNgramLM::loadSecLevelTable(const char *fileName)
{
  unsigned int i;
  Vector<string> s;
  string t;
  awkInputStream awk;
  Count count_s_;
  Count count_s_t_;
  im_pair<Count,Count> imp;
    
  if(awk.open(fileName)==ERROR)
  {
    cerr<<"Error in table file: "<<fileName<<endl;
    return ERROR;
  }
  else
  {   
    while(awk.getline())
    {
      if(awk.FNR>=1 && awk.NF>=3)
      {
            // Read source vector
        i=1; 
        s.clear();	  
        while(i<=awk.NF-3)
        {
          s.push_back(awk.dollar(i)); 
          ++i;
        }

            // Read target word
        t=(awk.dollar(i));

            // Read counts
        ++i;
        count_s_=atof(awk.dollar(i).c_str());
        ++i;
        count_s_t_=atof(awk.dollar(i).c_str());
        
        imp.first=count_s_;
        imp.second=count_s_t_;

            // Add new entry
        addTableEntryHigh(s,t,imp);
      }
      else
      {
        if(awk.NF!=0)
        {
          return ERROR;
        }
      }
    }
  }
  return OK;  
}

//-------------------------
bool CacheIncrJelMerNgramLM::print(const char *fileName)
{
  int retval;
  
      // Print weights
  std::string fileNameW=fileName;
  fileNameW=fileNameW+".weights";
  retval=printWeights(fileNameW.c_str());
  if(retval==ERROR) return ERROR;

      // Print vocabulary
  std::string fileNameVoc=fileName;
  fileNameVoc=fileNameVoc+".voc";
  retval=printEncodingInfo(fileNameVoc.c_str());
  if(retval==ERROR) return ERROR;
  
      // Print n-grams in sec. level table
  std::string fileNameSecLevel=fileName;
  fileNameSecLevel=fileNameSecLevel+".sec_level";
  retval=printSecLevelTable(fileNameSecLevel.c_str());
  if(retval==ERROR) return ERROR;

      // Create symbolic links to files containing binarized table

      // Dictionary file
      // Check if file exists
  std::string oldDictFile=modelFileName+".dict";
  if(access(oldDictFile.c_str(),F_OK)==0)
  {
        // Create symbolic link
    std::string newDictFile=fileName;
    newDictFile=newDictFile+".dict";
    retval=symlink(oldDictFile.c_str(),newDictFile.c_str());
    if(retval==-1)
    {
      cerr<<"Error while creating symbolic link to file "<<oldDictFile<<endl;
      return ERROR;
    } 
  }
      // TBD (repeat with rest of files)
       
      // Print main file
 retval=printMainFile(fileName);
 if(retval==ERROR) return ERROR;

  return OK;
}

//-------------------------
bool CacheIncrJelMerNgramLM::printMainFile(const char *fileName)
{
  ofstream outF;

  outF.open(fileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing model to file."<<endl;
    return ERROR;
  }
  else
  {
    outF<<"**** cache lm"<<endl;
    outF.close();	
    return OK;
  }
}

//-------------------------
bool CacheIncrJelMerNgramLM::printSecLevelTable(const char *fileName)
{
  ofstream outF;

  outF.open(fileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing model to file."<<endl;
    return ERROR;
  }
  else
  {
    printSecLevelTable(outF);
    outF.close();	
    return OK;
  }
}

//-------------------------
ostream& CacheIncrJelMerNgramLM::printSecLevelTable(ostream &outS)
{
  Vector<std::string> hs;
  Vector<WordIndex> s;
  std::string ht;
  CacheLmIncrCpTable* tableCptPtr=0;

  if(this->encPtr!=NULL)
  {
    tableCptPtr=dynamic_cast<CacheLmIncrCpTable*>(this->tablePtr);
  
    if(tableCptPtr) // C++ RTTI
    {
      CacheLmIncrCpTable::const_iterator tableIter;
      
          // Set float precision.
      outS.setf( ios::fixed, ios::floatfield );
      outS.precision(8);

          // TO-DO
      // for(tableIter=tableCptPtr->begin();tableIter!=tableCptPtr->end();++tableIter)
      // {
      //   CacheLmIncrCpTable::SrcTableNode srctn;
      //   CacheLmIncrCpTable::SrcTableNode::iterator srctnIter;
      //   if(!tableIter->first.empty())
      //   {
      //     tableCptPtr->getEntriesForTarget(tableIter->first[0],srctn);
      //     for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      //     {
      //       Vector<WordIndex>::const_iterator vectorWordIndexIter;
      //       for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
      //         outS<<wordIndexToString(*vectorWordIndexIter)<<" "; 
      //       outS<<"|||"; 
      //       for(vectorWordIndexIter=tableIter->first.begin();vectorWordIndexIter!=tableIter->first.end();++vectorWordIndexIter)
      //         outS<<" "<<wordIndexToString(*vectorWordIndexIter);
      //       outS<<" ||| "<<srctnIter->second.first.get_c_s()<<" "<<srctnIter->second.second.get_c_st()<<endl;
      //     }
      //   } 
      // }
    }
  }
  return outS; 
}

//-------------------------
void CacheIncrJelMerNgramLM::clearTempVars(void)
{
  if(tablePtr!=NULL)
  {
    CacheLmIncrCpTable* cptPtr=dynamic_cast<CacheLmIncrCpTable*>(tablePtr);
    
    if(cptPtr) // C++ RTTI
    {
      cptPtr->clearTempVars();
    }
  }
}

//-------------------------
CacheIncrJelMerNgramLM::~CacheIncrJelMerNgramLM()
{
  
}

//------------------------------
