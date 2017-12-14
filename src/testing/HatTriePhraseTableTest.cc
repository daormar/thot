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
/* Module: HatTriePhraseTableTest                                   */
/*                                                                  */
/* Definitions file: HatTriePhraseTableTest.cc                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "HatTriePhraseTableTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( HatTriePhraseTableTest );

//--------------- HatTriePhraseTableTest class functions

//---------------------------------------
void HatTriePhraseTableTest::setUp()
{
    tabHatTrie = new HatTriePhraseTable();
    tab = tabHatTrie;
}

//---------------------------------------
void HatTriePhraseTableTest::tearDown()
{
    delete tabHatTrie;  // Removes also tab
}

//---------------------------------------
void HatTriePhraseTableTest::testAddSrcTrgInfo()
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
    Count trg_src_count = tabHatTrie->getSrcTrgInfo(s, t, found);

    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1, src_trg_count.get_c_s(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(src_trg_count.get_c_s(), trg_src_count.get_c_s(), EPSILON);
}

//---------------------------------------
void HatTriePhraseTableTest::testIteratorsLoop()
{
    /* TEST:
       Check basic implementation of iterators - functions
       begin(), end() and operators (++ postfix, *).

       Note that iterator returns only target elements.
    */
    std::vector<WordIndex> s1 = getVector("Uniwersytet Gdanski");
    std::vector<WordIndex> t1 = getVector("Gdansk University");
    std::vector<WordIndex> s2 = getVector("Politechnika Gdanska");
    std::vector<WordIndex> t2 = getVector("Gdansk University of Technology");
    std::vector<WordIndex> empty;

    tab->clear();
    tab->incrCountsOfEntry(s1, t1, Count(40));
    tab->incrCountsOfEntry(s2, t2, Count(60));
    tab->incrCountsOfEntry(s1, t2, Count(0));
    tab->incrCountsOfEntry(s2, t1, Count(1));

    CPPUNIT_ASSERT(tabHatTrie->begin() != tabHatTrie->end());
    CPPUNIT_ASSERT(tabHatTrie->begin() == tabHatTrie->begin());

    int i = 0;
    const int MAX_ITER = 10;

    // Construct dictionary to record results returned by iterator
    // Dictionary structure: (key, (total count value, number of occurences))
    std::map<std::vector<WordIndex>, std::pair<Count, int> > d;
    d[t1] = std::make_pair(0, 0);
    d[t2] = std::make_pair(0, 0);

    for(HatTriePhraseTable::const_iterator iter = tabHatTrie->begin();
        iter != tabHatTrie->end() && i < MAX_ITER;
        iter++, i++)
    {
        HatTriePhraseTable::PhraseInfoElement x = *iter;

        CPPUNIT_ASSERT_MESSAGE("Phrase returned by iterator is not the one of expected targets",
                               x.first == t1 || x.first == t2);

        d[x.first].first += x.second;
        d[x.first].second++;
    }

    // Check if element returned by iterator is correct
    CPPUNIT_ASSERT_DOUBLES_EQUAL(41, d[t1].first.get_c_s(), EPSILON);
    CPPUNIT_ASSERT_EQUAL(1, d[t1].second);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(60, d[t2].first.get_c_s(), EPSILON);
    CPPUNIT_ASSERT_EQUAL(1, d[t2].second);

    CPPUNIT_ASSERT_EQUAL(2, i);
}

//---------------------------------------
void HatTriePhraseTableTest::testIteratorsOperatorsPlusPlusStar()
{
    /* TEST:
      Check basic implementation of iterators - function
      begin() and operators (++ prefix, ++ postfix, *, ->).

      Note that iterator returns only target elements.
    */
    bool found;

    std::vector<WordIndex> s1 = getVector("Uniwersytet Gdanski");
    std::vector<WordIndex> t1 = getVector("Gdansk University");
    std::vector<WordIndex> s2 = getVector("Politechnika Gdanska");
    std::vector<WordIndex> t2 = getVector("Gdansk University of Technology");
    std::vector<WordIndex> s3 = getVector("Gdanski Uniwersytet Medyczny");
    std::vector<WordIndex> t3 = getVector("Gdansk Medical University");
    std::vector<WordIndex> empty;
  
    tab->clear();
    tab->incrCountsOfEntry(s1, t1, Count(40));
    tab->incrCountsOfEntry(s2, t2, Count(60));
    tab->incrCountsOfEntry(s1, t2, Count(0));
    tab->incrCountsOfEntry(s2, t1, Count(1));
    tab->incrCountsOfEntry(s3, t3, Count(50));

    // Check correctness of iterators
    CPPUNIT_ASSERT(tabHatTrie->begin() != tabHatTrie->end());
    CPPUNIT_ASSERT(tabHatTrie->begin() == tabHatTrie->begin());

    // Check if the results returned by iterator are correct
    // and operators work as expected
    HatTriePhraseTable::const_iterator iter = tabHatTrie->begin();
    CPPUNIT_ASSERT(t2 == iter->first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(60, iter->second.get_c_s(), EPSILON);

    found = ++iter;
    CPPUNIT_ASSERT(found);
    CPPUNIT_ASSERT(t1 == (*iter).first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(41, (*iter).second.get_c_s(), EPSILON);

    found = (iter++);
    CPPUNIT_ASSERT(found);
    CPPUNIT_ASSERT(t3 == (*iter).first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(50, (*iter).second.get_c_s(), EPSILON);

    found = ++iter;
    CPPUNIT_ASSERT(!found);
    CPPUNIT_ASSERT(iter == tabHatTrie->end());
}

//---------------------------------------
void HatTriePhraseTableTest::testIteratorsOperatorsEqualNotEqual()
{
    /* TEST:
      Check basic implementation of iterators - operators == and !=

      Note that iterator returns only target elements.
    */
    std::vector<WordIndex> s = getVector("kemping w Kretowinach");
    std::vector<WordIndex> t = getVector("camping Kretowiny");

    tab->clear();
    tab->incrCountsOfEntry(s, t, Count(1));

    HatTriePhraseTable::const_iterator iter1 = tabHatTrie->begin();
    iter1++;
    HatTriePhraseTable::const_iterator iter2 = tabHatTrie->begin();

    CPPUNIT_ASSERT( iter1 == iter1 );
    CPPUNIT_ASSERT( !(iter1 != iter1) );
    CPPUNIT_ASSERT( !(iter1 == iter2) );
    CPPUNIT_ASSERT( iter1 != iter2 );
}
