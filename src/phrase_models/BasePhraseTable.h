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
 * @file BasePhraseTable.h
 * 
 * @brief Implements a bilingual phrase dictionary (Abstract class).
 */

#ifndef _BasePhraseTable
#define _BasePhraseTable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseDefs.h"
#include "PhrasePairInfo.h"
#include "NbestTableNode.h"
#include "PhraseTransTableNodeData.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- BasePhraseTable class

class BasePhraseTable
{
 public:

    typedef std::map<std::vector<WordIndex>,PhrasePairInfo> SrcTableNode;
    typedef std::map<std::vector<WordIndex>,PhrasePairInfo> TrgTableNode;

        // Abstract function definitions
    virtual void addTableEntry(const std::vector<WordIndex>& s,
                               const std::vector<WordIndex>& t,
                               PhrasePairInfo inf)=0;
        // Adds an entry to the probability table
    virtual void addSrcInfo(const std::vector<WordIndex>& s,Count s_inf)=0;
    virtual void addSrcTrgInfo(const std::vector<WordIndex>& s,
                               const std::vector<WordIndex>& t,
                               Count st_inf)=0;
    virtual void incrCountsOfEntry(const std::vector<WordIndex>& s,
                                   const std::vector<WordIndex>& t,
                                   Count c)=0;
    virtual void incrCountsOfEntryLog(const std::vector<WordIndex>& s,
                                      const std::vector<WordIndex>& t,
                                      LogCount lc)
      {
        incrCountsOfEntry(s,t,exp((float)lc));
      };
    
        // Increase the counts of a given phrase pair
    virtual PhrasePairInfo infSrcTrg(const std::vector<WordIndex>& s,
                                     const std::vector<WordIndex>& t,
                                     bool &found)=0;
        // Returns information related to a given s and t.
    virtual Count getSrcInfo(const std::vector<WordIndex>& s,bool &found)=0;
        // Returns information related to a given s and t.
    virtual Count getSrcTrgInfo(const std::vector<WordIndex>& s,
                                const std::vector<WordIndex>& t,
                                bool &found)=0;
        // Returns information related to a given s and t.
    virtual Prob pTrgGivenSrc(const std::vector<WordIndex>& s,
                              const std::vector<WordIndex>& t)=0;
    virtual LgProb logpTrgGivenSrc(const std::vector<WordIndex>& s,
                                   const std::vector<WordIndex>& t)=0;
    virtual Prob pSrcGivenTrg(const std::vector<WordIndex>& s,
                              const std::vector<WordIndex>& t)=0;
    virtual LgProb logpSrcGivenTrg(const std::vector<WordIndex>& s,
                                   const std::vector<WordIndex>& t)=0;
    virtual bool getEntriesForTarget(const std::vector<WordIndex>& t,
                                     SrcTableNode& srctn)=0;
        // Stores in srctn the entries associated to a given target
        // phrase t, returns true if there are one or more entries
    virtual bool getEntriesForSource(const std::vector<WordIndex>& s,
                                     TrgTableNode& trgtn)=0;
        // Stores in trgtn the entries associated to a given source
        // phrase s, returns true if there are one or more entries
    virtual bool getNbestForSrc(const std::vector<WordIndex>& s,
                                NbestTableNode<PhraseTransTableNodeData>& nbt)=0;
    virtual bool getNbestForTrg(const std::vector<WordIndex>& t,
                                NbestTableNode<PhraseTransTableNodeData>& nbt,
                                int N=-1)=0;

       // Counts-related functions
    virtual Count cSrcTrg(const std::vector<WordIndex>& s,
                          const std::vector<WordIndex>& t)=0;
    virtual Count cSrc(const std::vector<WordIndex>& s)=0;
    virtual Count cTrg(const std::vector<WordIndex>& t)=0;

    virtual LogCount lcSrcTrg(const std::vector<WordIndex>& s,
                              const std::vector<WordIndex>& t)
      {
        return log((float)cSrcTrg(s,t));
      };
    virtual LogCount lcSrc(const std::vector<WordIndex>& s)
      {
        return log((float)cSrc(s));
      };
    virtual LogCount lcTrg(const std::vector<WordIndex>& t)
      {
        return log((float)cTrg(t));
      };

        // size and clear functions
    virtual size_t size(void)=0;
    virtual void clear(void)=0;   

        // Destructor
    virtual ~BasePhraseTable(){};

 protected:
};

#endif
