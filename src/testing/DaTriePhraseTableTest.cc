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

  for(int i = 0; i < phrase.size(); i++) {
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
void DaTriePhraseTableTest::testMock()
{
  CPPUNIT_ASSERT( 1 == 1 );
}
