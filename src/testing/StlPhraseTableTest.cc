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
/* Module: StlPhraseTableTest                                       */
/*                                                                  */
/* Definitions file: StlPhraseTableTest.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "StlPhraseTableTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( StlPhraseTableTest );

//--------------- StlPhraseTableTest class functions

//---------------------------------------
void StlPhraseTableTest::setUp()
{
    tabStl = new StlPhraseTable();
    tab = tabStl;
}

//---------------------------------------
void StlPhraseTableTest::tearDown()
{
    delete tabStl;  // Removes also tab
}

//---------------------------------------
std::vector<WordIndex> StlPhraseTableTest::getTrgSrc(std::vector<WordIndex>& s,
                                                     std::vector<WordIndex>& t)
{
    std::vector<WordIndex> ts = t;
    ts.push_back(UNUSED_WORD);
    ts.insert(ts.end(), s.begin(), s.end());

    return ts;
}

//---------------------------------------
void StlPhraseTableTest::testAddSrcTrgInfo()
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
  Count trg_src_count = tabStl->getSrcTrgInfo(s, t, found);

  CPPUNIT_ASSERT( found );
  CPPUNIT_ASSERT( (int) src_trg_count.get_c_s() == 1 );
  CPPUNIT_ASSERT( (int) src_trg_count.get_c_s() == (int) trg_src_count.get_c_s() );
}

//---------------------------------------
void StlPhraseTableTest::testIteratorsLoop()
{
  /* TEST:
     Check basic implementation of iterators - functions
     begin(), end() and operators (++ postfix, *).
  */
  std::vector<WordIndex> s = getVector("jezioro Skiertag");
  std::vector<WordIndex> t = getVector("Skiertag lake");
  
  tab->clear();
  tab->incrCountsOfEntry(s, t, Count(1));

  CPPUNIT_ASSERT(tabStl->begin() != tabStl->end());
  CPPUNIT_ASSERT(tabStl->begin() != tabStl->begin());

  int i = 0;
  const int MAX_ITER = 10;

  // Construct dictionary to record results returned by iterator
  // Dictionary structure: (key, (total count value, number of occurences))
  map<std::vector<WordIndex>, pair<int, int> > d;
  d[s] = make_pair(0, 0);
  d[t] = make_pair(0, 0);
  d[getTrgSrc(s, t)] = make_pair(0, 0);

  for(StlPhraseTable::const_iterator iter = tabStl->begin();
      iter != tabStl->end() && i < MAX_ITER;
      iter++, i++)
  {
    pair<std::vector<WordIndex>, int> x = *iter;
    d[x.first].first += x.second;
    d[x.first].second++;  
  }

  // Check if element returned by iterator is correct
  CPPUNIT_ASSERT(d.size() == 3);
  CPPUNIT_ASSERT(d[s].first == 1);
  CPPUNIT_ASSERT(d[s].second == 1);
  CPPUNIT_ASSERT(d[t].first == 1);
  CPPUNIT_ASSERT(d[t].second == 1);
  CPPUNIT_ASSERT(d[getTrgSrc(s, t)].first == 1);
  CPPUNIT_ASSERT(d[getTrgSrc(s, t)].second == 1);

  CPPUNIT_ASSERT( i == 3 );
}

//---------------------------------------
void StlPhraseTableTest::testIteratorsOperatorsPlusPlusStar()
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
  d[s] = make_pair(0, 0);
  d[t] = make_pair(0, 0);
  d[getTrgSrc(s, t)] = make_pair(0, 0);
 
  for(StlPhraseTable::const_iterator iter = tabStl->begin();
      iter != tabStl->end();
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
  CPPUNIT_ASSERT(d[s].first == 2);
  CPPUNIT_ASSERT(d[s].second == 1);
  CPPUNIT_ASSERT(d[t].first == 2);
  CPPUNIT_ASSERT(d[t].second == 1);
  CPPUNIT_ASSERT(d[getTrgSrc(s, t)].first == 2);
  CPPUNIT_ASSERT(d[getTrgSrc(s, t)].second == 1);
}

//---------------------------------------
void StlPhraseTableTest::testIteratorsOperatorsEqualNotEqual()
{
  /* TEST:
     Check basic implementation of iterators - operators == and !=
  */
  std::vector<WordIndex> s = getVector("kemping w Kretowinach");
  std::vector<WordIndex> t = getVector("camping Kretowiny");
  
  tab->clear();
  tab->incrCountsOfEntry(s, t, Count(1));

  StlPhraseTable::const_iterator iter1 = tabStl->begin();
  iter1++;
  StlPhraseTable::const_iterator iter2 = tabStl->begin();
  
  CPPUNIT_ASSERT( iter1 == iter1 );
  CPPUNIT_ASSERT( !(iter1 != iter1) );
  CPPUNIT_ASSERT( !(iter1 == iter2) );
  CPPUNIT_ASSERT( iter1 != iter2 );
}
