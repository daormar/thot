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
/* Module: IncrLexLevelDbTableTest                                  */
/*                                                                  */
/* Definitions file: IncrLexLevelDbTableTest.cc                     */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrLexLevelDbTableTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( IncrLexLevelDbTableTest );

//--------------- IncrLexLevelDbTableTest class functions

//---------------------------------------
void IncrLexLevelDbTableTest::setUp()
{
  tab = new IncrLexLevelDbTable();
  ((IncrLexLevelDbTable*) tab)->init(dbNamePrefix.c_str());
}

//---------------------------------------
void IncrLexLevelDbTableTest::tearDown()
{
  ((IncrLexLevelDbTable*) tab)->drop();
  delete tab;
}

//---------------------------------------
void IncrLexLevelDbTableTest::testLoadFromBinFile()
{
    bool found;
    float result;

    tab->clear();

    // Define word indexes
    WordIndex s1 = 16;
    WordIndex s2 = 32;

    WordIndex t1 = 128;
    WordIndex t2 = 256;
    WordIndex t3 = 512;
    WordIndex t4 = 1024;

    // Define numerators and denominators
    const float s1_t1_num = -0.3;
    const float s1_t2_num = 2.11;
    const float s2_t1_num = 3.66;
    const float s2_t3_num = 7.17;
    const float s2_t4_num = 9.9999999;
    const float s1_denom = 2.3;
    const float s2_denom = 4.41;

    // Fill table with data
    tab->setLexNumDen(s1, t1, s1_t1_num, s1_denom);
    tab->setLexNumDen(s1, t2, s1_t2_num, s1_denom);
    tab->setLexNumDen(s2, t1, s2_t1_num, s2_denom);
    tab->setLexNumDen(s2, t3, s2_t3_num, s2_denom);
    tab->setLexNumDen(s2, t4, s2_t4_num, s2_denom);

    // Save data to binary file
    ((IncrLexLevelDbTable*) tab)->printBin(dbNamePrefix.c_str());
    // Remove DB directory to prevent loading it
    ((IncrLexLevelDbTable*) tab)->drop();

    // Reload DB
    delete tab;
    tab = new IncrLexLevelDbTable();
    ((IncrLexLevelDbTable*) tab)->load(dbNamePrefix.c_str());

    // Check if loaded results are correct
    // (s1, t1)
    result = tab->getLexNumer(s1, t1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(s1_t1_num, result, EPSILON);
    result = tab->getLexDenom(s1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(s1_denom, result, EPSILON);
    // (s1, t2) - denominator has been already checked
    result = tab->getLexNumer(s1, t2, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(s1_t2_num, result, EPSILON);
    // (s2, t1)
    result = tab->getLexNumer(s2, t1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(s2_t1_num, result, EPSILON);
    result = tab->getLexDenom(s2, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(s2_denom, result, EPSILON);
    // (s2, t2) - should not be found
    tab->getLexNumer(s2, t2, found);
    CPPUNIT_ASSERT( !found );
    // (s2, t3)
    result = tab->getLexNumer(s2, t3, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(s2_t3_num, result, EPSILON);
    // (s2, t4)
    result = tab->getLexNumer(s2, t4, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(s2_t4_num, result, EPSILON);
}

//---------------------------------------
void IncrLexLevelDbTableTest::testLoadFromLevelDb()
{
    bool found;
    float result;

    tab->clear();

    WordIndex s1 = 16;
    WordIndex s2 = 32;

    WordIndex t1 = 128;
    WordIndex t2 = 256;

    // Fill table with data
    tab->setLexNumDen(s1, t1, 0.5, 2.5);
    tab->setLexNumDen(s1, t2, 2.125, 2.5);
    tab->setLexNumDen(s2, t1, 3.5, 4.5);

    // Reload DB
    delete tab;
    tab = new IncrLexLevelDbTable();
    tab->load(dbNamePrefix.c_str());

    // Check if loaded results are correct
    // (s1, t1)
    result = tab->getLexNumer(s1, t1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, result, EPSILON);
    result = tab->getLexDenom(s1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.5, result, EPSILON);
    // (s1, t2) - denominator has been already checked
    result = tab->getLexNumer(s1, t2, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.125, result, EPSILON);
    // (s2, t1)
    result = tab->getLexNumer(s2, t1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.5, result, EPSILON);
    result = tab->getLexDenom(s2, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, result, EPSILON);
    // (s2, t2) - should not be found
    tab->getLexNumer(s2, t2, found);
    CPPUNIT_ASSERT( !found );
}