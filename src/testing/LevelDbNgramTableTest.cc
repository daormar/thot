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
/* Module: LevelDbNgramTableTest                                    */
/*                                                                  */
/* Definitions file: LevelDbNgramTableTest.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "LevelDbNgramTableTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( LevelDbNgramTableTest );

//--------------- LevelDbNgramTableTest class functions
//

//---------------------------------------
void LevelDbNgramTableTest::setUp()
{
    tab = new LevelDbNgramTable();
    tab->init(dbName);
}

//---------------------------------------
void LevelDbNgramTableTest::tearDown()
{
    tab->drop();
    delete tab;
}

//---------------------------------------
std::vector<WordIndex> LevelDbNgramTableTest::getVector(std::string phrase) {
    std::vector<WordIndex> v;

    for(unsigned int i = 0; i < phrase.size(); i++) {
        v.push_back(phrase[i]);
    }

    return(v);
}

//---------------------------------------
void LevelDbNgramTableTest::testAddTrgWithEmptySrc()
{
    std::vector<WordIndex> s;
    WordIndex t = 81;
    std::vector<WordIndex> t_vec;
    t_vec.push_back(t);

    LogCount lc = LogCount(log(5));

    tab->clear();
    
    tab->incrCountsOfEntryLog(s, t, lc);
    Count tc = tab->cSrc(t_vec);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, tc.get_c_s(), FLT_EPSILON);

    tab->incrCountsOfEntryLog(s, t, lc);
    tc = tab->cSrc(t_vec);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, tc.get_c_s(), FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testCTrg()
{
    // Prepare phrases and counters
    std::vector<WordIndex> s1;
    s1.push_back(13);
    s1.push_back(17);
    std::vector<WordIndex> s2;
    s2.push_back(19);
    s2.push_back(23);
    s2.push_back(29);

    WordIndex t1 = 3;
    WordIndex t2 = 4;

    LogCount lc1 = LogCount(log(23));
    LogCount lc2 = LogCount(log(1));
    
    // Prepare data
    tab->clear();
    
    tab->addSrcInfo(s1, Count((lc1 + lc1 + lc2).get_c_s()));
    tab->incrCountsOfEntryLog(s1, t1, lc1);
    tab->incrCountsOfEntryLog(s1, t1, lc1);
    tab->incrCountsOfEntryLog(s1, t2, lc2);
    
    tab->addSrcInfo(s2, Count((lc1 + lc2).get_c_s()));
    tab->incrCountsOfEntryLog(s2, t1, lc2);
    tab->incrCountsOfEntryLog(s2, t2, lc1);

    // Retrieve values
    Count tc1 = tab->cTrg(t1);
    Count tc2 = tab->cTrg(t2);
    
    // Validate results
    CPPUNIT_ASSERT_DOUBLES_EQUAL(47.0, tc1.get_c_s(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(24.0, tc2.get_c_s(), FLT_EPSILON);
}


//---------------------------------------
void LevelDbNgramTableTest::testLogCountRetrieving()
{
    // Prepare phrases and counters
    std::vector<WordIndex> s1;
    s1.push_back(13);
    s1.push_back(17);
    std::vector<WordIndex> s2;
    s2.push_back(19);
    s2.push_back(23);
    s2.push_back(29);

    WordIndex t1 = 3;
    WordIndex t2 = 4;

    LogCount lc1 = LogCount(log(23));
    LogCount lc2 = LogCount(log(1));
    
    // Prepare data
    tab->clear();
    
    tab->addSrcInfo(s1, Count(lc1.get_c_s() + lc1.get_c_s() + lc2.get_c_s()));
    tab->incrCountsOfEntryLog(s1, t1, lc1);
    tab->incrCountsOfEntryLog(s1, t1, lc1);
    tab->incrCountsOfEntryLog(s1, t2, lc2);
    
    tab->addSrcInfo(s2, Count(lc1.get_c_s() + lc2.get_c_s()));
    tab->incrCountsOfEntryLog(s2, t1, lc2);
    tab->incrCountsOfEntryLog(s2, t2, lc1);

    // Retrieve values
    LogCount sc1 = tab->lcSrc(s1);
    LogCount sc2 = tab->lcSrc(s2);
    LogCount tc1 = tab->lcTrg(t1);
    LogCount tc2 = tab->lcTrg(t2);
    LogCount stc1 = tab->lcSrcTrg(s1, t1);
    LogCount stc2 = tab->lcSrcTrg(s2, t2);
    
    // Validate results
    // Use a bit lower precision for comparison
    // TODO: Identify source of loosing precision
    const float eps = 100 * FLT_EPSILON;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(47.0, sc1.get_c_s(), eps);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(24.0, sc2.get_c_s(), eps);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(47.0, tc1.get_c_s(), eps);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(24.0, tc2.get_c_s(), eps);
    
    CPPUNIT_ASSERT_DOUBLES_EQUAL(46.0, stc1.get_c_st(), eps);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(23.0, stc2.get_c_st(), eps);
}

//---------------------------------------
void LevelDbNgramTableTest::testStoreFloatValues()
{
    std::vector<WordIndex> s;
    s.push_back(7);
    s.push_back(77);
    WordIndex t = 777;

    float s_count_val = 2.33;
    float st_count_val = 7.77;

    tab->clear();

    tab->addSrcInfo(s, Count(s_count_val));
    tab->incrCountsOfEntryLog(s, t, LogCount(log(st_count_val)));

    Count s_count = tab->cSrc(s);
    Count st_count = tab->cSrcTrg(s, t);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(s_count_val, s_count.get_c_s(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(st_count_val, st_count.get_c_st(), FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testStoreAndRestoreSrcInfo()
{
    std::vector<WordIndex> s1 = getVector("Ulica Krancowa");
    std::vector<WordIndex> s2 = getVector("Ulica Sienkiewicza");
    Count cs1 = Count(1);
    Count cs2 = Count(23);
    tab->clear();
    tab->addSrcInfo(s1, cs1);
    tab->addSrcInfo(s2, cs2);

    bool found;
    Count s1_count = tab->getSrcInfo(s1, found);
    Count s2_count = tab->getSrcInfo(s2, found);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, s1_count.get_c_s(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(23.0, s2_count.get_c_s(), FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testKeyVectorConversion()
{
    std::vector<WordIndex> s;
    s.push_back(3);
    s.push_back(4);
    s.push_back(112175);
    s.push_back(90664);
    s.push_back(143);
    s.push_back(749);
    s.push_back(748);

    CPPUNIT_ASSERT( tab->keyToVector(tab->vectorToKey(s)) == s );
}

//---------------------------------------
void LevelDbNgramTableTest::testAddTableEntry()
{
    std::vector<WordIndex> s = getVector("race around Narie");
    WordIndex t = 10688;
    Count s_count = Count(7);
    Count st_count = Count(5);
    im_pair<Count, Count> ppi;
    ppi.first = s_count;
    ppi.second = st_count;

    tab->clear();
    tab->addTableEntry(s, t, ppi);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.0, tab->cSrc(s).get_c_s(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, tab->cSrcTrg(s, t).get_c_st(), FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testIncrCountsOfEntryLog()
{
    std::vector<WordIndex> s = getVector("Narie lake");
    WordIndex t1 = 140991;
    WordIndex t2 = 230689;
    LogCount c_init = LogCount(log(3));
    LogCount c = LogCount(log(17));

    tab->clear();
    tab->incrCountsOfEntryLog(s, t1, c_init);
    tab->incrCountsOfEntryLog(s, t2, c);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, tab->cSrcTrg(s, t1).get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(17.0, tab->cSrcTrg(s, t2).get_c_st(), FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testGetEntriesForTarget()
{
    LevelDbNgramTable::SrcTableNode node;
    std::vector<WordIndex> s1_1;
    s1_1.push_back(1);
    std::vector<WordIndex> s1_2;
    s1_2.push_back(1);
    s1_2.push_back(2);
    WordIndex t1_1 = 1;
    WordIndex t1_2 = 5;
    std::vector<WordIndex> s2;
    s2.push_back(3);
    WordIndex t2 = 2;
    LogCount c = LogCount(log(1));

    tab->clear();
    tab->incrCountsOfEntryLog(s1_1, t1_1, c);
    tab->incrCountsOfEntryLog(s1_2, t1_1, c);
    tab->incrCountsOfEntryLog(s1_1, t1_2, c);
    tab->incrCountsOfEntryLog(s2, t2, c);

    bool result;
    // Looking for phrases with 1 as a target
    result = tab->getEntriesForTarget(t1_1, node);
    CPPUNIT_ASSERT( result );
    CPPUNIT_ASSERT_EQUAL((size_t) 2, node.size());

    // Looking for phrases with 5 as a target
    result = tab->getEntriesForTarget(t1_2, node);
    CPPUNIT_ASSERT( result );
    CPPUNIT_ASSERT_EQUAL((size_t) 1, node.size());

    // Looking for phrases with 2 as a target
    result = tab->getEntriesForTarget(t2, node);
    CPPUNIT_ASSERT( result );
    CPPUNIT_ASSERT_EQUAL((size_t) 1, node.size());

    // '9' key shoud not be found
    result = tab->getEntriesForTarget(9, node);
    CPPUNIT_ASSERT( !result );
}

//---------------------------------------
void LevelDbNgramTableTest::testRetrievingSubphrase()
{
    //  TEST:
    //    Accessing element with the subphrase should return count 0
    //
    bool found;
    std::vector<WordIndex> s1;
    s1.push_back(1);
    s1.push_back(1);
    std::vector<WordIndex> s2 = s1;
    s2.push_back(1);
    WordIndex t1 = 2;
    WordIndex t2 = 3;


    tab->clear();
    tab->incrCountsOfEntryLog(s2, t1, LogCount(log(2)));
    Count c = tab->getSrcTrgInfo(s2, t2, found);

    CPPUNIT_ASSERT( !found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, c.get_c_s(), FLT_EPSILON);

    tab->incrCountsOfEntryLog(s2, t2, LogCount(log(3)));
    c = tab->getSrcTrgInfo(s2, t2, found);

    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, c.get_c_s(), FLT_EPSILON);

    c = tab->getSrcInfo(s1, found);

    CPPUNIT_ASSERT( !found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, c.get_c_s(), FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testGetEntriesForSource()
{
    //  TEST:
    //    Find targets for the source phrase
    //    WARNING: Src phrase has to be present to get results
    //
    bool found;
    LevelDbNgramTable::TrgTableNode node;
    std::vector<WordIndex> s1 = getVector("jezioro Narie");
    WordIndex t1_1 = 230689;
    WordIndex t1_2 = 140991;
    std::vector<WordIndex> s2 = getVector("jezioro Krzywe");
    WordIndex t2_1 = 110735;
    std::vector<WordIndex> s3 = getVector("jezioro Jeziorak");
    WordIndex t3_1 = 5;
    WordIndex t3_2 = 10;

    Count c = Count(1);
    
    // Prepare data structure
    tab->clear();
    // Add Narie phrases
    tab->addSrcInfo(s1, c + c);
    tab->incrCountsOfEntry(s1, t1_1, c);
    tab->incrCountsOfEntry(s1, t1_2, c);
    // Add Krzywe phrases
    tab->addSrcInfo(s2, c);
    tab->incrCountsOfEntry(s2, t2_1, c);
    // Add Jeziorak phrases
    tab->addSrcInfo(s3, c + c);
    tab->incrCountsOfEntry(s3, t3_1, c);
    tab->incrCountsOfEntry(s3, t3_2, c);

    // Looking for targets
    // Narie phrases
    found = tab->getEntriesForSource(s1, node);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, node.size(), FLT_EPSILON);
    // Krzywe phrases
    found = tab->getEntriesForSource(s2, node);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, node.size(), FLT_EPSILON);
    // Jeziorak phrases
    found = tab->getEntriesForSource(s3, node);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, node.size(), FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testRetrievingEntriesWithCountEqualZero()
{
    // TEST:
    //   Function getEntriesForTarget for retrieving entries should skip
    //   elements with count equals 0
    //
    bool found;
    LevelDbNgramTable::SrcTableNode node;
    std::vector<WordIndex> s1 = getVector("Palac Dohnow");
    std::vector<WordIndex> s2 = getVector("Palac Dohnow w Moragu");
    WordIndex t = 1935;
    
    tab->clear();
    tab->addSrcInfo(s1, Count(0));
    tab->addSrcInfo(s2, Count(0));
    tab->incrCountsOfEntryLog(s1, t, LogCount(1));
    tab->incrCountsOfEntryLog(s2, t, LogCount(2));

    found = tab->getEntriesForTarget(t, node);

    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, node.size(), FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testGetNbestForSrc()
{
    //  TEST:
    //    Check if method getNbestForSrc returns correct elements
    //    WARNING: Both - (s) and (s,t) phrases have to present to
    //    retrieve results
    //
    bool found;
    NbestTableNode<WordIndex> node;
    NbestTableNode<WordIndex>::iterator iter;

    // Fill leveldb with data
    std::vector<WordIndex> s;
    s.push_back(1);

    WordIndex t1 = 1;
    WordIndex t2 = 2;
    WordIndex t3 = 3;
    WordIndex t4 = 4;
    
    tab->clear();
    tab->addSrcInfo(s, Count(10));
    tab->incrCountsOfEntryLog(s, t1, LogCount(log(4)));
    tab->incrCountsOfEntryLog(s, t2, LogCount(log(2)));
    tab->incrCountsOfEntryLog(s, t3, LogCount(log(3)));
    tab->incrCountsOfEntryLog(s, t4, LogCount(log(1)));

    // Returned elements should not exceed number of matching
    // elements in the structure
    found = tab->getNbestForSrc(s, node);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_EQUAL((unsigned int) 4, node.size());

    // If there are more available elements, at the beginning
    // the most frequent targets should be returned
    iter = node.begin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(t1, iter->second, FLT_EPSILON);
    iter++;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(t3, iter->second, FLT_EPSILON);
    iter++;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(t2, iter->second, FLT_EPSILON);
    iter++;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(t4, iter->second, FLT_EPSILON);
}

//---------------------------------------
void LevelDbNgramTableTest::testGetNbestForTrg()
{
    //  TEST:
    //    Check if method getNbestForTrg returns correct elements
    //
    bool found;
    NbestTableNode<std::vector<WordIndex> > node;
    NbestTableNode<std::vector<WordIndex> >::iterator iter;

    // Fill leveldb with data
    std::vector<WordIndex> s1;
    s1.push_back(1);
    std::vector<WordIndex> s2;
    s2.push_back(1);
    s2.push_back(1);
    std::vector<WordIndex> s3;
    s3.push_back(4);
    s3.push_back(1);
    std::vector<WordIndex> s4;
    s4.push_back(4);

    WordIndex t1 = 1;
    WordIndex t2 = 10;
    
    tab->clear();
    tab->addSrcInfo(s1, Count(6));
    tab->incrCountsOfEntryLog(s1, t2, LogCount(log(3)));  // Introduce element with different target
    tab->incrCountsOfEntryLog(s1, t1, LogCount(log(3)));
    tab->incrCountsOfEntryLog(s2, t1, LogCount(log(2)));
    tab->incrCountsOfEntryLog(s3, t1, LogCount(log(3)));
    tab->addSrcInfo(s4, Count(4));
    tab->incrCountsOfEntryLog(s4, t1, LogCount(log(4)));

    // Returned elements should not exceed number of elements
    // in the structure or given threshold
    found = tab->getNbestForTrg(t1, node);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_EQUAL((unsigned int) 4, node.size());

    found = tab->getNbestForTrg(t1, node, 2);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, node.size());

    // If there are more available elements, at the beginning
    // the most frequent targets should be returned
    iter = node.begin();
    CPPUNIT_ASSERT( iter->second == s4 );
    iter++;
    CPPUNIT_ASSERT( iter->second == s3 );
}

//---------------------------------------
void LevelDbNgramTableTest::testIteratorsLoop()
{
    //  TEST:
    //    Check basic implementation of iterators - functions
    //    begin(), end() and operators (++ postfix, *).
    //
    std::vector<WordIndex> s1;
    s1.push_back(1);
    WordIndex t1 = 2;

    std::vector<WordIndex> s2;
    s2.push_back(1);
    s2.push_back(2);
    WordIndex t2 = 3;
    
    tab->clear();
    tab->addSrcInfo(s1, Count(2));
    tab->incrCountsOfEntryLog(s1, t1, LogCount(log(2)));
    tab->incrCountsOfEntryLog(s2, t2, LogCount(log(1)));

    CPPUNIT_ASSERT(tab->begin() != tab->end());
    CPPUNIT_ASSERT(tab->begin() != tab->begin());

    int i = 0;
    const int MAX_ITER = 10;

    // Construct dictionary to record results returned by iterator
    // Dictionary structure: (key, (total count value, number of occurences))
    std::map<std::vector<WordIndex>, std::pair<Count, Count> > d;
    d[s1] = std::make_pair(Count(0), Count(0));
    d[tab->getSrcTrg(s1, t1)] = std::make_pair(Count(0), Count(0));
    d[tab->getSrcTrg(s2, t2)] = std::make_pair(Count(0), Count(0));

    for(LevelDbNgramTable::const_iterator iter = tab->begin(); iter != tab->end() && i < MAX_ITER; iter++, i++)
    {
        std::pair<std::vector<WordIndex>, Count> x = *iter;
        d[x.first].first += x.second;
        d[x.first].second += 1;
    }

    // Check if element returned by iterator is correct
    CPPUNIT_ASSERT_EQUAL((size_t) 3, d.size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, d[s1].first.get_c_s(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, d[s1].second.get_c_s(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, d[tab->getSrcTrg(s1, t1)].first.get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, d[tab->getSrcTrg(s1, t1)].second.get_c_s(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, d[tab->getSrcTrg(s2, t2)].first.get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, d[tab->getSrcTrg(s2, t2)].second.get_c_s(), FLT_EPSILON);

    CPPUNIT_ASSERT_EQUAL(3, i);
}

//---------------------------------------
void LevelDbNgramTableTest::testIteratorsOperatorsPlusPlusStar()
{
    //  TEST:
    //    Check basic implementation of iterators - function
    //    begin() and operators (++ prefix, ++ postfix, *, ->).
    //
    bool found = true;

    std::vector<WordIndex> s;
    s.push_back(14);
    s.push_back(9);
    WordIndex t = 91;
    
    tab->clear();
    tab->addSrcInfo(s, Count(2));
    tab->incrCountsOfEntryLog(s, t, LogCount(log(2)));

    // Construct dictionary to record results returned by iterator
    // Dictionary structure: (key, (total count value, number of occurences))
    std::map<std::vector<WordIndex>, std::pair<Count, int> > d;
    d[s] = std::make_pair(Count(), 0);
    d[tab->getSrcTrg(s, t)] = std::make_pair(Count(), 0);
  
    for(LevelDbNgramTable::const_iterator iter = tab->begin(); iter != tab->end(); found = (iter++))
    {
        CPPUNIT_ASSERT( found );
        std::pair<std::vector<WordIndex>, Count> x = *iter;
        d[x.first].first += x.second;
        d[x.first].second++;
    }

    // Iterating beyond the last element should return FALSE value
    CPPUNIT_ASSERT( !found );

    // Check if element returned by iterator is correct
    CPPUNIT_ASSERT_EQUAL((size_t) 2, d.size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0f, d[s].first.get_c_s(), FLT_EPSILON);
    CPPUNIT_ASSERT_EQUAL(1, d[s].second);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, d[tab->getSrcTrg(s, t)].first.get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_EQUAL(1, d[tab->getSrcTrg(s, t)].second);
}

//---------------------------------------
void LevelDbNgramTableTest::testIteratorsOperatorsEqualNotEqual()
{
    //  TEST:
    //    Check basic implementation of iterators - operators == and !=
    //
    std::vector<WordIndex> s;
    s.push_back(150);
    WordIndex t = 150150;
    
    tab->clear();
    tab->addSrcInfo(s, Count(3));
    tab->incrCountsOfEntryLog(s, t, LogCount(0));

    LevelDbNgramTable::const_iterator iter1 = tab->begin();
    iter1++;
    LevelDbNgramTable::const_iterator iter2 = tab->begin();
    
    CPPUNIT_ASSERT( iter1 == iter1 );
    CPPUNIT_ASSERT( !(iter1 != iter1) );
    CPPUNIT_ASSERT( !(iter1 == iter2) );
    CPPUNIT_ASSERT( iter1 != iter2 );
}

//---------------------------------------
void LevelDbNgramTableTest::testSize()
{
    //  TEST:
    //    Check if number of elements in the levelDB is returned correctly
    //
    tab->clear();
    CPPUNIT_ASSERT_EQUAL((size_t) 0, tab->size());  // Collection after cleaning should contain only entry for null key
    
    // Fill leveldb with data
    std::vector<WordIndex> s1;
    s1.push_back(100100);
    s1.push_back(100200);
    std::vector<WordIndex> s2;
    s2.push_back(200100);
    s2.push_back(200200);
    s2.push_back(200300);
    std::vector<WordIndex> s3;
    s3.push_back(100100);
    std::vector<WordIndex> s4;  // Empty key

    WordIndex t1 = 555001;
    WordIndex t2 = 555002;

    tab->incrCountsOfEntryLog(s1, t1, LogCount(log(1)));
    tab->incrCountsOfEntryLog(s1, t2, LogCount(log(2)));

    CPPUNIT_ASSERT_EQUAL((size_t) 2, tab->size());

    tab->clear();
    CPPUNIT_ASSERT_EQUAL((size_t) 0, tab->size());  // Collection after cleaning should be empty

    tab->incrCountsOfEntryLog(s2, t1, LogCount(log(1)));
    tab->incrCountsOfEntryLog(s2, t1, LogCount(log(2)));
    tab->incrCountsOfEntryLog(s2, t2, LogCount(log(1)));
    tab->incrCountsOfEntryLog(s3, t2, LogCount(log(2)));

    CPPUNIT_ASSERT_EQUAL((size_t) 3, tab->size());

    tab->incrCountsOfEntryLog(s3, t1, LogCount(log(5)));
    tab->incrCountsOfEntryLog(s4, t2, LogCount(log(7)));

    CPPUNIT_ASSERT_EQUAL((size_t) (3 + 2), tab->size());

    tab->addSrcInfo(s4, Count(17));

    CPPUNIT_ASSERT_EQUAL((size_t) (3 + 2), tab->size());
}

//---------------------------------------
void LevelDbNgramTableTest::testLoadingLevelDb()
{
    //  TEST:
    //    Check restoring levelDB from disk
    //
    bool result;
    
    // Fill leveldb with data
    tab->clear();
    tab->incrCountsOfEntryLog(getVector("kemping w Kretowinach"), 14, LogCount(1));
    tab->incrCountsOfEntryLog(getVector("kemping w Kretowinach"), 300, LogCount(2));

    tab->incrCountsOfEntryLog(getVector("Pan Samochodzik"), 14, LogCount(1));
    tab->incrCountsOfEntryLog(getVector("Pan Samochodzik"), 100, LogCount(4));
    tab->incrCountsOfEntryLog(getVector("Pan Samochodzik"), 14100, LogCount(20));
    tab->incrCountsOfEntryLog(getVector("Pan Samochodzik"), 58, LogCount(24));

    tab->incrCountsOfEntryLog(getVector("Pierwsza przygoda Pana Samochodzika"), 4, LogCount(5));
    tab->incrCountsOfEntryLog(getVector("Pierwsza przygoda Pana Samochodzika"), 2, LogCount(7));

    size_t original_size = tab->size();
    
    // Load structure
    result = tab->load(dbName.c_str());
    CPPUNIT_ASSERT( result == THOT_OK);
    CPPUNIT_ASSERT_EQUAL(original_size, tab->size());
}

//---------------------------------------
void LevelDbNgramTableTest::testLoadedDataCorrectness()
{
    //  TEST:
    //    Check if the data restored from disk
    //    contain all stored items and correct counts
    //
    bool result;
    
    // Fill levelDB with data
    tab->clear();

    // Define vectors
    std::vector<WordIndex> s1;
    s1.push_back(1000);
    s1.push_back(2000);
    WordIndex t1_1 = 22000;
    WordIndex t1_2 = 33000;
    WordIndex t1_3 = 44000;
    WordIndex t1_4 = 55000;

    std::vector<WordIndex> s2;
    s2.push_back(122000);
    s2.push_back(122);
    WordIndex t2_1 = 66000;
    WordIndex t2_2 = 77000;

    std::vector<WordIndex> s3;
    s3.push_back(155000);
    s3.push_back(177000);
    WordIndex t3_1 = 88000;
    WordIndex t3_2 = 99000;

    // Insert data to levelDB
    tab->incrCountsOfEntryLog(s1, t1_1, LogCount(log(1)));
    tab->incrCountsOfEntryLog(s1, t1_2, LogCount(log(2)));
    tab->incrCountsOfEntryLog(s1, t1_3, LogCount(log(4)));
    tab->incrCountsOfEntryLog(s1, t1_4, LogCount(log(8)));

    tab->incrCountsOfEntryLog(s2, t2_1, LogCount(log(16)));
    tab->incrCountsOfEntryLog(s2, t2_2, LogCount(log(32)));

    tab->incrCountsOfEntryLog(s3, t3_1, LogCount(log(64)));
    tab->incrCountsOfEntryLog(s3, t3_2, LogCount(log(128)));

    size_t original_size = tab->size();
    
    // Load structure
    result = tab->load(dbName.c_str());
    CPPUNIT_ASSERT( result == THOT_OK );
    CPPUNIT_ASSERT_EQUAL(original_size, tab->size());

    // Check count values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, tab->cSrcTrg(s1, t1_1).get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, tab->cSrcTrg(s1, t1_2).get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, tab->cSrcTrg(s1, t1_3).get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, tab->cSrcTrg(s1, t1_4).get_c_st(), FLT_EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(16.0, tab->cSrcTrg(s2, t2_1).get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(32.0, tab->cSrcTrg(s2, t2_2).get_c_st(), FLT_EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(64.0, tab->cSrcTrg(s3, t3_1).get_c_st(), FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(128.0, tab->cSrcTrg(s3, t3_2).get_c_st(), FLT_EPSILON);

    tab->clear();  // Remove data
    CPPUNIT_ASSERT_EQUAL((size_t) 0, tab->size());
}

//---------------------------------------
void LevelDbNgramTableTest::testLoadedDataNullCount()
{
    //  TEST:
    //    Check if the data restored from disk
    //    contain all null count
    //
    bool result;

    // Fill levelDB with data
    tab->clear();

    // Define vectors
    std::vector<WordIndex> s1;
    s1.push_back(1000);
    s1.push_back(2000);
    WordIndex t1 = 22000;

    std::vector<WordIndex> s2;
    s2.push_back(122000);
    s2.push_back(122);
    WordIndex t2 = 66000;

    std::vector<WordIndex> s3;

    // Insert data to levelDB
    tab->addSrcInfo(s1, Count(1));
    tab->addSrcTrgInfo(s1, t1, Count(2));
    tab->addSrcInfo(s2, Count(4));
    tab->addSrcTrgInfo(s2, t2, Count(8));
    tab->addSrcInfo(s3, Count(16));

    size_t original_size = tab->size();

    // Load structure
    result = tab->load(dbName.c_str());
    CPPUNIT_ASSERT( result == THOT_OK );
    CPPUNIT_ASSERT_EQUAL(original_size, tab->size());

    // Check count values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(16.0, tab->cSrc(s3).get_c_st(), FLT_EPSILON);
}
