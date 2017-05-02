/*
thot package for statistical machine translation
 
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
/* Module: DaTriePhraseTable                                        */
/*                                                                  */
/* Definitions file: DaTriePhraseTable.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "DaTriePhraseTable.h"

//#include <iterator>

//--------------- Function definitions

//-------------------------
DaTriePhraseTable::DaTriePhraseTable(void)
{
  // Create AlphaMap
  alphabet_map = alpha_map_new ();
  if(!alphabet_map)
  {
    printf("Cannot create AlphaMap\n");
    exit(1);
  }
  // Set AlphaMap's range to all ASCII characters
  if(alpha_map_add_range(alphabet_map, 0x0000, 0x007f) != 0)
  {
    printf("Cannot set AlphaMap range\n");
    exit(2);
  }
  // Create Trie object
  trie = trie_new(alphabet_map);
  if(!trie) {
    printf("Cannot create trie\n");
    exit(3);
  }
}

//-------------------------
void DaTriePhraseTable::trieStore(const Vector<WordIndex>& key, int value)
{
  wstring key_str(key.begin(), key.end());
  trie_store(trie, (AlphaChar *) key_str.c_str(), value);
}

//-------------------------
bool DaTriePhraseTable::trieRetrieve(const Vector<WordIndex>& key, TrieData &state)
{
  wstring key_str(key.begin(), key.end());
  bool found = trie_retrieve(trie, (AlphaChar *) key_str.c_str(), &state);
  return(found);
}

//-------------------------
Vector<WordIndex> DaTriePhraseTable::getTrg(const Vector<WordIndex>& t)
{
  // Prepare t vector as (UNUSED_WORD, t)
  Vector<WordIndex> uw_t_vec;
  uw_t_vec.push_back(UNUSED_WORD);
  uw_t_vec.insert(uw_t_vec.end(), t.begin(), t.end());

  return(uw_t_vec);
}

//-------------------------
Vector<WordIndex> DaTriePhraseTable::getSrcTrg(const Vector<WordIndex>& s,
                                               const Vector<WordIndex>& t)
{
  // Prepare (s,t) vector as (s, UNUSED_WORD, t)
  Vector<WordIndex> s_uw_t_vec = s;
  s_uw_t_vec.push_back(UNUSED_WORD);
  s_uw_t_vec.insert(s_uw_t_vec.end(), t.begin(), t.end());

  return(s_uw_t_vec);
}

//-------------------------
bool DaTriePhraseTable::getNbestForSrc(const Vector<WordIndex>& s,
                                       NbestTableNode<PhraseTransTableNodeData>& nbt)
{
      // TO-BE-DONE (LOW PRIORITY)
}
//-------------------------
bool DaTriePhraseTable::getNbestForTrg(const Vector<WordIndex>& t,
                                       NbestTableNode<PhraseTransTableNodeData>& nbt,
                                       int N)
{
      // TO-BE-DONE (MEDIUM PRIORITY)
}

//-------------------------
void DaTriePhraseTable::addTableEntry(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      PhrasePairInfo inf) 
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
void DaTriePhraseTable::addSrcInfo(const Vector<WordIndex>& s,
                                   Count s_inf)
{
  trieStore(s, (int) s_inf.get_c_s());
}

//-------------------------
void DaTriePhraseTable::addSrcTrgInfo(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      Count st_inf)
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
void DaTriePhraseTable::incrCountsOfEntry(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t,
                                          Count c) 
{
      // TO-BE-DONE (STARTING POINT)
      //
      // NOTES: It is important to store the counts for (s,t), s and t
      // separatedly. Therefore, this function could be implemented by
      // means of three new functions:
      // 
      // increaseJointCount(s,t,c);
      // increaseSrcCount(s,c);
      // increaseTrgCount(t,c);
      // 
      // It is possible to use only one trie for the three types of
      // counts. The keys are sequences of word indices and the values
      // are integers.
      // 
      // Let's suppose that s is [1,2,3] and t is [4,5], the proposed
      // key representation is as follows:
      // 
      // - (s,t): [1,2,3,UNUSED_WORD,4,5]
      // - s: [1,2,3]
      // - t: [UNUSED_WORD,4,5]
      // 
      // Here, UNUSED_WORD is a constant representing a word index that
      // is never used. The constant is defined in
      // src/nlp_common/StatModelDefs.h

  // Prepare vectors
  Vector<WordIndex> uw_t_vec = getTrg(t);
  Vector<WordIndex> s_uw_t_vec = getSrcTrg(s, t);

  // Retrieve previous states
  bool found;
  Count s_count = getSrcInfo(s, found);
  Count uw_t_count = getSrcInfo(uw_t_vec, found);
  Count s_uw_t_count = getSrcInfo(s_uw_t_vec, found);

  // Update counts
  addSrcInfo(s, s_count + c);
  trieStore(uw_t_vec, (int) (uw_t_count + c).get_c_s());
  trieStore(s_uw_t_vec, (int) (s_uw_t_count + c).get_c_st());


  //std::stringstream result;
  //std::copy(s_uw_t_vec.begin(), s_uw_t_vec.end(), std::ostream_iterator<uint>(result, " "));

  //printf("%s\n", result.str().c_str());
  //cout << result.str().c_str() << endl;
}

//-------------------------
PhrasePairInfo DaTriePhraseTable::infSrcTrg(const Vector<WordIndex>& s,
                                            const Vector<WordIndex>& t,
                                            bool& found) 
{
  PhrasePairInfo ppi;

  ppi.first = getSrcInfo(s, found);
  if (!found)
  {
    ppi.second = 0;
    return ppi;
  }
  else
  {
    ppi.second = getSrcTrgInfo(s, t, found);
    return ppi;
  }
}

//-------------------------
Count DaTriePhraseTable::getSrcInfo(const Vector<WordIndex>& s,
                                    bool &found)
{
  TrieData state;
  found = trieRetrieve(s, state);

  Count result = (found) ? Count((float) state) : Count();
  
  return(result);
}

//-------------------------
Count DaTriePhraseTable::getTrgInfo(const Vector<WordIndex>& t,
                                    bool &found)
{
  // Retrieve counter state
  return(getSrcInfo(getTrg(t), found));
}

//-------------------------
Count DaTriePhraseTable::getSrcTrgInfo(const Vector<WordIndex>& s,
                                       const Vector<WordIndex>& t,
                                       bool &found)
{
  // Retrieve counter state
  return(getSrcInfo(getSrcTrg(s, t), found));
}

//-------------------------
Prob DaTriePhraseTable::pTrgGivenSrc(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t)
{
  // Calculates p(t|s)=count(s,t)/count(s)
  Count st_count = cSrcTrg(s, t);	
  if ((float) st_count > 0)
  {
    bool found;
    Count s_count = getSrcInfo(s, found);
	  if ((float) s_count > 0)
      return (float) st_count / (float) s_count;
	  else
      return PHRASE_PROB_SMOOTH;
  }
  else return PHRASE_PROB_SMOOTH;
}

//-------------------------
LgProb DaTriePhraseTable::logpTrgGivenSrc(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t)
{
  return log((double)pTrgGivenSrc(s,t));
}

//-------------------------
Prob DaTriePhraseTable::pSrcGivenTrg(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t)
{
  // p(s|t)=count(s,t)/count(t)
  Count st_count = cSrcTrg(s, t);
  if ((float) st_count > 0)
  {
    Count t_count = cTrg(t);
	  if ((float) t_count > 0)
      return (float) st_count / (float) t_count;
	  else
      return PHRASE_PROB_SMOOTH;
  }
  else return PHRASE_PROB_SMOOTH;
}

//-------------------------
LgProb DaTriePhraseTable::logpSrcGivenTrg(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t)
{
  return log((double)pSrcGivenTrg(s,t));
}

//-------------------------
bool DaTriePhraseTable::getEntriesForTarget(const Vector<WordIndex>& t,
                                            DaTriePhraseTable::SrcTableNode& srctn) 
{
      // TO-BE-DONE (MEDIUM PRIORITY)
}

//-------------------------
bool DaTriePhraseTable::getEntriesForSource(const Vector<WordIndex>& s,
                                            DaTriePhraseTable::TrgTableNode& trgtn) 
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
Count DaTriePhraseTable::cSrcTrg(const Vector<WordIndex>& s,
                                 const Vector<WordIndex>& t)
{
  bool found;
  return getSrcTrgInfo(s,t,found);
}

//-------------------------
Count DaTriePhraseTable::cSrc(const Vector<WordIndex>& s)
{
  bool found;
  return getSrcInfo(s,found);
}

//-------------------------
Count DaTriePhraseTable::cTrg(const Vector<WordIndex>& t)
{
  bool found;
  return getTrgInfo(t,found);
}

//-------------------------
size_t DaTriePhraseTable::size(void)
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
void DaTriePhraseTable::clear(void)
{
  AlphaMap *map = alpha_map_clone(alphabet_map);
  trie_free(trie);
  alphabet_map = map;
  // Create empty trie
  trie = trie_new(alphabet_map);
  if(!trie) {
    printf("Cannot recreate trie\n");
    exit(3);
  }
}

//-------------------------
DaTriePhraseTable::~DaTriePhraseTable(void)
{
  trie_free(trie);
}

//-------------------------
DaTriePhraseTable::const_iterator DaTriePhraseTable::begin(void)const
{
  // DaTriePhraseTable::const_iterator iter(this,phraseDict.begin());
  // return iter;
}

//-------------------------
DaTriePhraseTable::const_iterator DaTriePhraseTable::end(void)const
{
  // DaTriePhraseTable::const_iterator iter(this,phraseDict.end());
  // return iter;
}

// const_iterator function definitions
//--------------------------
bool DaTriePhraseTable::const_iterator::operator++(void) //prefix
{
  // if(ptPtr!=NULL)
  // {
  //   ++pdIter;
  //   if(pdIter==ptPtr->phraseDict.end()) return false;
  //   else
  //   {
  //     return true;
  //   }
  // }
  // else return false;
}

//--------------------------
bool DaTriePhraseTable::const_iterator::operator++(int)  //postfix
{
  return operator++();
}

//--------------------------
int DaTriePhraseTable::const_iterator::operator==(const const_iterator& right)
{
  // return (ptPtr==right.ptPtr && pdIter==right.pdIter);	
}

//--------------------------
int DaTriePhraseTable::const_iterator::operator!=(const const_iterator& right)
{
  return !((*this)==right);
}

//--------------------------
// const PhraseDict::const_iterator&
// DaTriePhraseTable::const_iterator::operator->(void)const
// {
//   return pdIter;
// }

//-------------------------
