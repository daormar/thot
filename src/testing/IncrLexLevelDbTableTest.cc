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
//

//---------------------------------------
void IncrLexLevelDbTableTest::setUp()
{
  tab = new IncrLexLevelDbTable();
  tab->init(dbName);
}

//---------------------------------------
void IncrLexLevelDbTableTest::tearDown()
{
  tab->drop();
  delete tab;
}

//---------------------------------------
void IncrLexLevelDbTableTest::testGetSetLexDenom()
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
    CPPUNIT_ASSERT( denom == restoredDenom);
}

//---------------------------------------
void IncrLexLevelDbTableTest::testGetSetLexNumer()
{
    bool found;
    WordIndex s = 14;
    WordIndex t = 10;
    float denom = 15.7;

    tab->clear();

    tab->getLexNumer(s, t, found);
    CPPUNIT_ASSERT( !found );  // Element should not be found

    tab->setLexNumer(s, t, denom);
    float restoredDenom = tab->getLexNumer(s, t, found);
    CPPUNIT_ASSERT( found );  // Element should be found
    CPPUNIT_ASSERT( denom == restoredDenom);
}