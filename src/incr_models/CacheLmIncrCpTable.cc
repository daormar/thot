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
/* Module: CacheLmIncrCpTable                                       */
/*                                                                  */
/* Definitions file: CacheLmIncrCpTable.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "CacheLmIncrCpTable.h"

//--------------- Global variables -----------------------------------

//--------------- Function declarations 

//--------------- Constants


//--------------- Classes --------------------------------------------

//------------------------------

//-------------------------
bool CacheLmIncrCpTable::init(const char *fileName)
{
  awkInputStream awk;

  cerr<<"Initializing cache variables..."<<endl;

      // get alpha value
  std::string alphafile=fileName;
  alphafile=alphafile+".alpha";
  bool ret=awk.open(alphafile.c_str());
  if(ret==ERROR)
  {
    cerr<<"Error while opening file with alpha value: "<<alphafile<<endl;
    return ERROR;
  }
  awk.getline();
  alpha=atof(awk.dollar(1).c_str());

      // Init file n-gram table
  ret=fileNgTable.init(fileName);
  if(ret==ERROR)
  {
    return ERROR;
  }
  
  return OK;
}

//-------------------------
void CacheLmIncrCpTable::init_alpha100(void)
{
      // clear data structures
  clear();
  
      // Set value of alpha
  alpha=100;
}

//-------------------------
size_t CacheLmIncrCpTable::size(void)
{
  return secondLevelTable.size();
}
//-------------------------
void CacheLmIncrCpTable::clear(void)
{
      // Clear data structures
  firstLevelTable.clear();
  nodesPresentInFirstLevel.clear();
  secondLevelTable.clear();
  fileNgTable.clear();

}

//-------------------------
void CacheLmIncrCpTable::clearTempVars(void)
{
  firstLevelTable.clear();
  nodesPresentInFirstLevel.clear();
  fileNgTable.clearTempVars();
}

//-------------------------
void CacheLmIncrCpTable::addTableEntry(const Vector<WordIndex>& s,
                                       const WordIndex& t,
                                       im_pair<Count,Count> inf)
{
      // Add entry to second level table
  secondLevelTable.addTableEntry(s,t,inf);

      // Since the second level cache table has been modified, the first
      // level cache table has to be cleared
  clearTempVars();
}

//-------------------------
void CacheLmIncrCpTable::addSrcInfo(const Vector<WordIndex>& s,Count s_inf)
{
      // TO-DO
  cerr<<"Warning: addSrcInfo() member function not implemented"<<endl;
}

//-------------------------
void CacheLmIncrCpTable::addSrcTrgInfo(const Vector<WordIndex>& s,
                                       const WordIndex& t,
                                       Count st_inf)
{
        // TO-DO
  cerr<<"Warning: addSrcTrgInfo() member function not implemented"<<endl;
}

//-------------------------
void CacheLmIncrCpTable::incrCountsOfEntryLog(const Vector<WordIndex>& s,
                                              const WordIndex& t,
                                              LogCount lc)
{
      // TO-DO
  cerr<<"Warning: incrCountsOfEntryLog() member function not implemented"<<endl;
}

//-------------------------
im_pair<Count,Count> CacheLmIncrCpTable::infSrcTrg(const Vector<WordIndex>& s,
                                                   const WordIndex& t,
                                                   bool& found)
{
      // Invoke function
  im_pair<Count,Count> pcc;
  pcc.first=getSrcInfo(s,found);
  pcc.second=getSrcTrgInfo(s,t,found);

  return pcc;
}

//-------------------------
Count CacheLmIncrCpTable::getSrcInfo(const Vector<WordIndex>& s,bool& found)
{
      // Search on first level cache table
  Count c_s;
  c_s=firstLevelTable.getSrcInfo(s,found);

      // Search on second level cache table
  if(!found)
  {
    c_s=secondLevelTable.getSrcInfo(s,found);
  }

      // Search on disk if alpha not equal to 100
  if(!found && alpha<100)
  {
        // Adapt source vector if necessary
    Vector<WordIndex> preproc_s;
    preproc_s=s;
    preprocSrcVector(preproc_s);

    c_s=fileNgTable.getSrcInfo(preproc_s,found);
  }
      // Return count
  return c_s;
}
//-------------------------
Count CacheLmIncrCpTable::getSrcTrgInfo(const Vector<WordIndex>& s,
                                        const WordIndex& t,
                                        bool& found)
{
  if(!nodeForSrcIsPresentInFirstLevelTable(s))
  {
        // Search on second level cache table
    bool found;
    Count c_st=secondLevelTable.getSrcTrgInfo(s,t,found);

        // Search on disk if alpha not equal to 100
    if(!found && alpha<100)
    {
          // VERSION I
          //     // Adapt source vector if necessary
          // Vector<WordIndex> preproc_s;
          // preproc_s=s;
          // preprocSrcVector(preproc_s);
      
          // c_st=fileNgTable.getSrcTrgInfo(preproc_s,t,found);
      
          // VERSION II
          // Retrieve entries
      TrgTableNode trgtn;
      getEntriesForSource(s,trgtn);

          // Retrieve count
      TrgTableNode::iterator trgtnIter;
      for(trgtnIter=trgtn.begin();trgtnIter!=trgtn.end();++trgtnIter)
      {
        if(trgtnIter->first==t)
        {
          found=true;
          c_st=trgtnIter->second.second;
          break;
        }
      }
          // Set count value if not found
      if(!found)
      {
        c_st=0;
      }
    }
    return c_st;
  }
  else
  {
        // Search on first level cache table
    bool found;
    return firstLevelTable.getSrcTrgInfo(s,t,found);
  }
}

//-------------------------
Prob CacheLmIncrCpTable::pTrgGivenSrc(const Vector<WordIndex>& s,
                                      const WordIndex& t)
{
        // Obtain counts and probability
  Count c_srctrg;
  Count c_src;
  
  c_srctrg=cSrcTrg(s,t);
  if((float)c_srctrg==0)
  {
    return 0;
  }
  else
  {
    c_src=cSrc(s);
    if((float)c_src==0) return 0;
    else return (float)c_srctrg/(float)c_src;
  }
}

//-------------------------
LgProb CacheLmIncrCpTable::logpTrgGivenSrc(const Vector<WordIndex>& s,
                                           const WordIndex& t)
{
      // Obtain counts and probability
  LogCount lc_srctrg;
  LogCount lc_src;
  
  lc_srctrg=lcSrcTrg(s,t);
  if((float)lc_srctrg==SMALL_LG_NUM)
  {
    return SMALL_LG_NUM;
  }
  else
  {
    lc_src=lcSrc(s);
    if((float)lc_src==SMALL_LG_NUM) return SMALL_LG_NUM;
    else return (float)lc_srctrg-(float)lc_src;
  }
}

//-------------------------
Prob CacheLmIncrCpTable::pSrcGivenTrg(const Vector<WordIndex>& s,
                                      const WordIndex& t)
{
      // Obtain counts and probability
  Count ctrg;
  Count c_srctrg;
  
  ctrg=cTrg(t);
  if((float)ctrg==0) return 0;
  else
  {
    c_srctrg=cSrcTrg(s,t);
    return (float)c_srctrg/(float)ctrg;
  }
}

//-------------------------
LgProb CacheLmIncrCpTable::logpSrcGivenTrg(const Vector<WordIndex>& s,
                                           const WordIndex& t)
{
      // Obtain counts and probability
  LogCount lctrg;
  LogCount lc_srctrg;
  
  lctrg=lcTrg(t);
  if((float)lctrg==SMALL_LG_NUM) return SMALL_LG_NUM;
  else
  {
    lc_srctrg=lcSrcTrg(s,t);
    return (float)lc_srctrg-(float)lctrg;
  }
}
//-------------------------
bool CacheLmIncrCpTable::getEntriesForSource(const Vector<WordIndex>& s,
                                             TrgTableNode& trgtn)
{
  trgtn.clear();

      // Search target phrase within cache hierarchy
      // Search in first level table
  if(!nodeForSrcIsPresentInFirstLevelTable(s))
  {
        // Search in second level table
    if(!secondLevelTable.getEntriesForSource(s,trgtn))
    {
          // If alpha=100 search on disk can be avoided
      if(alpha==100)
      {
        markSrcAsPresentInFirstLevelTable(s);
        return false;
      }
      else
      {
            // alpha is not equal to 100

            // Adapt source vector if necessary
        Vector<WordIndex> preproc_s;
        preproc_s=s;
        preprocSrcVector(preproc_s);

            // Search on disk cannot be avoided
        fileNgTable.getEntriesForHist(preproc_s,trgtn);

        if(trgtn.size()>0)
        {
          TrgTableNode::iterator trgtnIter;
          for(trgtnIter=trgtn.begin();trgtnIter!=trgtn.end();++trgtnIter)
          {
            firstLevelTable.addTableEntry(s,trgtnIter->first,trgtnIter->second);
          }
          markSrcAsPresentInFirstLevelTable(s);
          return true;
        }
        else
        {
          markSrcAsPresentInFirstLevelTable(s);
          return false;
        }
      }
    }
    else
    {
          // Second level search successful
      TrgTableNode::iterator trgtnIter;
      for(trgtnIter=trgtn.begin();trgtnIter!=trgtn.end();++trgtnIter)
      {
        firstLevelTable.addTableEntry(s,trgtnIter->first,trgtnIter->second);
      }
      markSrcAsPresentInFirstLevelTable(s);
      return true;
    }
  }
  else
  {
    firstLevelTable.getEntriesForSource(s,trgtn);
    return true;
  }
}

//-------------------------
bool CacheLmIncrCpTable::getEntriesForTarget(const WordIndex& t,
                                             CacheLmIncrCpTable::SrcTableNode& srctn)
{
      // TO-DO
  cerr<<"Warning: getEntriesForTarget() member function not implemented"<<endl;
  return false;
}

//-------------------------
bool CacheLmIncrCpTable::getNbestForSrc(const Vector<WordIndex>& s,
                                        NbestTableNode<WordIndex>& nbt)
{
      // TO-DO
  cerr<<"Warning: getNbestForSrc() member function not implemented"<<endl;
  return false;
}

//-------------------------
bool CacheLmIncrCpTable::getNbestForTrg(const WordIndex& t,
                                        NbestTableNode<Vector<WordIndex> >& nbt,
                                        int N)
{
      // TO-DO
  cerr<<"Warning: getNbestForTrg() member function not implemented"<<endl;
  return false;
}

//-------------------------
Count CacheLmIncrCpTable::cSrcTrg(const Vector<WordIndex>& s,
                                  const WordIndex& t)
{
      // Obtain count
  bool found;

  return getSrcTrgInfo(s,t,found).get_c_st();
}

//-------------------------
Count CacheLmIncrCpTable::cSrc(const Vector<WordIndex>& s)
{
      // Obtain count
  bool found;

  return getSrcInfo(s,found).get_c_s();
}

//-------------------------
Count CacheLmIncrCpTable::cTrg(const WordIndex& t)
{
      // TO-DO
  cerr<<"Warning: cTrg() member function not implemented"<<endl;
  return 0;
}

//-------------------------
LogCount CacheLmIncrCpTable::lcSrcTrg(const Vector<WordIndex>& s,
                                      const WordIndex& t)
{
      // Obtain count
  bool found;

  Count c=getSrcTrgInfo(s,t,found).get_c_st();
  if((float)c!=0.0) return c.get_lc_st();
  else return SMALL_LG_NUM;
}

//-------------------------
LogCount CacheLmIncrCpTable::lcSrc(const Vector<WordIndex>& s)
{
      // Obtain count
  bool found;

  Count c=getSrcInfo(s,found).get_c_s();
  if((float)c!=0.0) return c.get_lc_s();
  else return SMALL_LG_NUM;
}

//-------------------------
LogCount CacheLmIncrCpTable::lcTrg(const WordIndex& t)
{
      // TO-DO
  cerr<<"Warning: cTrg() member function not implemented"<<endl;
  return SMALL_LG_NUM;
}

//-------------------------
bool CacheLmIncrCpTable::nodeForSrcIsPresentInFirstLevelTable(const Vector<WordIndex>& srcVec)
{
  bool found;
  firstLevelTable.getSrcInfo(srcVec,found);
  if(found)
  {
    return true;
  }
  else
  {
    set<Vector<WordIndex> >::iterator setIter;

    setIter=nodesPresentInFirstLevel.find(srcVec);
    if(setIter!=nodesPresentInFirstLevel.end())
      return true;
    else
      return false;
  }
}

//-------------------------
void CacheLmIncrCpTable::markSrcAsPresentInFirstLevelTable(const Vector<WordIndex>& srcVec)
{
  nodesPresentInFirstLevel.insert(srcVec);
}

//-------------------------
void CacheLmIncrCpTable::preprocSrcVector(Vector<WordIndex>& s)
{
  Vector<WordIndex> s_aux=s;
  s.clear();
  s.push_back(SP_SYM1_LM);
  for(unsigned int i=0;i<s_aux.size();++i)
    s.push_back(s_aux[i]);
}

// nested-const_iterator member functions
//---------------
// CacheLmIncrCpTable::const_iterator::const_iterator(CachePhrTable::const_iterator _cachePtIter)
// {
//   cachePtIter=_cachePtIter;
// }
// 
//---------------
bool CacheLmIncrCpTable::const_iterator::operator++(void) //prefix
{
      // TO-DO
  return false;
}
//---------------
bool CacheLmIncrCpTable::const_iterator::operator++(int)  //postfix
{
  return operator++();
}
//---------------
int CacheLmIncrCpTable::const_iterator::operator==(const const_iterator& right)
{
      // TO-DO
  // return this->cachePtIter==right.cachePtIter;
}
//---------------
int CacheLmIncrCpTable::const_iterator::operator!=(const const_iterator& right)
{
  return !(*this==right);
}
//---------------
// const CachePhrTable::const_iterator&
// CacheLmIncrCpTable::const_iterator::operator->(void)const
// {
//   return cachePtIter;
// }

// const_iterator functions
//---------------
CacheLmIncrCpTable::const_iterator
CacheLmIncrCpTable::begin(void)const
{
      // TO-DO
  // CacheLmIncrCpTable::const_iterator iter(this->cachePhrTable.begin());
  // return iter;
}

//---------------
CacheLmIncrCpTable::const_iterator
CacheLmIncrCpTable::end(void)const
{
      // TO-DO
  // CacheLmIncrCpTable::const_iterator iter(this->cachePhrTable.end());
  // return iter;
}
//------------------------------
