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
/* Module: FastBdbPhraseTable                                       */
/*                                                                  */
/* Prototype file: FastBdbPhraseTable                               */
/*                                                                  */
/* Description: Implements a phrase table stored in files with      */
/*              fast search capabilities                            */
/*                                                                  */
/********************************************************************/

#ifndef _FastBdbPhraseTable
#define _FastBdbPhraseTable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseTransTableNodeData.h"
#include "NbestTableNode.h"
#include "PhrasePairInfo.h"
#include "PhraseDefs.h"
#include "MathDefs.h"
#include <stdio.h>
#include <db_cxx.h>
#include <db.h>
#include <algorithm>
#include <AwkInputStream.h>
#include <string.h>
#include <map>
#include <vector>

//--------------- Constants ------------------------------------------

#define MAX_PHR_SIZE_SBDB            10
#define MAX_PHR_DICT_KEY_SIZE_SBDB   MAX_PHR_SIZE_SBDB*2+1

//--------------- typedefs -------------------------------------------

struct PhrDictKey
{
  WordIndex words[MAX_PHR_DICT_KEY_SIZE_SBDB];

  PhrDictKey(void)
    {
      clear();
    }
  unsigned int getSize(void)
    {
      unsigned int nw=0;
      for(unsigned int i=0;i<MAX_PHR_DICT_KEY_SIZE_SBDB;++i)
      {
        if(words[i]!=MAX_VOCAB_SIZE)
          nw+=1;
        else
          break;
      }
      return sizeof(WordIndex)*nw;
    }
  void clear()
    {
      for(unsigned int i=0;i<MAX_PHR_DICT_KEY_SIZE_SBDB;++i)
        words[i]=MAX_VOCAB_SIZE;
    }
  void getPhrPair(std::vector<WordIndex>& srcPhr,
                  std::vector<WordIndex>& trgPhr)
    {
      std::vector<WordIndex> phrase1;
      std::vector<WordIndex> phrase2;
      if(words[0]!=MAX_VOCAB_SIZE)
      {
        for(unsigned int i=0;i<MAX_PHR_DICT_KEY_SIZE_SBDB;++i)
        {
          if(words[i]==UNUSED_WORD)
            break;
          else
            phrase1.push_back(words[i]);
        }
        for(unsigned int i=phrase1.size()+1;i<MAX_PHR_DICT_KEY_SIZE_SBDB;++i)
        {
          if(words[i]==MAX_VOCAB_SIZE)
            break;
          else
            phrase2.push_back(words[i]);
        }
      }
      srcPhr=phrase1;
      trgPhr=phrase2;
    }
  int setPhrPair(const std::vector<WordIndex>& srcPhr,
                 const std::vector<WordIndex>& trgPhr)
    {
      if(srcPhr.size()+trgPhr.size()+1<=MAX_PHR_DICT_KEY_SIZE_SBDB)
      {
        clear();
        std::vector<WordIndex> phrase1=srcPhr;
        std::vector<WordIndex> phrase2=trgPhr;
        for(unsigned int i=0;i<phrase1.size();++i)
        {
          words[i]=phrase1[i];
        }
        words[phrase1.size()]=UNUSED_WORD;
        for(unsigned int i=0;i<phrase2.size();++i)
        {
          words[phrase1.size()+1+i]=phrase2[i];
        }
        return THOT_OK;
      }
      else
        return THOT_ERROR;
    }
};

struct PhrDictValue
{
  Count count;
  Count auxCount;

  PhrDictValue(void)
    {
      count=0;
      auxCount=0;
    }
};
  
//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- FastBdbPhraseTable class

class FastBdbPhraseTable
{
 public:
    typedef std::map<std::vector<WordIndex>,PhrasePairInfo> SrcTableNode;
    typedef std::map<std::vector<WordIndex>,PhrasePairInfo> TrgTableNode;
  
        // Constructor
    FastBdbPhraseTable(void);

        // Initialization
    bool init(const char *fileName);

        // Functions to update table counts
    void incrCountsOfEntry(const std::vector<WordIndex>& s,
                           const std::vector<WordIndex>& t,
                           Count c);
        // Function to enable fast search. Fast search is disabled
        // whenever incrCountsOfEntry() is called
    void enableFastSearch(void);
    
        // Functions to access table
    Count getSrcInfo(const std::vector<WordIndex>& s,
                     bool& found);
    Count getTrgInfo(const std::vector<WordIndex>& t,
                     bool& found);
    Count getSrcTrgInfo(const std::vector<WordIndex>& s,
                        const std::vector<WordIndex>& t,
                        bool &found);
    bool getEntriesForTarget(const std::vector<WordIndex>& t,
                             SrcTableNode& srctn);

        // Functions to obtain log-probabilities
    Prob pTrgGivenSrc(const std::vector<WordIndex>& s,
                      const std::vector<WordIndex>& t);
    LgProb logpTrgGivenSrc(const std::vector<WordIndex>& s,
                           const std::vector<WordIndex>& t);
    Prob pSrcGivenTrg(const std::vector<WordIndex>& s,
                      const std::vector<WordIndex>& t);
    LgProb logpSrcGivenTrg(const std::vector<WordIndex>& s,
                           const std::vector<WordIndex>& t);

        // Additional functions
    bool getNbestForTrg(const std::vector<WordIndex>& t,
                        NbestTableNode<PhraseTransTableNodeData>& nbt,
                        int N);

        // size and clear functions
    size_t size(void);
    void clear(void);
    
        // Destructor
    virtual ~FastBdbPhraseTable();
	
 protected:

        // Environment pointers
    DbEnv* envPtr;
    
        // Database pointers
    Db* phrDictDb;

        // Functions to control fastSearchAvailable flag
    void setFastSearchAvailableFlag(void);
    void resetFastSearchAvailableFlag(void);
    bool getFastSearchAvailableFlag(void);
    bool fastSearchAvailableFlagIsDefined(void);
    
        // Auxiliary functions
    void initDbtKey(Dbt& key,
                    PhrDictKey& phrDictKey);
    void initDbtKeyCursor(Dbt& key,
                          PhrDictKey& phrDictKey);
    void initDbtData(Dbt& data,
                     PhrDictValue& phrDictValue);
    int retrieveDataForPhrDict(const std::vector<WordIndex>& s,
                               const std::vector<WordIndex>& t,
                              PhrDictValue& phrDictValue);
    int putDataForPhrDict(const std::vector<WordIndex>& s,
                          const std::vector<WordIndex>& t,
                          Count c);
    int incrPhrDictCount(const std::vector<WordIndex>& s,
                         const std::vector<WordIndex>& t,
                         Count c);
    std::string extractDirName(std::string filePath);
};

#endif
