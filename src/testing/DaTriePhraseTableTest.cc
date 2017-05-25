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
/* Module: DaTriePhraseTableTest                                    */
/*                                                                  */
/* Definitions file: DaTriePhraseTableTest.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "DaTriePhraseTableTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DaTriePhraseTableTest );

//--------------- DaTriePhraseTableTest class functions
//

//---------------------------------------
void DaTriePhraseTableTest::setUp()
{
  tab = new DaTriePhraseTable();
}

//---------------------------------------
void DaTriePhraseTableTest::tearDown()
{
  delete tab;
}

//---------------------------------------
Vector<WordIndex> DaTriePhraseTableTest::getVector(string phrase) {
  Vector<WordIndex> v;

  for(unsigned int i = 0; i < phrase.size(); i++) {
    v.push_back(phrase[i]);
  }

  return(v);
}

//---------------------------------------
void DaTriePhraseTableTest::testStoreAndRestore()
{
  Vector<WordIndex> s1 = getVector("Morag city");
  Vector<WordIndex> s2 = getVector("Narie lake");
  Count cs1 = Count(5);
  Count cs2 = Count(2);
  tab->clear();
  tab->addSrcInfo(s1, cs1);
  tab->addSrcInfo(s2, cs2);

  bool found;
  Count s1_count = tab->getSrcInfo(s1, found);
  Count s2_count = tab->getSrcInfo(s2, found);

  CPPUNIT_ASSERT( (int) s1_count.get_c_s() == 5 );
  CPPUNIT_ASSERT( (int) s2_count.get_c_s() == 2 );
}

//---------------------------------------
void DaTriePhraseTableTest::testAddTableEntry()
{
  Vector<WordIndex> s = getVector("Narie lake");
  Vector<WordIndex> t = getVector("jezioro Narie");
  Count s_count = Count(3);
  Count t_count = Count(2);
  PhrasePairInfo ppi(s_count, t_count);

  tab->clear();
  tab->addTableEntry(s, t, ppi);

  CPPUNIT_ASSERT( (int) tab->cSrc(s).get_c_s() == 3 );
  CPPUNIT_ASSERT( (int) tab->cTrg(t).get_c_s() == 2 );
  CPPUNIT_ASSERT( (int) tab->cSrcTrg(s, t).get_c_st() == 2 );
}

//---------------------------------------
void DaTriePhraseTableTest::testIncCountsOfEntry()
{
  Vector<WordIndex> s = getVector("Narie lake");
  Vector<WordIndex> t = getVector("jezioro Narie");
  Count c_init = Count(3);
  Count c = Count(17);

  tab->clear();
  tab->addSrcInfo(s, c_init);
  tab->incrCountsOfEntry(s, t, c);

  CPPUNIT_ASSERT( (int) tab->cSrc(s).get_c_s() == 20 );
  CPPUNIT_ASSERT( (int) tab->cTrg(t).get_c_s() == 17 );
  CPPUNIT_ASSERT( (int) tab->cSrcTrg(s, t).get_c_st() == 17 );
}

//---------------------------------------
void DaTriePhraseTableTest::testGetEntriesForTarget()
{
  DaTriePhraseTable::SrcTableNode node;
  Vector<WordIndex> s1_1 = getVector("Pasleka river");
  Vector<WordIndex> s1_2 = getVector("Pasleka");
  Vector<WordIndex> t1_1 = getVector("rzeka Pasleka");
  Vector<WordIndex> t1_2 = getVector("Pasleka");
  Vector<WordIndex> s2 = getVector("river");
  Vector<WordIndex> t2 = getVector("rzeka");
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
  CPPUNIT_ASSERT( node.size() == 2 );

  // Looking for phrases for which 'Pasleka' is translation
  result = tab->getEntriesForTarget(t1_2, node);
  CPPUNIT_ASSERT( result );
  CPPUNIT_ASSERT( node.size() == 1 );

  // Looking for phrases for which 'rzeka' is translation
  result = tab->getEntriesForTarget(t2, node);
  CPPUNIT_ASSERT( result );
  CPPUNIT_ASSERT( node.size() == 1 );

  // 'xyz'' key shoud not be found
  result = tab->getEntriesForTarget(getVector("xyz"), node);
  CPPUNIT_ASSERT( !result );
}

//---------------------------------------
void DaTriePhraseTableTest::testRetrievingSubphrase()
{
  /* TEST:
     Accessing element with the subphrase should return count 0
  */
  bool found;
  Vector<WordIndex> s = getVector("Hello");
  Vector<WordIndex> t1 = getVector("Buenos Dias");
  Vector<WordIndex> t2 = getVector("Buenos");

  Count c = Count(1);
  
  tab->clear();
  tab->incrCountsOfEntry(s, t1, c);
  c = tab->getSrcTrgInfo(s, t2, found);

  CPPUNIT_ASSERT( !found );
  CPPUNIT_ASSERT( (int) c.get_c_s() == 0);
}

//---------------------------------------
void DaTriePhraseTableTest::testRetrieveNonLeafPhrase()
{
  /* TEST:
     Phrases with count > 0 and not stored in the leaves
     should be also retrieved
  */
  bool found;
  DaTriePhraseTable::SrcTableNode node;
  Vector<WordIndex> s = getVector("Hello");
  Vector<WordIndex> t1 = getVector("Buenos Dias");
  Vector<WordIndex> t2 = getVector("Buenos");

  Count c = Count(1);
  
  tab->clear();
  tab->incrCountsOfEntry(s, t1, c);
  tab->incrCountsOfEntry(s, t2, c);

  // Check phrases and their counts
  // Phrase pair 1
  c = tab->getSrcTrgInfo(s, t1, found);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( (int) c.get_c_s() == 1);
  // Phrase pair 2
  c = tab->getSrcTrgInfo(s, t2, found);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( (int) c.get_c_s() == 1);

  // Looking for phrases for which 'Buenos' is translation
  found = tab->getEntriesForTarget(t2, node);
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( node.size() == 1 );
}

//---------------------------------------
void DaTriePhraseTableTest::testGetEntriesForSource()
{
  /* TEST:
     Find translations for the source phrase
  */
  bool found;
  DaTriePhraseTable::TrgTableNode node;
  Vector<WordIndex> s1 = getVector("jezioro Narie");
  Vector<WordIndex> t1_1 = getVector("Narie lake");
  Vector<WordIndex> t1_2 = getVector("Narie");
  Vector<WordIndex> s2 = getVector("jezioro Skiertag");
  Vector<WordIndex> t2_1 = getVector("Skiertag");
  Vector<WordIndex> s3 = getVector("jezioro Jeziorak");
  Vector<WordIndex> t3_1 = getVector("Jeziorak lake");
  Vector<WordIndex> t3_2 = getVector("Jeziorak");

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
  cout << node.size();  // TODO: Remove line
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( node.size() == 2 );
  // Skiertag phrases
  found = tab->getEntriesForSource(s2, node);
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( node.size() == 1 );
  // Jeziorak phrases
  found = tab->getEntriesForSource(s3, node);
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( node.size() == 2 );
}

//---------------------------------------
void DaTriePhraseTableTest::testRetrievingEntriesWithCountEqualZero()
{
  /* TEST:
     Function getEntriesForTarget for retrieving entries should skip
     elements with count equals 0
  */
  bool found;
  DaTriePhraseTable::SrcTableNode node;
  Vector<WordIndex> s1 = getVector("Palac Dohnow");
  Vector<WordIndex> s2 = getVector("Palac Dohnow w Moragu");
  Vector<WordIndex> t = getVector("Dohn's Palace");
  
  tab->clear();
  tab->incrCountsOfEntry(s1, t, Count(1));
  tab->incrCountsOfEntry(s2, t, Count(0));

  found = tab->getEntriesForTarget(t, node);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( node.size() == 1 );
}

//---------------------------------------
void DaTriePhraseTableTest::testGetNbestForTrg()
{
  /* TEST:
     Check if method getNbestForTrg returns correct elements
  */
  bool found;
  NbestTableNode<PhraseTransTableNodeData> node;
  NbestTableNode<PhraseTransTableNodeData>::iterator iter;

  // Fill trie structure with data
  Vector<WordIndex> s1 = getVector("city hall");
  Vector<WordIndex> s2 = getVector("city hall in Morag");
  Vector<WordIndex> s3 = getVector("town hall");
  Vector<WordIndex> s4 = getVector("town hall in Morag");
  Vector<WordIndex> t = getVector("ratusz miejski w Moragu");
  
  tab->clear();
  tab->incrCountsOfEntry(s1, t, Count(4));
  tab->incrCountsOfEntry(s2, t, Count(2));
  tab->incrCountsOfEntry(s3, t, Count(3));
  tab->incrCountsOfEntry(s4, t, Count(0));

  // Returned elements should not exceed number of elements
  // in the structure
  found = tab->getNbestForTrg(t, node, 10);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( node.size() == 3 );

  // If there are more available elements, only elements
  // with the highest score should be returned
  found = tab->getNbestForTrg(t, node, 2);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( node.size() == 2 );

  iter = node.begin();
  CPPUNIT_ASSERT( iter->second == s1 );
  iter++;
  CPPUNIT_ASSERT( iter->second == s3 );
}

//---------------------------------------
void DaTriePhraseTableTest::testAddSrcTrgInfo()
{
  /* TEST:
     Check if two keys were added (for (s, t) and (t, s) vectors)
     and if their values are the same
  */
  bool found;

  Vector<WordIndex> s = getVector("jezioro Skiertag");
  Vector<WordIndex> t = getVector("Skiertag lake");
  
  tab->clear();
  tab->addSrcTrgInfo(s, t, Count(1));

  Count src_trg_count = tab->cSrcTrg(s, t);
  Count trg_src_count = tab->getInfo(tab->getTrgSrc(s, t), found);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( (int) src_trg_count.get_c_s() == 1 );
  CPPUNIT_ASSERT( (int) src_trg_count.get_c_s() == (int) trg_src_count.get_c_s() );
}

//---------------------------------------
void DaTriePhraseTableTest::testIteratorsLoop()
{
  /* TEST:
     Check basic implementation of iterators - functions
     begin(), end() and operators (++ postfix, *).
  */
  Vector<WordIndex> s = getVector("jezioro Skiertag");
  Vector<WordIndex> t = getVector("Skiertag lake");
  
  tab->clear();
  tab->incrCountsOfEntry(s, t, Count(1));

  CPPUNIT_ASSERT(tab->begin() != tab->end());
  CPPUNIT_ASSERT(tab->begin() != tab->begin());

  int i = 0;
  const int MAX_ITER = 10;

  for(DaTriePhraseTable::const_iterator iter = tab->begin(); iter != tab->end() && i < MAX_ITER; iter++, i++)
  {
    pair<Vector<WordIndex>, int> x = *iter;
    if (i == 0)
      CPPUNIT_ASSERT( x.first == tab->getSrc(s) );
    else if (i == 1)
      CPPUNIT_ASSERT( x.first == t );
    else if (i == 2)
      CPPUNIT_ASSERT( x.first == tab->getTrgSrc(s, t) );
    
    CPPUNIT_ASSERT( x.second == 1);   
  }

  CPPUNIT_ASSERT( i == 3 );
}

//---------------------------------------
void DaTriePhraseTableTest::testIteratorsOperatorsPlusPlusStar()
{
  /* TEST:
     Check basic implementation of iterators - function
     begin() and operators (++ prefix, ++ postfix, *, ->).
  */
  bool found;

  Vector<WordIndex> s = getVector("zamek krzyzacki w Malborku");
  Vector<WordIndex> t = getVector("teutonic castle in Malbork");
  
  tab->clear();
  tab->incrCountsOfEntry(s, t, Count(2));

  // First element s - should be found
  DaTriePhraseTable::const_iterator iter = tab->begin();
  pair<Vector<WordIndex>, int> x = *iter;
  CPPUNIT_ASSERT( x.first == tab->getSrc(s) );
  CPPUNIT_ASSERT( x.second == 2);
  
  // Second element (s, t) - should be found
  //found = ++iter;
  //CPPUNIT_ASSERT( found );
  //x = *iter;
  //CPPUNIT_ASSERT( x.first == tab->getSrcTrg(s, t) );
  //CPPUNIT_ASSERT( x.second == 2);

  // Third element (t) - should be found
  found = (iter++);
  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( iter->first == t );
  CPPUNIT_ASSERT( iter->second == 2);
  
  // Fourth element (t, s) - should be found
  found = ++iter;
  CPPUNIT_ASSERT( found );
  x = *iter;
  CPPUNIT_ASSERT( x.first == tab->getTrgSrc(s, t) );
  CPPUNIT_ASSERT( x.second == 2);

  // Fifth element - should not be found
  found = (iter++);
  CPPUNIT_ASSERT( !found );
}

//---------------------------------------
void DaTriePhraseTableTest::testIteratorsOperatorsEqualNotEqual()
{
  /* TEST:
     Check basic implementation of iterators - operators == and !=
  */
  Vector<WordIndex> s = getVector("kemping w Kretowinach");
  Vector<WordIndex> t = getVector("camping Kretowiny");
  
  tab->clear();
  tab->incrCountsOfEntry(s, t, Count(1));

  DaTriePhraseTable::const_iterator iter1 = tab->begin();
  iter1++;
  DaTriePhraseTable::const_iterator iter2 = tab->begin();
  
  CPPUNIT_ASSERT( iter1 == iter1 );
  CPPUNIT_ASSERT( !(iter1 != iter1) );
  CPPUNIT_ASSERT( !(iter1 == iter2) );
  CPPUNIT_ASSERT( iter1 != iter2 );
}

//---------------------------------------
void DaTriePhraseTableTest::testSize()
{
  /* TEST:
     Check if number of elements in trie is returned correctly
  */
  tab->clear();
  CPPUNIT_ASSERT( tab->size() == 0 );  // Collection after cleaning should be empty
  
  // Fill trie structure with data
  tab->incrCountsOfEntry(getVector("kemping w Kretowinach"), getVector("camping Kretowiny"), Count(1));
  tab->incrCountsOfEntry(getVector("kemping w Kretowinach"), getVector("camping in Kretowiny"), Count(2));

  CPPUNIT_ASSERT( tab->size() == 5 );

  tab->clear();
  CPPUNIT_ASSERT( tab->size() == 0 );  // Collection after cleaning should be empty

  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mr Car"), Count(1));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Pan Samochodzik"), Count(4));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mister Automobile"), Count(20));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mr Automobile"), Count(24));

  CPPUNIT_ASSERT( tab->size() == 9 );

  tab->incrCountsOfEntry(getVector("Pierwsza przygoda Pana Samochodzika"),
                         getVector("First Adventure of Mister Automobile"), Count(5));
  tab->incrCountsOfEntry(getVector("Pierwsza przygoda Pana Samochodzika"),
                         getVector("First Adventure of Pan Samochodzik"), Count(7));


  CPPUNIT_ASSERT( tab->size() == 9 + 5 );

}

//---------------------------------------
void DaTriePhraseTableTest::testSavingAndRestoringTrie()
{
  /* TEST:
     Check saving and restoring trie structure on disk
  */
  bool result;
  
  // Fill trie structure with data
  tab->clear();
  tab->incrCountsOfEntry(getVector("kemping w Kretowinach"), getVector("camping Kretowiny"), Count(1));
  tab->incrCountsOfEntry(getVector("kemping w Kretowinach"), getVector("camping in Kretowiny"), Count(2));

  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mr Car"), Count(1));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Pan Samochodzik"), Count(4));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mister Automobile"), Count(20));
  tab->incrCountsOfEntry(getVector("Pan Samochodzik"), getVector("Mr Automobile"), Count(24));

  tab->incrCountsOfEntry(getVector("Pierwsza przygoda Pana Samochodzika"),
                         getVector("First Adventure of Mister Automobile"), Count(5));
  tab->incrCountsOfEntry(getVector("Pierwsza przygoda Pana Samochodzika"),
                         getVector("First Adventure of Pan Samochodzik"), Count(7));

  unsigned int original_size = tab->size();
  // Save structue on disk
  const char* file_name = tmpnam(NULL);

  // Save structure
  result = tab->save(file_name);
  CPPUNIT_ASSERT( result );

  tab->clear();  // Remove structure to make sure that loading trie was performed

  CPPUNIT_ASSERT( tab->size() == 0 );  // Collection after cleaning should be empty
  
  // Load structure
  result = tab->load(file_name);
  CPPUNIT_ASSERT( result );
  CPPUNIT_ASSERT( tab->size() == original_size );

  tab->clear();  // Unmap loaded file
}

//---------------------------------------
void DaTriePhraseTableTest::testMmap()
{
  /* TEST:
     Check if the trie restored from disk with mmap
     contains all stored items and correct counts
  */
  bool result;
  
  // Fill trie structure with data
  tab->clear();

  // Define vectors
  Vector<WordIndex> s1 = getVector("Pan Samochodzik");
  Vector<WordIndex> t1_1 = getVector("Mr Car");
  Vector<WordIndex> t1_2 = getVector("Pan Samochodzik");
  Vector<WordIndex> t1_3 = getVector("Mister Automobile");
  Vector<WordIndex> t1_4 = getVector("Mr Automobile");

  Vector<WordIndex> s2 = getVector("Pan Samochodzik i templariusze");
  Vector<WordIndex> t2_1 = getVector("Mister Automobile and the Knights Templar");
  Vector<WordIndex> t2_2 = getVector("Pan Samochodzik and the Knights Templar");

  Vector<WordIndex> s3 = getVector("Pan Samochodzik i niesamowity dwor");
  Vector<WordIndex> t3_1 = getVector("Mister Automobile and the Unearthly Mansion");
  Vector<WordIndex> t3_2 = getVector("Pan Samochodzik and the Unearthly Mansion");

  // Insert data to trie
  tab->incrCountsOfEntry(s1, t1_1, Count(1));
  tab->incrCountsOfEntry(s1, t1_2, Count(2));
  tab->incrCountsOfEntry(s1, t1_3, Count(4));
  tab->incrCountsOfEntry(s1, t1_4, Count(8));

  tab->incrCountsOfEntry(s2, t2_1, Count(16));
  tab->incrCountsOfEntry(s2, t2_2, Count(32));

  tab->incrCountsOfEntry(s3, t3_1, Count(64));
  tab->incrCountsOfEntry(s3, t3_2, Count(128));

  unsigned int original_size = tab->size();
  // Save structue on disk
  const char* file_name = tmpnam(NULL);

  // Save structure
  result = tab->save(file_name);
  CPPUNIT_ASSERT( result );

  tab->clear();  // Remove structure to make sure that loading trie was performed

  CPPUNIT_ASSERT( tab->size() == 0 );  // Collection after cleaning should be empty
  
  // Load structure
  result = tab->load(file_name);
  CPPUNIT_ASSERT( result );
  CPPUNIT_ASSERT( tab->size() == original_size );

  // Check count values
  CPPUNIT_ASSERT( tab->cSrc(s1).get_c_s() == 1 + 2 + 4 + 8 );
  CPPUNIT_ASSERT( tab->cTrg(t1_1).get_c_s() == 1 );
  CPPUNIT_ASSERT( tab->cTrg(t1_2).get_c_s() == 2 );
  CPPUNIT_ASSERT( tab->cTrg(t1_3).get_c_s() == 4 );
  CPPUNIT_ASSERT( tab->cTrg(t1_4).get_c_s() == 8 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s1, t1_1).get_c_st() == 1 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s1, t1_2).get_c_st() == 2 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s1, t1_3).get_c_st() == 4 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s1, t1_4).get_c_st() == 8 );

  CPPUNIT_ASSERT( tab->cSrc(s2).get_c_s() == 16 + 32 );
  CPPUNIT_ASSERT( tab->cTrg(t2_1).get_c_s() == 16 );
  CPPUNIT_ASSERT( tab->cTrg(t2_2).get_c_s() == 32 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s2, t2_1).get_c_st() == 16 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s2, t2_2).get_c_st() == 32 );

  CPPUNIT_ASSERT( tab->cSrc(s3).get_c_s() == 64 + 128 );
  CPPUNIT_ASSERT( tab->cTrg(t3_1).get_c_s() == 64 );
  CPPUNIT_ASSERT( tab->cTrg(t3_2).get_c_s() == 128 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s3, t3_1).get_c_st() == 64 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s3, t3_2).get_c_st() == 128 );

  tab->clear();  // Unmap loaded file
  CPPUNIT_ASSERT( tab->size() == 0 );
}

//---------------------------------------
void DaTriePhraseTableTest::testSubkeys()
{
  /* TEST:
     Check if subkeys are stored correctly
  */
 
  // Fill trie structure with data
  tab->clear();

  // Define vectors
  Vector<WordIndex> s1 = getVector("Pan Samochodzik");
  Vector<WordIndex> t1_1 = getVector("Mr Car");
  Vector<WordIndex> t1_2 = getVector("Pan");
  Vector<WordIndex> t1_3 = getVector("Mr");

  Vector<WordIndex> s2 = getVector("Pan");
  Vector<WordIndex> t2_1 = getVector("Mister");
  Vector<WordIndex> t2_2 = getVector("Mr");

  // Insert data to trie
  tab->incrCountsOfEntry(s1, t1_1, Count(1));
  tab->incrCountsOfEntry(s1, t1_2, Count(2));
  tab->incrCountsOfEntry(s1, t1_3, Count(4));

  tab->incrCountsOfEntry(s2, t2_1, Count(8));
  tab->incrCountsOfEntry(s2, t2_2, Count(16));
  

  CPPUNIT_ASSERT( tab->size() == 11 );

  // Check count values
  CPPUNIT_ASSERT( tab->cSrc(s1).get_c_s() == 1 + 2 + 4 );
  CPPUNIT_ASSERT( tab->cTrg(t1_1).get_c_s() == 1 );
  CPPUNIT_ASSERT( tab->cTrg(t1_2).get_c_s() == 2 );
  CPPUNIT_ASSERT( tab->cTrg(t1_3).get_c_s() == 4 + 16 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s1, t1_1).get_c_st() == 1 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s1, t1_2).get_c_st() == 2 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s1, t1_3).get_c_st() == 4 );

  CPPUNIT_ASSERT( tab->cSrc(s2).get_c_s() == 8 + 16 );
  CPPUNIT_ASSERT( tab->cTrg(t2_1).get_c_s() == 8 );
  CPPUNIT_ASSERT( tab->cTrg(t2_2).get_c_s() == 4 + 16 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s2, t2_1).get_c_st() == 8 );
  CPPUNIT_ASSERT( tab->cSrcTrg(s2, t2_2).get_c_st() == 16 );
}

//---------------------------------------
void DaTriePhraseTableTest::test32bitRange()
{
  /* TEST:
     Check if datrie supports codes from 32bit range
     (except 0xFFFFFFFF as it is reserved for internal usage)
  */
  tab->clear();

  Vector<WordIndex> minVector, maxVector;

  minVector.push_back(0);
  maxVector.push_back(0xFFFFFFFE);

  // Insert data to trie and check their correctness
  tab->incrCountsOfEntry(minVector, maxVector, Count(20));
  CPPUNIT_ASSERT( tab->size() == 3 );
  CPPUNIT_ASSERT( (int) tab->cSrcTrg(minVector, maxVector).get_c_st() == 20 );
}