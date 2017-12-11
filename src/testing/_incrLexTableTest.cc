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
/* Module: _incrLexTableTest                                        */
/*                                                                  */
/* Definitions file: _incrLexTableTest.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "_incrLexTableTest.h"

//--------------- _incrLexTableTest class functions
//

//---------------------------------------
void _incrLexTableTest::testGetSetLexDenom()
{
    bool found;
    WordIndex s = 20;
    float denom = 1.22;

    tab->clear();

    tab->getLexDenom(s, found);
    CPPUNIT_ASSERT( !found );  // Element should not be found

    tab->setLexDenom(s, denom);
    float restoredDenom = tab->getLexDenom(s, found);
    CPPUNIT_ASSERT( found );  // Element should be found
    CPPUNIT_ASSERT_DOUBLES_EQUAL(denom, restoredDenom, DELTA);
}

//---------------------------------------
void _incrLexTableTest::testGetSetLexNumer()
{
    bool found;
    WordIndex s = 14;
    WordIndex t = 10;
    float numer = 15.7;

    tab->clear();

    tab->getLexNumer(s, t, found);
    CPPUNIT_ASSERT( !found );  // Element should not be found

    tab->setLexNumer(s, t, numer);
    float restoredNumer = tab->getLexNumer(s, t, found);
    CPPUNIT_ASSERT( found );  // Element should be found
    CPPUNIT_ASSERT_DOUBLES_EQUAL(numer, restoredNumer, DELTA);
}

//---------------------------------------
void _incrLexTableTest::testSetLexNumerDenom()
{
    bool found;
    WordIndex s = 14;
    WordIndex t = 9;
    float numer = 1.9;
    float denom = 9.1;

    tab->clear();

    tab->getLexNumer(s, t, found);
    CPPUNIT_ASSERT( !found );  // Element should not be found
    tab->getLexDenom(s, found);
    CPPUNIT_ASSERT( !found );  // Element should not be found

    tab->setLexNumDen(s, t, numer, denom);

    float restoredNumer = tab->getLexNumer(s, t, found);
    CPPUNIT_ASSERT( found );  // Element should be found
    CPPUNIT_ASSERT_DOUBLES_EQUAL(numer, restoredNumer, DELTA);

    float restoredDenom = tab->getLexDenom(s, found);
    CPPUNIT_ASSERT( found );  // Element should be found
    CPPUNIT_ASSERT_DOUBLES_EQUAL(denom, restoredDenom, DELTA);
}

//---------------------------------------
void _incrLexTableTest::testGetTransForTarget()
{
    bool found;

    WordIndex s1_1 = 1;
    WordIndex s1_2 = 2;
    WordIndex t1 = 3;

    WordIndex s2 = 9;
    WordIndex t2 = 11;

    tab->clear();

    // Fill structure with data
    tab->setLexNumDen(s1_1, t1, 2.2, 3.3);
    tab->setLexNumDen(s1_2, t1, 4.4, 5.5);
    tab->setLexNumDen(s2, t2, 22.1, 22.7);

    // Query structure and validate results
    std::set<WordIndex> transSet;

    // t1
    std::set<WordIndex> t1Set;
    t1Set.insert(s1_1);
    t1Set.insert(s1_2);

    found = tab->getTransForTarget(t1, transSet);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_EQUAL((size_t) 2, transSet.size());
    CPPUNIT_ASSERT( transSet == t1Set);

    // t2
    std::set<WordIndex> t2Set;
    t2Set.insert(s2);

    found = tab->getTransForTarget(t2, transSet);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_EQUAL((size_t) 1, transSet.size());
    CPPUNIT_ASSERT( transSet == t2Set);
}