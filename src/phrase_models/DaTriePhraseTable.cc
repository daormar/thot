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
  alphabet_map = alpha_map_new();
  if(!alphabet_map)
  {
    printf("Cannot create AlphaMap\n");
    exit(1);
  }
  // Set AlphaMap's range to 32-bit (0xFFFFFFFF value is reserved for internal usage)
  if(alpha_map_add_range(alphabet_map, 0x0000, 0xFFFFFFFE) != 0)
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
  // Prepare iterator and root node state
  trie_root_node = trie_root (trie);
}

//-------------------------
wstring DaTriePhraseTable::vectorToWstring(const Vector<WordIndex>& s) const
{
  wstring str(s.begin(), s.end());
  return str;
}

//-------------------------
Vector<WordIndex> DaTriePhraseTable::alphaCharToVector(AlphaChar *a) const
{
  Vector<WordIndex> vec;

  for(AlphaChar *ptr = a; *ptr; ptr++)
  {
    vec.push_back((int) *ptr);
  }

  return vec;
}

//-------------------------
void DaTriePhraseTable::trieStore(const Vector<WordIndex>& key, int value)
{
  trie_store(trie, (AlphaChar *) vectorToWstring(key).c_str(), value);
}

//-------------------------
bool DaTriePhraseTable::trieRetrieve(const Vector<WordIndex>& key, TrieData &state)
{
  bool found = trie_retrieve(trie, (AlphaChar *) vectorToWstring(key).c_str(), &state);
  return found;
}

//-------------------------
bool DaTriePhraseTable::trieSaveToFile(const char *path)
{
  int result = trie_save(trie, path);
  if (result == 0)
    return true;  // Trie saved successuflly
  return false;  // Trie could not be saved properly
}

//-------------------------
bool DaTriePhraseTable::trieLoadFromFile(const char *path)
{
  Trie* loaded_trie = trie_new_from_file(path);
  if (loaded_trie == NULL)
    return false;  // Cannot load trie

  // Release old resources
  trie_state_free(trie_root_node);
  trie_free(trie);
  // Assign loaded trie
  trie = loaded_trie;
  // Recreate root node indicator and trie iterator
  trie_root_node = trie_root (trie);
  
  return true;
}

//-------------------------
bool DaTriePhraseTable::save(const char *path)
{
  return trieSaveToFile(path);
}

//-------------------------
bool DaTriePhraseTable::load(const char *path)
{
  return trieLoadFromFile(path);
}

//-------------------------
Vector<WordIndex> DaTriePhraseTable::getSrc(const Vector<WordIndex>& s)
{
  // Prepare s vector as (UNUSED_WORD, s)
  Vector<WordIndex> uw_s_vec;
  uw_s_vec.push_back(UNUSED_WORD);
  uw_s_vec.insert(uw_s_vec.end(), s.begin(), s.end());

  return uw_s_vec;
}

//-------------------------
Vector<WordIndex> DaTriePhraseTable::getSrcTrg(const Vector<WordIndex>& s,
                                               const Vector<WordIndex>& t)
{
  // Prepare (s,t) vector as (UNUSED_WORD, s, UNUSED_WORD, t)
  Vector<WordIndex> uw_s_uw_t_vec = getSrc(s);
  uw_s_uw_t_vec.push_back(UNUSED_WORD);
  uw_s_uw_t_vec.insert(uw_s_uw_t_vec.end(), t.begin(), t.end());

  return uw_s_uw_t_vec;
}

//-------------------------
Vector<WordIndex> DaTriePhraseTable::getTrgSrc(const Vector<WordIndex>& s,
                                               const Vector<WordIndex>& t)
{
  // Prepare (t,s) vector as (t, UNUSED_WORD, s)
  Vector<WordIndex> t_uw_s_vec = t;
  t_uw_s_vec.push_back(UNUSED_WORD);
  t_uw_s_vec.insert(t_uw_s_vec.end(), s.begin(), s.end());

  return t_uw_s_vec;
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
  DaTriePhraseTable::SrcTableNode::iterator iter;	

  bool found;
  Count t_count;
  DaTriePhraseTable::SrcTableNode node;
  LgProb lgProb;

  nbt.clear();
  
  found = getEntriesForTarget(t, node);
  t_count = cTrg(t);

  if(found) {
    // Generate transTableNode
    for(iter = node.begin(); iter != node.end(); iter++) 
    {
      Vector<WordIndex> s = iter->first;
      PhrasePairInfo ppi = (PhrasePairInfo) iter->second;
      lgProb = log((float) ppi.second.get_c_st() / (float) t_count);
      nbt.insert(lgProb, s); // Insert pair <log probability, source phrase>
    }
    
#   ifdef DO_STABLE_SORT_ON_NBEST_TABLE
        // Performs stable sort on n-best table, this is done to ensure
        // that the n-best lists generated by cache models and
        // conventional models are identical. However this process is
        // time consuming and must be avoided if possible
    nbt.stableSort();
#   endif
    
    while(nbt.size() > (unsigned int) N && N >= 0)
    {
      // node contains N inverse translations, remove last element
      nbt.removeLastElement();
    }
    
    return true;
  }
  else
  {
    // Cannot find the target phrase
    return false;
  }
}

//-------------------------
void DaTriePhraseTable::addTableEntry(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      PhrasePairInfo inf) 
{
  addSrcInfo(s, inf.first.get_c_s());  // (USUSED_WORD, s)
  trieStore(t, (int) inf.second.get_c_s());  // (t)
  addSrcTrgInfo(s, t, (int) inf.second.get_c_st()); // (t, UNUSED_WORD, s)
}

//-------------------------
void DaTriePhraseTable::addSrcInfo(const Vector<WordIndex>& s,
                                   Count s_inf)
{
  trieStore(getSrc(s), (int) s_inf.get_c_s());
}

//-------------------------
void DaTriePhraseTable::addSrcTrgInfo(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      Count st_inf)
{
  //trieStore(getSrcTrg(s, t), (int) st_inf.get_c_st());  // (USUSED_WORD, s, UNUSED_WORD, t)
  trieStore(getTrgSrc(s, t), (int) st_inf.get_c_st());  // (t, UNUSED_WORD, s)
}

//-------------------------
void DaTriePhraseTable::incrCountsOfEntry(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t,
                                          Count c) 
{
  // Retrieve previous states
  Count s_count = cSrc(s);
  Count t_count = cTrg(t);
  Count src_trg_count = cSrcTrg(s, t);

  // Update counts
  addSrcInfo(s, s_count + c);  // (USUSED_WORD, s)
  trieStore(t, (int) (t_count + c).get_c_s());  // (t)
  addSrcTrgInfo(s, t, (src_trg_count + c).get_c_st());
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
Count DaTriePhraseTable::getInfo(const Vector<WordIndex>& key,
                                    bool &found)
{
  TrieData state;
  found = trieRetrieve(key, state);

  Count result = (found) ? Count((float) state) : Count();
  
  return result;
}

//-------------------------
Count DaTriePhraseTable::getSrcInfo(const Vector<WordIndex>& s,
                                    bool &found)
{
  return getInfo(getSrc(s), found);
}

//-------------------------
Count DaTriePhraseTable::getTrgInfo(const Vector<WordIndex>& t,
                                    bool &found)
{
  // Retrieve counter state
  return getInfo(t, found);
}

//-------------------------
Count DaTriePhraseTable::getSrcTrgInfo(const Vector<WordIndex>& s,
                                       const Vector<WordIndex>& t,
                                       bool &found)
{
  // Retrieve counter state
  return getInfo(getTrgSrc(s, t), found);
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
  bool found;
  TrieState *state;
  TrieIterator *iter;

  // Find node which starts from (t, UNUSED_WORD) to find possible translations
  Vector<WordIndex> t_uw_vec = t;
  t_uw_vec.push_back(UNUSED_WORD);
  state = trie_root (trie);

  for (unsigned int i = 0; i < t_uw_vec.size(); i++) {
    if (!trie_state_walk(state, (AlphaChar) t_uw_vec[i])) {
      return false;
    }
  }

  // Check for the source phrases
  iter = trie_iterator_new(state);
  srctn.clear();  // Make sure that structure does not keep old values

  while(trie_iterator_next(iter))
  {
    AlphaChar *key;

    key = trie_iterator_get_key(iter);
    if (!key)
      continue;

    Vector<WordIndex> src = alphaCharToVector(key);
    PhrasePairInfo ppi = infSrcTrg(src, t, found);
    if (!found || (int) ppi.first.get_c_s() == 0 || (int) ppi.second.get_c_s() == 0)
      continue;

    srctn.insert(pair<Vector<WordIndex>, PhrasePairInfo>(src, ppi));
  }

  // Free memory
  trie_iterator_free(iter);
  trie_state_free(state);

  return true;
}

//-------------------------
bool DaTriePhraseTable::getEntriesForSource(const Vector<WordIndex>& s,
                                            DaTriePhraseTable::TrgTableNode& trgtn) 
{
  /*DaTriePhraseTable::SrcTableNode srctn;
  Vector<WordIndex> uw_s_vec = getSrc(s);
  bool found = getEntriesForTarget(uw_s_vec, srctn);
  trgtn = (DaTriePhraseTable::TrgTableNode) srctn;

  return found;*/
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
  size_t len = 0;

  for(DaTriePhraseTable::const_iterator iter = begin(); iter != end(); iter++, len++)
  {
    // Do nothing; iterates only over the elements in trie
  }

  return len;
}

//-------------------------
void DaTriePhraseTable::print(void)
{
  cout << "Trie structure:" << endl;
  for(DaTriePhraseTable::const_iterator iter = begin(); iter != end(); iter++)
  {
    pair<Vector<WordIndex>, int> x = *iter;
    wcout << vectorToWstring(x.first);
    cout << "\t" << x.second << endl;
  }
}

//-------------------------
void DaTriePhraseTable::clear(void)
{
  trie_state_free(trie_root_node);
  trie_free(trie);
  // Create empty trie
  trie = trie_new(alphabet_map);
  if(!trie) {
    printf("Cannot recreate trie\n");
    exit(3);
  }
  // Recreate root node indicator and trie iterator
  trie_root_node = trie_root (trie);
}

//-------------------------
DaTriePhraseTable::~DaTriePhraseTable(void)
{
  trie_state_free(trie_root_node);
  alpha_map_free(alphabet_map);
  trie_free(trie);
}

//-------------------------
DaTriePhraseTable::const_iterator DaTriePhraseTable::begin(void)const
{
  TrieIterator *trie_iter;

  trie_iter = trie_iterator_new(trie_root_node);
  /*while(!trie_iterator_get_key(trie_iter) && trie_iterator_next(trie_iter))
  {
    // Do nothing - only moving iterator to the first valid key
  }*/
  if (!trie_iterator_next(trie_iter))
    trie_iter = NULL;
    
  DaTriePhraseTable::const_iterator iter(this, trie_iter);

  return iter;
}

//-------------------------
DaTriePhraseTable::const_iterator DaTriePhraseTable::end(void)const
{
  DaTriePhraseTable::const_iterator iter(this, NULL);
  return iter;
}

// const_iterator function definitions
//--------------------------
bool DaTriePhraseTable::const_iterator::operator++(void) //prefix
{
  if(internalTrieIter != NULL)
  {
    AlphaChar *key;

    while(trie_iterator_next(internalTrieIter)) {
      key = trie_iterator_get_key(internalTrieIter);
      if (key) {
        return true;
      }
    }

    internalTrieIter = NULL;
    return false;  // We have not found any more elements in the trie
  }
  else
  {
    return false;  // Iterator does not exist
  }
}

//--------------------------
bool DaTriePhraseTable::const_iterator::operator++(int)  //postfix
{
  return operator++();
}

//--------------------------
int DaTriePhraseTable::const_iterator::operator==(const const_iterator& right)
{
  return (ptPtr == right.ptPtr && internalTrieIter == right.internalTrieIter);
}

//--------------------------
int DaTriePhraseTable::const_iterator::operator!=(const const_iterator& right)
{
  return !((*this) == right);
}

//--------------------------
pair<Vector<WordIndex>, int> DaTriePhraseTable::const_iterator::operator*(void)
{
  return *operator->();
}

//--------------------------
const pair<Vector<WordIndex>, int>*
DaTriePhraseTable::const_iterator::operator->(void)
{
  AlphaChar *key;
  key = trie_iterator_get_key(internalTrieIter);
  Vector<WordIndex> key_vec = ptPtr->alphaCharToVector(key);
  int status = (int) trie_iterator_get_data(internalTrieIter);

  trieItem = make_pair(key_vec, status);

  return &trieItem;
}

//-------------------------
