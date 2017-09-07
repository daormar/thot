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
  ((IncrLexLevelDbTable*) tab)->init(dbName);
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
    const string binFileName = "/tmp/thot_bin_file_unit_test";

    tab->clear();

    WordIndex s1 = 16;
    WordIndex s2 = 32;

    WordIndex t1 = 128;
    WordIndex t2 = 256;
    WordIndex t3 = 512;
    WordIndex t4 = 1024;

    // Fill table with data
    tab->setLexNumDen(s1, t1, 0.3, 2.3);
    tab->setLexNumDen(s1, t2, 2.11, 2.3);
    tab->setLexNumDen(s2, t1, 3.66, 4.41);
    tab->setLexNumDen(s2, t3, 7.17, 4.41);
    tab->setLexNumDen(s2, t4, 9.9999999, 4.41);

    // Save data to binary file
    ((IncrLexLevelDbTable*) tab)->printBin(binFileName.c_str());

    // Reload DB
    delete tab;
    tab = new IncrLexLevelDbTable();
    ((IncrLexLevelDbTable*) tab)->init(dbName);
    ((IncrLexLevelDbTable*) tab)->load(binFileName.c_str());

    // Check if loaded results are correct
    // (s1, t1)
    result = tab->getLexNumer(s1, t1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 0.3 );
    result = tab->getLexDenom(s1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 2.3 );
    // (s1, t2) - denominator has been already checked
    result = tab->getLexNumer(s1, t2, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 2.111 );
    // (s2, t1)
    result = tab->getLexNumer(s2, t1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 3.66 );
    result = tab->getLexDenom(s2, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 4.41 );
    // (s2, t2) - should not be found
    tab->getLexNumer(s2, t2, found);
    CPPUNIT_ASSERT( !found );
    // (s2, t3)
    result = tab->getLexNumer(s2, t3, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 7.17 );
    // (s2, t4)
    result = tab->getLexNumer(s2, t4, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 9.9999999 );
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
    tab->load(dbName.c_str());

    // Check if loaded results are correct
    // (s1, t1)
    result = tab->getLexNumer(s1, t1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 0.5 );
    result = tab->getLexDenom(s1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 2.5 );
    // (s1, t2) - denominator has been already checked
    result = tab->getLexNumer(s1, t2, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 2.125 );
    // (s2, t1)
    result = tab->getLexNumer(s2, t1, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 3.5 );
    result = tab->getLexDenom(s2, found);
    CPPUNIT_ASSERT( found );
    CPPUNIT_ASSERT( result == 4.5 );
    // (s2, t2) - should not be found
    tab->getLexNumer(s2, t2, found);
    CPPUNIT_ASSERT( !found );
}