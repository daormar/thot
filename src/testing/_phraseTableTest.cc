/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz

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
 * @file _phraseTableTest.cc
 * 
 * @brief Definitions file for _phraseTableTest.h
 */

//--------------- Include files --------------------------------------

#include "_phraseTableTest.h"

//--------------- _phraseTableTest class functions

//---------------------------------------
std::vector<WordIndex> _phraseTableTest::getVector(std::string phrase) {
  std::vector<WordIndex> v;

  for(unsigned int i = 0; i < phrase.size(); i++) {
    v.push_back(phrase[i]);
  }

  return(v);
}

//---------------------------------------
void _phraseTableTest::testStoreAndRestore()
{
  std::vector<WordIndex> s1 = getVector("Morag city");
  std::vector<WordIndex> s2 = getVector("Narie lake");
  Count cs1 = Count(5);
  Count cs2 = Count(2);
  tab->clear();
  tab->addSrcInfo(s1, cs1);
  tab->addSrcInfo(s2, cs2);

  bool found;
  Count s1_count = tab->getSrcInfo(s1, found);
  Count s2_count = tab->getSrcInfo(s2, found);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(5, s1_count.get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2, s2_count.get_c_s(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testAddTableEntry()
{
  std::vector<WordIndex> s = getVector("Narie lake");
  std::vector<WordIndex> t = getVector("jezioro Narie");
  Count s_count = Count(3);
  Count t_count = Count(2);
  PhrasePairInfo ppi(s_count, t_count);

  tab->clear();
  tab->addTableEntry(s, t, ppi);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(3, tab->cSrc(s).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2, tab->cTrg(t).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2, tab->cSrcTrg(s, t).get_c_st(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testIncCountsOfEntry()
{
  std::vector<WordIndex> s = getVector("Narie lake");
  std::vector<WordIndex> t = getVector("jezioro Narie");
  Count c_init = Count(3);
  Count c = Count(17);

  tab->clear();
  tab->addSrcInfo(s, c_init);
  tab->incrCountsOfEntry(s, t, c);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(20, tab->cSrc(s).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(17, tab->cTrg(t).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(17, tab->cSrcTrg(s, t).get_c_st(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testGetEntriesForTarget()
{
  BasePhraseTable::SrcTableNode node;
  std::vector<WordIndex> s1_1 = getVector("Pasleka river");
  std::vector<WordIndex> s1_2 = getVector("Pasleka");
  std::vector<WordIndex> t1_1 = getVector("rzeka Pasleka");
  std::vector<WordIndex> t1_2 = getVector("Pasleka");
  std::vector<WordIndex> s2 = getVector("river");
  std::vector<WordIndex> t2 = getVector("rzeka");
  Count c = Count(1);

  tab->clear();
  tab->incrCountsOfEntry(s1_1, t1_1, c);
  tab->incrCountsOfEntry(s1_2, t1_1, c);
  tab->incrCountsOfEntry(s1_1, t1_2, c);
  tab->incrCountsOfEntry(s2, t2, c);

  bool result;
  // Looking for phrases for which 'rzeka Pasleka' is translation
  result = tab->getEntriesForTarget(t1_1, node);
  CPPUNIT_ASSERT( result );
  CPPUNIT_ASSERT_EQUAL((size_t) 2, node.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2, node[s1_1].first.get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node[s1_1].second.get_c_st(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node[s1_2].first.get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node[s1_2].second.get_c_st(), EPSILON);

  // Looking for phrases for which 'Pasleka' is translation
  result = tab->getEntriesForTarget(t1_2, node);
  CPPUNIT_ASSERT( result );
  CPPUNIT_ASSERT_EQUAL((size_t) 1, node.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2, node[s1_1].first.get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node[s1_1].second.get_c_st(), EPSILON);

  // Looking for phrases for which 'rzeka' is translation
  result = tab->getEntriesForTarget(t2, node);
  CPPUNIT_ASSERT( result );
  CPPUNIT_ASSERT_EQUAL((size_t) 1, node.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node[s2].first.get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node[s2].second.get_c_st(), EPSILON);

  // 'xyz'' key shoud not be found
  result = tab->getEntriesForTarget(getVector("xyz"), node);
  CPPUNIT_ASSERT( !result );
}

//---------------------------------------
void _phraseTableTest::testRetrievingSubphrase()
{
  /* TEST:
     Accessing element with the subphrase should return count 0
  */
  bool found;
  std::vector<WordIndex> s = getVector("Hello");
  std::vector<WordIndex> t1 = getVector("Buenos Dias");
  std::vector<WordIndex> t2 = getVector("Buenos");

  Count c = Count(1);

  tab->clear();
  tab->addSrcInfo(s, c);
  tab->incrCountsOfEntry(s, t1, c);
  c = tab->getSrcTrgInfo(s, t2, found);

  CPPUNIT_ASSERT( !found );
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0, c.get_c_s(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testRetrieveNonLeafPhrase()
{
  /* TEST:
     Phrases with count > 0 and not stored in the leaves
     should be also retrieved
  */
  bool found;
  BasePhraseTable::SrcTableNode node;
  std::vector<WordIndex> s = getVector("Hello");
  std::vector<WordIndex> t1 = getVector("Buenos Dias");
  std::vector<WordIndex> t2 = getVector("Buenos");

  Count c = Count(1);

  tab->clear();
  tab->incrCountsOfEntry(s, t1, c);
  tab->incrCountsOfEntry(s, t2, c);

  // Check phrases and their counts
  // Phrase pair 1
  c = tab->getSrcTrgInfo(s, t1, found);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, c.get_c_s(), EPSILON);
  // Phrase pair 2
  c = tab->getSrcTrgInfo(s, t2, found);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, c.get_c_s(), EPSILON);

  // Looking for phrases for which 'Buenos' is translation
  found = tab->getEntriesForTarget(t2, node);
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node.size(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testGetEntriesForSource()
{
  /* TEST:
     Find translations for the source phrase
  */
  bool found;
  BasePhraseTable::TrgTableNode node;
  std::vector<WordIndex> s1 = getVector("jezioro Narie");
  std::vector<WordIndex> t1_1 = getVector("Narie lake");
  std::vector<WordIndex> t1_2 = getVector("Narie");
  std::vector<WordIndex> s2 = getVector("jezioro Skiertag");
  std::vector<WordIndex> t2_1 = getVector("Skiertag");
  std::vector<WordIndex> s3 = getVector("jezioro Jeziorak");
  std::vector<WordIndex> t3_1 = getVector("Jeziorak lake");
  std::vector<WordIndex> t3_2 = getVector("Jeziorak");

  Count c = Count(1);

  // Prepare data struture
  tab->clear();
  // Add Narie phrases
  tab->incrCountsOfEntry(s1, t1_1, c);
  tab->incrCountsOfEntry(s1, t1_2, c);
  // Add Skiertag phrases
  tab->incrCountsOfEntry(s2, t2_1, c);
  // Add Jeziorak phrases
  tab->incrCountsOfEntry(s3, t3_1, c);
  tab->incrCountsOfEntry(s3, t3_2, c);

  // Looking for translations
  // Narie phrases
  found = tab->getEntriesForSource(s1, node);
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_EQUAL((size_t) 2, node.size());
  // Skiertag phrases
  found = tab->getEntriesForSource(s2, node);
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_EQUAL((size_t) 1, node.size());
  // Jeziorak phrases
  found = tab->getEntriesForSource(s3, node);
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_EQUAL((size_t) 2, node.size());
}

//---------------------------------------
void _phraseTableTest::testRetrievingEntriesWithCountEqualZero()
{
  /* TEST:
     Function getEntriesForTarget for retrieving entries should skip
     elements with count equals 0
  */
  bool found;
  BasePhraseTable::SrcTableNode node;
  std::vector<WordIndex> s1 = getVector("Palac Dohnow");
  std::vector<WordIndex> s2 = getVector("Palac Dohnow w Moragu");
  std::vector<WordIndex> t = getVector("Dohn's Palace");

  tab->clear();
  tab->incrCountsOfEntry(s1, t, Count(1));
  tab->incrCountsOfEntry(s2, t, Count(0));

  found = tab->getEntriesForTarget(t, node);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_EQUAL((size_t) 1, node.size());
}

//---------------------------------------
void _phraseTableTest::testGetNbestForTrg()
{
  /* TEST:
     Check if method getNbestForTrg returns correct elements
  */
  bool found;
  NbestTableNode<PhraseTransTableNodeData> node;
  NbestTableNode<PhraseTransTableNodeData>::iterator iter;

  // Fill phrase table with data
  std::vector<WordIndex> s1 = getVector("city hall");
  std::vector<WordIndex> s2 = getVector("city hall in Morag");
  std::vector<WordIndex> s3 = getVector("town hall");
  std::vector<WordIndex> s4 = getVector("town hall in Morag");
  std::vector<WordIndex> t = getVector("ratusz miejski w Moragu");

  tab->clear();
  tab->incrCountsOfEntry(s1, t, Count(4));
  tab->incrCountsOfEntry(s2, t, Count(2));
  tab->incrCountsOfEntry(s3, t, Count(3));
  tab->incrCountsOfEntry(s4, t, Count(0));

  // Returned elements should not exceed number of elements
  // in the structure
  found = tab->getNbestForTrg(t, node, 10);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_EQUAL((unsigned int) 3, node.size());

  // If there are more available elements, only elements
  // with the highest score should be returned
  found = tab->getNbestForTrg(t, node, 2);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_EQUAL((unsigned int) 2, node.size());

  iter = node.begin();
  CPPUNIT_ASSERT( iter->second == s1 );
  iter++;
  CPPUNIT_ASSERT( iter->second == s3 );
}

//---------------------------------------
void _phraseTableTest::testAddSrcTrgInfo()
{
  /* TEST:
     Check if two keys were added (for (s, t) and (t, s) vectors)
     and if their values are the same
  */
  bool found;

  std::vector<WordIndex> s = getVector("jezioro Skiertag");
  std::vector<WordIndex> t = getVector("Skiertag lake");

  Count c = Count(1);

  tab->clear();
  tab->addSrcInfo(s, c);
  tab->addSrcTrgInfo(s, t, c);

  Count src_trg_count = tab->cSrcTrg(s, t);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, src_trg_count.get_c_s(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testPSrcGivenTrg()
{
  /* TEST:
     Check retrieving probabilities for phrases based on stored
     counts for a given target.
  */
  std::vector<WordIndex> s1 = getVector("Morag");
  std::vector<WordIndex> s2 = getVector("Gdansk");
  std::vector<WordIndex> t1 = getVector("Candas");
  std::vector<WordIndex> t2 = getVector("Aviles");

  // Fill phrase table with data
  tab->incrCountsOfEntry(s1, t1, Count(3));
  tab->incrCountsOfEntry(s2, t1, Count(7));
  tab->incrCountsOfEntry(s1, t2, Count(1));
  tab->incrCountsOfEntry(s2, t2, Count(2));

  // Check probabilities
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3, tab->pSrcGivenTrg(s1, t1), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.7, tab->pSrcGivenTrg(s2, t1), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 3., tab->pSrcGivenTrg(s1, t2), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2. / 3., tab->pSrcGivenTrg(s2, t2), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testPTrgGivenSrc()
{
  /* TEST:
     Check retrieving probabilities for phrases based on stored
     counts for a given source.
  */
  std::vector<WordIndex> s1 = getVector("Morag");
  std::vector<WordIndex> s2 = getVector("Gdansk");
  std::vector<WordIndex> t1 = getVector("Candas");
  std::vector<WordIndex> t2 = getVector("Aviles");

  // Fill phrase table with data
  tab->incrCountsOfEntry(s1, t1, Count(10));
  tab->incrCountsOfEntry(s1, t2, Count(12));

  tab->incrCountsOfEntry(s2, t1, Count(11));
  tab->incrCountsOfEntry(s2, t2, Count(13));

  // Check probabilities
  CPPUNIT_ASSERT_DOUBLES_EQUAL(10. / 22., tab->pTrgGivenSrc(s1, t1), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(12. / 22., tab->pTrgGivenSrc(s1, t2), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(11. / 24., tab->pTrgGivenSrc(s2, t1), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(13. / 24., tab->pTrgGivenSrc(s2, t2), EPSILON);
}
//---------------------------------------
void _phraseTableTest::testAddingSameSrcAndTrg()
{
  /* TEST:
     Check if the results are returned correctly when source
     and target has the same values.
  */
  std::vector<WordIndex> v1 = getVector("Morag");
  std::vector<WordIndex> v2 = getVector("~ \" ()( -");

  // Fill phrase table with data
  tab->incrCountsOfEntry(v1, v1, Count(1));
  tab->incrCountsOfEntry(v1, v2, Count(2));
  tab->incrCountsOfEntry(v2, v1, Count(4));
  tab->incrCountsOfEntry(v2, v2, Count(8));

  // Check probabilities
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1 + 2, tab->cSrc(v1).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1 + 4, tab->cTrg(v1).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(4 + 8, tab->cSrc(v2).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2 + 8, tab->cTrg(v2).get_c_s(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testSize()
{
  /* TEST:
     Check if number of elements in the phrase table is returned correctly
  */
  tab->clear();
  CPPUNIT_ASSERT_EQUAL((size_t) 0, tab->size());  // Collection after cleaning should be empty

  // Fill phrase table with data
  tab->incrCountsOfEntry(getVector("kemping w Kretowinach"), getVector("camping Kretowiny"), Count(1));
  tab->incrCountsOfEntry(getVector("kemping w Kretowinach"), getVector("camping in Kretowiny"), Count(2));

  CPPUNIT_ASSERT_EQUAL((size_t) 5, tab->size());

  tab->clear();
  CPPUNIT_ASSERT_EQUAL((size_t) 0, tab->size());  // Collection after cleaning should be empty

  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mr Car"), Count(1));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Pan Samochodzik"), Count(4));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mister Automobile"), Count(20));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mr Automobile"), Count(24));

  CPPUNIT_ASSERT_EQUAL((size_t) 9, tab->size());

  tab->incrCountsOfEntry(getVector("Pierwsza przygoda Pana Samochodzika"),
                         getVector("First Adventure of Mister Automobile"), Count(5));
  tab->incrCountsOfEntry(getVector("Pierwsza przygoda Pana Samochodzika"),
                         getVector("First Adventure of Pan Samochodzik"), Count(7));


  CPPUNIT_ASSERT_EQUAL((size_t) (9 + 5), tab->size());

}

//---------------------------------------
void _phraseTableTest::testSubkeys()
{
  /* TEST:
     Check if subkeys are stored correctly
  */

  // Fill phrase table with data
  tab->clear();

  // Define vectors
  std::vector<WordIndex> s1 = getVector("Pan Samochodzik");
  std::vector<WordIndex> t1_1 = getVector("Mr Car");
  std::vector<WordIndex> t1_2 = getVector("Pan");
  std::vector<WordIndex> t1_3 = getVector("Mr");

  std::vector<WordIndex> s2 = getVector("Pan");
  std::vector<WordIndex> t2_1 = getVector("Mister");
  std::vector<WordIndex> t2_2 = getVector("Mr");

  // Insert data to phrase table
  tab->incrCountsOfEntry(s1, t1_1, Count(1));
  tab->incrCountsOfEntry(s1, t1_2, Count(2));
  tab->incrCountsOfEntry(s1, t1_3, Count(4));

  tab->incrCountsOfEntry(s2, t2_1, Count(8));
  tab->incrCountsOfEntry(s2, t2_2, Count(16));


  CPPUNIT_ASSERT_EQUAL((size_t) 11, tab->size());

  // Check count values
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1 + 2 + 4, tab->cSrc(s1).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, tab->cTrg(t1_1).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2, tab->cTrg(t1_2).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(4 + 16, tab->cTrg(t1_3).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, tab->cSrcTrg(s1, t1_1).get_c_st(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2, tab->cSrcTrg(s1, t1_2).get_c_st(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(4, tab->cSrcTrg(s1, t1_3).get_c_st(), EPSILON);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(8 + 16, tab->cSrc(s2).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8, tab->cTrg(t2_1).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(4 + 16, tab->cTrg(t2_2).get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8, tab->cSrcTrg(s2, t2_1).get_c_st(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(16, tab->cSrcTrg(s2, t2_2).get_c_st(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::test32bitRange()
{
  /* TEST:
     Check if phrase table supports codes from positive integer range
  */
  tab->clear();

  std::vector<WordIndex> minVector, maxVector;

  minVector.push_back(0);
  maxVector.push_back(0x7FFFFFFE);

  // Insert data to phrase table and check their correctness
  tab->incrCountsOfEntry(minVector, maxVector, Count(20));
  CPPUNIT_ASSERT_EQUAL((size_t) 3, tab->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(20, tab->cSrcTrg(minVector, maxVector).get_c_st(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testByteMax()
{
  /* TEST:
     Check if items with maximum byte values are added correctly
  */
  tab->clear();

  std::vector<WordIndex> s, t;
  s.push_back(201);
  s.push_back(8);
  t.push_back(255);

  // Insert data and check their correctness
  tab->incrCountsOfEntry(s, t, Count(1));
  CPPUNIT_ASSERT_EQUAL((size_t) 3, tab->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, tab->cSrcTrg(s, t).get_c_st(), EPSILON);
}

//---------------------------------------
void _phraseTableTest::testByteMin()
{
  /* TEST:
     Check if items with minimum byte values are added correctly
  */
  tab->clear();

  std::vector<WordIndex> s1, s2, t1, t2;
  // s1
  s1.push_back(4);
  // s2
  s2.push_back(0);
  s2.push_back(1);
  s2.push_back(0);
  // t1
  t1.push_back(0);
  t1.push_back(3);
  // t2
  t2.push_back(0);
  t2.push_back(3);
  t2.push_back(0);

  // Insert data and check their correctness
  tab->incrCountsOfEntry(s1, t1, Count(1));
  tab->incrCountsOfEntry(s2, t2, Count(1));
  CPPUNIT_ASSERT_EQUAL((size_t) 6, tab->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, tab->cSrcTrg(s2, t2).get_c_st(), EPSILON);

  bool found;
  BasePhraseTable::SrcTableNode node;
  found = tab->getEntriesForTarget(t2, node);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT_EQUAL((size_t) 1, node.size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node[s2].first.get_c_s(), EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1, node[s2].second.get_c_s(), EPSILON);
}
