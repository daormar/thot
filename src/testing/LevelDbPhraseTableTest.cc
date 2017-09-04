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

/********************************************************************/
/*                                                                  */
/* Module: LevelDbPhraseTableTest                                   */
/*                                                                  */
/* Definitions file: LevelDbPhraseTableTest.cc                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "LevelDbPhraseTableTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( LevelDbPhraseTableTest );

//--------------- LevelDbPhraseTableTest class functions
//

//---------------------------------------
void LevelDbPhraseTableTest::setUp()
{
  tabLdb = new LevelDbPhraseTable();
  tab = tabLdb;

  tabLdb->init(dbName);
}

//---------------------------------------
void LevelDbPhraseTableTest::tearDown()
{
  tabLdb->drop();
  delete tabLdb;  // Removes also tab
}

//---------------------------------------
void LevelDbPhraseTableTest::testKeyVectorConversion()
{
  std::vector<WordIndex> s;
  s.push_back(3);
  s.push_back(4);
  s.push_back(112175);
  s.push_back(90664);
  s.push_back(143);
  s.push_back(749);
  s.push_back(748);

  CPPUNIT_ASSERT(tabLdb->keyToVector(tabLdb->vectorToKey(s)) == s );
}

//---------------------------------------
void LevelDbPhraseTableTest::testAddSrcTrgInfo()
{
  /* TEST:
     Check if two keys were added (for (s, t) and (t, s) vectors)
     and if their values are the same
  */
  bool found;

  std::vector<WordIndex> s = getVector("jezioro Skiertag");
  std::vector<WordIndex> t = getVector("Skiertag lake");
  
  tab->clear();
  tab->addSrcTrgInfo(s, t, Count(1));

  Count src_trg_count = tab->cSrcTrg(s, t);
  Count trg_src_count = tabLdb->getInfo(tabLdb->getTrgSrc(s, t), found);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( (int) src_trg_count.get_c_s() == 1 );
  CPPUNIT_ASSERT( (int) src_trg_count.get_c_s() == (int) trg_src_count.get_c_s() );
}

//---------------------------------------
void LevelDbPhraseTableTest::testIteratorsLoop()
{
  /* TEST:
     Check basic implementation of iterators - functions
     begin(), end() and operators (++ postfix, *).
  */
  std::vector<WordIndex> s = getVector("jezioro Skiertag");
  std::vector<WordIndex> t = getVector("Skiertag lake");
  
  tab->clear();
  tab->incrCountsOfEntry(s, t, Count(1));

  CPPUNIT_ASSERT(tabLdb->begin() != tabLdb->end());
  CPPUNIT_ASSERT(tabLdb->begin() != tabLdb->begin());

  int i = 0;
  const int MAX_ITER = 10;

  // Construct dictionary to record results returned by iterator
  // Dictionary structure: (key, (total count value, number of occurences))
  map<std::vector<WordIndex>, pair<int, int> > d;
  d[tabLdb->getSrc(s)] = make_pair(0, 0);
  d[t] = make_pair(0, 0);
  d[tabLdb->getTrgSrc(s, t)] = make_pair(0, 0);

  for(LevelDbPhraseTable::const_iterator iter = tabLdb->begin();
      iter != tabLdb->end() && i < MAX_ITER;
      iter++, i++)
  {
    pair<std::vector<WordIndex>, int> x = *iter;
    d[x.first].first += x.second;
    d[x.first].second++;  
  }

  // Check if element returned by iterator is correct
  CPPUNIT_ASSERT(d.size() == 3);
  CPPUNIT_ASSERT(d[tabLdb->getSrc(s)].first == 1);
  CPPUNIT_ASSERT(d[tabLdb->getSrc(s)].second == 1);
  CPPUNIT_ASSERT(d[t].first == 1);
  CPPUNIT_ASSERT(d[t].second == 1);
  CPPUNIT_ASSERT(d[tabLdb->getTrgSrc(s, t)].first == 1);
  CPPUNIT_ASSERT(d[tabLdb->getTrgSrc(s, t)].second == 1);

  CPPUNIT_ASSERT( i == 3 );
}

//---------------------------------------
void LevelDbPhraseTableTest::testIteratorsOperatorsPlusPlusStar()
{
  /* TEST:
     Check basic implementation of iterators - function
     begin() and operators (++ prefix, ++ postfix, *, ->).
  */
  bool found = true;

  std::vector<WordIndex> s = getVector("zamek krzyzacki w Malborku");
  std::vector<WordIndex> t = getVector("teutonic castle in Malbork");
  
  tab->clear();
  tab->incrCountsOfEntry(s, t, Count(2));

  // Construct dictionary to record results returned by iterator
  // Dictionary structure: (key, (total count value, number of occurences))
  map<std::vector<WordIndex>, pair<int, int> > d;
  d[tabLdb->getSrc(s)] = make_pair(0, 0);
  d[t] = make_pair(0, 0);
  d[tabLdb->getTrgSrc(s, t)] = make_pair(0, 0);
 
  for(LevelDbPhraseTable::const_iterator iter = tabLdb->begin();
      iter != tabLdb->end();
      found = (iter++))
  {
    CPPUNIT_ASSERT( found );
    pair<std::vector<WordIndex>, int> x = *iter;
    d[x.first].first += x.second;
    d[x.first].second++;
  }

  // Iterating beyond the last element should return FALSE value
  CPPUNIT_ASSERT( !found );

  // Check if element returned by iterator is correct
  CPPUNIT_ASSERT(d.size() == 3);
  CPPUNIT_ASSERT(d[tabLdb->getSrc(s)].first == 2);
  CPPUNIT_ASSERT(d[tabLdb->getSrc(s)].second == 1);
  CPPUNIT_ASSERT(d[t].first == 2);
  CPPUNIT_ASSERT(d[t].second == 1);
  CPPUNIT_ASSERT(d[tabLdb->getTrgSrc(s, t)].first == 2);
  CPPUNIT_ASSERT(d[tabLdb->getTrgSrc(s, t)].second == 1);
}

//---------------------------------------
void LevelDbPhraseTableTest::testIteratorsOperatorsEqualNotEqual()
{
  /* TEST:
     Check basic implementation of iterators - operators == and !=
  */
  std::vector<WordIndex> s = getVector("kemping w Kretowinach");
  std::vector<WordIndex> t = getVector("camping Kretowiny");
  
  tab->clear();
  tab->incrCountsOfEntry(s, t, Count(1));

  LevelDbPhraseTable::const_iterator iter1 = tabLdb->begin();
  iter1++;
  LevelDbPhraseTable::const_iterator iter2 = tabLdb->begin();
  
  CPPUNIT_ASSERT( iter1 == iter1 );
  CPPUNIT_ASSERT( !(iter1 != iter1) );
  CPPUNIT_ASSERT( !(iter1 == iter2) );
  CPPUNIT_ASSERT( iter1 != iter2 );
}

//---------------------------------------
void LevelDbPhraseTableTest::testLoadingLevelDb()
{
  /* TEST:
     Check restoring levelDB from disk
  */
  bool result;
  
  // Fill leveldb with data
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
   
  // Load structure
  result = tabLdb->load(dbName);
  CPPUNIT_ASSERT( result == THOT_OK);
  CPPUNIT_ASSERT( tab->size() == original_size );
}

//---------------------------------------
void LevelDbPhraseTableTest::testLoadedDataCorrectness()
{
  /* TEST:
     Check if the data restored from disk
     contains all stored items and correct counts
  */
  bool result;
  
  // Fill levelDB with data
  tab->clear();

  // Define vectors
  std::vector<WordIndex> s1 = getVector("Pan Samochodzik");
  std::vector<WordIndex> t1_1 = getVector("Mr Car");
  std::vector<WordIndex> t1_2 = getVector("Pan Samochodzik");
  std::vector<WordIndex> t1_3 = getVector("Mister Automobile");
  std::vector<WordIndex> t1_4 = getVector("Mr Automobile");

  std::vector<WordIndex> s2 = getVector("Pan Samochodzik i templariusze");
  std::vector<WordIndex> t2_1 = getVector("Mister Automobile and the Knights Templar");
  std::vector<WordIndex> t2_2 = getVector("Pan Samochodzik and the Knights Templar");

  std::vector<WordIndex> s3 = getVector("Pan Samochodzik i niesamowity dwor");
  std::vector<WordIndex> t3_1 = getVector("Mister Automobile and the Unearthly Mansion");
  std::vector<WordIndex> t3_2 = getVector("Pan Samochodzik and the Unearthly Mansion");

  // Insert data to levelDB
  tab->incrCountsOfEntry(s1, t1_1, Count(1));
  tab->incrCountsOfEntry(s1, t1_2, Count(2));
  tab->incrCountsOfEntry(s1, t1_3, Count(4));
  tab->incrCountsOfEntry(s1, t1_4, Count(8));

  tab->incrCountsOfEntry(s2, t2_1, Count(16));
  tab->incrCountsOfEntry(s2, t2_2, Count(32));

  tab->incrCountsOfEntry(s3, t3_1, Count(64));
  tab->incrCountsOfEntry(s3, t3_2, Count(128));

  unsigned int original_size = tab->size();
   
  // Load structure
  result = tabLdb->load(dbName);
  CPPUNIT_ASSERT( result == THOT_OK );
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

  tab->clear();  // Remove data
  CPPUNIT_ASSERT( tab->size() == 0 );
}
