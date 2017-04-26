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
void DaTriePhraseTableTest::testMock()
{
  // TODO: Implement real test
  CPPUNIT_ASSERT( 1 == 1 );
}
